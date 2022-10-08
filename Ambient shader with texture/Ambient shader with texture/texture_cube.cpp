#include <windows.h> // Подключаемый заголовочный файл Windows
#include <d3d9.h> // Подключаемый заголовочный файл DirectX 9 SDK
#include <d3dx9.h> // Подключаемый заголовочный файл DirectX 9 SDK с функциями для работы с матрицами
#include <mmsystem.h> // Подключаемый заголовочный файл для работы с системными функциями 

const unsigned short Index[] = {
	0, 1, 2,		2, 3, 0,
	4, 5, 6,		6, 7, 4,
	8, 9, 10,		10, 11, 8,
	12, 13, 14,		14, 15, 12,
	16, 17, 18,		18, 19, 16,
	20, 21, 22,		22, 23, 20
};

struct CUSTOMVERTEX
{
	FLOAT X, Y, Z; // Координаты
	FLOAT nx, ny, nz; // Нормали
	FLOAT tu, tv; // Текстурные кординаты
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1) // D3DFVF_TEX1 текстурный режим

LPDIRECT3D9 pDirect3D = nullptr;
LPDIRECT3DDEVICE9 pDirect3DDevice = nullptr;
LPDIRECT3DVERTEXBUFFER9 pBufferVertex = nullptr;
LPDIRECT3DINDEXBUFFER9 pBufferIndex = nullptr;
LPDIRECT3DTEXTURE9 pTexture_01 = nullptr; // Указатель на интерфейс, в котором будет хранится информация о загружаемой текстуре

ID3DXEffect* g_pEffect = NULL;

IDirect3DVertexShader9* g_pVertexShader = NULL;
ID3DXConstantTable* g_pPixelShaderConstantTable = NULL;

LRESULT CALLBACK MainWinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT InitialDirect3D(HWND hWnd);
void RenderingDirect3D();
void DeleteDirect3D();
HRESULT InitObject();
void Matrix();
void LightMaterial();

//---------------------------------------------------------------------------------------
// Функция
// InitialShader()
// Иннициализирует вершинный шейдер
//---------------------------------------------------------------------------------------
void InitialVertexShader()
{
	// Код ошибки
	HRESULT hr{ S_OK };

	ID3DXBuffer* pError = nullptr; // Буфер с текстом ошибки компиляции

	// Компилируем шейдер из файла
	D3DXCreateEffectFromFile(
		pDirect3DDevice,
		L"Ambient.fx", 
		nullptr,
		nullptr,
		D3DXSHADER_DEBUG,
		nullptr,
		&g_pEffect, 
		&pError
	);

	// Если на этапе компиляции произошли ошибки и есть текст с ошибкой компиляции
	if (pError)
	{
		// Выводим сообщение с текстом
		::MessageBox(0, (wchar_t*)pError->GetBufferPointer(), 0, 0);

		// Освобождаем ресурсы буфера под ошибку
		pError->Release();
	}

	// Функция компиляции вернула ошибку
	if (FAILED(hr))
	{
		::MessageBox(0, L"D3DXCompileShaderFromFile() - FAILED!", 0, 0);
		return;
	}
}


int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) // Точка входа в приложение
{
	WNDCLASSEX windowclass; // Создаем класс окна
	windowclass.cbSize = sizeof(WNDCLASSEX);
	windowclass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC | CS_DBLCLKS;
	windowclass.lpfnWndProc = MainWinProc;
	windowclass.cbClsExtra = 0;
	windowclass.cbWndExtra = 0;
	windowclass.hInstance = hInstance;
	windowclass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	windowclass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowclass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	windowclass.lpszMenuName = nullptr;
	windowclass.lpszClassName = L"XEngineClass";
	windowclass.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

	// Регистрируем класс окна
	if (!RegisterClassEx(&windowclass))
		return 0;

	HWND hwnd; // Создаем дескриптор окна
	hwnd = CreateWindowEx(
		0, // Стиль окна
		L"XEngineClass", // Класс окна
		L"Рассеянное освещение с текстурой", // Заголовок окна 
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0, 0, // Левый верхний угол 
		500, 400, // Ширина и высота 
		nullptr, // Дескриптор родительского окна
		nullptr, // Дескриптор меню
		hInstance, // Дескриптор приложения
		nullptr); // Указатель на данные окна

	if (!hwnd)
		return 0;

	MSG msg = { 0 }; // Создаем идентификатор сообщения 
	if (SUCCEEDED(InitialDirect3D(hwnd)))
	{
		InitialVertexShader();
		if (SUCCEEDED(InitObject()))
		{
			ShowWindow(hwnd, nCmdShow); // Нарисовать окно
			UpdateWindow(hwnd); // Обновить окно
			while (msg.message != WM_QUIT)
			{
				if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else
				{
					RenderingDirect3D();
				}
			}
		}
	}

	return 0;
}

LRESULT CALLBACK MainWinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) // Обработка сообщений
{
	switch (uMsg)
	{
	case WM_PAINT:
		RenderingDirect3D();
		ValidateRect(hWnd, nullptr);
		break;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		DeleteDirect3D();
		return 0;
	} break;
	}
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

HRESULT InitialDirect3D(HWND hWnd)
{
	if (nullptr == (pDirect3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;

	D3DDISPLAYMODE Display;
	if (FAILED(pDirect3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &Display)))
		return E_FAIL;

	D3DPRESENT_PARAMETERS Direct3DParametr;
	ZeroMemory(&Direct3DParametr, sizeof(Direct3DParametr));
	Direct3DParametr.Windowed = TRUE;
	Direct3DParametr.SwapEffect = D3DSWAPEFFECT_DISCARD;
	Direct3DParametr.BackBufferFormat = Display.Format;
	Direct3DParametr.EnableAutoDepthStencil = TRUE;
	Direct3DParametr.AutoDepthStencilFormat = D3DFMT_D16;
	if (FAILED(pDirect3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&Direct3DParametr, &pDirect3DDevice)))
		return E_FAIL;

	pDirect3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW); // Включить отсечение Direct3D
	pDirect3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE); // Отключить отсечение Direct3D
	pDirect3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE); // Включить буффер глубины

	return S_OK;
}

void RenderingDirect3D() // Рисование
{
	pDirect3DDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(60, 100, 150), 1.0f, 0);
	pDirect3DDevice->BeginScene(); // Начало сцены

	LightMaterial(); // Освещение
	Matrix(); // Вращение


	UINT cPasses;
	UINT iPass;

	g_pEffect->SetTechnique("RenderScene");
	g_pEffect->Begin(&cPasses, 0);
	{
		for (iPass = 0; iPass < cPasses; iPass++)
		{
			g_pEffect->BeginPass(iPass);
			{
				pDirect3DDevice->SetStreamSource(0, pBufferVertex, 0, sizeof(CUSTOMVERTEX));
				pDirect3DDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
				pDirect3DDevice->SetIndices(pBufferIndex);
				pDirect3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 36, 0, 12);
			}
			g_pEffect->EndPass();
		}
	}
	g_pEffect->End();

	// Конец сцены
	pDirect3DDevice->EndScene();
	pDirect3DDevice->Present(nullptr, nullptr, nullptr, nullptr);
}

void DeleteDirect3D() // Освобождение захваченных ресурсов
{
	if (pBufferIndex != nullptr)
		pBufferIndex->Release();

	if (pBufferVertex != nullptr)
		pBufferVertex->Release();

	if (pDirect3DDevice != nullptr)
		pDirect3DDevice->Release();

	if (pDirect3D != nullptr)
		pDirect3D->Release();

	if (pTexture_01 != nullptr)
		pTexture_01->Release();
}

HRESULT InitObject()
{
	CUSTOMVERTEX Vertex[] =
	{
		// Синий
		{1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f},     // A
		{1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f},      // B
		{-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f},     // C
		{-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f},    // D

		// Красный
		{-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f},    // A2
		{-1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f},     // B2
		{-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f},     // C2
		{-1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f},     // D2

		// Синий
		{-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f},    // A3
		{-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},     // B3
		{1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},      // C3
		{1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f},     // D3

		// Зеленый
		{1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f},    // A4
		{1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f},     // B4
		{1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f},    // C4
		{1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f},   // D4

		// Розовый
		{1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f},    // A5
		{-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f},   // B5
		{-1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f},    // C5
		{1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f},     // D5

		// Розовый
		{1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},     // A6
		{-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},    // B6
		{-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f},   // C6
		{1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},    // D6
	};

	if (FAILED(D3DXCreateTextureFromFile(pDirect3DDevice, L"texture.jpg", &pTexture_01))) // Загрузка текстуры из каталога приложения
		return E_FAIL;

	if (FAILED(pDirect3DDevice->CreateVertexBuffer(36 * sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &pBufferVertex, nullptr)))
		return E_FAIL;

	VOID* pBV;
	if (FAILED(pBufferVertex->Lock(0, sizeof(Vertex), (void**)&pBV, 0)))
		return E_FAIL;
	memcpy(pBV, Vertex, sizeof(Vertex));
	pBufferVertex->Unlock();

	if (FAILED(pDirect3DDevice->CreateIndexBuffer(36 * sizeof(CUSTOMVERTEX),
		0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &pBufferIndex, NULL)))
		return E_FAIL;

	VOID* pBI;
	pBufferIndex->Lock(0, sizeof(Index), (void**)&pBI, 0);
	memcpy(pBI, Index, sizeof(Index));
	pBufferIndex->Unlock();

	return S_OK;
}

void Matrix()
{
	// Мировая матрица
	D3DXMATRIX MatrixWorld;
	D3DXMATRIX MatrixWorldX;
	D3DXMATRIX MatrixWorldY;
	D3DXMATRIX MatrixWorldZ;

	D3DXMATRIX MatrixView; // Видовая матрица
	D3DXMATRIX MatrixProjection; // Матрица проекции

	// MatrixWorld
	UINT Time = timeGetTime() % 5000;
	FLOAT Angle = Time * (2.0f * D3DX_PI) / 5000.0f;
	D3DXMatrixRotationX(&MatrixWorldX, Angle);
	D3DXMatrixRotationY(&MatrixWorldY, Angle);
	D3DXMatrixRotationZ(&MatrixWorldZ, Angle);
	D3DXMatrixMultiply(&MatrixWorld, &MatrixWorldX, &MatrixWorldY);

	// MatrixView
	D3DXVECTOR3 vectorX(0.0f, 0.0f, -5.0f);
	D3DXVECTOR3 vectorY(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vectorZ(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&MatrixView, &vectorX, &vectorY, &vectorZ);

	// MatrixProjection
	D3DXMatrixPerspectiveFovLH(&MatrixProjection, D3DX_PI / 4, 1.0f, 1.0f, 100.0f);

	D3DXMATRIX wvp = MatrixWorld * MatrixView * MatrixProjection;

	g_pEffect->SetMatrix("gWorld", &MatrixWorld);
	g_pEffect->SetMatrix("gWorldViewProjection", &wvp);
}

void LightMaterial()
{
	D3DXCOLOR color = D3DCOLOR_XRGB(90, 25, 10);
	float intensity = 1.0f;

	g_pEffect->SetFloatArray("gMaterialAmbientColor", color, 4);
	g_pEffect->SetValue("gAmbientIntensity", &intensity, sizeof(float));
	g_pEffect->SetTexture("gDiffuseTexture", pTexture_01);
}