#include<Windows.h>
#include<iostream>
#include <windows.h>
#include <iostream>
#include<atlstr.h>
#include<time.h>
#include<stdlib.h>
#include<vector>
#include<math.h>
#include "kaynak.h"

using namespace std;

BYTE* LoadBMP(int% width, int% height, long% size, LPCTSTR bmpfile)
{
	// declare bitmap structures
	BITMAPFILEHEADER bmpheader;
	BITMAPINFOHEADER bmpinfo;
	// value to be used in ReadFile funcs
	DWORD bytesread;
	// open file to read from
	HANDLE file = CreateFile(bmpfile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (NULL == file)
		return NULL; // coudn't open file

					 // read file header
	if (ReadFile(file, &bmpheader, sizeof(BITMAPFILEHEADER), &bytesread, NULL) == false) {
		CloseHandle(file);
		return NULL;
	}
	//read bitmap info
	if (ReadFile(file, &bmpinfo, sizeof(BITMAPINFOHEADER), &bytesread, NULL) == false) {
		CloseHandle(file);
		return NULL;
	}
	// check if file is actually a bmp
	if (bmpheader.bfType != 'MB') {
		CloseHandle(file);
		return NULL;
	}
	// get image measurements
	width = bmpinfo.biWidth;
	height = abs(bmpinfo.biHeight);

	// check if bmp is uncompressed
	if (bmpinfo.biCompression != BI_RGB) {
		CloseHandle(file);
		return NULL;
	}
	// check if we have 24 bit bmp
	if (bmpinfo.biBitCount != 24) {
		CloseHandle(file);
		return NULL;
	}

	// create buffer to hold the data
	size = bmpheader.bfSize - bmpheader.bfOffBits;
	BYTE* Buffer = new BYTE[size];
	// move file pointer to start of bitmap data
	SetFilePointer(file, bmpheader.bfOffBits, NULL, FILE_BEGIN);
	// read bmp data
	if (ReadFile(file, Buffer, size, &bytesread, NULL) == false) {
		delete[] Buffer;
		CloseHandle(file);
		return NULL;
	}
	// everything successful here: close file and return buffer
	CloseHandle(file);

	return Buffer;
}//LOADPMB

bool SaveBMP(BYTE* Buffer, int width, int height, long paddedsize, LPCTSTR bmpfile)
{
	// declare bmp structures 
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER info;

	// andinitialize them to zero
	memset(&bmfh, 0, sizeof(BITMAPFILEHEADER));
	memset(&info, 0, sizeof(BITMAPINFOHEADER));

	// fill the fileheader with data
	bmfh.bfType = 0x4d42;       // 0x4d42 = 'BM'
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;
	bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + paddedsize;
	bmfh.bfOffBits = 0x36;		// number of bytes to start of bitmap bits

								// fill the infoheader

	info.biSize = sizeof(BITMAPINFOHEADER);
	info.biWidth = width;
	info.biHeight = height;
	info.biPlanes = 1;			// we only have one bitplane
	info.biBitCount = 24;		// RGB mode is 24 bits
	info.biCompression = BI_RGB;
	info.biSizeImage = 0;		// can be 0 for 24 bit images
	info.biXPelsPerMeter = 0x0ec4;     // paint and PSP use this values
	info.biYPelsPerMeter = 0x0ec4;
	info.biClrUsed = 0;			// we are in RGB mode and have no palette
	info.biClrImportant = 0;    // all colors are important

								// now we open the file to write to
	HANDLE file = CreateFile(bmpfile, GENERIC_WRITE, FILE_SHARE_READ,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == NULL) {
		CloseHandle(file);
		return false;
	}
	// write file header
	unsigned long bwritten;
	if (WriteFile(file, &bmfh, sizeof(BITMAPFILEHEADER), &bwritten, NULL) == false) {
		CloseHandle(file);
		return false;
	}
	// write infoheader
	if (WriteFile(file, &info, sizeof(BITMAPINFOHEADER), &bwritten, NULL) == false) {
		CloseHandle(file);
		return false;
	}
	// write image data
	if (WriteFile(file, Buffer, paddedsize, &bwritten, NULL) == false) {
		CloseHandle(file);
		return false;
	}

	// and close file
	CloseHandle(file);

	return true;
} // SaveBMP

BYTE* ConvertBMPToIntensity(BYTE* Buffer, int width, int height)
{
	// first make sure the parameters are valid
	if ((NULL == Buffer) || (width == 0) || (height == 0))
		return NULL;

	// find the number of padding bytes

	int padding = 0;
	int scanlinebytes = width * 3;
	while ((scanlinebytes + padding) % 4 != 0)     // DWORD = 4 bytes
		padding++;
	// get the padded scanline width
	int psw = scanlinebytes + padding;

	// create new buffer
	BYTE* newbuf = new BYTE[width * height];

	// now we loop trough all bytes of the original buffer, 
	// swap the R and B bytes and the scanlines
	long bufpos = 0;
	long newpos = 0;
	for (int row = 0; row < height; row++)
		for (int column = 0; column < width; column++) {
			newpos = row * width + column;
			bufpos = (height - row - 1) * psw + column * 3;
			newbuf[newpos] = BYTE(0.11 * Buffer[bufpos + 2] + 0.59 * Buffer[bufpos + 1] + 0.3 * Buffer[bufpos]);
		}

	return newbuf;
}//ConvetBMPToIntensity

BYTE* ConvertIntensityToBMP(BYTE* Buffer, int width, int height, long% newsize)
{
	// first make sure the parameters are valid
	if ((NULL == Buffer) || (width == 0) || (height == 0))
		return NULL;

	// now we have to find with how many bytes
	// we have to pad for the next DWORD boundary	

	int padding = 0;
	int scanlinebytes = width * 3;
	while ((scanlinebytes + padding) % 4 != 0)     // DWORD = 4 bytes
		padding++;
	// get the padded scanline width
	int psw = scanlinebytes + padding;
	// we can already store the size of the new padded buffer
	newsize = height * psw;

	// and create new buffer
	BYTE* newbuf = new BYTE[newsize];

	// fill the buffer with zero bytes then we dont have to add
	// extra padding zero bytes later on
	memset(newbuf, 0, newsize);
	// now we loop trough all bytes of the original buffer, 
	// swap the R and B bytes and the scanlines
	long bufpos = 0;
	long newpos = 0;
	for (int row = 0; row < height; row++)
		for (int column = 0; column < width; column++) {
			bufpos = row * width + column;     // position in original buffer
			newpos = (height - row - 1) * psw + column * 3;           // position in padded buffer
			newbuf[newpos] = Buffer[bufpos];       //  blue
			newbuf[newpos + 1] = Buffer[bufpos];   //  green
			newbuf[newpos + 2] = Buffer[bufpos];   //  red
		}

	return newbuf;
} //ConvertIntensityToBMP

int D2_K_Mean_Clustering(BYTE* Buffer, int Width, int Height)//hocanýn istediði
{
	int T1, T2;
	int T1Complement, T2Complement, Mean;
	FLOAT  SumT1 = 0.0f, SumT2 = 0.0f, sumindisT1 = 0.0f, sumindisT2 = 0.0f;
	int* Hist = new int[256];
	for (int i = 0; i < 256; i++)
	{
		Hist[i] = 0;
	}
	for (int i = 0; i < Width * Height; i++)
	{
		Hist[(int)Buffer[i]]++;
	}


	srand(time(NULL));
	T1 = rand() % 256;
	T2 = rand() % 256;
	if (T1 == T2)
	{
		T1 = rand() % 256;
		T2 = rand() % 256;
	}

	while (true)
	{

		Mean = (T1 + T2) / 2;
		for (int i = 0; i < Mean; i++)
		{
			SumT1 += i * Hist[i];
			sumindisT1 += Hist[i];
		}
		T1Complement = SumT1 / sumindisT1;
		for (int j = Mean; j < 256; j++)
		{
			SumT2 += j * Hist[j];
			sumindisT2 += Hist[j];
		}
		T2Complement = SumT2 / sumindisT2;
		if (T1 == T1Complement && T2 == T2Complement)
		{
			return Mean;
			break;
		}
		else
		{
			Mean = 0;
			sumindisT1 = 0.0f;
			sumindisT2 = 0.0f;
			SumT1 = 0.0f;
			SumT2 = 0.0f;
			T1 = T1Complement;
			T2 = T2Complement;
			T2Complement = 0.0f;
			T1Complement = 0.0f;
		}

	}

}

BYTE* BinaryImage(BYTE* Buffer, int Width, int Height, int Threshold)
{

	for (int j = 0; j < Width * Height; j++)
	{
		if (int(Buffer[j] < Threshold))
		{
			Buffer[j] = 0;
		}
		if (int(Buffer[j] >= Threshold))
		{
			Buffer[j] = 255;
		}
	}
	return Buffer;//binary*/


}

int* Equal(BYTE* buffer, int Width, int Height)
{
	int max_Brightness, temp;
	int index;
	int* equal_Hist = new int[256];
	//int* equal_Hist = new int[256];
	
	//renk deðerleri sayýlýr
	int deger = 0;
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j < Width * Height; j++) {
			if (i == buffer[j]) {
				deger++;
			}
		}
		equal_Hist[i] = deger;
		deger = 0;

	}



	int* cumulative_Hist = new int[256];
	
	int one = round((cumulative_Hist[0] * max_Brightness / (Width * Height)));
	equal_Hist[one] = cumulative_Hist[0];
	
	cumulative_Hist[one] = buffer[0];

	
	equal_Hist[one] = cumulative_Hist[0];

	for (int i = 1; i < 256; i++)
	{
		cumulative_Hist[i] = buffer[i] + cumulative_Hist[i - 1];
	}

	for (int j = 1; j < 256; j++)
	{
		temp = round(cumulative_Hist[j] * max_Brightness / (Width * Height));
		equal_Hist[temp] += (cumulative_Hist[j] - cumulative_Hist[j - 1]);
	}

	return equal_Hist;
}

BYTE* Dilation(BYTE* buffer, int Width, int Height, BYTE* Mask, int m_w, int m_h)
{
	int maskpw = (m_w - (m_w + 1 / 2)) * 2;
	int maskph = (m_h - (m_h + 1 / 2)) * 2;
	int w_new = Width + maskpw;
	int h_new = Height + maskph;

	BYTE* genisBuffer= new BYTE[w_new*h_new];
	BYTE* newgenisBuffer= new BYTE[w_new*h_new];

	
	
	// mantýksal iþlem için dönüþüm
	int temp;
	for (int row = 0; row < Height; row++)
	{
		for (int column = 0; column < Width; column++)
		{
			temp = row * Width + column;
			if (buffer[temp] == 255) {
				buffer[temp] = 0;
			}
			else if (buffer[temp] == 0) {
				buffer[temp] = 1;
			}
		}
	}
	
	// sýfýrlama 
	for (int row = 0; row < h_new; row++)
	{
		for (int column = 0; column < w_new; column++)
		{
			genisBuffer[row * w_new +column] = 1;
		}
	}

	//geniþleilmiþ matrisin içine binary resmi yereþtirme
	for (int row = maskph/2; row < Height + maskph/2; row++)
	{
		for (int column = maskpw/2; column < Width + maskpw/2; column++)
		{
			genisBuffer[row * w_new + column] = buffer[(row - maskph/2) * Width + column - maskpw/2];
		}
	}
	
	//mask gezdirerek geniþletme yapma
	//yeni buffer datutuluyor  iþlemler orjinal binary üreinden okunuyor
	for (int row = 0; row < h_new-(m_h-1); row++)
	{
		for (int column = 0; column < w_new-(m_w-1); column++)
		{
			for (int a = row; a < row + m_h; a++)
			{
				for (int b = column; b < column + m_w; b++)
				{
					if (genisBuffer[(row+1)*w_new+(column+1)]==1) {
						newgenisBuffer[a * w_new + b] = (genisBuffer[a * w_new + b] || Mask[(a - row) * m_w + (b - column)]);
					}
				}
			}
		}
	}

	
	//tekrar binary dönüþümü
	for (int row = maskph / 2; row < h_new; row++)
	{
		for (int column = maskpw / 2; column < w_new; column++)
		{

			temp = row * w_new + column;
			if (newgenisBuffer[temp] == 1)
			{
				buffer[(row - maskph/2) * Width + column - maskpw/2] = 0;
			}
			else if (newgenisBuffer[temp] == 0)
			{
				buffer[(row - maskph/2) * Width + column - maskpw/2] = 255;
			}

		}
	}

	return buffer;

}

BYTE* Erosion(BYTE* Buffer, int Width, int Height, BYTE* Mask, int m_w, int m_h)
{
	int maskpw = (m_w - (m_w + 1 / 2)) * 2;
	int maskph = (m_h - (m_h + 1 / 2)) * 2;
	int w_new = Width + maskpw;
	int h_new = Height + maskph;

	BYTE* genisBuffer = new BYTE[w_new * h_new];
	BYTE* newgenisBuffer = new BYTE[w_new * h_new];



	// mantýksal iþlem için dönüþüm
	int temp;
	for (int row = 0; row < Height; row++)
	{
		for (int column = 0; column < Width; column++)
		{
			temp = row * Width + column;
			if (Buffer[temp] == 255) {
				Buffer[temp] = 0;
			}
			else if (Buffer[temp] == 0) {
				Buffer[temp] = 1;
			}
		}
	}

	// sýfýrlama 
	for (int row = 0; row < h_new; row++)
	{
		for (int column = 0; column < w_new; column++)
		{
			genisBuffer[row * w_new + column] = 1;
		}
	}

	//geniþleilmiþ matrisin içine binary resmi yereþtirme
	for (int row = maskph / 2; row < Height + maskph / 2; row++)
	{
		for (int column = maskpw / 2; column < Width + maskpw / 2; column++)
		{
			genisBuffer[row * w_new + column] = Buffer[(row - maskph / 2) * Width + column - maskpw / 2];
		}
	}

	//mask gezdirerek geniþletme yapma
	//yeni buffer datutuluyor  iþlemler orjinal binary üreinden okunuyor
	//erosion iþleminde 1 sayýsý fazla oldugunda mask bozulmaya neden olacagý için eþik deðeri tutulur

	int onecount = 0;

	for (int row = 0; row < h_new - (m_h - 1); row++)
	{
		for (int column = 0; column < w_new - (m_w - 1); column++)
		{
			for (int a = row; a < row + m_h; a++)
			{
				for (int b = column; b < column + m_w; b++)
				{
					if (genisBuffer[a*w_new+b] == 1  && Mask[(a-row)*m_w+(b-column)]) {
						onecount++;
					}
				}
			}


			if (onecount >= 5) {
				//newgenisBuffer[a * w_new + b] = (genisBuffer[a * w_new + b] && Mask[(a - row) * m_w + (b - column)]);
				newgenisBuffer[row * w_new + column] = 1;
				onecount = 0;
			}
			else {
				newgenisBuffer[row * w_new + column] = 0;
				onecount = 0;
			}

		}
	}


	//tekrar binary dönüþümü
	for (int row = maskph / 2; row < h_new; row++)
	{
		for (int column = maskpw / 2; column < w_new; column++)
		{

			temp = row * w_new + column;
			if (newgenisBuffer[temp] == 1)
			{
				Buffer[(row - maskph / 2) * Width + column - maskpw / 2] = 0;
			}
			else if (newgenisBuffer[temp] == 0)
			{
				Buffer[(row - maskph / 2) * Width + column - maskpw / 2] = 255;
			}

		}
	}

	return Buffer;
}

BYTE* MULTID_K_Means_Clustering(BYTE* buffer, int Width, int Height, int Tnumber)
{
	int* TComparray = new int[Tnumber];
	int temp, tp, control = 0;//tp t1 2 3 degerimi onu tutuyor
	FLOAT MinT = 0.0f;
	int* Thresholdarraynumber = new int[Tnumber];
	FLOAT* Means = new FLOAT[Tnumber];
	int* TArrays = new int[Tnumber];
	BYTE* tempbuf = new BYTE[Width * Height];
	int* SumT = new int[Tnumber];


	for (int i = 0; i < Tnumber; i++)//dizi 0 lama
	{
		Thresholdarraynumber[i] = 0;
		Means[i] = 0;
		TComparray[i] = 0;
		SumT[i] = 0;
		TArrays[i] = 0;
	}
	srand(time(NULL));
	for (int i = 0; i < Tnumber; i++)//sayi üretilmesi
	{
		TArrays[i] = rand() % 256;
		//TArrays[i] = 256/(i+1);
	}

	while (true)
	{
		for (int row = 0; row < Height; row++)
		{
			for (int column = 0; column < Width; column++)
			{
				temp = row * Width + column;
				for (int j = 0; j < Tnumber; j++)
				{
					Means[j] = sqrt(pow((buffer[temp] - TArrays[j]), 2));
				}
				MinT = Means[0];
				tp = 0;
				for (int j = 1; j < Tnumber; j++)//en kucuk uzakligin bulunmasi
				{
					if (Means[j] < MinT)
					{
						MinT = Means[j];
						tp = j;
					}

				}
				tempbuf[temp] = tp;
			}
		}

		for (int i = 0; i < Width * Height; i++)
		{
			Thresholdarraynumber[tempbuf[i]]++;//pixel sayýsý etiket toplam
			for (int j = 0; j < Tnumber; j++)
			{
				if (tempbuf[i] == j)
				{
					SumT[j] += buffer[i];
				}
			}
		}
		for (int j = 0; j < Tnumber; j++)
		{
			if (Thresholdarraynumber[j] == 0)
			{
				TComparray[j] = SumT[j] / (Thresholdarraynumber[j] + 1);
			}
			else
			{
				TComparray[j] = SumT[j] / Thresholdarraynumber[j];
			}

		}

		for (int i = 0; i < Tnumber; i++)
		{
			if (TArrays[i] == TComparray[i])
			{
				control++;
			}
		}
		if (control <= Tnumber)
		{
			for (int row = 0; row < Height; row++)
			{
				for (int column = 0; column < Width; column++)
				{
					temp = row * Width + column;
					buffer[temp] = TArrays[tempbuf[temp]];
				}
			}
			break;
		}
		else
		{
			for (int i = 0; i < Tnumber; i++)
			{
				TArrays[i] = TComparray[i];
			}
		}
	}


	return buffer;
}

BYTE* RGBScale(BYTE* buffer, int width, int height, long newsize)
{
	if ((NULL == buffer) || (width == 0) || (height == 0))
		return NULL;

	// now we have to find with how many bytes
	// we have to pad for the next DWORD boundary	

	int padding = 0;
	int scanlinebytes = width * 3;
	while ((scanlinebytes + padding) % 4 != 0)     // DWORD = 4 bytes
		padding++;
	// get the padded scanline width
	int psw = scanlinebytes + padding;
	// we can already store the size of the new padded buffer
	newsize = height * psw;

	// and create new buffer
	BYTE* newbuf = new BYTE[newsize];

	// fill the buffer with zero bytes then we dont have to add
	// extra padding zero bytes later on
	memset(newbuf, 0, newsize);
	// now we loop trough all bytes of the original buffer, 
	// swap the R and B bytes and the scanlines
	long bufpos = 0;
	long newpos = 0;
	for (int row = 0; row < height; row++)
		for (int column = 0; column < width; column++) {
			bufpos = row * width + column;     // position in original buffer
			newpos = (height - row - 1) * psw + column * 3;           // position in padded buffer
			newbuf[newpos] = buffer[3 * bufpos];       //  blue
			newbuf[newpos + 1] = buffer[3 * bufpos + 1];   //  green
			newbuf[newpos + 2] = buffer[3 * bufpos + 2];   //  red
		}

	return newbuf;
}

BYTE* MultD_K_Means_Clustering(BYTE* buffer, int Width, int Height, int Tnumber)
{
	int* TComparray = new int[Tnumber * 3];
	int temp, tp, control = 0;//tp t1 2 3 degerimi onu tutuyor
	FLOAT MinT = 0.0f;
	int* Thresholdarraynumber = new int[Tnumber];
	FLOAT* Means = new FLOAT[Tnumber];
	int* TArrays = new int[Tnumber * 3];
	BYTE* tempbuf = new BYTE[Width * Height];
	int* SumT = new int[Tnumber * 3];

	for (int i = 0; i < Tnumber; i++)
	{
		Means[i] = 0.0f;
		SumT[3 * i] = 0;
		SumT[3 * i + 1] = 0;
		SumT[3 * i + 2] = 0;
		TComparray[3 * i] = 0;
		TComparray[3 * i + 1] = 0;
		TComparray[3 * i + 2] = 0;
		Thresholdarraynumber[i] = 0;
		TArrays[3 * i] = 0;
		TArrays[3 * i + 1] = 0;
		TArrays[3 * i + 2] = 0;

	}
	srand(time(NULL));

	for (int i = 0; i < Tnumber; i++)
	{

		TArrays[3 * i] = rand() % 256;
		TArrays[3 * i + 1] = rand() % 256;
		TArrays[3 * i + 2] = rand() % 256;
	}


	while (true)
	{
		for (int row = 0; row < Height; row++)
		{
			for (int column = 0; column < Width; column++)
			{
				temp = row * Width + column;
				for (int j = 0; j < Tnumber; j++)
				{
					Means[j] = sqrt(pow((buffer[3 * temp] - TArrays[3 * j] + buffer[3 * temp + 1] - TArrays[3 * j + 1] + buffer[3 * temp + 2] - TArrays[3 * j + 2]), 2));//xr-tr xg-tg xb-tb
				}
				MinT = Means[0];
				tp = 0;
				for (int i = 1; i < Tnumber; i++)
				{
					if (Means[i] < MinT)
					{
						MinT = Means[i];
						tp = i;
					}
				}
				tempbuf[temp] = tp;
			}
		}
		for (int row = 0; row < Height; row++)
		{
			for (int column = 0; column < Width; column++)
			{
				temp = row * Width + column;
				Thresholdarraynumber[tempbuf[temp]]++;
				for (int j = 0; j < Tnumber; j++)
				{
					if (tempbuf[temp] == j)
					{
						SumT[3 * j + 2] += buffer[3 * temp + 2];
						SumT[3 * j + 1] += buffer[3 * temp + 1];
						SumT[3 * j] += buffer[3 * temp];
					}
				}
			}
		}
		for (int j = 0; j < Tnumber; j++)
		{
			if (Thresholdarraynumber[j] == 0)
			{
				TComparray[3 * j + 2] = SumT[3 * j + 2] / (Thresholdarraynumber[j] + 1);
				TComparray[3 * j + 1] = SumT[3 * j + 1] / (Thresholdarraynumber[j] + 1);
				TComparray[3 * j] = SumT[3 * j] / (Thresholdarraynumber[j] + 1);
			}
			else
			{
				TComparray[3 * j + 2] = SumT[3 * j + 2] / Thresholdarraynumber[j];
				TComparray[3 * j + 1] = SumT[3 * j + 1] / Thresholdarraynumber[j];
				TComparray[3 * j] = SumT[3 * j] / Thresholdarraynumber[j];
			}
		}
		for (int i = 0; i < Tnumber; i++)
		{
			if (TArrays[i] == TComparray[i])
			{
				control++;
			}
		}
		if (control <= Tnumber)
		{
			for (int row = 0; row < Height; row++)
			{
				for (int column = 0; column < Width; column++)
				{
					temp = row * Width + column;
					buffer[3 * temp + 2] = TArrays[3 * tempbuf[temp] + 2];
					buffer[3 * temp + 1] = TArrays[3 * tempbuf[temp] + 1];
					buffer[3 * temp] = TArrays[3 * tempbuf[temp]];

				}
			}
			break;
		}
		else
		{
			for (int i = 0; i < Tnumber; i++)
			{
				TArrays[3 * i + 2] = TComparray[3 * i + 2];
				TArrays[3 * i + 1] = TComparray[3 * i + 1];
				TArrays[3 * i] = TComparray[3 * i];
			}
		}
	}//end loop

	return buffer;
}

BYTE* Boundary(BYTE* Buffer, BYTE* DBuffer, int Width, int Height)
{
	BYTE* newbuf = new BYTE[Width * Height];
	int tep;
	for (int row = 0; row < Height; row++)//boundary
	{
		for (int column = 0; column < Width; column++)
		{
			tep = row * Width + column;

		}
	}
	for (int row = 0; row < Height; row++)
	{
		for (int column = 0; column < Width; column++)
		{
			tep = row * Width + column;
			if (Buffer[tep] == 255)
			{
				Buffer[tep] = 1;
			}
			if (DBuffer[tep] == 255)
			{
				DBuffer[tep] = 1;
			}

		}
	}//end




	for (int row = 0; row < Height; row++)//boundary
	{
		for (int column = 0; column < Width; column++)
		{
			tep = row * Width + column;
			newbuf[tep] = (Buffer[tep] ^ DBuffer[tep]);
		}
	}

	//siyah beyaz renk atamasý
	for (int row = 0; row < Height; row++)
	{
		for (int column = 0; column < Width; column++)
		{
			tep = row * Width + column;
			if (newbuf[tep] == 1)
			{
				newbuf[tep] = 0;
			}
			else if (newbuf[tep] == 0)
			{
				newbuf[tep] = 255;
			}
		}
	}
	return newbuf;
}
