#pragma once
#include "ICompressor.h"
class BaseCompressor : public ICompressor
{
protected:
	short a = -7, b = 7, Xmin = -255, Xmax = 255;
	/// <summary>
	/// Переводит отрицательное число (до -7 включительно) в положительное, но к разрядности в 4 бита в ПРЯМОМ КОДЕ.
	/// Положительные числа не изменяет.
	/// </summary>
	/// <param name="digit"></param>
	/// <returns></returns>
	unsigned short ConvertTo4BitDigit(short digit)
	{
		unsigned short minus = 8;  // 1000 в двоичной системе - нужный мне знак 'минус'
		if (digit >= 0)
			return digit;
		digit *= -1;
		digit |= minus;
		return digit;
	}
	/// <summary>
	/// Переводит положительное четырехбитное число (больше 8) в ПРЯМОМ КОДЕ в отрицательное в ОБРАТНОМ КОДЕ.
	/// Все, что меньше 8 не изменяется.
	/// </summary>
	/// <param name="digit"></param>
	/// <returns></returns>
	short ConvertFrom4BitDigit(unsigned short digit)
	{
		if (digit < 8)
			return digit;
		unsigned short plus = 7; // 0111 в двоичной системе, нужный мне знак 'плюс'
		digit &= plus;
		digit *= -1;
		return digit;
	}
	/// <summary>
	/// Переводит отрицательное число (до -255 включительно) в положительное, но к разрядности в 8 бит в ПРЯМОМ КОДЕ.
	/// Положительные числа не изменяет.
	/// </summary>
	/// <param name="digit"></param>
	/// <returns></returns>
	unsigned int ConvertTo8BitDigit(short digit)
	{
		if (digit >= 0)
			return digit;
		unsigned int result = digit;
		result *= -1;
		unsigned int minus = 0x00000100u; // 256 в десятичной и 0000000100000000 в двоичной, нужный мне знак 'минус'
		result |= minus;
		return result;
	}
};

