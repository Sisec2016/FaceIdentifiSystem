
// RtspTestDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "RtspTest.h"
#include "RtspTestDlg.h"
#include "afxdialogex.h"
#include <string>
#include "CvvImage.h"
#include <regex>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CRtspTestDlg �Ի���



CRtspTestDlg::CRtspTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRtspTestDlg::IDD, pParent)
	, m_IP(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_rtspClient = new RtspClient();
	m_bStart = false;
}

void CRtspTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_IP);
}

BEGIN_MESSAGE_MAP(CRtspTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CRtspTestDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CRtspTestDlg::OnBnClickedButton2)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON3, &CRtspTestDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CRtspTestDlg::OnBnClickedButton4)
END_MESSAGE_MAP()


// CRtspTestDlg ��Ϣ�������

BOOL CRtspTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	cv::Mat srcMat(640, 480, CV_8UC3);
	DrawPicToHDC(srcMat, IDC_VIDEO);

	CString csIp = "192.168.0.15";
	SetDlgItemText(IDC_EDIT1, csIp);
	SetDlgItemInt(IDC_EDIT2, 554);
	CString csAdmin = "admin";
	SetDlgItemText(IDC_EDIT3, csAdmin);
	SetDlgItemInt(IDC_EDIT5, 1);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CRtspTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CRtspTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CRtspTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CRtspTestDlg::OnBnClickedButton1()
{
	
	//std::string szRtspURL("rtsp://184.72.239.149/vod/mp4://BigBuckBunny_175k.mov");
	//std::string szRtspURL("rtsp://192.168.0.15:554/user=admin&password=&channel=1&stream=0.sdp");
	//std::string szRtspURL("rtsp://172.16.0.180:554/user=admin&password=&channel=1&stream=0.sdp");
	
		
	GetDlgItemText(IDC_EDIT1, m_IP);
	CString csPort;
	GetDlgItemText(IDC_EDIT2, csPort);
	CString csUser;
	GetDlgItemText(IDC_EDIT3, csUser);
	CString csPwd;
	GetDlgItemText(IDC_EDIT4, csPwd);
	CString csChannel;
	GetDlgItemText(IDC_EDIT5, csChannel);

	if (m_IP.IsEmpty())
	{
		MessageBoxA( "ip��ַ����Ϊ��", "", MB_OK);
		return;
	}
	if (!checkIp(m_IP))
	{
		MessageBoxA("ip��ʽ����ȷ��", "", MB_OK);
		return;
	}

	if (m_bStart)
	{
		MessageBoxA("���Ȱ�ֹͣ��ť��", "", MB_OK);
		return;
	}
	m_bStart = true;

	std::string szRtspURL("rtsp://");
	szRtspURL += m_IP;
	szRtspURL += ":";
	szRtspURL += csPort;
	szRtspURL += "/user=";
	szRtspURL += csUser;
	szRtspURL += "&password=";
	szRtspURL += csPwd;
	szRtspURL += "&channel=";
	szRtspURL += csChannel;
	szRtspURL += "&stream = 0.sdp";

	
	if (!m_rtspClient->start(szRtspURL.c_str(), (ThreadCallback)CRtspTestDlg::RealFrameCBK, this))
	{
		MessageBoxA("�޷����豸��", "", MB_OK);
		m_bStart = false;
		return;
	}	
}

void CRtspTestDlg::DrawPicToHDC(cv::Mat srcMat, UINT ID)
{
	HWND hFrameWnd;
	GetDlgItem(ID, &hFrameWnd);
	RtspClient::DrawBmpBuf(srcMat, hFrameWnd);
	/*IplImage * Image;
	CvSize Size = cvSize(srcMat.rows, srcMat.cols);
	Image = cvCreateImage(Size, IPL_DEPTH_8U, 3);
	memcpy(Image->imageData, srcMat.data, srcMat.rows*srcMat.cols * 3);
	Image->widthStep = srcMat.rows * 3;
	Image->origin = 0;

	CDC *pDC = GetDlgItem(ID)->GetDC();
	HDC hDC = pDC->GetSafeHdc();
	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);
	CvvImage cimg;
	cimg.CopyOf(Image, 1);

	cimg.DrawToHDC(hDC, &rect);
	ReleaseDC(pDC);
	cvReleaseImage(&Image);*/
}

void CRtspTestDlg::RealFrameCBK(const char * lpRealFrame, UINT nSize, int nWidth, int nHeight, UINT nUser)
{
	
	CRtspTestDlg * ptThis = (CRtspTestDlg *)nUser;

	cv::Mat srcMat1(nWidth, nHeight, CV_8UC3, (char *)lpRealFrame);
	//RtspClient::MirrorDIB((LPSTR)srcMat1.data, nWidth, nHeight, TRUE, 24);
	RtspClient::MirrorDIB((LPSTR)srcMat1.data, nWidth, nHeight, FALSE, 24);
	ptThis->DrawPicToHDC(srcMat1, IDC_VIDEO);
}

void CRtspTestDlg::OnBnClickedButton2()
{
	if (!m_bStart)
		return;
	m_bStart = false;
	m_rtspClient->stop();

	cv::Mat srcMat(640, 480, CV_8UC3);
	srcMat = cv::Scalar(255, 255, 255);
	DrawPicToHDC(srcMat, IDC_VIDEO);
}


void CRtspTestDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	if (m_rtspClient)
	{
		delete m_rtspClient;
		m_rtspClient = NULL;
	}	
}

bool CRtspTestDlg::checkIp(CString IP)
{	
	std::string strIP = IP.GetBuffer(0);
	std::regex pattern("((25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.){3}(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])");
	std::smatch base_match;
	bool valid = std::regex_match(strIP, base_match, pattern);	

	return valid;
}

void CRtspTestDlg::OnBnClickedButton3()
{
	GetDlgItemText(IDC_EDIT1, m_IP);
	CString csPort;
	GetDlgItemText(IDC_EDIT2, csPort);
	CString csUser;
	GetDlgItemText(IDC_EDIT3, csUser);
	CString csPwd;
	GetDlgItemText(IDC_EDIT4, csPwd);
	CString csChannel;
	GetDlgItemText(IDC_EDIT5, csChannel);

	if (m_IP.IsEmpty())
	{
		MessageBoxA("ip��ַ����Ϊ��", "", MB_OK);
		return;
	}
	if (!checkIp(m_IP))
	{
		MessageBoxA("ip��ʽ����ȷ��", "", MB_OK);
		return;
	}

	if (m_bStart)
	{
		MessageBoxA("���Ȱ�ֹͣ��ť��", "", MB_OK);
		return;
	}
	m_bStart = true;

	std::string szRtspURL("rtsp://");
	szRtspURL += csUser;	
	szRtspURL += ":";
	szRtspURL += csPwd;
	szRtspURL += "@";
	szRtspURL += m_IP;
	szRtspURL += ":";
	szRtspURL += csPort;
	szRtspURL += "/h264/ch";
	szRtspURL += csChannel;
	szRtspURL += "/main/av_stream";


	if (!m_rtspClient->start(szRtspURL.c_str(), (ThreadCallback)CRtspTestDlg::RealFrameCBK, this))
	{
		MessageBoxA("�޷����豸��", "", MB_OK);
		m_bStart = false;
		return;
	}
}


void CRtspTestDlg::OnBnClickedButton4()
{
	GetDlgItemText(IDC_EDIT1, m_IP);
	CString csPort;
	GetDlgItemText(IDC_EDIT2, csPort);
	CString csUser;
	GetDlgItemText(IDC_EDIT3, csUser);
	CString csPwd;
	GetDlgItemText(IDC_EDIT4, csPwd);
	CString csChannel;
	GetDlgItemText(IDC_EDIT5, csChannel);

	if (m_IP.IsEmpty())
	{
		MessageBoxA("ip��ַ����Ϊ��", "", MB_OK);
		return;
	}
	if (!checkIp(m_IP))
	{
		MessageBoxA("ip��ʽ����ȷ��", "", MB_OK);
		return;
	}

	if (m_bStart)
	{
		MessageBoxA("���Ȱ�ֹͣ��ť��", "", MB_OK);
		return;
	}
	m_bStart = true;

	std::string szRtspURL("rtsp://");
	szRtspURL += csUser;
	szRtspURL += ":";
	szRtspURL += csPwd;
	szRtspURL += "@";
	szRtspURL += m_IP;
	szRtspURL += ":";
	szRtspURL += csPort;
	szRtspURL += "/cam/realmonitor?channel=";
	szRtspURL += csChannel;
	szRtspURL += "&subtype=0";


	if (!m_rtspClient->start(szRtspURL.c_str(), (ThreadCallback)CRtspTestDlg::RealFrameCBK, this))
	{
		MessageBoxA("�޷����豸��", "", MB_OK);
		m_bStart = false;
		return;
	}
}
