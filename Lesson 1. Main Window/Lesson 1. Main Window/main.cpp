#include <windows.h>

LRESULT CALLBACK MainWinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
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
	windowclass.lpszClassName = "XEngineClass";
	windowclass.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

	// Регистрируем класс окна
	if (!RegisterClassEx(&windowclass))
		return 0;

	HWND hwnd; // Создаем дескриптор окна
	hwnd = CreateWindowEx(
		0, // Стиль окна
		"XEngineClass", // Класс окна
		"Базовое окно для DirectX", // Заголовок окна 
		WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
		0, 0, // Левый верхний угол 
		500, 400, // Ширина и высота 
		nullptr, // Дескриптор родительского окна
		nullptr, // Дескриптор меню
		hInstance, // Дескриптор приложения
		nullptr); // Указатель на данные окна

	if (!hwnd)
		return 0;

	ShowWindow(hwnd, nCmdShow); // Нарисовать окно
	UpdateWindow(hwnd); // Обновить окно

	MSG msg; // Создаем идентификатор сообщения 
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (msg.wParam);
}

LRESULT CALLBACK MainWinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_PAINT:
			break;
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		} break;
	}
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}