//-----------------------------------------------------------------------------------
// shader.cpp 
// В данном уроке мы меняем цвет вершины в вершинном шейдере, мы также 
// изменили FVF, заменили цвет на нормали, это необходимо для дальнейших уроков 
// с освещением. 
//-----------------------------------------------------------------------------------
#include <windows.h>         // Подключаем заголовочный файл Windows
#include <d3d9.h>            // Подключаем заголовочный файл DirectX 9 SDK
#include <d3dx9.h>           // Подключаем из D3DX утилит для работы с матрицами
#include <mmsystem.h>        // Подключаем системную библеотеку 
//-----------------------------------------------------------------------------------
// Глобальные переменные
//-----------------------------------------------------------------------------------


LPDIRECT3D9                 pDirect3D          = NULL;      // Главный Direct3D обьект
LPDIRECT3DDEVICE9           pDirect3DDevice    = NULL;      // Устройство 
LPDIRECT3DVERTEXBUFFER9     pBufferVershin     = NULL;      // Буфер вершин
LPDIRECT3DINDEXBUFFER9      pBufferIndex       = NULL;      // Индексный буфер 

// Указатель на объект вершинного шейдера
IDirect3DVertexShader9* g_pVertexShader = NULL;

// Таблица для установки параметров в глобальные переменные шейдера
ID3DXConstantTable* g_pConstantTable = NULL;

struct CUSTOMVERTEX
{
    FLOAT x, y, z;      //кординаты 
    FLOAT nx, ny, nz;     //нормали
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL)  // Формат вершин
//-----------------------------------------------------------------------------------
// Функция
// InitialDirect3D()
// Инициализация Direct3D
//-----------------------------------------------------------------------------------
HRESULT InitialDirect3D( HWND hwnd )
{
    
    if( NULL == ( pDirect3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return E_FAIL;

    D3DDISPLAYMODE Display;
    if( FAILED( pDirect3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &Display ) ) )
        return E_FAIL;

    D3DPRESENT_PARAMETERS Direct3DParametr; 
    ZeroMemory( &Direct3DParametr, sizeof(Direct3DParametr) );
    Direct3DParametr.Windowed         = TRUE;
    Direct3DParametr.SwapEffect       = D3DSWAPEFFECT_DISCARD;
    Direct3DParametr.BackBufferFormat = Display.Format;
    Direct3DParametr.EnableAutoDepthStencil = TRUE;
    Direct3DParametr.AutoDepthStencilFormat = D3DFMT_D16;

    if( FAILED( pDirect3D -> CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
                                      D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                      &Direct3DParametr, &pDirect3DDevice ) ) )
        return E_FAIL;
    
    // Включаем отсеченние Direct3D
    pDirect3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
    // Отключить освещенние Direct3D
	pDirect3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	// Включаем  Z-буфер
    pDirect3DDevice->SetRenderState (D3DRS_ZENABLE, D3DZB_TRUE);

    return S_OK;
}

//---------------------------------------------------------------------------------------
// Функция
// InitialShader()
// Иннициализирует шейдер
//---------------------------------------------------------------------------------------
void InitialShader()
{
    // Код ошибки
    HRESULT hr{ S_OK };

    ID3DXBuffer* pShader = nullptr; // Буфер с текстом шейдера
    ID3DXBuffer* pError  = nullptr; // Буфер с текстом ошибки компиляции

    // Компилируем шейдер из файла
    D3DXCompileShaderFromFile(
        "vertex_shader.hlsl", // Путь к файлу шейдера 
        nullptr,
        nullptr,
        "main", // Имя точки входа
        "vs_1_1", // Версия шейдера
        D3DXSHADER_DEBUG | D3DXSHADER_USE_LEGACY_D3DX9_31_DLL,
        &pShader, // Указатель на буфер, куда будет записан текст 
        &pError,
        &g_pConstantTable
        );

    // Если на этапе компиляции произошли ошибки и есть текст с ошибкой компиляции
    if (pError)
    {
        // Выводим сообщение с текстом
        ::MessageBox(0, (char*)pError->GetBufferPointer(), 0, 0);

        // Освобождаем ресурсы буфера под ошибку
        pError->Release();
    }

    // Функция компиляции вернула ошибку
    if (FAILED(hr))
    {
        ::MessageBox(0, "D3DXCompileShaderFromFile() - FAILED!", 0, 0);
        return;
    }

    // Создаём объект вершинного шейдера
    hr = pDirect3DDevice->CreateVertexShader((DWORD*)pShader->GetBufferPointer(), &g_pVertexShader);

    // Функция создания объекта шейдера вернула ошибку
    if (FAILED(hr))
    {
        ::MessageBox(0, "CreateVertexShader() - FAILED!", 0, 0);
        return;
    }

    // Освобождаем ресурсы буфера под шейдер
    pShader->Release();
}

//---------------------------------------------------------------------------------------
// Функция
// InitialObject()
// Иннициализирует вершины
//---------------------------------------------------------------------------------------

HRESULT InitialObject()
{
   CUSTOMVERTEX Vershin[] =
    {   
       {  1.0f,-1.0f,-1.0f, 0.0f, 0.0f,-1.0f, }, //А
       {  1.0f, 1.0f,-1.0f, 0.0f, 0.0f,-1.0f, }, //В
       { -1.0f, 1.0f,-1.0f, 0.0f, 0.0f,-1.0f, }, //С
       { -1.0f,-1.0f,-1.0f, 0.0f, 0.0f,-1.0f, }, //D

       { -1.0f,-1.0f,-1.0f,-1.0f, 0.0f, 0.0f, }, //A2
       { -1.0f, 1.0f,-1.0f,-1.0f, 0.0f, 0.0f, }, //B2
       { -1.0f, 1.0f, 1.0f,-1.0f, 0.0f, 0.0f, }, //C2
       { -1.0f,-1.0f, 1.0f,-1.0f, 0.0f, 0.0f, }, //D2

       { -1.0f,-1.0f, 1.0f, 0.0f, 0.0f, 1.0f, }, //A3
       { -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, }, //B3
       {  1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, }, //C3
       {  1.0f,-1.0f, 1.0f, 0.0f, 0.0f, 1.0f, }, //D3

       {  1.0f,-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, }, //A4
       {  1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, }, //B4
       {  1.0f, 1.0f,-1.0f, 1.0f, 0.0f, 0.0f, }, //C4
       {  1.0f,-1.0f,-1.0f, 1.0f, 0.0f, 0.0f, }, //D4

       {  1.0f,-1.0f,-1.0f, 0.0f,-1.0f, 0.0f, }, //A5
       { -1.0f,-1.0f,-1.0f, 0.0f,-1.0f, 0.0f, }, //B5
       { -1.0f,-1.0f, 1.0f, 0.0f,-1.0f, 0.0f, }, //C5
       {  1.0f,-1.0f, 1.0f, 0.0f,-1.0f, 0.0f, }, //D5

       {  1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, }, //A6 
       { -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, }, //B6
       { -1.0f, 1.0f,-1.0f, 0.0f, 1.0f, 0.0f, }, //C6
       {  1.0f, 1.0f,-1.0f, 0.0f, 1.0f, 0.0f, }, //D6
    };
    const unsigned short Index[]={
    0,1,2,      2,3,0,
    4,5,6,      6,7,4,
    8,9,10,     10,11,8,
	12,13,14,   14,15,12,
    16,17,18,   18,19,16,
	20,21,22,   22,23,20,
    };
	// Создаем буфер вершин
    if( FAILED( pDirect3DDevice->CreateVertexBuffer( 36 * sizeof(CUSTOMVERTEX),
        0, D3DFVF_CUSTOMVERTEX,D3DPOOL_DEFAULT, &pBufferVershin, NULL ) ) )
        return E_FAIL;
    //Блокируем
    VOID* pBV;
    if( FAILED( pBufferVershin->Lock( 0, sizeof(Vershin), (void**)&pBV, 0 ) ) ) 
        return E_FAIL;
    //Копируем
    memcpy( pBV, Vershin, sizeof(Vershin) );
    // Разблокируем
    pBufferVershin->Unlock(); 
    // Создаем индексный буфер
    pDirect3DDevice->CreateIndexBuffer( 36 * sizeof(Index), 
       0, D3DFMT_INDEX16, D3DPOOL_DEFAULT,&pBufferIndex, NULL);
    //Блокируем
    VOID* pBI;
    pBufferIndex->Lock( 0, sizeof(Index) , (void**)&pBI, 0 );
    //Копируем
    memcpy( pBI, Index, sizeof(Index) );
    // Разблокируем
    pBufferIndex->Unlock();

   return S_OK;
}
//-----------------------------------------------------------------------------
// Функция
// Matrix()
// Мировая матрица, матрица вида, матрица проекции
//-----------------------------------------------------------------------------
VOID Matrix()
{
    
    D3DXMATRIX MatrixWorld, MatrixWorldX, MatrixWorldY, MatrixWorldZ ; //  Мировая матрица по X и Y(MatrixWorld)
    D3DXMATRIX MatrixView;       //  Матрица вида (MatrixView)
    D3DXMATRIX MatrixProjection; //  Матрица проекции (MatrixProjection)

    // MatrixWorld
    UINT  Time  = timeGetTime() % 5000;
    FLOAT Angel = Time * (2.0f * D3DX_PI) / 5000.0f;
    D3DXMatrixRotationX( &MatrixWorldX, Angel  );
    D3DXMatrixRotationY( &MatrixWorldY, Angel  );
    // Перемнажаем матрицы
    D3DXMatrixMultiply( &MatrixWorld, &MatrixWorldX, &MatrixWorldY );

    // MatrixView
    D3DXMatrixLookAtLH( &MatrixView,  &D3DXVECTOR3 ( 0.0f, 0.0f,-8.0f ),
                                      &D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f ),
                                      &D3DXVECTOR3 ( 0.0f, 1.0f, 0.0f ) );
    // MatrixProjection
    D3DXMatrixPerspectiveFovLH( &MatrixProjection, D3DX_PI/4, 1.0f, 1.0f, 100.0f );
    // Matrix view-projection
    D3DXMATRIX vp = MatrixView * MatrixProjection;

    // передаём матрицу мира в вертексный шейдер
    g_pConstantTable->SetMatrix(pDirect3DDevice, "mWorld", &MatrixWorld);
    // передаём матрицу вида/проекции в вертексный шейдер
    g_pConstantTable->SetMatrix(pDirect3DDevice, "mViewProjection", &vp);
}
//-----------------------------------------------------------------------------------
//  Функция
//  RenderingDirect3D()
//  Рисуем
//-----------------------------------------------------------------------------------

VOID RenderingDirect3D()
{
    
    if(pDirect3DDevice == NULL)        // Проверяем ошибки
        return;
    
    pDirect3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                           D3DCOLOR_XRGB(60,100,150), 1.0f, 0 );
    // Начало сцены
    pDirect3DDevice->BeginScene();     
    
    // Здесь происходит прорисовка сцены
    Matrix();

    pDirect3DDevice->SetVertexShader(g_pVertexShader);
    pDirect3DDevice->SetStreamSource( 0, pBufferVershin, 0, sizeof(CUSTOMVERTEX) );
    pDirect3DDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
    pDirect3DDevice->SetIndices(pBufferIndex);

    // Вывод объекта
    pDirect3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 36, 0, 12);
    // Конец сцены
    pDirect3DDevice->EndScene();       
     
    pDirect3DDevice->Present( NULL, NULL, NULL, NULL );
	  
}
//-----------------------------------------------------------------------------------
//  Функция
//  DeleteDirect3D()
//  Освобождает захваченные ресурсы
//-----------------------------------------------------------------------------------

VOID DeleteDirect3D()
{

     if( pBufferIndex  != NULL)
        pBufferIndex->Release(); 

    if( pBufferVershin  != NULL)
        pBufferVershin->Release(); 

    if( pDirect3DDevice != NULL) 
        pDirect3DDevice->Release();

    if( pDirect3D != NULL)
        pDirect3D->Release();
}
//-----------------------------------------------------------------------------------
// Функция
// MainWinProc()
// Здесь происходит обработка сообщений
//-----------------------------------------------------------------------------------

LRESULT CALLBACK MainWinProc(HWND   hwnd,            
					        UINT    msg,              
                            WPARAM  wparam,           
                            LPARAM  lparam)          
{

switch(msg)
	{	
	
     case WM_DESTROY: 
		{
		DeleteDirect3D();
		PostQuitMessage(0);
		return(0);
		} 
	} 
  return DefWindowProc(hwnd, msg, wparam, lparam);
} 
//-----------------------------------------------------------------------------------
// Функция
// WinMain
// Входная точка приложения
//-----------------------------------------------------------------------------------

int WINAPI WinMain(	HINSTANCE hinstance,
					HINSTANCE hprevinstance,
					LPSTR lpcmdline,
					int ncmdshow)
{

WNDCLASSEX windowsclass;  // Создаем класс
HWND	   hwnd;	      // Создаем дескриптор окна
MSG        msg;           // Идентификатор сообщения

// Определим класс окна WNDCLASSEX
windowsclass.cbSize         = sizeof(WNDCLASSEX);
windowsclass.style			= CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
windowsclass.lpfnWndProc	= MainWinProc;
windowsclass.cbClsExtra		= 0;
windowsclass.cbWndExtra		= 0;
windowsclass.hInstance		= hinstance;
windowsclass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
windowsclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
windowsclass.hbrBackground	= (HBRUSH)GetStockObject(GRAY_BRUSH);
windowsclass.lpszMenuName	= NULL;
windowsclass.lpszClassName	= "WINDOWSCLASS";
windowsclass.hIconSm        = LoadIcon(NULL, IDI_APPLICATION);

// Зарегистрируем класс
if (!RegisterClassEx(&windowsclass))
	return(0);

// Теперь когда класс зарегестрирован можно создать окно

if (!(hwnd = CreateWindowEx(NULL,              // стиль окна
                       "WINDOWSCLASS",         // класс
					   "Ипользуем простой вершинный шейдер ", // название окна
					   WS_OVERLAPPEDWINDOW | WS_VISIBLE,
					   300,150,	               // левый верхний угол
					   500,400,                // ширина и высота
					   NULL,	               // дескриптор родительского окна 
					   NULL,	               // дескриптор меню
					   hinstance,              // дескриптор экземпляра приложения
					   NULL)))	               // указатель на данные окна                                            
return 0;
     
 if( SUCCEEDED( InitialDirect3D( hwnd ) ) )
  { 
     InitialShader();
       if( SUCCEEDED( InitialObject( ) ) )
	    {
           ShowWindow( hwnd, SW_SHOWDEFAULT );
           UpdateWindow( hwnd );

              ZeroMemory( &msg, sizeof(msg));
              while( msg.message!=WM_QUIT) 
                {
                  if(PeekMessage( &msg, NULL,0,0,PM_REMOVE ))
		           {
			          TranslateMessage( &msg );
                      DispatchMessage( &msg );
                   }
                  else
                  RenderingDirect3D();
                } 
        }
  } 
   return 0;
}