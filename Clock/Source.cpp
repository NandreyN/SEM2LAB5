#include <windows.h>
#include <string>

using namespace std;
#define TIMER1 1
#define MYKEY 1
#define WIDTHFACTOR 0.8
#define HEIGHTFACTOR 1.5

BOOL InitApplication(HINSTANCE hinstance);
BOOL InitInstance(HINSTANCE hinstance, int nCMdShow);
LRESULT CALLBACK WndProc(HWND hwnd, UINT messagem, WPARAM wparam, LPARAM lparam);
void DrawCircle(HDC& hdc, RECT& rect);
void DisplayTime(HDC& hdc, RECT& rect, int seconds);
string GetOutTime(int seconds);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	InitApplication(hInstance);
	InitInstance(hInstance, nCmdShow);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	static bool ifTicks;
	static int x, y;
	static HDC hdc;
	PAINTSTRUCT ps;
	static RECT clientRect;
	static int seconds;

	switch (message)
	{
	case WM_CREATE:
		ifTicks = true;
		SetTimer(hwnd, TIMER1, 1000, (TIMERPROC)NULL);
		break;
	case  WM_SIZE:
		x = LOWORD(lparam);
		y = HIWORD(lparam);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &clientRect);
		DrawCircle(hdc, clientRect);
		DisplayTime(hdc, clientRect, seconds);
		EndPaint(hwnd, &ps);
		break;
	case WM_HOTKEY:
		switch (wparam)
		{
		case MYKEY:
			seconds = -1;
			break;
		}
		break;
	case WM_CHAR:
		switch (wparam)
		{
		case '-':
			ifTicks = false;
			break;
		case '+':
			ifTicks = true;
			break;
		}

		break;
	case WM_TIMER:
		switch (wparam)
		{
		case TIMER1:
		{
			if (!ifTicks) break;
			seconds++;
			string dsp = GetOutTime(seconds);
			int wdth = (WIDTHFACTOR*x) / dsp.size();
			int hght = wdth  * HEIGHTFACTOR;
			RECT rect;
			rect.left = 0; rect.top = y / 2 - hght;
			rect.right = x; rect.bottom = y / 2 + hght;

			InvalidateRect(hwnd, &rect, true);
			break;
		}
		default: break;
		}
		break;

	case  WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		KillTimer(hwnd, TIMER1);
		UnregisterHotKey(hwnd, MYKEY);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wparam, lparam);
	}
	return FALSE;
}

BOOL InitApplication(HINSTANCE hinstance)
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(wc);
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.hInstance = hinstance;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = (LPSTR)"Clock";
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpszMenuName = NULL;

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Unable to init app", "Error", MB_OK);
		return FALSE;
	}
	return TRUE;
}
BOOL InitInstance(HINSTANCE hinstance, int nCMdShow)
{
	HWND hwnd; hwnd = CreateWindow(
		"Clock",
		"Clock",
		WS_OVERLAPPEDWINDOW,
		0,
		0,
		600,
		600,
		NULL,
		NULL,
		hinstance,
		NULL
	);

	if (!hwnd)
	{
		MessageBox(NULL, "Cannot init instance", "Error", MB_OK);
		return FALSE;
	}

	ShowWindow(hwnd, nCMdShow);
	UpdateWindow(hwnd);
	RegisterHotKey(hwnd, MYKEY, MOD_SHIFT, 'S');
	return TRUE;
}
void DrawCircle(HDC& hdc, RECT& rect)
{
	HPEN newPen, oldPen;
	newPen = CreatePen(PS_SOLID, 3, BLACK_PEN);
	oldPen = (HPEN)SelectObject(hdc, newPen);
	Ellipse(hdc, rect.left, rect.top, rect.right, rect.bottom);
	SelectObject(hdc, oldPen);
	DeleteObject(newPen);
}

void DisplayTime(HDC& hdc, RECT& rect, int seconds)
{
	string displayText = GetOutTime(seconds);
	LOGFONT lf;
	lf.lfWidth = (WIDTHFACTOR*rect.right) / displayText.size();
	lf.lfHeight = lf.lfWidth * HEIGHTFACTOR;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfEscapement = 0;
	lf.lfItalic = 0;
	lf.lfOrientation = 0;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfPitchAndFamily = DEFAULT_PITCH;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfStrikeOut = 0;
	lf.lfUnderline = 0;

	HFONT newFont = CreateFontIndirect(&lf);
	HFONT oldFont = (HFONT)SelectObject(hdc, newFont);

	RECT rectToDraw;
	rectToDraw.left = 0;
	rectToDraw.top = (rect.bottom / 2) - lf.lfHeight;
	rectToDraw.right = rect.right;
	rectToDraw.bottom = (rect.bottom / 2) + lf.lfHeight;

	DrawText(hdc, displayText.data(), displayText.size(), &rectToDraw, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

	SelectObject(hdc, oldFont);
	DeleteObject(newFont);
}

string GetOutTime(int seconds)
{
	int minutes = seconds / 60;
	seconds %= 60;
	string displayText;
	displayText += to_string(minutes) + ":" + to_string(seconds);
	return displayText;
}