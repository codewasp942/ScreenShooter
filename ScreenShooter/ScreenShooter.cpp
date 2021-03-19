#include <iostream>
#include <fstream>
#include <windows.h>

void screen(char* fileName)
{
	HWND window = GetDesktopWindow();
	HDC _dc = GetWindowDC(window);
	HDC dc = CreateCompatibleDC(0);

	RECT re;
	GetWindowRect(window, &re);
	int w = re.right,
		h = re.bottom;
	void* buf = new char[w * h * 4];

	HBITMAP bm = CreateCompatibleBitmap(_dc, w, h);
	SelectObject(dc, bm);

	StretchBlt(dc, 0, 0, w, h, _dc, 0, 0, w, h, SRCCOPY);

	void* f = CreateFile((LPCWSTR)fileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, 0);

	GetObject(bm, 84, buf);

	tagBITMAPINFO bi;
	bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
	bi.bmiHeader.biWidth = w;
	bi.bmiHeader.biHeight = h;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biCompression = 0;
	bi.bmiHeader.biSizeImage = 0;

	CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &buf, 0, 0);
	GetDIBits(dc, bm, 0, h, buf, &bi, DIB_RGB_COLORS);

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
	DeleteDC(_dc);
	DeleteDC(dc);
}


int main()
{
	/*
    HDC scrDc = GetDC(HWND_DESKTOP);
    HDC comDc = CreateCompatibleDC(scrDc);
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);
    
    HBITMAP ScrBitmap = CreateCompatibleBitmap(comDc, width, height);

	BITMAPINFOHEADER infoHeader;
	infoHeader.biSize = sizeof(BITMAPINFOHEADER);
	infoHeader.biHeight = height;
	infoHeader.biWidth = width;
	infoHeader.biPlanes = 1;
	infoHeader.biBitCount = 16;
	infoHeader.biCompression = BI_RGB;
	infoHeader.biSizeImage = 0;
	infoHeader.biXPelsPerMeter = 0;
	infoHeader.biYPelsPerMeter = 0;
	infoHeader.biClrUsed = 0;
	infoHeader.biClrImportant = 0;

	DWORD bmpSize = ((width * infoHeader.biBitCount + 31) / 32) * 4 * height;
	DWORD sizeFlie = bmpSize + sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);
	BITMAPFILEHEADER fileHeader;
	fileHeader.bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);
	fileHeader.bfSize = sizeFlie;
	fileHeader.bfType = 0x4D42;

	char* bitmapData = new char[bmpSize];
	char* imgName = new char[100];

	for (int i = 1;i <= 10;i++) {
		SelectObject(comDc, ScrBitmap);
		if (!BitBlt(comDc, 0, 0, width, height, scrDc, 0, 0, SRCCOPY)) {
			puts("Err:BitBlt err");
		}
		memset(bitmapData, 0, bmpSize);

		GetDIBits(comDc, ScrBitmap, 0, height, bitmapData, (BITMAPINFO*)&infoHeader, DIB_RGB_COLORS);
		
		memset(imgName, 0, sizeof(imgName));
		sprintf_s(imgName, 100, "%d.bmp", i);
		std::ofstream fileStream;
		fileStream.open(imgName, std::ios_base::binary);
		fileStream.write((char*)&fileHeader, sizeof(BITMAPFILEHEADER));
		fileStream.write((char*)&infoHeader, sizeof(BITMAPINFOHEADER));
		fileStream.write(bitmapData, bmpSize);
		fileStream.close();

		Sleep(1000);
	}

    ReleaseDC(HWND_DESKTOP,scrDc);
    ReleaseDC(HWND_DESKTOP,comDc);
	*/
	screen((char*)"111.bmp");
}