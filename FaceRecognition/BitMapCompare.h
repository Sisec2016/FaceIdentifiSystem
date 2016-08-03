#pragma once

#include "Poco/Runnable.h"
#include "Poco/Thread.h"
#include "QMFileSqlite.h"

struct CapBitmapData
{
	CapBitmapData()
	{
		data = nullptr;
		size = 0;
		width = 0;
		height = 0;
	}

	~CapBitmapData()
	{
		if (nullptr != data)
		{
			delete[] data;
			data = nullptr;
			size = 0;
			width = 0;
			height = 0;
		}
	}

	CapBitmapData(const CapBitmapData& other)
	{
		size = other.size;
		width = other.width;
		height = other.height;
		data = nullptr;
		setPacketData(other.data, other.size);
	}

	CapBitmapData(const BYTE* data, const long len, const long BitmapWidth, const long BitmapHeight)
	{
		size = len;
		width = BitmapWidth;
		height = BitmapHeight;
		data = nullptr;
		setPacketData((BYTE*)data, len);
	}


	void setPacketData(BYTE* pData, int len)
	{
		if (nullptr != data)
		{
			delete[] data;
			data = nullptr;
			size = 0;
		}

		if (len > 0)
		{
			data = new BYTE[len];
			size = len;
			memcpy(data, pData, len);
		}
	}

	CapBitmapData& operator= (const CapBitmapData& other)
	{
		if (&other == this)
		{
			return *this;
		}
		size = other.size;
		width = other.width;
		height = other.height;
		setPacketData(other.data, other.size);
		return *this;
	}

	//
	BYTE* getData() const
	{
		return data;
	}
	long getDataSize() const
	{
		return size;
	}

	long getWidth() const
	{
		return width;
	}

	long getHeigth() const
	{
		return height;
	}
	
	BYTE *		data;			//λͼ������
	long		size;			//λͼ�Ĵ�С
	long        width;          //λͼ�Ŀ�
	long        height;         //λͼ�ĸ�
};

class BitMapCompare : public Poco::Runnable
{
public:
	BitMapCompare(void *pthis);
	~BitMapCompare();
	void run();	
	void onEvent(const void* pSender, bool& arg);

private:
	void CompareBitmap(BYTE *pFirst, BYTE *pSecond, long nFirstWidth, long nSecondWidth, long nFirstHeight, long nSecondHeight, float& fRet);
	Mat LoadBmpFile1(std::string strFilePath);
	void getCamBuf();
	void writeDB();
	
	void getUserInfo();

private:
	bool             _break;
	void *           _pWnd;
	vector<readUserInfo> _vUserInfo;
};