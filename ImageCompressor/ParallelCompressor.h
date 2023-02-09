#pragma once
#include "BaseCompressor.h"
#include <thread>
#include <vector>


class ParallelCompressor : public BaseCompressor
{
private:
	int _threadsCount;
	std::vector<std::unique_ptr<std::thread>> _threads;

	void CompressRow(unsigned int* pixels, int firstRowIndexToProcess, int lastRowIndexToProcess, int rowLength, unsigned short* compressedPixels)
	{
		unsigned char B, G, R, A;  // sizeof(char) = 1 byte
		unsigned char nextB, nextG, nextR, nextA;
		short modulatedB, modulatedG, modulatedR, modulatedA;
		char normalizedA, normalizedB, normalizedG, normalizedR;
		unsigned int Bmask = 0xff000000u;  // 11111111000000000000000000000000 = 0xff000000u
		unsigned int Gmask = 0x00ff0000u;  // 00000000111111110000000000000000 = 0x00ff0000u
		unsigned int Rmask = 0x0000ff00u;  // 00000000000000001111111100000000 = 0x0000ff00u
		unsigned int Amask = 0x000000ffu;  // 00000000000000000000000011111111 = 0x000000ffu
		unsigned int currentPixel = 0, nextPixel;
		int rowIndex = firstRowIndexToProcess;
		for ( ; rowIndex <= lastRowIndexToProcess; rowIndex++)
		{
			for (int j = 0; j < rowLength; j++)
			{
				nextPixel = pixels[rowIndex * rowLength + j];
				//
				// ��������� ������� � �� ���������
				//
				nextB = (unsigned char)((nextPixel & Bmask) >> 24);
				B = (unsigned char)((currentPixel & Bmask) >> 24);
				modulatedB = (short)(nextB - B);

				nextG = (unsigned char)((nextPixel & Gmask) >> 16);
				G = (unsigned char)((currentPixel & Gmask) >> 16);
				modulatedG = (short)(nextG - G);

				nextR = (unsigned char)((nextPixel & Rmask) >> 8);
				R = (unsigned char)((currentPixel & Rmask) >> 8);
				modulatedR = (short)(nextR - R);

				nextA = (unsigned char)((nextPixel & Amask));  // ����� �� ������, �.�. �����-����� ��������� �������� �� 0 �� 1
				A = (unsigned char)(currentPixel & Amask);
				modulatedA = (short)(nextA - A);
				//
				// ������������ �������������� ������� � ������� [-7; 7]
				//
				int denominator = Xmax - Xmin, coeff = b - a;

				normalizedB = a + ((modulatedB - Xmin) * coeff) / denominator;
				normalizedG = a + ((modulatedG - Xmin) * coeff) / denominator;
				normalizedR = a + ((modulatedR - Xmin) * coeff) / denominator;
				normalizedA = a + ((modulatedA - Xmin) * coeff) / denominator;
				//
				// ������ ��������������� �������� � ��� short
				//				
				unsigned short bgra = 0, b = 0, g = 0, r = 0, alfa = 0;
				b = ConvertTo4BitDigit(normalizedB);
				b = b << 12;
				g = ConvertTo4BitDigit(normalizedG);
				g = g << 8;
				r = ConvertTo4BitDigit(normalizedR);
				r = r << 4;
				alfa = ConvertTo4BitDigit(normalizedA);
				bgra = b | g | r | alfa;

				compressedPixels[rowIndex * rowLength + j] = bgra;

				currentPixel = nextPixel;
			}
		}			
	}

	void DecompressRow(unsigned short* compressedPixels, int firstRowIndexToProcess, int lastRowIndexToProcess, int rowLength, unsigned int* pixels)
	{
		unsigned short compressedPixel, currentPixel = 0;
		unsigned short Bmask = 0xf000u;
		unsigned short Gmask = 0x0f00u;
		unsigned short Rmask = 0x00f0u;
		unsigned short Amask = 0x000fu;
		unsigned int restoredPixel, B, G, R, A;
		short deltaNormalizedA, deltaNormalizedB, deltaNormalizedG, deltaNormalizedR, b, g, r, alfa;
		short lastNormilizedA = 0, lastNormalizedB = 0, lastNormalizedG = 0, lastNormalizedR = 0;
		int rowIndex = firstRowIndexToProcess;
		for ( ; rowIndex <= lastRowIndexToProcess; rowIndex++)
		{
			for (int j = 0; j < rowLength; j++)
			{
				compressedPixel = compressedPixels[rowIndex * rowLength + j];
				//
				// �������� ������ � �������� � ���������������� ���� (�������� ����� ����� �� -7 �� 7)
				//
				b = compressedPixel & Bmask;
				b = b >> 12;
				deltaNormalizedB = ConvertFrom4BitDigit(b);

				g = compressedPixel & Gmask;
				g = g >> 8;
				deltaNormalizedG = ConvertFrom4BitDigit(g);

				r = compressedPixel & Rmask;
				r = r >> 4;
				deltaNormalizedR = ConvertFrom4BitDigit(r);

				alfa = compressedPixel & Amask;
				deltaNormalizedA = ConvertFrom4BitDigit(alfa);
				//
				// ��������� �������� ������������ � ������� [-255; 255]
				//
				int denominator = this->b - this->a, coeff = Xmax - Xmin;
				deltaNormalizedA = Xmin + (deltaNormalizedA - this->a) * coeff / denominator;
				deltaNormalizedB = Xmin + (deltaNormalizedB - this->a) * coeff / denominator;
				deltaNormalizedG = Xmin + (deltaNormalizedG - this->a) * coeff / denominator;
				deltaNormalizedR = Xmin + (deltaNormalizedR - this->a) * coeff / denominator;
				//
				// �������������� (� ����������� �������� ������ � �������� �� -255 �� 255) �� ��������� (� ��������, �����������)
				//
				lastNormalizedB += deltaNormalizedB;
				lastNormalizedG += deltaNormalizedG;
				lastNormalizedR += deltaNormalizedR;
				lastNormilizedA += deltaNormalizedA;
				//
				// ����� ��������������� �������� �� 8 ��� ������ � �������� � restoredPixel
				//
				restoredPixel = 0;
				B = ConvertTo8BitDigit(lastNormalizedB);
				B = B << 24;
				G = ConvertTo8BitDigit(lastNormalizedG);
				G = G << 16;
				R = ConvertTo8BitDigit(lastNormalizedR);
				R = R << 8;
				A = ConvertTo8BitDigit(lastNormilizedA);
				restoredPixel = B | G | R | A;
				pixels[rowIndex * rowLength + j] = restoredPixel;
			}
		}		
	}
public:
	ParallelCompressor(int threadsCount)
	{
		_threadsCount = threadsCount;
	}

	unsigned short* Compress(unsigned int* pixels, int width, int height) override
	{
		unsigned short* compressedPixels = new unsigned short[width * height];
		if (!_threads.empty())
			_threads.clear();
		int rowsCountToProcessByThread = height / _threadsCount;  // ����� �����, ������� ������ ���������� ������ �����
		// �.�. ��� ���������� height / _threadsCount ����� ���������� ������, ��
		// �������� ����� ���������� �����, ������� ������� � rowsCountToProcessByThread
		// ��� ���������� ������
		int additionalRowsCountToProcessByLastThread = height - rowsCountToProcessByThread * _threadsCount; 
		int firstIndex = 0, lastIndex;

		// ���������� �������
		for (int i = 0; i < _threadsCount - 1; i++)
		{
			lastIndex = firstIndex + rowsCountToProcessByThread - 1; // ��������, firstIndex = 0, rowsCountToProcessByThread = 5 => lastIndex = 0 + 5 - 1 = 4
			_threads.push_back(std::make_unique<std::thread>(&ParallelCompressor::CompressRow, this, pixels, firstIndex, lastIndex, width, compressedPixels));
			firstIndex = lastIndex + 1; // ��������, lastIndex = 0 + 5 - 1 = 4 => firstIndex = 4 + 1 = 5
		}
		// �� ���� ����� firstIndex ��������� �� ������ ������ ���������� ����� ��� ���������
		lastIndex = firstIndex + rowsCountToProcessByThread + additionalRowsCountToProcessByLastThread - 1; // ������� ���. ������ � ������ 1 �.�. ������
		_threads.push_back(std::make_unique<std::thread>(&ParallelCompressor::CompressRow, this, pixels, firstIndex, lastIndex, width, compressedPixels));

		// ������� ���������� ���� �������
		for (int i = 0; i < _threadsCount; i++)
		{
			_threads[i]->join();
		}
		return compressedPixels;
	}

	unsigned int* Decompress(unsigned short* compressedPixels, int width, int height) override
	{
		unsigned int* pixels = new unsigned int[width * height];
		if (!_threads.empty())
			_threads.clear();
		int rowsCountToProcessByThread = height / _threadsCount;  // ����� �����, ������� ������ ���������� ������ �����
		// �.�. ��� ���������� height / _threadsCount ����� ���������� ������, ��
		// �������� ����� ���������� �����, ������� ������� � rowsCountToProcessByThread
		// ��� ���������� ������
		int additionalRowsCountToProcessByLastThread = height - rowsCountToProcessByThread * _threadsCount;
		int firstIndex = 0, lastIndex;

		// ���������� �������
		for (int i = 0; i < _threadsCount - 1; i++)
		{
			lastIndex = firstIndex + rowsCountToProcessByThread - 1; // ��������, firstIndex = 0, rowsCountToProcessByThread = 5 => lastIndex = 0 + 5 - 1 = 4
			_threads.push_back(std::make_unique<std::thread>(&ParallelCompressor::DecompressRow, this, compressedPixels, firstIndex, lastIndex, width, pixels));
			firstIndex = lastIndex + 1; // ��������, lastIndex = 0 + 5 - 1 = 4 => firstIndex = 4 + 1 = 5
		}
		// �� ���� ����� firstIndex ��������� �� ������ ������ ���������� ����� ��� ���������
		lastIndex = firstIndex + rowsCountToProcessByThread + additionalRowsCountToProcessByLastThread - 1; // ������� ���. ������ � ������ 1 �.�. ������
		_threads.push_back(std::make_unique<std::thread>(&ParallelCompressor::DecompressRow, this, compressedPixels, firstIndex, lastIndex, width, pixels));

		// ������� ���������� ���� �������
		for (int i = 0; i < _threadsCount; i++)
		{
			_threads[i]->join();
		}
		return pixels;
	}
};

