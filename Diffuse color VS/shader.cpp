//-----------------------------------------------------------------------------------
// shader.cpp 
// � ������ ����� �� ������ ���� ������� � ��������� �������, �� ����� 
// �������� FVF, �������� ���� �� �������, ��� ���������� ��� ���������� ������ 
// � ����������. 
//-----------------------------------------------------------------------------------
#include <windows.h>         // ���������� ������������ ���� Windows
#include <d3d9.h>            // ���������� ������������ ���� DirectX 9 SDK
#include <d3dx9.h>           // ���������� �� D3DX ������ ��� ������ � ���������
#include <mmsystem.h>        // ���������� ��������� ���������� 
//-----------------------------------------------------------------------------------
// ���������� ����������
//-----------------------------------------------------------------------------------


LPDIRECT3D9                 pDirect3D          = NULL;      // ������� Direct3D ������
LPDIRECT3DDEVICE9           pDirect3DDevice    = NULL;      // ���������� 
LPDIRECT3DVERTEXBUFFER9     pBufferVershin     = NULL;      // ����� ������
LPDIRECT3DINDEXBUFFER9      pBufferIndex       = NULL;      // ��������� ����� 

// ��������� �� ������ ���������� �������
IDirect3DVertexShader9* g_pVertexShader = NULL;

// ������� ��� ��������� ���������� � ���������� ���������� �������
ID3DXConstantTable* g_pConstantTable = NULL;

struct CUSTOMVERTEX
{
    FLOAT x, y, z;      //��������� 
    FLOAT nx, ny, nz;     //�������
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL)  // ������ ������
//-----------------------------------------------------------------------------------
// �������
// InitialDirect3D()
// ������������� Direct3D
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
    
    // �������� ���������� Direct3D
    pDirect3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
    // ��������� ���������� Direct3D
	pDirect3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	// ��������  Z-�����
    pDirect3DDevice->SetRenderState (D3DRS_ZENABLE, D3DZB_TRUE);

    return S_OK;
}

//---------------------------------------------------------------------------------------
// �������
// InitialShader()
// ��������������� ������
//---------------------------------------------------------------------------------------
void InitialShader()
{
    // ��� ������
    HRESULT hr{ S_OK };

    ID3DXBuffer* pShader = nullptr; // ����� � ������� �������
    ID3DXBuffer* pError  = nullptr; // ����� � ������� ������ ����������

    // ����������� ������ �� �����
    D3DXCompileShaderFromFile(
        "vertex_shader.hlsl", // ���� � ����� ������� 
        nullptr,
        nullptr,
        "main", // ��� ����� �����
        "vs_1_1", // ������ �������
        D3DXSHADER_DEBUG | D3DXSHADER_USE_LEGACY_D3DX9_31_DLL,
        &pShader, // ��������� �� �����, ���� ����� ������� ����� 
        &pError,
        &g_pConstantTable
        );

    // ���� �� ����� ���������� ��������� ������ � ���� ����� � ������� ����������
    if (pError)
    {
        // ������� ��������� � �������
        ::MessageBox(0, (char*)pError->GetBufferPointer(), 0, 0);

        // ����������� ������� ������ ��� ������
        pError->Release();
    }

    // ������� ���������� ������� ������
    if (FAILED(hr))
    {
        ::MessageBox(0, "D3DXCompileShaderFromFile() - FAILED!", 0, 0);
        return;
    }

    // ������ ������ ���������� �������
    hr = pDirect3DDevice->CreateVertexShader((DWORD*)pShader->GetBufferPointer(), &g_pVertexShader);

    // ������� �������� ������� ������� ������� ������
    if (FAILED(hr))
    {
        ::MessageBox(0, "CreateVertexShader() - FAILED!", 0, 0);
        return;
    }

    // ����������� ������� ������ ��� ������
    pShader->Release();
}

//---------------------------------------------------------------------------------------
// �������
// InitialObject()
// ��������������� �������
//---------------------------------------------------------------------------------------

HRESULT InitialObject()
{
   CUSTOMVERTEX Vershin[] =
    {   
       {  1.0f,-1.0f,-1.0f, 0.0f, 0.0f,-1.0f, }, //�
       {  1.0f, 1.0f,-1.0f, 0.0f, 0.0f,-1.0f, }, //�
       { -1.0f, 1.0f,-1.0f, 0.0f, 0.0f,-1.0f, }, //�
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
	// ������� ����� ������
    if( FAILED( pDirect3DDevice->CreateVertexBuffer( 36 * sizeof(CUSTOMVERTEX),
        0, D3DFVF_CUSTOMVERTEX,D3DPOOL_DEFAULT, &pBufferVershin, NULL ) ) )
        return E_FAIL;
    //���������
    VOID* pBV;
    if( FAILED( pBufferVershin->Lock( 0, sizeof(Vershin), (void**)&pBV, 0 ) ) ) 
        return E_FAIL;
    //��������
    memcpy( pBV, Vershin, sizeof(Vershin) );
    // ������������
    pBufferVershin->Unlock(); 
    // ������� ��������� �����
    pDirect3DDevice->CreateIndexBuffer( 36 * sizeof(Index), 
       0, D3DFMT_INDEX16, D3DPOOL_DEFAULT,&pBufferIndex, NULL);
    //���������
    VOID* pBI;
    pBufferIndex->Lock( 0, sizeof(Index) , (void**)&pBI, 0 );
    //��������
    memcpy( pBI, Index, sizeof(Index) );
    // ������������
    pBufferIndex->Unlock();

   return S_OK;
}
//-----------------------------------------------------------------------------
// �������
// Matrix()
// ������� �������, ������� ����, ������� ��������
//-----------------------------------------------------------------------------
VOID Matrix()
{
    
    D3DXMATRIX MatrixWorld, MatrixWorldX, MatrixWorldY, MatrixWorldZ ; //  ������� ������� �� X � Y(MatrixWorld)
    D3DXMATRIX MatrixView;       //  ������� ���� (MatrixView)
    D3DXMATRIX MatrixProjection; //  ������� �������� (MatrixProjection)

    // MatrixWorld
    UINT  Time  = timeGetTime() % 5000;
    FLOAT Angel = Time * (2.0f * D3DX_PI) / 5000.0f;
    D3DXMatrixRotationX( &MatrixWorldX, Angel  );
    D3DXMatrixRotationY( &MatrixWorldY, Angel  );
    // ����������� �������
    D3DXMatrixMultiply( &MatrixWorld, &MatrixWorldX, &MatrixWorldY );

    // MatrixView
    D3DXMatrixLookAtLH( &MatrixView,  &D3DXVECTOR3 ( 0.0f, 0.0f,-8.0f ),
                                      &D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f ),
                                      &D3DXVECTOR3 ( 0.0f, 1.0f, 0.0f ) );
    // MatrixProjection
    D3DXMatrixPerspectiveFovLH( &MatrixProjection, D3DX_PI/4, 1.0f, 1.0f, 100.0f );
    // Matrix view-projection
    D3DXMATRIX vp = MatrixView * MatrixProjection;

    // ������� ������� ���� � ���������� ������
    g_pConstantTable->SetMatrix(pDirect3DDevice, "mWorld", &MatrixWorld);
    // ������� ������� ����/�������� � ���������� ������
    g_pConstantTable->SetMatrix(pDirect3DDevice, "mViewProjection", &vp);
}
//-----------------------------------------------------------------------------------
//  �������
//  RenderingDirect3D()
//  ������
//-----------------------------------------------------------------------------------

VOID RenderingDirect3D()
{
    
    if(pDirect3DDevice == NULL)        // ��������� ������
        return;
    
    pDirect3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                           D3DCOLOR_XRGB(60,100,150), 1.0f, 0 );
    // ������ �����
    pDirect3DDevice->BeginScene();     
    
    // ����� ���������� ���������� �����
    Matrix();

    pDirect3DDevice->SetVertexShader(g_pVertexShader);
    pDirect3DDevice->SetStreamSource( 0, pBufferVershin, 0, sizeof(CUSTOMVERTEX) );
    pDirect3DDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
    pDirect3DDevice->SetIndices(pBufferIndex);

    // ����� �������
    pDirect3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 36, 0, 12);
    // ����� �����
    pDirect3DDevice->EndScene();       
     
    pDirect3DDevice->Present( NULL, NULL, NULL, NULL );
	  
}
//-----------------------------------------------------------------------------------
//  �������
//  DeleteDirect3D()
//  ����������� ����������� �������
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
// �������
// MainWinProc()
// ����� ���������� ��������� ���������
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
// �������
// WinMain
// ������� ����� ����������
//-----------------------------------------------------------------------------------

int WINAPI WinMain(	HINSTANCE hinstance,
					HINSTANCE hprevinstance,
					LPSTR lpcmdline,
					int ncmdshow)
{

WNDCLASSEX windowsclass;  // ������� �����
HWND	   hwnd;	      // ������� ���������� ����
MSG        msg;           // ������������� ���������

// ��������� ����� ���� WNDCLASSEX
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

// �������������� �����
if (!RegisterClassEx(&windowsclass))
	return(0);

// ������ ����� ����� ��������������� ����� ������� ����

if (!(hwnd = CreateWindowEx(NULL,              // ����� ����
                       "WINDOWSCLASS",         // �����
					   "��������� ������� ��������� ������ ", // �������� ����
					   WS_OVERLAPPEDWINDOW | WS_VISIBLE,
					   300,150,	               // ����� ������� ����
					   500,400,                // ������ � ������
					   NULL,	               // ���������� ������������� ���� 
					   NULL,	               // ���������� ����
					   hinstance,              // ���������� ���������� ����������
					   NULL)))	               // ��������� �� ������ ����                                            
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