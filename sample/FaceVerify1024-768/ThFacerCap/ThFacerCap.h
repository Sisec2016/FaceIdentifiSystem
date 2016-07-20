

#ifndef __TICATPURE_H__
#define __TICATPURE_H__


#ifndef TIEXPORT
//	#define TIEXPORT		__declspec(dllexport)
//#else
	#define TIEXPORT		__declspec(dllimport)
#endif

enum ECapType
{
	CAP_INVALID	= 0,			//	��Ч��
	CAP_WDM		= 1,			//	WDM �豸
	CAP_IP		= 3,			//	Sony IP Camera
};


struct SCapParam
{
	ECapType	eType;			//	�豸����
	SIZE		szCap;			//	�ɼ��ߴ�
	union
	{
		LONG	lIndex;			//	WDM  �豸�е����
		char	szIP[128];		//	Sony IP Camera �� IP ��ַ
	};
	DWORD		dwSingle;		//	��ʽ
	BYTE		bReserve[16];	//	����
};

//	�ɼ�Ʒ��
#define		CAP_QUALITY_MINVALUE	-100
#define		CAP_QUALITY_MAXVALUE	100
#define		CAP_QUALITY_RANGE		(CAP_QUALITY_MAXVALUE - CAP_QUALITY_MINVALUE)

#define		CAP_QUALITY_BRIGHT		0x0001
#define		CAP_QUALITY_CONST		0x0002
#define		CAP_QUALITY_HUE			0x0004
#define		CAP_QUALITY_SATURA		0x0008

#define		CAP_QUALITY_SHARP   	0x0010
#define		CAP_QUALITY_GAMMA		0x0020
#define		CAP_QUALITY_COLOR		0x0040
#define		CAP_QUALITY_WBALANCE	0x0080
#define		CAP_QUALITY_BACKLIGHTC	0x0100

#define		CAP_QUALITY_COMPRESS	0x1000
#define		CAP_QUALITY_FRAMERATE	0x2000

struct SCapQuality
{
	WORD		wFlag;
	//	ȡֵ��Χ: -100 ~ +100, Flag: ���� CAP_QUALITY_XXXXX
	SHORT		sBrightness;//����
	SHORT		sConstant;//�Աȶ�
	SHORT		sHue;//ɫ��
	SHORT		sSaturation;//���Ͷ�

	SHORT       sSharpness;//������
	SHORT       sGamma;//٤��
	SHORT       sColorEnable;//������ɫ
	SHORT       sWhiteBalance;//�׹�ƽ��
	SHORT       sBacklightCompensation;//���Ա�
	//
	SHORT		sCompress;
	SHORT		sFrameRate;
};

#define MsgTiCapture	WM_USER + 0x100


//////////////////////////////////////////////////////////////////////////
//	Base Capture

class TIEXPORT MCapture 
{
public:
	static LONG GetCaptureNum(ECapType eType);		//	ȡĳ�����豸����
	static MCapture* Create(SCapParam* pParam);		//	�����豸ʵ��

public:
	MCapture(SCapParam* pParam);
	virtual ~MCapture(void);

	virtual BOOL InitCapture(void) = 0;				//	��ʼ���豸
	virtual BOOL Play(BOOL bVideo = TRUE) = 0;		//	�����豸
	virtual void Stop(void) = 0;					//	ֹͣ�豸
	virtual BOOL SetQuality(SCapQuality* pQuality) = 0;			//	����ͼ������
	virtual BOOL GetFrame(BYTE* pFrame, LONG& lFrameSize) = 0;	//	ȡһ֡ͼ��
	virtual BOOL DeviceConfig(int nType)=0;

protected:
	SCapParam	m_Param;

	
private:
	MCapture(MCapture& cap);	//	����

};


#endif	//	__TICATPURE_H__
