#include <windows.h>

using namespace std;

BOOL InitApplication(HINSTANCE hinstance);
BOOL InitInstance(HINSTANCE hinstance, int nCMdShow);
LRESULT CALLBACK WndProc(HWND hwnd, UINT messagem, WPARAM wparam, LPARAM lparam);
void DisplayText(HDC& hdc, LOGFONT& logfont, RECT& rect, char* text);
void ModifyFont(LOGFONT& logfont, char c);

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
	static int x, y;
	static char textToDisplay[] = "Text \0";
	RECT rect;
	static LOGFONT logfont;
	static HDC hdc;
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_CREATE:
		logfont.lfHeight = 500 / strlen(textToDisplay);
		logfont.lfWidth = 0.75*logfont.lfHeight;
		break;
	case WM_SIZE:
		x = LOWORD(lparam);
		y = HIWORD(lparam);
		break;
	case WM_CHAR:
		switch (wparam)
		{
		case '-':
			ModifyFont(logfont, '-');
			textToDisplay[strlen(textToDisplay) - 1] = '-'; // textToDisplay[strlen(textToDisplay)] = '-';
			break;
		case	'+':
			ModifyFont(logfont, '+');
			textToDisplay[strlen(textToDisplay) - 1] = '+'; // textToDisplay[strlen(textToDisplay)] = '+';
			break;
		default:
			break;
		}
		InvalidateRect(hwnd, NULL, true);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rect);
		DisplayText(hdc, logfont, rect, textToDisplay);
		EndPaint(hwnd, &ps);
		break;
	case  WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wparam, lparam);
	}
	return FALSE;
}

void DisplayText(HDC& hdc, LOGFONT& logfont, RECT &rect, char* text)
{
	logfont.lfEscapement = 0;
	logfont.lfOrientation = 2;
	logfont.lfWeight = 2;
	logfont.lfItalic = 1;
	logfont.lfUnderline = 0;
	logfont.lfStrikeOut = 0;
	logfont.lfCharSet = ANSI_CHARSET;
	logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logfont.lfPitchAndFamily = DEFAULT_PITCH;
	logfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logfont.lfQuality = DEFAULT_QUALITY;

	HFONT newFont; newFont = CreateFontIndirect(&logfont);
	HFONT oldFont; oldFont = (HFONT)SelectObject(hdc, newFont);

	TextOut(hdc, rect.left, rect.top, text, strlen(text));
	DeleteObject(newFont);
	SelectObject(hdc, oldFont);
}

void ModifyFont(LOGFONT& logfont, char c)
{
	if (c == '-')
	{
		if (logfont.lfHeight >= 4 && logfont.lfWidth >= 4)
		{
			logfont.lfHeight -= 4;
			logfont.lfWidth -= 3;
		}
	}
	else
	{
		logfont.lfHeight += 4;
		logfont.lfWidth += 3;
	}
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
	wc.lpszClassName = (LPSTR)"Timetable";
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
		"Timetable",
		"Timetable",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
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
	return TRUE;
}