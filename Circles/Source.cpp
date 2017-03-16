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
	Circle circle;
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_CREATE:
		//hdc = GetDC(hwnd);
		LoadFromFile(circleCollection);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		
		for_each(circleCollection.begin(), circleCollection.end(), [](Circle crcl) { DrawCircle(hdc, crcl.center.x, crcl.center.y, crcl.R); });

		EndPaint(hwnd, &ps);
		break;
	case WM_LBUTTONUP:
		int xC; xC = LOWORD(lparam);
		int yC;  yC = HIWORD(lparam);

		circle = GetCircle(circleCollection, xC, yC);
		int cR; cR = circle.R;

		if (cR == -1)
		{
			circle.R = 5;
			circleCollection.push_back(circle);
		}
		else
		{
			circle.R += 5;
			circleCollection.push_back(circle); // Region
		}
		//HRGN toRedraw; toRedraw = CreateEllipticRgn(circle.center.x - cR, circle.center.y - cR, circle.center.x + cR, circle.center.y + cR);
		hdc = GetDC(hwnd);
		DrawCircle(hdc, circle.center.x, circle.center.y, circle.R);
		ReleaseDC(hwnd, hdc);
		ValidateRect(hwnd, NULL);
		//InvalidateRgn(hwnd, toRedraw, true);

		//DeleteObject(toRedraw);
		//InvalidateRect(hwnd, NULL, true);
		break;
	case WM_SIZE:
		x = LOWORD(lparam);
		y = HIWORD(lparam);
		break;
	case  WM_CLOSE:
		SaveToFile(circleCollection);
		DestroyWindow(hwnd);
		//ReleaseDC(hwnd, hdc);
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
	double dist = pow((circle.center.x - x), 2) + pow((circle.center.y - y), 2); //double
	return (dist <= pow(circle.R,2)) ? true : false; // r*r
}

Circle GetCircle(vector<Circle>& circles, int x, int y)
{
	vector<Circle>::reverse_iterator iter = find_if(circles.rbegin(), circles.rend(), [x, y](Circle c)
	{
		return IsInCircle(x, y, c);
	});

	Circle c; c.center.x = x; c.center.y = y;
	if (iter != circles.rend())
	{
		c = *iter;
		circles.erase((iter + 1).base());
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
	copy(istream_iterator<Circle>(out), istream_iterator<Circle>(), back_inserter(circles));
	out.close();
}