// LibImgProc.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"


#include "LibImgProc.h"

#include<iostream>
#include<fstream>

using namespace std;

void GetHist(unsigned aF[65536], unsigned short *pImg, int nW, int nH)
{
	for (int n = 0; n < 65536; n++)
	{
		aF[n] = 0;
	}

	for (int y = 0; y < nH; y++)
	{
		for (int x = 0; x < nW; x++)
		{
			aF[pImg[y*nW + x]]++;
		}
	}

	return;
}


void GetHist(unsigned aF[256], unsigned char *pImg, int nW, int nP, int nH)
{
	for (int n = 0; n < 256; n++)
	{
		aF[n] = 0;
	}

	for (int y = 0; y < nH; y++)
	{
		for (int x = 0; x < nW; x++)
		{
			aF[pImg[y*nP + x]]++;
		}
	}

	return;
}


//RAW转灰度图像
bool Convert(unsigned char *pDst, unsigned short *pSrc, int nW, int nP, int nH, long &maxIntensity, long &minIntensity)
{
	if (NULL == pDst || NULL == pSrc || nW < 0 || nP < 0 || nH < 0)
		return false;

	int i;

	maxIntensity = pSrc[0];
	minIntensity = pSrc[0];
	int N = nH * nW;
	for (i = 0; i < N; i++)
	{
		if (pSrc[i] > maxIntensity)
			maxIntensity = pSrc[i];
		if (pSrc[i] < minIntensity)
			minIntensity = pSrc[i];
	}

	if (maxIntensity == minIntensity)
	{
		memset(pDst, 255, nH*nP * sizeof(unsigned char));
	}
	else
	{
		const double dRatio = 255.0 / (maxIntensity - minIntensity);

		for (int y = 0; y < nH; y++)
			for (int x = 0; x < nW; x++)
			{
				unsigned char g = (unsigned char)((pSrc[y*nW + x] - minIntensity)*dRatio);
				//unsigned char g = (unsigned char)((pSrc[y*nW+x]-minIntensity)*255/(maxIntensity-minIntensity)); 
				pDst[y*nP + x] = g;
				if (g != 0)
				{
					int dbg = 10;
				}
			}

		unsigned aFS[65536];
		GetHist(aFS, pSrc, nW, nH);
		ofstream ofs("shortFreq.txt");
		if (ofs)
		{
			for (int n = 0; n < 65536; n++)
			{
				if (aFS[n] > 0)
					ofs << n << "\t" << aFS[n] << endl;
			}
		}

		unsigned aFC[256];
		GetHist(aFC, pDst, nW, nP, nH);
		ofstream ofs1("ucFreq.txt");
		if (ofs1)
		{
			for (int n = 0; n < 255; n++)
			{
				if (aFC[n] > 0)
					ofs1 << n << "\t" << aFC[n] << endl;
			}
		}

	}

	return true;
}

bool Convert(unsigned short *pDst, unsigned char *pSrc, int nW, int nP, int nH)
{
	const double dRatio = 65535.0 / 255;

	for (int y = 0; y < nH; y++)
		for (int x = 0; x < nW; x++)
		{
			pDst[y*nW + x] = (unsigned short)(pSrc[y*nP + x] * dRatio);
		}

	return true;
}


void GetKernel(int pKernel[9], EEmbOp eop)
{
	switch (eop)
	{
		//梯度检测, 南（垂直）
	case EMB_IMAGE_GRADIENT_SOUTH_DETECT:
	{
		pKernel[0] = pKernel[2] = -1;  pKernel[1] = -2;
		pKernel[3] = pKernel[4] = pKernel[5] = 0;
		pKernel[6] = pKernel[8] = 1;   pKernel[7] = 2;
		break;
	}
	//梯度检测, 东(水平)
	case EMB_IMAGE_GRADIENT_EAST_DETECT:
	{
		pKernel[0] = pKernel[6] = -1;  pKernel[3] = -2;
		pKernel[1] = pKernel[4] = pKernel[7] = 0;
		pKernel[2] = pKernel[8] = 1;   pKernel[5] = 2;
		break;
	}
	//梯度检测, 东北-西南
	case EMB_IMAGE_GRADIENT_NORTHEAST_DETECT:
	{
		pKernel[1] = pKernel[5] = 1;    pKernel[2] = 2;
		pKernel[0] = pKernel[4] = pKernel[8] = 0;
		pKernel[3] = pKernel[7] = -1;   pKernel[6] = -2;
		break;
	}
	//梯度检测, 西北-东南
	case EMB_IMAGE_GRADIENT_SOUTHEAST_DETECT:
	{
		pKernel[0] = -2;  pKernel[1] = pKernel[3] = -1;
		pKernel[2] = pKernel[4] = pKernel[6] = 0;
		pKernel[5] = pKernel[7] = 1;   pKernel[8] = 2;
		break;
	}


	//平移和差分边缘增强, 东（水平）
	case EMB_IMAGE_DIFFERENCE_EAST_DETECT:
	{
		pKernel[0] = pKernel[1] = pKernel[2] = 0;
		pKernel[3] = -1; pKernel[4] = 1; pKernel[5] = 0;
		pKernel[6] = pKernel[7] = pKernel[8] = 0;
		break;
	}
	//平移和差分边缘增强, 南（垂直）
	case EMB_IMAGE_DIFFERENCE_SOUTH_DETECT:
	{
		pKernel[0] = pKernel[2] = 0; pKernel[1] = -1;
		pKernel[3] = 0; pKernel[4] = 1; pKernel[5] = 0;
		pKernel[6] = pKernel[7] = pKernel[8] = 0;
		break;
	}
	//平移和差分边缘增强, 东北-西南
	case EMB_IMAGE_DIFFERENCE_NORTHEAST_DETECT:
	{
		pKernel[0] = pKernel[1] = pKernel[2] = 0;
		pKernel[3] = 0; pKernel[4] = 1; pKernel[5] = 0;
		pKernel[6] = -1; pKernel[7] = pKernel[8] = 0;
		break;
	}
	//平移和差分边缘增强, 西北-东南
	case EMB_IMAGE_DIFFERENCE_SOUTHEAST_DETECT:
	{
		pKernel[0] = -1; pKernel[1] = pKernel[2] = 0;
		pKernel[3] = 0; pKernel[4] = 1; pKernel[5] = 0;
		pKernel[6] = pKernel[7] = pKernel[8] = 0;
		break;
	}


	//Prewitt边缘增强, 东（水平）
	case EMB_IMAGE_PREWITT_EAST_DETECT:
	{
		pKernel[0] = -1; pKernel[1] = pKernel[2] = 1;
		pKernel[3] = -1; pKernel[4] = -2; pKernel[5] = 1;
		pKernel[6] = -1; pKernel[7] = pKernel[8] = 1;
		break;
	}
	//Prewitt边缘增强, 南（垂直）
	case EMB_IMAGE_PREWITT_SOUTH_DETECT:
	{
		pKernel[0] = pKernel[1] = pKernel[2] = -1;
		pKernel[3] = 1; pKernel[4] = -2; pKernel[5] = 1;
		pKernel[6] = pKernel[7] = pKernel[8] = 1;
		break;
	}
	//Prewitt边缘增强, 东北-西南
	case EMB_IMAGE_PREWITT_NORTHEAST_DETECT:
	{
		pKernel[0] = pKernel[1] = pKernel[2] = 1;
		pKernel[3] = -1; pKernel[4] = -2; pKernel[5] = 1;
		pKernel[6] = pKernel[7] = -1; pKernel[8] = 1;
		break;
	}
	//Prewitt边缘增强, 西北-东南
	case EMB_IMAGE_PREWITT_SOUTHEAST_DETECT:
	{
		pKernel[0] = pKernel[1] = -1; pKernel[2] = 1;
		pKernel[3] = -1; pKernel[4] = -2; pKernel[5] = 1;
		pKernel[6] = pKernel[7] = pKernel[8] = 1;
		break;
	}

	default:
		break;
	}

	return;
}

bool Emboss(unsigned short *pData, int nRow, int nCol, EEmbOp eop)
{
	unsigned short *pTmp = new unsigned short[nRow*nCol];
	memcpy(pTmp, pData, nRow*nCol * sizeof(unsigned short));

	int pKernel[9];
	GetKernel(pKernel, eop);

	int nWeightSum = 0;
	for (int k = 0; k < 9; k++)
	{
		nWeightSum += pKernel[k];
	}

	if (nWeightSum == 0 || nWeightSum == 1)
	{
		for (int r = 1; r < nRow - 1; r++)
			for (int c = 1; c < nCol - 1; c++)
			{
				int pos = r * nCol + c;

				int sum = pKernel[0] * pTmp[pos - nCol - 1] + pKernel[1] * pTmp[pos - nCol] + pKernel[2] * pTmp[pos - nCol + 1] +
					pKernel[3] * pTmp[pos - 1] + pKernel[4] * pTmp[pos] + pKernel[5] * pTmp[pos + 1] +
					pKernel[6] * pTmp[pos + nCol - 1] + pKernel[7] * pTmp[pos + nCol] + pKernel[8] * pTmp[pos + nCol + 1] + 32768;

				pData[pos] = sum < 0 ? 0 : (sum > 65535 ? 65535 : sum);
			}
	}
	else
	{
		for (int r = 1; r < nRow - 1; r++)
			for (int c = 1; c < nCol - 1; c++)
			{
				int pos = r * nCol + c;

				int sum = pKernel[0] * pTmp[pos - nCol - 1] + pKernel[1] * pTmp[pos - nCol] + pKernel[2] * pTmp[pos - nCol + 1] +
					pKernel[3] * pTmp[pos - 1] + pKernel[4] * pTmp[pos] + pKernel[5] * pTmp[pos + 1] +
					pKernel[6] * pTmp[pos + nCol - 1] + pKernel[7] * pTmp[pos + nCol] + pKernel[8] * pTmp[pos + nCol + 1] + 32768;

				sum /= nWeightSum;

				pData[pos] = sum < 0 ? 0 : (sum > 65535 ? 65535 : sum);
			}
	}

	delete[]pTmp;

	return true;
}

bool Emboss(unsigned char *pData, int nRow, int nCol, EEmbOp eop)
{
	unsigned char *pTmp = new unsigned char[nRow*nCol];
	memcpy(pTmp, pData, nRow*nCol * sizeof(unsigned char));

	int pKernel[9];
	GetKernel(pKernel, eop);

	int nWeightSum = 0;
	for (int k = 0; k < 9; k++)
	{
		nWeightSum += pKernel[k];
	}

	if (nWeightSum == 0 || nWeightSum == 1)
	{
		for (int r = 1; r < nRow - 1; r++)
			for (int c = 1; c < nCol - 1; c++)
			{
				int pos = r * nCol + c;

				int sum = pKernel[0] * pTmp[pos - nCol - 1] + pKernel[1] * pTmp[pos - nCol] + pKernel[2] * pTmp[pos - nCol + 1] +
					pKernel[3] * pTmp[pos - 1] + pKernel[4] * pTmp[pos] + pKernel[5] * pTmp[pos + 1] +
					pKernel[6] * pTmp[pos + nCol - 1] + pKernel[7] * pTmp[pos + nCol] + pKernel[8] * pTmp[pos + nCol + 1];

				pData[pos] = sum < 0 ? 0 : (sum > 255 ? 255 : sum);
			}
	}
	else
	{
		for (int r = 1; r < nRow - 1; r++)
			for (int c = 1; c < nCol - 1; c++)
			{
				int pos = r * nCol + c;

				int sum = pKernel[0] * pTmp[pos - nCol - 1] + pKernel[1] * pTmp[pos - nCol] + pKernel[2] * pTmp[pos - nCol + 1] +
					pKernel[3] * pTmp[pos - 1] + pKernel[4] * pTmp[pos] + pKernel[5] * pTmp[pos + 1] +
					pKernel[6] * pTmp[pos + nCol - 1] + pKernel[7] * pTmp[pos + nCol] + pKernel[8] * pTmp[pos + nCol + 1];

				sum /= nWeightSum;

				pData[pos] = sum < 0 ? 0 : (sum > 255 ? 255 : sum);
			}
	}

	delete[]pTmp;

	return true;
}


template<class T>
T compare(const void *a, const void *b)
{
	return *(T*)a - *(T*)b;
}

template<class T>
T Median(T *pArray, int begin, int end)
{
	T *pA = new T[end - begin + 1];
	memcpy(pA, pArray + begin, (end - begin + 1) * sizeof(T));
	qsort(pA, (end - begin + 1), sizeof(T), compare);
	T m = *(pA + (end - begin + 1) / 2);
	delete[]pA;

	return m;
}

bool MedianFiltering(unsigned short *pImg, int nW, int nH, int nFilterRadius)
{
	if (NULL == pImg || nW < 0 || nH < 0 || nFilterRadius < 0)
		return false;

	if (0 == nFilterRadius)
		return true;

	//备份源图像
	unsigned short *pBuf = new unsigned short[nW*nH];
	memcpy(pBuf, pImg, nW*nH * sizeof(unsigned short));

	//将屏幕坐标转换为图像坐标（原点在左下角），得到图像矩形
	int xmin = 0;
	int xmax = nW - 1;
	int ymin = 0;
	int ymax = nH - 1;

	//对图像中的矩形区域进行中值滤波
	for (int y = ymin; y < ymax; y++)
		for (int x = xmin; x < xmax; x++)
		{
			int ys = 0 > (y - nFilterRadius) ? 0 : (y - nFilterRadius);
			int ye = nH < (y + nFilterRadius) ? nH : (y + nFilterRadius);
			int xs = 0 > (x - nFilterRadius) ? 0 : (x - nFilterRadius);
			int xe = nW < (x + nFilterRadius) ? nW : (x + nFilterRadius);

			int w = xe - xs;
			int h = ye - ys;
			int *pImg = new int[(xe - xs)*(ye - ys)];
			for (int i = ys; i < ye; i++)
				for (int j = xs; j < xe; j++)
					pImg[(i - ys)*w + (j - xs)] = pImg[i*nW + j];

			pBuf[y*nW + x] = (unsigned short)Median(pImg, 0, w*h);

			delete pImg;
		}

	//将滤波后的结果拷贝至源图像中
	memcpy(pImg, pBuf, nW*nH * sizeof(unsigned short));

	delete[]pBuf;

	return true;
}


bool WindowLevelTransform(unsigned short *pImg, int nW, int nH, int nWinCentre, int nWindWidth)
{
	// TODO: Add your command handler code here
	if (NULL == pImg || nW < 0 || nH < 0 ||
		nWinCentre < 0 || nWindWidth < 0)
	{
		return false;
	}

	int window_center = nWinCentre;      //窗位,窗位是指窗宽上下限的平均数,或称窗中心
	int window_width = nWindWidth;      //窗宽,窗宽即是指CT图像上所包含的CT值范围,宽的宽窄直接影响到图像的对比度和清晰度

	float f_min = (float)((2 * window_center - window_width) / 2.0 + 0.5);
	float f_max = (float)((2 * window_center + window_width) / 2.0 + 0.5);

	float dFactor;
	dFactor = (float)65535.0 / (f_max - f_min);

	//循环变量
	long i;

	long N = nH * nW;

	for (i = 0; i < N; i++)
	{
		if (pImg[i] < f_min)
			pImg[i] = 0;
		else if (pImg[i] > f_max)
			pImg[i] = 65535;
		else
			pImg[i] = (unsigned short)((pImg[i] - f_min)*dFactor);

		if (pImg[i] < 0)
			pImg[i] = 0;
		if (pImg[i] > 65535)
			pImg[i] = 65535;
	}

	return true;
}

bool Laplacian(unsigned short *pImg, int nW, int nH)
{
	if (NULL == pImg || nW < 0 || nH < 0)
	{
		return false;
	}

	unsigned short *pTmp = new unsigned short[nW*nH];
	memcpy(pTmp, pImg, nW*nH * sizeof(unsigned short));

	for (int y = 1; y < nH - 1; y++)
		for (int x = 1; x < nW - 1; x++)
		{
			int pos = y * nW + x;
			int g = pTmp[pos] * 9 -
				(pTmp[pos - nW - 1] + pTmp[pos - nW] + pTmp[pos - nW + 1] +
					pTmp[pos - 1] + pTmp[pos + 1] +
					pTmp[pos + nW - 1] + pTmp[pos + nW] + pTmp[pos + nW + 1]);
			pImg[pos] = g > 65535 ? 65535 : (g < 0 ? 0 : g);
		}

	delete[]pTmp;

	return true;
}

bool Laplacian(unsigned char *pImg, int nW, int nH)
{
	if (NULL == pImg || nW < 0 || nH < 0)
	{
		return false;
	}

	unsigned char *pTmp = new unsigned char[nW*nH];
	memcpy(pTmp, pImg, nW*nH * sizeof(unsigned char));

	for (int y = 1; y < nH - 1; y++)
		for (int x = 1; x < nW - 1; x++)
		{
			int pos = y * nW + x;
			int g = pTmp[pos] * 9 -
				(pTmp[pos - nW - 1] + pTmp[pos - nW] + pTmp[pos - nW + 1] +
					pTmp[pos - 1] + pTmp[pos + 1] +
					pTmp[pos + nW - 1] + pTmp[pos + nW] + pTmp[pos + nW + 1]);
			pImg[pos] = g > 255 ? 255 : (g < 0 ? 0 : g);
		}

	delete[]pTmp;

	return true;
}


bool Resize(unsigned short *&pImg, int &nW, int &nH, int nNewW, int nNewH)
{
	if (NULL == pImg || nW < 0 || nH < 0)
		return false;

	int w0 = nW;
	int h0 = nH;
	int p0 = w0;

	int w1 = nNewW;
	int h1 = nNewH;
	int p1 = w1;

	float fw = float(w0) / w1;
	float fh = float(h0) / h1;

	float *arr_x = new float[w1];
	float *arr_y = new float[h1];
	for (int y = 0; y < h1; y++)
	{
		arr_y[y] = y * fh;
	}
	for (int x = 0; x < w1; x++)
	{
		arr_x[x] = x * fw;
	}

	unsigned short *pSrc = pImg;

	unsigned short *pDst = new unsigned short[p1*h1];

	for (int y = 0; y < h1; y++)
	{
		int ySrc = (int)(arr_y[y]);
		float v = arr_y[y] - ySrc;

		for (int x = 0; x < w1; x++)
		{
			int xSrc = (int)(arr_x[x]);

			float u = arr_x[x] - xSrc;

			//coefficients
			float c11 = u * v;
			float c10 = v - c11;
			float c01 = u - c11;
			float c00 = 1 - u - v + c11;

			int pos = ySrc * p0 + xSrc;
			unsigned short g = pSrc[pos];
			if (xSrc < w0 - 1)
			{
				if (ySrc < h0 - 1)
				{
					g = (unsigned short)(c00*pSrc[pos] + c01 * pSrc[pos + 1] + c10 * pSrc[pos + p0] + c11 * pSrc[pos + p0 + 1]);
				}
				else
				{
					g = (unsigned short)((1 - u)*pSrc[pos] + u * pSrc[pos + 1]);
				}
			}
			else
			{
				if (ySrc < h0 - 1)
				{
					g = (unsigned short)((1 - v)*pSrc[pos] + v * pSrc[pos + p0]);
				}
				else
				{
					g = pSrc[pos];
				}
			}

			pDst[y*p1 + x] = g;
		}
	}

	delete[]arr_x;
	delete[]arr_y;

	delete[]pImg;
	pImg = new unsigned short[h1*p1];
	memcpy(pImg, pDst, h1*p1 * sizeof(unsigned short));
	delete[]pDst;

	nW = w1;
	nH = h1;

	return true;
}

bool ZoomIn2(unsigned short *&pImg, int &nW, int &nH)
{
	if (NULL == pImg || nW < 0 || nH < 0)
		return false;

	int w0 = nW;
	int h0 = nH;
	int p0 = w0;

	int w1 = 2 * nW;
	int h1 = 2 * nH;
	int p1 = w1;

	float fw = float(w0) / w1;
	float fh = float(h0) / h1;

	float *arr_x = new float[w1];
	float *arr_y = new float[h1];
	for (int y = 0; y < h1; y++)
	{
		arr_y[y] = y * fh;
	}
	for (int x = 0; x < w1; x++)
	{
		arr_x[x] = x * fw;
	}

	unsigned short *pSrc = pImg;

	unsigned short *pDst = new unsigned short[p1*h1];

	for (int y = 0; y < h1; y++)
	{
		int ySrc = (int)(arr_y[y]);
		float v = arr_y[y] - ySrc;

		for (int x = 0; x < w1; x++)
		{
			int xSrc = (int)(arr_x[x]);

			float u = arr_x[x] - xSrc;

			//coefficients
			float c11 = u * v;
			float c10 = v - c11;
			float c01 = u - c11;
			float c00 = 1 - u - v + c11;

			int pos = ySrc * p0 + xSrc;
			unsigned short g = pSrc[pos];
			if (xSrc < w0 - 1)
			{
				if (ySrc < h0 - 1)
				{
					g = (unsigned short)(c00*pSrc[pos] + c01 * pSrc[pos + 1] + c10 * pSrc[pos + p0] + c11 * pSrc[pos + p0 + 1]);
				}
				else
				{
					g = (unsigned short)((1 - u)*pSrc[pos] + u * pSrc[pos + 1]);
				}
			}
			else
			{
				if (ySrc < h0 - 1)
				{
					g = (unsigned short)((1 - v)*pSrc[pos] + v * pSrc[pos + p0]);
				}
				else
				{
					g = pSrc[pos];
				}
			}

			pDst[y*p1 + x] = g;
		}
	}

	delete[]arr_x;
	delete[]arr_y;

	delete[]pImg;
	pImg = new unsigned short[h1*p1];
	memcpy(pImg, pDst, h1*p1 * sizeof(unsigned short));
	delete[]pDst;

	nW = w1;
	nH = h1;

	return true;
}

bool ZoomOut2(unsigned short *&pImg, int &nWidth, int &nHeight)
{
	if (NULL == pImg || nWidth < 0 || nHeight < 0)
		return false;

	int nW = nWidth / 2;
	int nH = nHeight / 2;

	unsigned short *pTemp = new unsigned short[nW*nH];
	for (int y = 0; y < nHeight; y += 2)
	{
		for (int x = 0; x < nWidth; x += 2)
		{
			int pos = y * nWidth + x;
			int ave = (pImg[pos] + pImg[pos + 1] + pImg[pos + nWidth] + pImg[pos + 1 + nWidth]) / 4;
			pTemp[y / 2 * nW + x / 2] = (unsigned short)ave;
		}
	}

	delete[]pImg;

	pImg = new unsigned short[nW*nH];
	memcpy(pImg, pTemp, nW*nH * sizeof(unsigned short));
	nWidth = nW;
	nHeight = nH;

	delete[]pTemp;

	return true;
}

bool Rotate90(unsigned short *&pImg, int &nWidth, int &nHeight)
{
	if (NULL == pImg || nWidth < 0 || nHeight < 0)
		return false;

	int nW = nHeight;
	int nH = nWidth;

	unsigned short *pTemp = new unsigned short[nW*nH];

	for (int y = 0; y < nHeight; y++)
	{
		for (int x = 0; x < nWidth; x++)
		{
			pTemp[(nH - 1 - x)*nW + y] = pImg[y*nWidth + x];
		}
	}

	delete[]pImg;
	pImg = new unsigned short[nW*nH];
	memcpy(pImg, pTemp, nW*nH * sizeof(unsigned short));
	nWidth = nW;
	nHeight = nH;

	delete[]pTemp;

	return true;
}

bool Flip(unsigned short *pImg, int nW, int nH)
{
	if (NULL == pImg || nW < 0 || nH < 0)
	{
		return false;
	}

	int nHalfH = nH / 2;

	for (int y = 0; y < nHalfH; y++)
	{
		for (int x = 0; x < nW; x++)
		{
			unsigned short t = pImg[y*nW + x];
			pImg[y*nW + x] = pImg[(nH - 1 - y)*nW + x];
			pImg[(nH - 1 - y)*nW + x] = t;
		}
	}

	return true;
}

bool Mirror(unsigned short *pImg, int nW, int nH)
{
	if (NULL == pImg || nW < 0 || nH < 0)
	{
		return false;
	}

	int nHalfW = nW / 2;

	for (int y = 0; y < nH; y++)
	{
		for (int x = 0; x < nHalfW; x++)
		{
			int pos = y * nW;

			unsigned short t = pImg[pos + x];
			pImg[pos + x] = pImg[pos + nW - 1 - x];
			pImg[pos + nW - 1 - x] = t;
		}
	}

	return true;
}

bool Invert(unsigned short *pImg, int nW, int nH)
{
	if (NULL == pImg || nW < 0 || nH < 0)
		return false;

	for (int y = 0; y < nH; y++)
		for (int x = 0; x < nW; x++)
		{
			pImg[y*nW + x] = 65535 - pImg[y*nW + x];
		}

	return true;
}

bool Crop(unsigned short *&pImg, int &nW, int &nH, int xs, int xe, int ys, int ye)
{
	if (NULL == pImg ||
		xs < 0 || xs > xe || xe >= nW ||
		ys < 0 || ys > ye || ye >= nH)
	{
		return false;
	}

	int w = xe - xs + 1;
	int h = ye - ys + 1;

	unsigned short *pTmp = new unsigned short[w*h];
	for (int y = ys; y <= ye; y++)
		for (int x = xs; x <= xe; x++)
		{
			pTmp[(y - ys)*w + (x - xs)] = pImg[y*nW + x];
		}

	delete[]pImg;
	pImg = new unsigned short[w*h];
	memcpy(pImg, pTmp, w*h * sizeof(unsigned short));
	delete[]pTmp;

	nW = w;
	nH = h;

	return true;
}


bool GetIntensityCurve(vector<unsigned short> &aIntensity, unsigned short *pImg, int nW, int nH, int xs, int ys, int xe, int ye)
{
	if (NULL == pImg ||
		xs < 0 || xs >= nW ||
		ys < 0 || ys >= nH ||
		xe < 0 || xe >= nW ||
		ye < 0 || ye >= nH)
	{
		return false;
	}

	if (xs == xe && ys == ye)
	{
		return false;
	}

	double dx = xe - xs;
	double dy = ye - ys;
	double dLen = sqrt(dx*dx + dy * dy);
	double nx = dx / dLen;
	double ny = dy / dLen;

	aIntensity.push_back(pImg[ys*nW + xs]);

	int nLen = (int)(dLen);

	for (int i = 1; i < nLen; i++)
	{
		double x = xs + i * nx;
		double y = ys + i * ny;

		int x0 = (int)x;
		int y0 = (int)y;
		int p0 = y0 * nW + x0;

		unsigned short g;

		if (x < nW - 1)
		{
			if (y < nH - 1)
			{
				g = (unsigned short)((y0 + 1 - y)*((x0 + 1 - x)*pImg[p0] + (x - x0)*pImg[p0 + 1]) +
					(y - y0)*((x0 + 1 - x)*pImg[p0 + nW] + (x - x0)*pImg[p0 + nW + 1]));
			}
			else
			{
				g = (unsigned short)((x0 + 1 - x)*pImg[p0] + (x - x0)*pImg[p0 + 1]);
			}
		}
		else
		{
			if (y < nH - 1)
			{
				g = (unsigned short)((y0 + 1 - y)*pImg[p0] + (y - y0)*pImg[p0 + nW]);
			}
			else
			{
				g = pImg[p0];
			}
		}

		aIntensity.push_back(g);
	}

	//aIntensity.push_back(pImg[ye*nW+xe]);

	return true;
}

//计算信噪比
bool GetSNR(double &dMean, double &dStd, double &dSNR, unsigned short *pImg, int nW, int nH, int xs, int ys, int xe, int ye)
{
	if (NULL == pImg ||
		xs < 0 || xs >= nW ||
		ys < 0 || ys >= nH ||
		xe < 0 || xe >= nW ||
		ye < 0 || ye >= nH)
	{
		return false;
	}

	if (xs == xe && ys == ye)
	{
		return false;
	}

	unsigned nPx = (xe - xs + 1)*(ye - ys + 1);

	dMean = 0.0;
	dStd = 0.0;
	for (int y = ys; y <= ye; y++)
		for (int x = xs; x <= xe; x++)
		{
			dMean += pImg[y*nW + x];
		}
	dMean /= nPx;

	dStd = 0.0;
	for (int y = ys; y <= ye; y++)
		for (int x = xs; x <= xe; x++)
		{
			dStd += (pImg[y*nW + x] - dMean) * (pImg[y*nW + x] - dMean);
		}
	dStd = sqrt(dStd / nPx);

	if (dStd < 1.0e-10)
	{
		dSNR = 0.0;
	}
	else
	{
		dSNR = dMean / dStd;
	}

	return true;
}
