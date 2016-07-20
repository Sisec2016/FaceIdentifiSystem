// FaceAttendServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FaceAttendServer.h"
#include "FaceAttendServerDlg.h"
#include "LogBrowseDlg.h"
/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/
/////////////////////////////////////////////////////////////////////////////
// CNVL_TESTDlg dialog

CRITICAL_SECTION g_CriticalSection;
class MyLock
{
public:
	MyLock(){EnterCriticalSection(&g_CriticalSection);};
	~MyLock(){LeaveCriticalSection(&g_CriticalSection);};
};

CNVL_TESTDlg::CNVL_TESTDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNVL_TESTDlg::IDD, pParent)
	, m_bHorizontalImage(FALSE)
{
	//{{AFX_DATA_INIT(CNVL_TESTDlg)
	m_nIdentifyThreshold = 75;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bCapThreadExit=TRUE;
	m_pCapThread=NULL;
	m_nIdentifyStatus=0;

	m_pLogPersonLiveBuf=NULL;
	m_bHorizontalImage=TRUE;

	m_bCapThreadWorking=TRUE;

	m_pIDFeature=NULL;
	m_pIDPhotoBuf=NULL;
	m_nIDPhotoWidth=102;
	m_nIDPhotoHeight=126;
	m_pCardReaderThread=NULL;
	m_bCardReaderThreadExit=FALSE;

	m_nCardReaderPort=1001;

	m_pffi=NULL;

}

void CNVL_TESTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNVL_TESTDlg)
	DDX_Text(pDX, IDC_THRESHOLD, m_nIdentifyThreshold);
	DDV_MinMaxInt(pDX, m_nIdentifyThreshold, 0, 100);
	DDX_Check(pDX, IDC_HORIZONTAL, m_bHorizontalImage);
	//}}AFX_DATA_MAP
	
}

BEGIN_MESSAGE_MAP(CNVL_TESTDlg, CDialog)
	ON_MESSAGE(WM_SAY_SORRY,OnSaySorry)
	//{{AFX_MSG_MAP(CNVL_TESTDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_STUFF_MANAGE, OnStaffManage)
	ON_BN_CLICKED(IDC_CAMERA_CONFIG, OnCameraConfig)
	ON_BN_CLICKED(IDC_HORIZONTAL, OnBnClickedHorizontal)
	ON_BN_CLICKED(IDC_THRESHOLD_APPLY, OnBnClickedThresholdApply)
	//}}AFX_MSG_MAP
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNVL_TESTDlg message handlers

BOOL CNVL_TESTDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	InitializeCriticalSection(&g_CriticalSection);

	m_GuiBmp.LoadBitmap(IDB_GUI1); 
	((CStatic*)GetDlgItem(IDC_GUI))->SetBitmap((HBITMAP)m_GuiBmp);	
	SubButtons();

	//��ʼ��������������������ȶ�SDK
	THFI_Param param;

	param.nMinFaceSize=50;
	param.nRollAngle=45;
	param.bOnlyDetect=true;

	THFI_Create(1,&param);

	EF_Init(1);

	//����������С
	g_nFeatureSize=EF_Size();
	CString str;
	str.Format("size=%d",g_nFeatureSize);
//	AfxMessageBox(str);

	//��������Ϣ
	LoadConfig();

	::CreateDirectory(g_strAppPath+"\\mvp",NULL);

	g_hMainWnd=m_hWnd;

	///////////////
	//������ͷ�豸
	m_pCap[0]=MCapture::Create(&m_capParam[0]);
	if(NULL==m_pCap[0])
	{
		AfxMessageBox("Open WDM camera device failed.");
		ExitSystem();
		return FALSE;
	}
	BOOL bOK=m_pCap[0]->InitCapture();
	if(!bOK)
	{
		AfxMessageBox("Connect WDM camera device Failed");
		delete m_pCap[0];
		m_pCap[0]=NULL;
		ExitSystem();
		return FALSE;
	}

	m_strTopTip="��ˢ���֤";
	m_clTopTip=RGB(0,0,255);

	m_pCap[0]->Play();

//	m_nIdentifyThreshold=75;
//	m_nTimeOut=5;

	//�Ƿ���ʾ������Ϣ�ؼ�
	if(m_nDebug==0)
		GetDlgItem(IDC_SCORE)->ShowWindow(SW_HIDE);
	else
		GetDlgItem(IDC_SCORE)->ShowWindow(SW_SHOW);


	UpdateData(FALSE);
	
	//������Ƶ�����߳�
	m_pCapThread=AfxBeginThread(VideoThreadProc,this);	
	//���������ȶ��߳�
	m_pCompareThread=AfxBeginThread(CompareThreadProc,this);	

	//��ʼ�����֤������ģ��
	m_CardReader.InitModal();
	m_pIDFeature=new BYTE[g_nFeatureSize];
	
	//�������֤�������߳�
	m_pCardReaderThread=AfxBeginThread(CardReaderThreadProc,this);	

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CNVL_TESTDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CNVL_TESTDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CNVL_TESTDlg::LoadConfig()
{
	char szSettingFile[256];
	strcpy(szSettingFile,g_strAppPath);
	strcat(szSettingFile,"\\config.ini");

//default value
	m_capParam[0].szCap.cx=640;
	m_capParam[0].szCap.cy=480;
	m_capParam[0].eType=CAP_WDM;
	m_capParam[0].lIndex=0;

	m_capParam[0].lIndex=GetPrivateProfileInt("camera","id",0,szSettingFile);//WDM�豸ID
	m_capParam[0].szCap.cx=GetPrivateProfileInt("camera","cx",640,szSettingFile);//ͼ��ֱ���cx
	m_capParam[0].szCap.cy=GetPrivateProfileInt("camera","cy",480,szSettingFile);//ͼ��ֱ���cy

	m_nIdentifyThreshold=GetPrivateProfileInt("identify","threshold",75,szSettingFile);//������֤����
	m_nDebug=GetPrivateProfileInt("identify","debug",0,szSettingFile);//�Ƿ���ʾ������Ϣ
	m_nTimeOut=GetPrivateProfileInt("identify","timeout",5,szSettingFile);//������֤��ʱʱ��

	m_nCardReaderPort=GetPrivateProfileInt("cardreader","port",1001,szSettingFile);//���֤�������˿ں�
}


void CNVL_TESTDlg::OnCancel() 
{
	
	if(MessageBox("Really Exit System?          ","Confirm Exit",MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2)==IDYES) 
	{
		ExitSystem();
		CDialog::OnCancel();
	}	
    
}
//�˳�ϵͳ���ͷ������Դ
void CNVL_TESTDlg::ExitSystem()
{
	try
	{
		m_bCardReaderThreadExit=TRUE;
		Sleep(300);
		m_bCapThreadExit=TRUE;
		Sleep(500);
		m_bCompareThreadExit=TRUE;
		Sleep(500);

		if(m_pCap[0])
		{
			m_pCap[0]->Stop();
			delete m_pCap[0];
			m_pCap[0]=NULL;
		}

		m_LogPersonBmp.DeleteObject();	
		m_LogPersonLiveBmp.DeleteObject();

		delete []m_pLogPersonLiveBuf;m_pLogPersonLiveBuf=NULL;
		delete []m_pIDFeature;m_pIDFeature=NULL;
		delete []m_pIDPhotoBuf;m_pIDPhotoBuf=NULL;	
	}
	catch(...)
	{
		m_LogPersonBmp.DeleteObject();	
		m_LogPersonLiveBmp.DeleteObject();	
		delete []m_pLogPersonLiveBuf;m_pLogPersonLiveBuf=NULL;
		delete []m_pIDFeature;m_pIDFeature=NULL;
		delete []m_pIDPhotoBuf;m_pIDPhotoBuf=NULL;		
	}

	DeleteCriticalSection(&g_CriticalSection);

	THFI_Release();
	EF_Release();
	
	m_CardReader.UnInitModal();

	CDialog::OnCancel();

	exit(0);
	
}
void CNVL_TESTDlg::OnClose() 
{
	OnCancel();
}

void CNVL_TESTDlg::OnStaffManage() 
{	
	CLogBrowseDlg dlg;
	dlg.DoModal();
}

//��Ƶ�����̣߳���ȡ����ͷ�豸��֡ͼ�񣬽�����������ͼ����ʾ
UINT CNVL_TESTDlg::VideoThreadProc(LPVOID lpVoid)
{
	::CoInitialize(NULL);
	CNVL_TESTDlg* pDlg=(CNVL_TESTDlg*)lpVoid;

	pDlg->m_bCapThreadExit=FALSE;

	while(!pDlg->m_bCapThreadExit)
	{
		if(!pDlg->m_bCapThreadWorking)
		{
			Sleep(100);
			continue;
		}

		try
		{
			pDlg->ProcessCurrFrame();

		}
		catch(...)
		{
		
		}
		Sleep(30);
	}

	pDlg->m_pCapThread=NULL;
	::CoUninitialize();

	return 1;
}

void CNVL_TESTDlg::ProcessCurrFrame()
{

	GetDlgItem(IDC_VIDEO,&m_hVideoWnd);

	//�б�������֤�Ƿ�ʱʧ��
	if(m_nIdentifyStatus==1)//��ǰΪ����������֤״̬
	{
		CTimeSpan timeSpan=CTime::GetCurrentTime()-m_StartIdentifyTime;
		if(timeSpan.GetSeconds()>=m_nTimeOut)//timeout , face verify failed
		{
			//��ʾ������֤ʧ�ܵ������Ϣ����λ��ǰ״̬
			::SendMessage(g_hMainWnd,WM_SAY_SORRY,(WPARAM)0,(LPARAM)0);

			Sleep(200);
			m_nIdentifyStatus=0;
			m_bCompareThreadWorking=FALSE;
			SetDlgItemText(IDC_VERIFY_INFO,"������֤ʧ��");
			m_strTopTip="������֤ʧ�ܣ�����ˢ���֤!";
			m_clTopTip=RGB(255,0,0);

			//дʶ��ʧ�ܼ�¼
			if(!m_FailedFace.empty())
			{
				Mat imFaceStd;
				resize(m_FailedFace,imFaceStd,Size(m_nIDPhotoWidth,m_nIDPhotoHeight),0,0,INTER_LINEAR);
				WriteLog(m_FailedLog,&imFaceStd);
				m_FailedFace.release();
			}
		}
	}

	long nWidth=m_capParam[0].szCap.cx;
	long nHeight=m_capParam[0].szCap.cy;
	long lLength=nWidth*nHeight*3;

	int i,j;

	BYTE* pCamBuf=new BYTE[lLength];
	
	//��ȡ����ͷͼ��֡
	long lLegthCap=lLength;
	BOOL bGet=m_pCap[0]->GetFrame(pCamBuf, lLegthCap);
	MirrorDIB((LPSTR)pCamBuf, nWidth, nHeight, FALSE,24);
	if(m_bHorizontalImage)
		MirrorDIB((LPSTR)pCamBuf, nWidth, nHeight, TRUE,24);

	//��ǰΪ������֤״̬���������������ȣ�����ֱ����ʾ����.
	if(m_nIdentifyStatus==1)
	{	
		DWORD t1,t2;
		t1=GetTickCount();

		//��֡ͼ������������(�����һ�������������������)
		THFI_FacePos ptfp[1];
		ptfp[0].dwReserved=(DWORD)new BYTE[512];//�������һ���ڴ��Է��������ڲ�����,���������µ�������ȡ�㷨
		int nFace=0;

		nFace=THFI_DetectFace(0,pCamBuf,24,nWidth,nHeight,ptfp,1);

		t2=GetTickCount();
		CString str;
		str.Format("time=%d",t2-t1);
	/////////////////////
		//��⵽����
		if(nFace > 0)
		{
			if(m_nIdentifyStatus==1)
			{
				//ͨ��ȫ���ڴ���������֤�߳̽�������֡���ݣ�
				MyLock lock;//��������ȫ���߳̽����ڴ�
				delete m_pffi;
				m_pffi=new FaceFrameInfo(nWidth,nHeight,&ptfp[0]);
				memcpy(m_pffi->pRgb24Buf,pCamBuf,nWidth*nHeight*3);
			}
			//��֡�ڴ��ϻ������������
			DrawFaceRects(pCamBuf,nWidth,nHeight,&ptfp[0].rcFace,1,RGB(0,255,0),4);
		}
	}

	//��ʾ��Ƶ֡
	//show video frame
	BITMAPINFOHEADER bih;
	ContructBih(nWidth,nHeight,bih);
	MirrorDIB((LPSTR)pCamBuf, nWidth, nHeight, FALSE,24);
//	DrawBmpBuf(bih,pCamBuf,m_hVideoWnd,TRUE);

	DrawBmpBuf1(bih,pCamBuf,m_hVideoWnd,TRUE,(LPCTSTR)m_strTopTip,m_clTopTip);

	delete []pCamBuf;

}
//������֤�߳�(���ϻ�ȡ��Ƶ�����߳����ύ��ȫ������֡���ݣ���������������ȡ���������֤ͼƬ���бȶ�)
UINT CNVL_TESTDlg::CompareThreadProc(LPVOID lpVoid)
{
	CNVL_TESTDlg* pDlg=(CNVL_TESTDlg*)lpVoid;
	
	pDlg->CompareProcRun();

	pDlg->m_pCompareThread=NULL;

	return 0;
}

void CNVL_TESTDlg::CompareProcRun()
{
	m_bCompareThreadExit=FALSE;
	m_bCompareThreadWorking=FALSE;

	while(!m_bCompareThreadExit)
	{
		if(!m_bCompareThreadWorking)
		{
			Sleep(100);
			continue;
		}

		FaceFrameInfo* pffi=NULL;
		if(1)//��ȡȫ������֡����
		{
			MyLock lock;//���������߳̽����ڴ�
			pffi=m_pffi;
			m_pffi=NULL;
		}

		if(pffi==NULL) 
		{
			Sleep(20);
			continue;
		}
		//������֤(��ǰ����ͷ֡�������������֤���������ȶ�)
		CompareProcess(pffi);
		delete pffi;
		Sleep(10);		
	}

}
//������֤�߼�(�õ�ǰ����ͷ���������������֤������������������֤)
void CNVL_TESTDlg::CompareProcess(FaceFrameInfo* pffi)
{

	BYTE* pCamBuf=pffi->pRgb24Buf;
	int nWidth=pffi->m_nWidth;
	int nHeight=pffi->m_nHeight;
	
	BYTE* pFeature=new BYTE[g_nFeatureSize];

	DWORD t1,t2;
	t1=GetTickCount();
	//EF_Extract�Ĳ���pFacialData������һ��������THFI_FacePos,����THFI_FacePos�ĳ�ԱdwReserved�ǺϷ��ڴ�(��THFI_DetectFace���ص������ڲ�����)
	int ret=EF_Extract(0,pCamBuf,nWidth,nHeight,3,(DWORD)pffi->pFaceData,pFeature);
	t2=GetTickCount();

	CString str1;
	str1.Format("time=%d",t2-t1);
//	SetWindowText(str1);

	if(!ret)
	{
		delete []pFeature;
		return;
	}
	
	double nBestScore=0;

	nBestScore=EF_Compare(m_pIDFeature,pFeature)*100+20.0f;//�����ȶ�
	if(nBestScore>=99.99) nBestScore=99.99;

	if(nBestScore>=m_nIdentifyThreshold)//������֤�ɹ�
	{
		//��λ��ǰ����״̬
		m_nIdentifyStatus=0;
		m_bCompareThreadWorking=FALSE;
		delete m_pffi;m_pffi=NULL;

		//���������֤�ɹ�UI��Ϣ
		CString str;
		str.Format("������֤�ɹ�[%s , ���ƶ�: %d%%]",m_idi.strName,(int)nBestScore);//nScore
		SetDlgItemText(IDC_VERIFY_INFO,str);
		m_strTopTip.Format("������֤�ɹ�[%s],��ˢ���֤",m_idi.strName);
		m_clTopTip=RGB(0,255,0);


		//��ȡ��ǰ��Ƶ֡�е���������(�����������С)
		int nFaceWidth=pffi->pFaceData->rcFace.right-pffi->pFaceData->rcFace.left;
		int nFaceHeight=pffi->pFaceData->rcFace.bottom-pffi->pFaceData->rcFace.top;

		BYTE* pFaceBuf=new BYTE[nFaceWidth*nFaceHeight*3];
		::GetPartImage(pCamBuf,nWidth,nHeight,pFaceBuf,pffi->pFaceData->rcFace.left,pffi->pFaceData->rcFace.top,pffi->pFaceData->rcFace.right,pffi->pFaceData->rcFace.bottom);

		BITMAPINFOHEADER bih;
		::ContructBih(nFaceWidth,nFaceHeight,bih);
	    LONG lLineBytes;
		lLineBytes = WIDTHBYTES(nFaceWidth *24);

		//4�ֽڶ���ͼ�񻺳�
		BYTE* pTemp=new BYTE[lLineBytes*nFaceHeight];
		for(int i=0;i<nFaceHeight;i++)
		{
			for(int j=0;j<nFaceWidth;j++)
			{
				pTemp[i*lLineBytes+j*3+0]=pFaceBuf[(nFaceHeight-1-i)*nFaceWidth*3+j*3+0];
				pTemp[i*lLineBytes+j*3+1]=pFaceBuf[(nFaceHeight-1-i)*nFaceWidth*3+j*3+1];
				pTemp[i*lLineBytes+j*3+2]=pFaceBuf[(nFaceHeight-1-i)*nFaceWidth*3+j*3+2];
			}
		}
		//����������������̶���С(102*126)
		BYTE* pTemp1=new BYTE[WIDTHBYTES(m_nIDPhotoWidth *24)*m_nIDPhotoHeight];
		::ResampleBmp(bih,pTemp,this->m_nIDPhotoWidth,this->m_nIDPhotoHeight,pTemp1);

		//��ʾ��������
		m_LogPersonLiveBmp.DeleteObject();
		GetCBitmap(&bih, pTemp1,m_LogPersonLiveBmp);
		((CStatic*)GetDlgItem(IDC_RESULT_PHOTO))->SetBitmap((HBITMAP)m_LogPersonLiveBmp);

		//write log
		Mat imFace(nFaceHeight,nFaceWidth,CV_8UC3);
		imFace.data=pFaceBuf;
		Mat imFaceStd;
		resize(imFace,imFaceStd,Size(m_nIDPhotoWidth,m_nIDPhotoHeight),0,0,INTER_LINEAR);

		SYSTEMTIME	stTime = { NULL };
		GetLocalTime(&stTime);

		Log_Info li;
		li.logTime=stTime;

		strcpy(li.sName,m_idi.strName);
		strcpy(li.sSex,m_idi.strSex);
		strcpy(li.sNation,m_idi.strNation);
		strcpy(li.sBirthday,m_idi.strBirthday);
		strcpy(li.sAddress,m_idi.strAddress);
		strcpy(li.sId,m_idi.strID);
		strcpy(li.sStartDate,m_idi.strStartDate);
		strcpy(li.sEndDate,m_idi.strEndDate);
		strcpy(li.sDepartment,m_idi.strDepartment);

		li.fScore=nBestScore;
		li.nType=1;

		WriteLog(li,&imFaceStd);

		delete []pFaceBuf;
		delete []pTemp;
		delete []pTemp1;
	}
	else//��¼��֤ʶ����Ϣ
	{
		//��ȡ��ǰ��Ƶ֡�е���������(�����������С)
		int nFaceWidth=pffi->pFaceData->rcFace.right-pffi->pFaceData->rcFace.left;
		int nFaceHeight=pffi->pFaceData->rcFace.bottom-pffi->pFaceData->rcFace.top;

		BYTE* pFaceBuf=new BYTE[nFaceWidth*nFaceHeight*3];
		::GetPartImage(pCamBuf,nWidth,nHeight,pFaceBuf,pffi->pFaceData->rcFace.left,pffi->pFaceData->rcFace.top,pffi->pFaceData->rcFace.right,pffi->pFaceData->rcFace.bottom);

		m_FailedFace.release();
		m_FailedFace.create(nFaceHeight,nFaceWidth,CV_8UC3);
		memcpy(m_FailedFace.data,pFaceBuf,nFaceWidth*nFaceHeight*3);

		SYSTEMTIME	stTime = { NULL };
		GetLocalTime(&stTime);

		m_FailedLog.logTime=stTime;

		strcpy(m_FailedLog.sName,m_idi.strName);
		strcpy(m_FailedLog.sSex,m_idi.strSex);
		strcpy(m_FailedLog.sNation,m_idi.strNation);
		strcpy(m_FailedLog.sBirthday,m_idi.strBirthday);
		strcpy(m_FailedLog.sAddress,m_idi.strAddress);
		strcpy(m_FailedLog.sId,m_idi.strID);
		strcpy(m_FailedLog.sStartDate,m_idi.strStartDate);
		strcpy(m_FailedLog.sEndDate,m_idi.strEndDate);
		strcpy(m_FailedLog.sDepartment,m_idi.strDepartment);

		m_FailedLog.fScore=nBestScore;
		m_FailedLog.nType=0;

		delete []pFaceBuf;

	}
	CString str;
	str.Format("score=%f",nBestScore);//nScore
	///SetWindowText(str);
	SetDlgItemText(IDC_SCORE,str);


	delete []pFeature;

}
//���֤�������̣߳����ϵ�֡�����֤�豸�����Ƿ�������֤
UINT CNVL_TESTDlg::CardReaderThreadProc(LPVOID lpVoid)
{
	CNVL_TESTDlg* pDlg=(CNVL_TESTDlg*)lpVoid;
	
	pDlg->CardReaderProcRun();

	pDlg->m_pCompareThread=NULL;

	return 0;
}
//���֤����������
void CNVL_TESTDlg::CardReaderProcRun()
{
	m_bCardReaderThreadExit=FALSE;

	BOOL bOpen=m_CardReader.OpenDevice(m_nCardReaderPort);//open card reader
	if(!bOpen)
	{
		AfxMessageBox("�����֤������ʧ��.");
	}

	while(!m_bCardReaderThreadExit)
	{
		//�����ǰ���ڽ�������ʶ��ȶԣ��Ͳ���⵱ǰ���֤������״̬
		if(m_bCompareThreadWorking==TRUE) 
		{
			Sleep(300);
			continue;
		}
		

		if(m_CardReader.ValidateCard())
		{
			BYTE* pBuffer=new BYTE[64*1024];
			int len=64*1024;
			//��ȡ���֤��Ϣ
			int ret=m_CardReader.ReadCard(m_idi,pBuffer,len);

			if(ret==0)
			{
	
				int i;

				//�������֤ͼƬ��Ϣ
				vector<BYTE> pBuffs;
				pBuffs.reserve(len);
				for(i=0;i<len;i++)
					pBuffs.push_back(pBuffer[i]);

				Mat im;
				im = imdecode(Mat(pBuffs),CV_LOAD_IMAGE_COLOR);

				m_nIDPhotoWidth=im.cols;
				m_nIDPhotoHeight=im.rows;

				if(m_pIDPhotoBuf==NULL)
				{
					m_pIDPhotoBuf=new BYTE[m_nIDPhotoWidth*m_nIDPhotoHeight*3];
				}
				memcpy(m_pIDPhotoBuf,im.data,m_nIDPhotoWidth*m_nIDPhotoHeight*3);
				int nWidth=m_nIDPhotoWidth;
				int nHeight=m_nIDPhotoHeight;

				//�������,�����һ�������������������
				THFI_FacePos ptfp[1];
				ptfp[0].dwReserved=(DWORD)new BYTE[512];//�������һ���ڴ��Է��������ڲ�����,���������µ�������ȡ�㷨
				int nFace=THFI_DetectFace(0,m_pIDPhotoBuf,24,nWidth,nHeight,ptfp,1);
					
				if(nFace)
				{				
					//��ȡ��������
					memset(m_pIDFeature,0,g_nFeatureSize);
				
					int ret=EF_Extract(0,m_pIDPhotoBuf,nWidth,nHeight,3,(DWORD)&ptfp[0],m_pIDFeature);
				
					if(ret)
					{
						m_FailedFace.release();
						WriteIDPhoto(&im,(LPCTSTR)m_idi.strID);
						//��λ��ǰ����״̬��Ϣ�����������߳̽���������֤
						m_bCompareThreadWorking=TRUE;
						m_StartIdentifyTime=CTime::GetCurrentTime();
						m_nIdentifyStatus=1;
						CString str;

						SetDlgItemText(IDC_NAME,m_idi.strName);
						SetDlgItemText(IDC_SEX,m_idi.strSex);
						SetDlgItemText(IDC_NATION,m_idi.strNation);
						SetDlgItemText(IDC_BIRTHDAY,m_idi.strBirthday);
						SetDlgItemText(IDC_ADDRESS,m_idi.strAddress);
						SetDlgItemText(IDC_ID,m_idi.strID);
						str.Format("%s-%s",m_idi.strStartDate,m_idi.strEndDate);
						SetDlgItemText(IDC_VALID_DAY,str);
						SetDlgItemText(IDC_DEPARTMENT,m_idi.strDepartment);

						SetDlgItemText(IDC_VERIFY_INFO,"������֤��...");
						m_strTopTip="������֤��...";
						m_clTopTip=RGB(0,0,255);


						//��ʾ���֤ͼƬ��Ϣ������
						m_LogPersonBmp.DeleteObject();
						
						BITMAPINFOHEADER bih;
						::ContructBih(m_nIDPhotoWidth,m_nIDPhotoHeight,bih);
					    LONG lLineBytes;
						lLineBytes = WIDTHBYTES(m_nIDPhotoWidth *24);
						BYTE* pTemp=new BYTE[lLineBytes*m_nIDPhotoHeight];
						for(i=0;i<m_nIDPhotoHeight;i++)
						{
							for(int j=0;j<m_nIDPhotoWidth;j++)
							{
								pTemp[i*lLineBytes+j*3+0]=m_pIDPhotoBuf[(m_nIDPhotoHeight-1-i)*nWidth*3+j*3+0];
								pTemp[i*lLineBytes+j*3+1]=m_pIDPhotoBuf[(m_nIDPhotoHeight-1-i)*nWidth*3+j*3+1];
								pTemp[i*lLineBytes+j*3+2]=m_pIDPhotoBuf[(m_nIDPhotoHeight-1-i)*nWidth*3+j*3+2];
							}
						}
						
						GetCBitmap(&bih, pTemp,m_LogPersonBmp);
						((CStatic*)GetDlgItem(IDC_ID_PHOTO))->SetBitmap((HBITMAP)m_LogPersonBmp);
						delete []pTemp;


						m_LogPersonLiveBmp.DeleteObject();
						m_LogPersonLiveBmp.LoadBitmap(IDB_FACE); 
						((CStatic*)GetDlgItem(IDC_RESULT_PHOTO))->SetBitmap((HBITMAP)m_LogPersonLiveBmp);
					}
				}
				delete [](BYTE*)ptfp[0].dwReserved;
			}
			delete []pBuffer;
		}
		Sleep(300);
	}

	m_CardReader.CloseDevice();//close card reader

}
//������֤��ʱʶ����ʾ��Ϣ
LRESULT CNVL_TESTDlg::OnSaySorry(WPARAM wParam,LPARAM lParam)
{
	m_LogPersonLiveBmp.DeleteObject();
	m_LogPersonLiveBmp.LoadBitmap(IDB_SORRY); 
	((CStatic*)GetDlgItem(IDC_RESULT_PHOTO))->SetBitmap((HBITMAP)m_LogPersonLiveBmp);

	return 0;
}
//��������ͷ����
void CNVL_TESTDlg::OnCameraConfig() 
{
	if(m_pCap[0])
		m_pCap[0]->DeviceConfig(0);
}

//ˮƽ��������ͷ����
void CNVL_TESTDlg::OnBnClickedHorizontal()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData();
	g_bHorizontalImage=m_bHorizontalImage;
}
//����������֤����
void CNVL_TESTDlg::OnBnClickedThresholdApply()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	int nThreshold=GetDlgItemInt(IDC_THRESHOLD);

	char szSettingFile[256];
	strcpy(szSettingFile,g_strAppPath);
	strcat(szSettingFile,"\\config.ini");
	char t[16]="";

	if(m_nIdentifyThreshold!=nThreshold)
	{
		m_nIdentifyThreshold=nThreshold;

		sprintf(t,"%d",m_nIdentifyThreshold);
		WritePrivateProfileString("identify","threshold",t,szSettingFile);
	}


}


HBRUSH CNVL_TESTDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	int nID = pWnd->GetDlgCtrlID();

	switch(nCtlColor)
	{

//	case CTLCOLOR_DLG:
//	case CTLCOLOR_EDIT:
	case CTLCOLOR_STATIC:

		pDC->SetBkMode(TRANSPARENT);
		static HBRUSH hbrgui=::CreateSolidBrush(RGB(0, 162, 232));
		return hbrgui;

//		return HBRUSH(GetStockObject(HOLLOW_BRUSH));//NULL_BRUSH; 
	}

	return hbr;
}

void CNVL_TESTDlg::SubButtons()
{
	COLORREF fg = RGB(0, 0, 0);
	COLORREF bg = RGB(0, 162, 232);

	m_button1.SubclassDlgItem(IDC_CAMERA_CONFIG, this);
	m_button2.SubclassDlgItem(IDC_STUFF_MANAGE, this);
	m_button3.SubclassDlgItem(IDC_THRESHOLD_APPLY, this);
	m_button4.SubclassDlgItem(IDCANCEL, this);

	m_button1.SetColour(fg, bg);
	m_button2.SetColour(fg, bg);
	m_button3.SetColour(fg, bg);
	m_button4.SetColour(fg, bg);

}
void CNVL_TESTDlg::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���


}
