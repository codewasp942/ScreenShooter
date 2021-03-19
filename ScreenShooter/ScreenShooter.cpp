#include <iostream>
#include <fstream>
#include <windows.h>
#include <wchar.h>

HWND hWndDesktop;
HDC hDCDesktop;
HDC hDCDesktopCom;
RECT rectWindow;
HBITMAP hBitmapCom;
HBITMAP hBitmapOld;
BITMAP BitMapData;
BITMAPINFOHEADER BmpInfoHead;

bool initShot() {
	BmpInfoHead.biBitCount = 24;
	BmpInfoHead.biClrImportant = 0;
	BmpInfoHead.biCompression = BI_RGB;
	BmpInfoHead.biHeight = bit.bmHeight;
	BmpInfoHead.biPlanes = 1;
	BmpInfoHead.biSize = sizeof(BITMAPINFOHEADER);
	BmpInfoHead.biWidth = bit.bmWidth;
	BmpInfoHead.biXPelsPerMeter = 0;
	BmpInfoHead.biYPelsPerMeter = 0;
	hWndDesktop = GetDesktopWindow();
	hDCDesktop = GetWindowDC(hWndDesktop);
	hDCDesktopCom = CreateCompatibleDC(hDCDesktop);
	GetWindowRect(hWndDesktop, &rectWindow);
	hBitmapCom = CreateCompatibleBitmap(hDCDesktop, rectWindow.right, rectWindow.bottom);
	return 1;
}

void shot() {
	hBitmapOld=SelectObject(hDCDesktopCom, hBitmapCom);
	BitBlt(hDCDesktopCom, 0, 0, rectWindow.right, rectWindow.bottom, hWndDesktop, 0, 0, SRCCOPY);
	SelectObject(hDCDesktopCom, hBitmapOld);
	GetObject(hBitmapCom, sizeof(BITMAP), &BitMapData);
	DWORD iSizeBmp = BitMapData.bmWidthBytes * BitMapData.bmHeight;	
	LPSTR lpData = (LPSTR)GlobalAlloc(GPTR, iSizeBmp);
	BmpInfoHead.biSizeImage = iSizeBmp
	GetDIBits(hDCDesktop, hBitmapCom, 0, BmpInfoHead.biHeight, lpData, (BITMAPINFO*)&BmpInfoHead, DIB_RGB_COLORS); ;
}

void screen(char* fileName)
{
	
	

	
	int w = rectWindow.right,
		h = rectWindow.bottom;
	void* buf = new char[w * h * 4];

	
	SelectObject(hDCDesktopCom, hBitmapCom);

	StretchBlt(hDCDesktopCom, 0, 0, w, h, hDCDesktop, 0, 0, w, h, SRCCOPY);

	void* f = CreateFileA(fileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, 0);
	 
	GetObject(hBitmapCom, 84, buf);

	tagBITMAPINFO bi;
	bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
	bi.bmiHeader.biWidth = w;
	bi.bmiHeader.biHeight = h;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biCompression = 0;
	bi.bmiHeader.biSizeImage = 0;

	CreateDIBSection(hDCDesktopCom, &bi, DIB_RGB_COLORS, &buf, 0, 0);
	GetDIBits(hDCDesktopCom, hBitmapCom, 0, h, buf, &bi, DIB_RGB_COLORS);

	BITMAPFILEHEADER bif;
	bif.bfType = MAKEWORD('B', 'M');
	bif.bfSize = w * h * 4 + 54;
	bif.bfOffBits = 54;

	BITMAPINFOHEADER bii;
	bii.biSize = 40;
	bii.biWidth = w;
	bii.biHeight = h;
	bii.biPlanes = 1;
	bii.biBitCount = 32;
	bii.biCompression = 0;
	bii.biSizeImage = w * h * 4;

	DWORD r;
	WriteFile(f, &bif, sizeof(bif), &r, NULL);
	WriteFile(f, &bii, sizeof(bii), &r, NULL);
	WriteFile(f, buf, w * h * 4, &r, NULL);

	CloseHandle(f);
	DeleteDC(hDCDesktop);
	DeleteDC(hDCDesktopCom);
}


int main()
{
	std::cout << sizeof(BITMAP) << std::endl;
	initShot();
	shot();
}