#include "stdafx.h" 
#include "wav.h" 
#include <iostream>
#include <fstream>

#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |   \
                ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))

using namespace std;

#define ID_RIFF MAKEFOURCC('R', 'I', 'F', 'F')

#define ID_WAVE MAKEFOURCC('W', 'A', 'V', 'E')

#define ID_data MAKEFOURCC('d', 'a', 't', 'a')

#define ID_fmt  MAKEFOURCC('f', 'm', 't',  '\x20')

#define ID_fact MAKEFOURCC('f', 'a', 'c',  't')

// WavFile ����/����

WavFile::WavFile()
{
	// TODO: �ڴ����һ���Թ������

}

WavFile::~WavFile()
{
	delete data;
	delete Ldata;
	delete Rdata;
}
 
// CWavePlayerDoc ���л�

void WavFile::Serialize(string ar) 
{
	 
	DWORD id, chkLen, dw, fmt, fact;    // �޷���˫��
	BYTE b; // �޷����ֽ�
	DWORD ChunkSize;
	WORD w=0;              // unsined short  �޷��ŵ��ֳ�
	WORD fmtTag;           //��ʽ���  
	 

	//FILE * waveFile;
	//fopen_s(&waveFile, "D:\\record\\std1.wav", "r"); 
	ifstream inFile(ar, ios_base::in | ios_base::_Nocreate | ios_base::binary);
	//ifstream inFile("D:\\vp\\vi1480260354.wav", ios_base::in | ios_base::_Nocreate | ios_base::binary);
	
	 
	//fread(&id, sizeof(DWORD), 1, waveFile);
	inFile.read((char*)&id, sizeof(DWORD));
	if (id != ID_RIFF)
	{
		inFile.close();
		inFile.clear();
		//LOG(ERROR) << "��ȡ�ļ�"<< ar <<"����not riff file" << endl;
		return;
	}	
	//fread(&ChunkSize, sizeof(DWORD), 1, waveFile);
	inFile.read((char*)&ChunkSize, sizeof(DWORD));

	//fread(&dw, sizeof(DWORD), 1, waveFile);
	inFile.read((char*)&dw, sizeof(DWORD));
	if (dw != ID_WAVE)
	{
		inFile.close();
		inFile.clear();
		//LOG(ERROR) << "��ȡ�ļ�" << ar << "����not wave" << endl;
		return;
	}
		

	//fread(&fmt, sizeof(DWORD), 1, waveFile);
	inFile.read((char*)&fmt, sizeof(DWORD));
	if (fmt != ID_fmt)
	{
		inFile.close();
		inFile.clear();
		//LOG(ERROR) << "��ȡ�ļ�" << ar << "����not fmat" << endl;
		return;
	}
		
	/**/
	//fread(&chkLen, sizeof(DWORD), 1, waveFile);
	inFile.read((char*)&chkLen, sizeof(DWORD));
	//fread(&fmtTag, sizeof(WORD), 1, waveFile);
	inFile.read((char*)&fmtTag, sizeof(WORD));
	//fread(&wChannel, sizeof(WORD), 1, waveFile);
	inFile.read((char*)&wChannel, sizeof(WORD));
	//fread(&SampleRate, sizeof(DWORD), 1, waveFile);
	inFile.read((char*)&SampleRate, sizeof(DWORD));
	//fread(&ByteRate, sizeof(DWORD), 1, waveFile);
	inFile.read((char*)&ByteRate, sizeof(DWORD));
	//fread(&BlockAlign, sizeof(WORD), 1, waveFile);
	inFile.read((char*)&BlockAlign, sizeof(WORD));
	//fread(&wBitsPerSample, sizeof(WORD), 1, waveFile);
	inFile.read((char*)&wBitsPerSample, sizeof(WORD));
	if (chkLen >= 18)
	{
		// ��չ���С
		//fread(&extendSize, sizeof(WORD), 1, waveFile);
		inFile.read((char*)&extendSize, sizeof(WORD));
		//inFile.read((char*)&b, extendSize);
		/**/
		for (int i = 0; i < extendSize; i++)  // ������չ�������
		{
		//fread(&b, sizeof(BYTE), 1, waveFile);
		inFile.read((char*)&b, 1);
		}
		
	}

	//fread(&id, sizeof(DWORD), 1, waveFile);
	inFile.read((char*)&id, sizeof(DWORD));
	if (id == ID_fact)
	{
		//fread(&fact, sizeof(DWORD), 2, waveFile);
		inFile.read((char*)&fact, sizeof(DWORD) * 2);
		//fread(&id, sizeof(DWORD), 1, waveFile);
		inFile.read((char*)&id, sizeof(DWORD));
	}
	if (id != ID_data)
	{
		inFile.close();
		inFile.clear();
		//LOG(ERROR) << "��ȡ�ļ�" << ar << "����not data" << endl;
		return;
	}
	 
	//fread(&Len, sizeof(DWORD), 1, waveFile);
	inFile.read((char*)&Len, sizeof(DWORD));
	if (Len < 1 || wBitsPerSample < 1)
	{
		inFile.close();
		inFile.clear();
		//LOG(ERROR) << "��ȡ�ļ�" << ar << "����data����Ϊ0" << endl;
		return;
	}

	BytesPerSample = wBitsPerSample / 8;         // һ���������ֽ���
	num = Len / BytesPerSample;              // ������


	if (wChannel == 1)                 // ������
	{
		data = new LONG[num];
		if (BytesPerSample == 1)
			for (int i = 0; i<num; i++)
			{
				//fread(&b, sizeof(BYTE), 1, waveFile);
				inFile.read((char*)&b, sizeof(BYTE));
				data[i] = b - 128;
			}
		else if (BytesPerSample == 2)
			for (int i = 0; i<num; i++)
			{
				//fread(&w, sizeof(WORD), 1, waveFile);
				inFile.read((char*)&w, sizeof(WORD));
				data[i] = (SHORT)w;         // �޷�����ת���з�����
			}
		else if (BytesPerSample == 4)
			for (int i = 0; i<num; i++)
			{
				//fread(&dw, sizeof(DWORD), 1, waveFile);
				inFile.read((char*)&dw, sizeof(DWORD));
				data[i] = (LONG)w;          // �޷�����ת���з�����
			}
		else if (BytesPerSample == 3)
			for (int i = 0; i<num; i++)
			{
				//fread(&b, sizeof(BYTE), 1, waveFile);
				inFile.read((char*)&b, sizeof(BYTE));
				//fread(&w, sizeof(WORD), 1, waveFile);
				inFile.read((char*)&w, sizeof(WORD));
				data[i] = w + b;
			}
	}
	else if (wChannel == 2)             // ˫����
	{
		Ldata = new LONG[num / 2];     // ������
		Rdata = new LONG[num / 2];      // ������
		if (BytesPerSample == 1)
		{
			for (int i = 0; i<num / 2; i++)  // һ��������������Ϊnum/2, ������������
			{
				//fread(&b, sizeof(BYTE), 1, waveFile);
				inFile.read((char*)&b, sizeof(BYTE));
				Ldata[i] = b - 128;
				//fread(&b, sizeof(BYTE), 1, waveFile);
				inFile.read((char*)&b, sizeof(BYTE));
				Rdata[i] = b - 128;
			}
		}
		else if (BytesPerSample == 2)
		{
			int i = 0;
			for (i = 0; i<num / 2 && !inFile.eof(); i++)
			{
				//fread(&w, sizeof(WORD), 1, waveFile);
				inFile.read((char*)&w, sizeof(WORD));
				Ldata[i] = ((SHORT)w+65536/2)*256/65536;         // �޷�����ת���з�����
				//fread(&w, sizeof(WORD), 1, waveFile);
				inFile.read((char*)&w, sizeof(WORD));
				Rdata[i] = ((SHORT)w + 65536 / 2) * 256 / 65536;
			}
			//cout << i << endl;
		}

		else if (BytesPerSample == 4)
			for (int i = 0; i<num / 2; i++)
			{
				//fread(&dw, sizeof(DWORD), 1, waveFile);
				inFile.read((char*)&dw, sizeof(DWORD));
				Ldata[i] = (LONG)dw;
				//fread(&dw, sizeof(DWORD), 1, waveFile);
				inFile.read((char*)&dw, sizeof(DWORD));
				Rdata[i] = (LONG)dw;
			}
		else if (BytesPerSample == 3)
			for (int i = 0; i<num / 2; i++)
			{
				//fread(&b, sizeof(BYTE), 1, waveFile);
				inFile.read((char*)&b, sizeof(BYTE));
				//fread(&w, sizeof(WORD), 1, waveFile);
				inFile.read((char*)&w, sizeof(WORD));
				Ldata[i] = w + b;
				//fread(&b, sizeof(BYTE), 1, waveFile);
				inFile.read((char*)&b, sizeof(BYTE));
				//fread(&w, sizeof(WORD), 1, waveFile);
				inFile.read((char*)&w, sizeof(WORD));
				Rdata[i] = w + b;
			}
	}

	inFile.close();
	inFile.clear();
}

