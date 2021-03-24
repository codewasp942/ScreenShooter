#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <windows.h>
#include <ctime>
#include <direct.h>	//only for Visual C++ , you can replace it by system("mkdir ...");
#include <string>

struct ShotArg
{
	bool		isShot=1;
	int			MaxShotCount=10000;
	HDC			hScrDC, hComDC;
	int			High, Wide;
	HBITMAP		hComBmp;
	BITMAP		Bmp;
	DWORD		SizeBmp;
	HGLOBAL		hData;
	LPSTR		lpData;
	BITMAPFILEHEADER	BmpFileHead;
	BITMAPINFOHEADER	BmpInfoHead;
	char lpFileName[100], lpDateFix[30];
};

int initShot(ShotArg& ShotArgs) {

	ShotArgs.isShot = 1;
	ShotArgs.hScrDC = CreateDC(TEXT("DISPLAY"), 0, 0, 0);
	ShotArgs.hComDC = CreateCompatibleDC(ShotArgs.hScrDC);
	ShotArgs.Wide = GetDeviceCaps(ShotArgs.hScrDC, HORZRES);
	ShotArgs.High = GetDeviceCaps(ShotArgs.hScrDC, VERTRES);
	ShotArgs.hComBmp = CreateCompatibleBitmap(ShotArgs.hScrDC, ShotArgs.Wide, ShotArgs.High);
	SelectObject(ShotArgs.hComDC, ShotArgs.hComBmp);

	ShotArgs.BmpFileHead.bfType = MAKEWORD('B', 'M');
	ShotArgs.BmpFileHead.bfReserved1 = 0;
	ShotArgs.BmpFileHead.bfReserved2 = 0;

	ShotArgs.BmpInfoHead.biPlanes = 1;
	ShotArgs.BmpInfoHead.biCompression = 0;
	ShotArgs.BmpInfoHead.biSizeImage = 0;
	ShotArgs.BmpInfoHead.biXPelsPerMeter = 0;
	ShotArgs.BmpInfoHead.biYPelsPerMeter = 0;
	ShotArgs.BmpInfoHead.biClrUsed = 0;
	ShotArgs.BmpInfoHead.biClrImportant = 0;
	ShotArgs.BmpInfoHead.biSize = sizeof(BITMAPINFOHEADER);

	BitBlt(ShotArgs.hComDC, 0, 0, ShotArgs.Wide, ShotArgs.High, ShotArgs.hScrDC, 0, 0, SRCCOPY);
	GetObject(ShotArgs.hComBmp, sizeof(ShotArgs.Bmp), &ShotArgs.Bmp);
	ShotArgs.SizeBmp = ShotArgs.Bmp.bmWidthBytes * ShotArgs.Bmp.bmHeight;
	ShotArgs.hData = GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, ShotArgs.SizeBmp);
	if (ShotArgs.hData == NULL)return -1;
	ShotArgs.lpData = (LPSTR)GlobalLock(ShotArgs.hData);

	time_t SecTime;
	tm localTime;

	SecTime = time(NULL);
	localtime_s(&localTime, &SecTime);

	sprintf_s(ShotArgs.lpDateFix, "%d_%d_%d", localTime.tm_year + 1900,
		localTime.tm_mon, localTime.tm_mday);
	_mkdir((const char*)ShotArgs.lpDateFix);
	_chdir((const char*)ShotArgs.lpDateFix);
	sprintf_s(ShotArgs.lpDateFix, "%d_%d_%d", localTime.tm_hour,
		localTime.tm_min, localTime.tm_sec);
	_mkdir((const char*)ShotArgs.lpDateFix);
	_chdir((const char*)ShotArgs.lpDateFix);

	return 0;
}

int ShotScr(ShotArg ShotArgs,int id) {

	memset(ShotArgs.lpFileName, '\0', sizeof(ShotArgs.lpFileName));
	sprintf_s(ShotArgs.lpFileName, "%d_shot.bmp", id);

	HANDLE 	hFileBmp;
	DWORD	WriteNum;
	if ((hFileBmp = CreateFileA(ShotArgs.lpFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE) {
		std::cout << "ERR!" << std::endl;
		return -1;
	}

	BitBlt(ShotArgs.hComDC, 0, 0, ShotArgs.Wide, ShotArgs.High, ShotArgs.hScrDC, 0, 0, SRCCOPY);
	GetObject(ShotArgs.hComBmp, sizeof(ShotArgs.Bmp), &ShotArgs.Bmp);

	ShotArgs.BmpFileHead.bfSize = sizeof(ShotArgs.BmpFileHead) + ShotArgs.SizeBmp;
	ShotArgs.BmpFileHead.bfOffBits = sizeof(ShotArgs.BmpFileHead);

	ShotArgs.BmpInfoHead.biWidth = ShotArgs.Bmp.bmWidth;
	ShotArgs.BmpInfoHead.biHeight = ShotArgs.Bmp.bmHeight;
	ShotArgs.BmpInfoHead.biBitCount = ShotArgs.Bmp.bmBitsPixel;

	// GetDIBits(hScrDc,hComBmp,0,BmpInfoHead.biHeight,lpData,(BITMAPINFO*)&BmpInfoHead,DIB_RGB_COLORS);
	GetDIBits(ShotArgs.hScrDC, ShotArgs.hComBmp, 0, ShotArgs.Bmp.bmHeight, (LPVOID)ShotArgs.lpData, (BITMAPINFO*)&ShotArgs.BmpInfoHead, DIB_RGB_COLORS);

	WriteFile(hFileBmp, &ShotArgs.BmpFileHead, sizeof(ShotArgs.BmpFileHead), &WriteNum, 0);
	WriteFile(hFileBmp, &ShotArgs.BmpInfoHead, sizeof(ShotArgs.BmpInfoHead), &WriteNum, 0);
	WriteFile(hFileBmp, ShotArgs.lpData, ShotArgs.SizeBmp, &WriteNum, 0);

	CloseHandle(hFileBmp);

	return 0;
}

DWORD Ctrl(LPVOID args) {
	ShotArg* pArg = (ShotArg*)args;
	std::string Cmd;
	while (1) {
		system("cls");
		puts("--- ScreenShooter ---");
		puts("enter end to quit.");
		printf(":");
		std::cin >> Cmd;
		if (Cmd == "end") {
			pArg->isShot = 0;
		}
	}
	pArg->isShot = 0;
	return 0;
}

int main(int argc, const char** argv) {

	ShotArg ShtArg;
	initShot(ShtArg);
	DWORD idp;
	HANDLE hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Ctrl, (LPVOID)&ShtArg, 0, &idp);
	while (!ShtArg.isShot){}
	for (int i = 1;i <= ShtArg.MaxShotCount && ShtArg.isShot;i++) {
		ShotScr(ShtArg, i);
	}
	GlobalFree(ShtArg.hData);
	CloseHandle(hThread);
	system("pause");
	return 0;
}