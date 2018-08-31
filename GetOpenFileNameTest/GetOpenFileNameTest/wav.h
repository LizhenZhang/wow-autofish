#ifndef __WAV__
#define __WAV__
#include <string>
#include <Windows.h>
using namespace std;
class WavFile
{
public:
	WavFile();
	~WavFile();

	virtual void Serialize(string fileName);

	int   num;               // ������
	LONG *data = NULL;                // ��������
	LONG *Ldata = NULL, *Rdata = NULL;     // ˫��������
	WORD  BytesPerSample, wChannel;     // һ���������ֽ���,  ������
	DWORD SampleRate;
	DWORD ByteRate;
	WORD BlockAlign;
	WORD wBitsPerSample;   //����λ�� 
	DWORD Len;          // ���ݿ��С,  BYTE 
	WORD extendSize;


private:

};
#endif // !__WAV__

