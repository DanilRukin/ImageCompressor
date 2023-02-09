#pragma once


class ICompressor
{
public:
	virtual unsigned short* Compress(unsigned int* pixels, int width, int height) = 0;
	virtual unsigned int* Decompress(unsigned short* compressedPixels, int width, int height) = 0;
};

