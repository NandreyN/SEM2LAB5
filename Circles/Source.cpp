#include <windows.h>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iterator>
#include <sstream>

using namespace std;

struct Circle
{
	Circle() { R = -1; };
	POINT center;
	int R;
};
ostream& operator<<(ostream& o, Circle c)
{
	o << c.center.x << " " << c.center.y << " " << c.R;
	return o;
}

istream& operator >> (istream& i, Circle& c)
{
	string data;
	getline(i, data);
	istringstream iss(data);
	iss >> c.center.x >> c.center.y >> c.R;
	return i;
}
BOOL InitApplication(HINSTANCE hinstance);
BOOL InitInstance(HINSTANCE hinstance, int nCMdShow);
LRESULT CALLBACK WndProc(HWND hwnd, UINT messagem, WPARAM wparam, LPARAM lparam);
Circle DrawCircle(HDC& hdc, int x, int y, int R);
bool IsInCircle(int x, int y, Circle& circle);
Circle GetCircle(vector<Circle>& circles, int x, int y);
void SaveToFile(vector<Circle>& circles);
void LoadFromFile(vector<Circle>& circles);

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
	static HDC hdc;
	static vector<Circle> circleCollection;
	Circle c;

	switch (message)
	{
	case WM_CREATE:
		hdc = GetDC(hwnd);
		LoadFromFile(circleCollection);
		break;
	case WM_PAINT:
		for_each(circleCollection.begin(), circleCollection.end(), [](Circle circle) { DrawCircle(hdc, circle.center.x, circle.center.y, circle.R); });
		break;
	case WM_LBUTTONUP:
		int xC; xC = LOWORD(lparam);
		int yC;  yC = HIWORD(lparam);

		c = GetCircle(circleCollection, xC, yC);
		if (c.R == -1)
		{
			circleCollection.push_back(DrawCircle(hdc, xC, yC, 5));
		}
		else
		{
			circleCollection.push_back(DrawCircle(hdc, c.center.x, c.center.y, c.R + 5));
		}

		break;
	case WM_SIZE:
		x = LOWORD(lparam);
		y = HIWORD(lparam);
		break;
	case  WM_CLOSE:
		SaveToFile(circleCollection);
		DestroyWindow(hwnd);
		ReleaseDC(hwnd, hdc);
		break;
	case WM_DESTROY:
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
	wc.lpszClassName = (LPSTR)"Circles";
	wc.style = CS_HREDRAW | CS_VREDRAW ;
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
		"Circles",
		"Circles",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
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
	return TRUE;
}
Circle DrawCircle(HDC& hdc, int x0, int y0, int R)
{
	Circle c; POINT p; p.x = x0; p.y = y0;
	c.center = p; c.R = R;
	Ellipse(hdc, p.x - R, p.y - R, p.x + R, p.y + R);
	return c;
}

bool IsInCircle(int x, int y, Circle& circle)
{
	int dist = sqrt(pow((circle.center.x - x), 2) + pow((circle.center.y - y), 2));
	return (dist <= circle.R) ? true : false;
}

Circle GetCircle(vector<Circle>& circles, int x, int y)
{
	vector<Circle>::iterator iter = find_if(circles.begin(), circles.end(), [x, y](Circle c)
	{
		return IsInCircle(x, y, c);
	});
	Circle c;
	if (iter != circles.end())
	{
		c = *iter;
		circles.erase(iter);
	}
	return c;
}

void SaveToFile(vector<Circle>& circles)
{
	ofstream out("data.txt");
	out.clear();
	copy(circles.begin(), circles.end(), ostream_iterator<Circle>(out, "\n"));
	out.close();
}

void LoadFromFile(vector<Circle>& circles)
{
	ifstream out("data.txt");
	copy(istream_iterator<Circle>(out), istream_iterator<Circle>(),back_inserter(circles));
	out.close();
}