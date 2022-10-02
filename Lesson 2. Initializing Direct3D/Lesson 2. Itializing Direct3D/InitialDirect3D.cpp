#include <windows.h> // ������������ ������������ ���� Windows
#include <d3d9.h> // ������������ ������������ ���� DirectX 9 SDK

LPDIRECT3D9 pDirect3D = nullptr;
LPDIRECT3DDEVICE9 pDirect3DDevice = nullptr;


LRESULT CALLBACK MainWinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT InitialDirect3D(HWND hWnd);
void RenderingDirect3D();
void DeleteDirect3D();

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) // ����� ����� � ����������
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
	windowclass.lpszClassName = L"XEngineClass";
	windowclass.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

	// ������������ ����� ����
	if (!RegisterClassEx(&windowclass))
		return 0;

	HWND hwnd; // ������� ���������� ����
	hwnd = CreateWindowEx(
		0, // ����� ����
		L"XEngineClass", // ����� ����
		L"������������� Direct3D", // ��������� ���� 
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0, 0, // ����� ������� ���� 
		500, 400, // ������ � ������ 
		nullptr, // ���������� ������������� ����
		nullptr, // ���������� ����
		hInstance, // ���������� ����������
		nullptr); // ��������� �� ������ ����

	if (!hwnd)
		return 0;

	MSG msg; // ������� ������������� ��������� 
	if (SUCCEEDED(InitialDirect3D(hwnd)))
	{
		ShowWindow(hwnd, nCmdShow); // ���������� ����
		UpdateWindow(hwnd); // �������� ����
		while (GetMessage(&msg, nullptr, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return ((int)msg.wParam);
}

LRESULT CALLBACK MainWinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) // ��������� ���������
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

void RenderingDirect3D() // ���������
{
	pDirect3DDevice->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255, 0, 0), 1.0F, 0);
	pDirect3DDevice->BeginScene(); // ������ �����
	// ����� ���������� ���������� �����
	pDirect3DDevice->EndScene(); // ����� �����
	pDirect3DDevice->Present(nullptr, nullptr, nullptr, nullptr);
}

void DeleteDirect3D() // ������������ ����������� ��������
{
	if (pDirect3DDevice != nullptr)
		pDirect3DDevice->Release();
	if (pDirect3D != nullptr)
		pDirect3D->Release();
}