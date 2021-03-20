#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <windows.h>
#include <ctime>
#include <direct.h>	//only for Visual C++ , you can replace it by system("mkdir ...");

int main(int argc, const char** argv) {

	HDC 	hScrDc = CreateDC(TEXT("DISPLAY"), 0, 0, 0);
	HDC 	hComDC = CreateCompatibleDC(hScrDc);
	int 	Wide = GetDeviceCaps(hScrDc, HORZRES);
	int 	High = GetDeviceCaps(hScrDc, VERTRES);
	HBITMAP hComBmp = CreateCompatibleBitmap(hScrDc, Wide, High);
	SelectObject(hComDC, hComBmp);

	BITMAP 	Bmp;
	DWORD 	SizeBmp;
	HGLOBAL hData;
	LPSTR 	lpData;

	BITMAPFILEHEADER BmpFileHead;
	BITMAPINFOHEADER BmpInfoHead;

	BitBlt(hComDC, 0, 0, Wide, High, hScrDc, 0, 0, SRCCOPY);
	GetObject(hComBmp, sizeof(Bmp), &Bmp);
	SizeBmp = Bmp.bmWidthBytes * Bmp.bmHeight;
	hData = GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, SizeBmp);
	if (hData == NULL)return -1;
	lpData = (LPSTR)GlobalLock(hData);

	time_t SecTime;
	tm localTime;
	char lpFileName[100], lpDateFix[30];
	
	SecTime = time(NULL);
	localtime_s(&localTime,&SecTime);

	sprintf_s(lpDateFix, "%d_%d_%d", localTime.tm_year+1900,
		localTime.tm_mon, localTime.tm_mday);
	_mkdir((const char*)lpDateFix);
	_chdir((const char*)lpDateFix);
	sprintf_s(lpDateFix, "%d_%d_%d",localTime.tm_hour,
		localTime.tm_min, localTime.tm_sec);
	_mkdir((const char*)lpDateFix);
	_chdir((const char*)lpDateFix);
	
	for (int i = 1;i <= 100;i++) {

		//std::cout << i << std::endl;

		memset(lpFileName, '\0', sizeof(lpFileName));
		sprintf_s(lpFileName, "%d_shot.bmp", i);

		HANDLE 	hFileBmp;
		DWORD	WriteNum;
		if ((hFileBmp = CreateFileA(lpFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE) {
			std::cout << "ERR!" << std::endl;
			return -1;
		}

		BitBlt(hComDC, 0, 0, Wide, High, hScrDc, 0, 0, SRCCOPY);
		GetObject(hComBmp, sizeof(Bmp), &Bmp);

		BmpFileHead.bfType = MAKEWORD('B', 'M');
		BmpFileHead.bfReserved1 = 0;
		BmpFileHead.bfReserved2 = 0;
		BmpFileHead.bfSize = sizeof(BmpFileHead) + SizeBmp;
		BmpFileHead.bfOffBits = sizeof(BmpFileHead);

		BmpInfoHead.biSize = sizeof(BITMAPINFOHEADER);
		BmpInfoHead.biWidth = Bmp.bmWidth;
		BmpInfoHead.biHeight = Bmp.bmHeight;
		BmpInfoHead.biPlanes = 1;
		BmpInfoHead.biBitCount = Bmp.bmBitsPixel;
		BmpInfoHead.biCompression = 0;
		BmpInfoHead.biSizeImage = 0;
		BmpInfoHead.biXPelsPerMeter = 0;
		BmpInfoHead.biYPelsPerMeter = 0;
		BmpInfoHead.biClrUsed = 0;
		BmpInfoHead.biClrImportant = 0;

		// GetDIBits(hScrDc,hComBmp,0,BmpInfoHead.biHeight,lpData,(BITMAPINFO*)&BmpInfoHead,DIB_RGB_COLORS);
		GetDIBits(hScrDc, hComBmp, 0, Bmp.bmHeight, (LPVOID)lpData, (BITMAPINFO*)&BmpInfoHead, DIB_RGB_COLORS);

		WriteFile(hFileBmp, &BmpFileHead, sizeof(BmpFileHead), &WriteNum, 0);
		WriteFile(hFileBmp, &BmpInfoHead, sizeof(BmpInfoHead), &WriteNum, 0);
		WriteFile(hFileBmp, lpData, SizeBmp, &WriteNum, 0);

		CloseHandle(hFileBmp);
		//
	}

	GlobalFree(hData);

	return 0;
}