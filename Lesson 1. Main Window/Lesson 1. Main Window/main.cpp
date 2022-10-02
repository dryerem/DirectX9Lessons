#include <windows.h>

LRESULT CALLBACK MainWinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	WNDCLASSEX windowclass; // ������� ����� ����
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

	// ������������ ����� ����
	if (!RegisterClassEx(&windowclass))
		return 0;

	HWND hwnd; // ������� ���������� ����
	hwnd = CreateWindowEx(
		0, // ����� ����
		"XEngineClass", // ����� ����
		"������� ���� ��� DirectX", // ��������� ���� 
		WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
		0, 0, // ����� ������� ���� 
		500, 400, // ������ � ������ 
		nullptr, // ���������� ������������� ����
		nullptr, // ���������� ����
		hInstance, // ���������� ����������
		nullptr); // ��������� �� ������ ����

	if (!hwnd)
		return 0;

	ShowWindow(hwnd, nCmdShow); // ���������� ����
	UpdateWindow(hwnd); // �������� ����

	MSG msg; // ������� ������������� ��������� 
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