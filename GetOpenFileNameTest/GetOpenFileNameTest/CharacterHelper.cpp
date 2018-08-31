#include "stdafx.h"
#include "CharacterHelper.h"
  
str_ansi * CAPECharacterHelper::GetANSIFromUTF8(const str_utf8 * pUTF8)
{
	str_utf16 * pUTF16 = GetUTF16FromUTF8(pUTF8);
	str_ansi * pANSI = GetANSIFromUTF16(pUTF16);
	delete[] pUTF16;
	return pANSI;
}

str_ansi * CAPECharacterHelper::GetANSIFromUTF16(const str_utf16 * pUTF16)
{
	const int nCharacters = pUTF16 ? int(wcslen(pUTF16)) : 0;
#ifdef _WIN32
	int nANSICharacters = (2 * nCharacters);
	str_ansi * pANSI = new str_ansi[nANSICharacters + 1];
	memset(pANSI, 0, (nANSICharacters + 1) * sizeof(str_ansi));
	if (pUTF16)
		WideCharToMultiByte(CP_ACP, 0, pUTF16, -1, pANSI, nANSICharacters, NULL, NULL);
#else
	str_utf8 * pANSI = new str_utf8[nCharacters + 1];
	for (int z = 0; z < nCharacters; z++)
		pANSI[z] = (pUTF16[z] >= 256) ? '?' : (str_utf8)pUTF16[z];
	pANSI[nCharacters] = 0;
#endif

	return (str_ansi *)pANSI;
}

str_utf16 * CAPECharacterHelper::GetUTF16FromANSI(const str_ansi * pANSI)
{
	const int nCharacters = pANSI ? int(strlen(pANSI)) : 0;
	str_utf16 * pUTF16 = new str_utf16[nCharacters + 1];

#ifdef _WIN32
	memset(pUTF16, 0, sizeof(str_utf16) * (nCharacters + 1));
	if (pANSI)
		MultiByteToWideChar(CP_ACP, 0, pANSI, -1, pUTF16, nCharacters);
#else
	for (int z = 0; z < nCharacters; z++)
		pUTF16[z] = (str_utf16)((str_utf8)pANSI[z]);
	pUTF16[nCharacters] = 0;
#endif

	return pUTF16;
}

str_utf16 * CAPECharacterHelper::GetUTF16FromUTF8(const str_utf8 * pUTF8)
{
	// get the length
	int nCharacters = 0; int nIndex = 0;
	while (pUTF8[nIndex] != 0)
	{
		if ((pUTF8[nIndex] & 0x80) == 0)
			nIndex += 1;
		else if ((pUTF8[nIndex] & 0xE0) == 0xE0)
			nIndex += 3;
		else
			nIndex += 2;

		nCharacters += 1;
	}

	// make a UTF-16 string
	str_utf16 * pUTF16 = new str_utf16[nCharacters + 1];
	nIndex = 0; nCharacters = 0;
	while (pUTF8[nIndex] != 0)
	{
		if ((pUTF8[nIndex] & 0x80) == 0)
		{
			pUTF16[nCharacters] = pUTF8[nIndex];
			nIndex += 1;
		}
		else if ((pUTF8[nIndex] & 0xE0) == 0xE0)
		{
			pUTF16[nCharacters] = ((pUTF8[nIndex] & 0x1F) << 12) | ((pUTF8[nIndex + 1] & 0x3F) << 6) | (pUTF8[nIndex + 2] & 0x3F);
			nIndex += 3;
		}
		else
		{
			pUTF16[nCharacters] = ((pUTF8[nIndex] & 0x3F) << 6) | (pUTF8[nIndex + 1] & 0x3F);
			nIndex += 2;
		}

		nCharacters += 1;
	}
	pUTF16[nCharacters] = 0;

	return pUTF16;
}

str_utf8 * CAPECharacterHelper::GetUTF8FromANSI(const str_ansi * pANSI)
{
	str_utf16 * pUTF16 = GetUTF16FromANSI(pANSI);
	str_utf8 * pUTF8 = GetUTF8FromUTF16(pUTF16);
	delete[] pUTF16;
	return pUTF8;
}

str_utf8 * CAPECharacterHelper::GetUTF8FromUTF16(const str_utf16 * pUTF16)
{
	// get the size(s)
	int nCharacters = int(wcslen(pUTF16));
	int nUTF8Bytes = 0;
	for (int z = 0; z < nCharacters; z++)
	{
		if (pUTF16[z] < 0x0080)
			nUTF8Bytes += 1;
		else if (pUTF16[z] < 0x0800)
			nUTF8Bytes += 2;
		else
			nUTF8Bytes += 3;
	}

	// allocate a UTF-8 string
	str_utf8 * pUTF8 = new str_utf8[nUTF8Bytes + 1];

	// create the UTF-8 string
	int nUTF8Index = 0;
	for (int z = 0; z < nCharacters; z++)
	{
		if (pUTF16[z] < 0x0080)
		{
			pUTF8[nUTF8Index++] = (str_utf8)pUTF16[z];
		}
		else if (pUTF16[z] < 0x0800)
		{
			pUTF8[nUTF8Index++] = 0xC0 | (pUTF16[z] >> 6);
			pUTF8[nUTF8Index++] = 0x80 | (pUTF16[z] & 0x3F);
		}
		else
		{
			pUTF8[nUTF8Index++] = 0xE0 | (pUTF16[z] >> 12);
			pUTF8[nUTF8Index++] = 0x80 | ((pUTF16[z] >> 6) & 0x3F);
			pUTF8[nUTF8Index++] = 0x80 | (pUTF16[z] & 0x3F);
		}
	}
	pUTF8[nUTF8Index++] = 0;

	// return the UTF-8 string
	return pUTF8;
}


////-------------------------------------------   
//// ��BYTE����ת��Ϊ�ַ�������   
////-------------------------------------------   
string* CAPECharacterHelper::byteToHexStr(byte byte_arr[], int arr_len)
{
	string* hexstr = new string;
	for (int i = 0; i<arr_len; i++)
	{
		char hex1;
		char hex2;
		int value = byte_arr[i];
		int v1 = value / 16;
		int v2 = value % 16;
		//����ת��Ϊ��ĸ   
		if (v1 >= 0 && v1 <= 9)
		{
			hex1 = (char)(48 + v1);
		}
		else
		{
			hex1 = (char)(55 + v1);
		}
		//������ת����ĸ   
		if (v2 >= 0 && v2 <= 9)
		{
			hex2 = (char)(48 + v2);
		}
		else
		{
			hex2 = (char)(55 + v2);
		}
		//����ĸ����һ��   
		*hexstr = *hexstr + hex1 + hex2;
	}
	int t = (*hexstr).size();
	return hexstr;
}
////-------------------------------------------   
//// ���ַ�������ת��ΪBYTE����   
////-------------------------------------------   
void CAPECharacterHelper::HexStrToByte(string str_arr, byte byte_arr[])
{
	unsigned char ch1;
	unsigned char ch2;
	int k = 0;
	for (int i = 0; i<str_arr.length() - 1; i = i + 2)
	{
		ch1 = str_arr.at(i);
		ch2 = str_arr.at(i + 1);
		if (ch1 >= 48 && ch1 <= 57)
		{
			ch1 &= 0x0F;
		}
		if (ch1 >= 'A' && ch1 <= 'F')
		{
			ch1 &= 0x0F;
			ch1 += 0x09;
		}
		if (ch2 >= 48 && ch2 <= 57)
		{
			ch2 &= 0x0F;
		}
		if (ch2 >= 'A' && ch2 <= 'F')
		{
			ch2 &= 0x0F;
			ch2 += 0x09;
		}
		ch1 <<= 4;
		byte_arr[k] = ch1 + ch2;//int����תbyte���ͣ�������   
		k++;
	}
}

string CAPECharacterHelper::connectStr(string str1,string str2)
{ 
	std::stringstream strStream;
	strStream << str1 << str2;
	return strStream.str(); 
}

str_utf16* CAPECharacterHelper::getUTF16FromBool(bool src)
{
	if (src)
	{
		return L"1";
	}
	else
	{
		return L"0";
	}
}
str_utf16* CAPECharacterHelper::getUTF16FromBOOL(BOOL src)
{
	if (src)
	{
		return L"1";
	}
	else
	{
		return L"0";
	}

}