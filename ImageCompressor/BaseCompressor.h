#pragma once
#include "ICompressor.h"
class BaseCompressor : public ICompressor
{
protected:
	short a = -7, b = 7, Xmin = -255, Xmax = 255;
	/// <summary>
	/// ��������� ������������� ����� (�� -7 ������������) � �������������, �� � ����������� � 4 ���� � ������ ����.
	/// ������������� ����� �� ��������.
	/// </summary>
	/// <param name="digit"></param>
	/// <returns></returns>
	unsigned short ConvertTo4BitDigit(short digit)
	{
		unsigned short minus = 8;  // 1000 � �������� ������� - ������ ��� ���� '�����'
		if (digit >= 0)
			return digit;
		digit *= -1;
		digit |= minus;
		return digit;
	}
	/// <summary>
	/// ��������� ������������� ������������� ����� (������ 8) � ������ ���� � ������������� � �������� ����.
	/// ���, ��� ������ 8 �� ����������.
	/// </summary>
	/// <param name="digit"></param>
	/// <returns></returns>
	short ConvertFrom4BitDigit(unsigned short digit)
	{
		if (digit < 8)
			return digit;
		unsigned short plus = 7; // 0111 � �������� �������, ������ ��� ���� '����'
		digit &= plus;
		digit *= -1;
		return digit;
	}
	/// <summary>
	/// ��������� ������������� ����� (�� -255 ������������) � �������������, �� � ����������� � 8 ��� � ������ ����.
	/// ������������� ����� �� ��������.
	/// </summary>
	/// <param name="digit"></param>
	/// <returns></returns>
	unsigned int ConvertTo8BitDigit(short digit)
	{
		if (digit >= 0)
			return digit;
		unsigned int result = digit;
		result *= -1;
		unsigned int minus = 0x00000100u; // 256 � ���������� � 0000000100000000 � ��������, ������ ��� ���� '�����'
		result |= minus;
		return result;
	}
};

