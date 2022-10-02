#include <windows.h> // Подключаемый заголовочный файл Windows
#include <d3d9.h> // Подключаемый заголовочный файл DirectX 9 SDK

struct CUSTOMVERTEX
{
	FLOAT X, Y, Z;
	FLOAT rhw;
	DWORD color;
	FLOAT tu, tv;
};

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)

LPDIRECT3D9 pDirect3D = nullptr;
LPDIRECT3DDEVICE9 pDirect3DDevice = nullptr;
LPDIRECT3DVERTEXBUFFER9 pBufferVertex = nullptr;


LRESULT CALLBACK MainWinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT InitialDirect3D(HWND hWnd);
void RenderingDirect3D();
void DeleteDirect3D();
HRESULT InitVertexBuffer();

#define WIDTH 600
#define HEIGHT 480

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
		L"Треугольник", // Заголовок окна 
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0, 0, // Левый верхний угол 
		WIDTH, HEIGHT, // Ширина и высота 
		nullptr, // Дескриптор родительского окна
		nullptr, // Дескриптор меню
		hInstance, // Дескриптор приложения
		nullptr); // Указатель на данные окна

	if (!hwnd)
		return 0;

	MSG msg = { 0 }; // Создаем идентификатор сообщения 
	if (SUCCEEDED(InitialDirect3D(hwnd)))
	{
		if (SUCCEEDED(InitVertexBuffer()))
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
	if (FAILED(pDirect3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&Direct3DParametr, &pDirect3DDevice)))
		return E_FAIL;

	return S_OK;
}

void RenderingDirect3D() // Рисование
{
	pDirect3DDevice->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255, 0, 0), 1.0F, 0);
	pDirect3DDevice->BeginScene(); // Начало сцены

	pDirect3DDevice->SetStreamSource(0, pBufferVertex, 0, sizeof(CUSTOMVERTEX));
	pDirect3DDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
	pDirect3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

	pDirect3DDevice->EndScene(); // Конец сцены
	pDirect3DDevice->Present(nullptr, nullptr, nullptr, nullptr);
}

void DeleteDirect3D() // Освобождение захваченных ресурсов
{
	if (pDirect3DDevice != nullptr)
		pDirect3DDevice->Release();
	if (pDirect3D != nullptr)
		pDirect3D->Release();
}

HRESULT InitVertexBuffer()
{
	CUSTOMVERTEX Vertex[] =
	{
		// x, y, z, rhw, color
		{300.0f, 300.0f, 0.5f, 1.0f, 0xBCA13}, // правая вершина
		{150.0f, 300.0f, 0.5f, 1.0f, 0xBCA13}, // левая вершина
		{150.0f, 150.0f, 1.0f, 0.5f, 1.0f, 0xBCA13} // верхняя вершина
	};

	if (FAILED(pDirect3DDevice->CreateVertexBuffer(3 * sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &pBufferVertex, nullptr)))
		return E_FAIL;

	VOID* pBV;
	if (FAILED(pBufferVertex->Lock(0, sizeof(Vertex), (void**)&pBV, 0)))
		return E_FAIL;	
	memcpy(pBV, Vertex, sizeof(Vertex));
	pBufferVertex->Unlock();
	
	return S_OK;
}