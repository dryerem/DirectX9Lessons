#include <windows.h> // Подключаемый заголовочный файл Windows
#include <d3d9.h> // Подключаемый заголовочный файл DirectX 9 SDK
#include <d3dx9.h> // Подключаемый заголовочный файл DirectX 9 SDK с функциями для работы с матрицами
#include <d3dx9core.h> // Заголовочный файл для работы со шрифтом
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
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL)

LPDIRECT3D9 pDirect3D = nullptr;
LPDIRECT3DDEVICE9 pDirect3DDevice = nullptr;
LPDIRECT3DVERTEXBUFFER9 pBufferVertex = nullptr;
LPDIRECT3DINDEXBUFFER9 pBufferIndex = nullptr;

// Для работы со шрифтом
LPD3DXFONT pFont = nullptr;
RECT Rect;

LRESULT CALLBACK MainWinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT InitialDirect3D(HWND hWnd);
void RenderingDirect3D();
void DeleteDirect3D();
void DrawMyText(LPDIRECT3DDEVICE9 pDirect3DDevice, LPCSTR textString, int x, int y, int x1, int y1, D3DCOLOR color);

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
		L"Текст", // Заголовок окна 
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

	DrawMyText(pDirect3DDevice, "Текст в DirectX", 100, 100, 300, 200, D3DCOLOR_XRGB(0, 0, 0));

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

	if (pFont != nullptr)
		pFont->Release();
}

void DrawMyText(LPDIRECT3DDEVICE9 pDirect3DDevice, LPCSTR textString, int x, int y, int x1, int y1, D3DCOLOR color)
{
	// Создаём шрифт
	D3DXCreateFont(pDirect3DDevice, 30, 10, FW_NORMAL, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
		ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial", &pFont);

	// Координаты прямоугольника
	SetRect(&Rect, x, y, x1, y1);

	// Вывод текста
	pFont->DrawTextA(NULL, textString, -1, &Rect, DT_LEFT, color);
}