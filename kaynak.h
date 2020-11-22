#pragma once
#include <windows.h>

BYTE* LoadBMP(int% width, int% height, long% size, LPCTSTR bmpfile);
bool  SaveBMP(BYTE* Buffer, int width, int height, long paddedsize, LPCTSTR bmpfile);
BYTE* ConvertBMPToIntensity(BYTE* Buffer, int width, int height);
BYTE* ConvertIntensityToBMP(BYTE* Buffer, int width, int height, long% newsize);
int D2_K_Mean_Clustering(BYTE* Buffer, int Width, int Height);
BYTE* BinaryImage(BYTE* Buffer, int Width, int Height, int Threshold);
int* Equal(BYTE* buffer, int Width, int Height);
BYTE* Dilation(BYTE* buffer, int Width, int Height, BYTE* Mask, int m_w, int m_h);
BYTE* Erosion(BYTE* Buffer, int Width, int Height, BYTE* Mask, int m_w, int m_h);
BYTE* MULTID_K_Means_Clustering(BYTE* buffer, int Width, int Height, int Tnumber);
BYTE* RGBScale(BYTE* buffer, int width, int height, long newsize);
BYTE* MultD_K_Means_Clustering(BYTE* buffer, int Width, int Height, int Tnumber);
BYTE* Boundary(BYTE* Buffer, BYTE* DBuffer, int Width, int Height);