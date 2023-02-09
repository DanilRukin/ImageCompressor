#include <iostream>
#include <bitset>
#include "SimpleCompressor.h"
#include <vector>


void TestSimpleCompressor();
void TestShifting();

int main()
{
	setlocale(LC_ALL, "ru");
	TestSimpleCompressor();
    return 0;
}


void TestSimpleCompressor()
{
	SimpleCompressor compressor;
	unsigned int pixels[] { 2345879620u, 2312354870u, 134253650u, 323254780u };
	int width = 2, height = 2;
	unsigned short* result;
	std::cout << "Оригинал: ";
	for (int i = 0; i < height * width; i++)
	{
		std::bitset<32> bit(pixels[i]);
		std::cout << bit << " ";
	}
	std::cout << std::endl << "Сжатое: ";
	result = compressor.Compress(pixels, width, height);
	for (int i = 0; i < height * width; i++)
	{
		std::bitset<16> bit(result[i]);
		std::cout << bit << " ";
	}
	std::cout << std::endl << "Расшифрованное: ";
	unsigned int* decompressed = compressor.Decompress(result, width, height);
	for (int i = 0; i < height * width; i++)
	{
		std::bitset<32> bit(decompressed[i]);
		std::cout << bit << " ";
	}
	printf("\r\n");
}

void TestShifting()
{
	char B, G, R, A;  // sizeof(char) = 1 byte
	char modulatedB, modulatedG, modulatedR, modulatedA;
	unsigned int Bmask = 0xff000000u;  // 11111111000000000000000000000000 = 0xff000000u
	unsigned int Gmask = 0x00ff0000u;  // 00000000111111110000000000000000 = 0x00ff0000u
	unsigned int Rmask = 0x0000ff00u;  // 00000000000000001111111100000000 = 0x0000ff00u
	unsigned int Amask = 0x000000ffu;  // 00000000000000000000000011111111 = 0x000000ffu

	unsigned int pixel = 0xf5876589u; // 
	std::bitset<32> bitset{ pixel };
	std::bitset<32> mask{ Gmask };
	std::cout << "pixel = " << pixel << " = " << bitset << std::endl;
	std::cout << "mask = " << Gmask << " = " << mask << std::endl;
	unsigned int pixelWithMask = pixel & Gmask;
	std::bitset<32> pixelWithMaskBitset{ pixelWithMask };
	std::cout << "pixel with mask = " << pixelWithMask << " = " << pixelWithMaskBitset << std::endl;
	G = (char)(pixelWithMask >> 16);
	std::bitset<8> charBitset(G);
	std::bitset<16> shortBitset(((unsigned short)G << 8));
	std::cout << "Приведенное значение = " << G << " = " << shortBitset << " = " << charBitset << std::endl;
}
