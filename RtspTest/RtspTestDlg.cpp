
// RtspTestDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "RtspTest.h"
#include "RtspTestDlg.h"
#include "afxdialogex.h"
#include <string>
#include "CvvImage.h"

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
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRtspTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRtspTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CRtspTestDlg::OnBnClickedButton1)
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
	RtspClient::init();
	//std::string szRtspURL("rtsp://184.72.239.149/vod/mp4://BigBuckBunny_175k.mov");
	std::string szRtspURL("rtsp://192.168.0.15:554/user=admin&password=&channel=1&stream=0.sdp");
	//std::string szRtspURL("rtsp://172.16.0.180:554/user=admin&password=&channel=1&stream=0.sdp");
	m_rtspClient->start(szRtspURL.c_str(), (DownloadCallback)CRtspTestDlg::RealFrameCBK, this);
	RtspClient::uninit();
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

void CRtspTestDlg::RealFrameCBK(LPBYTE lpRealFrame, UINT nSize, int nWidth, int nHeight, UINT nUser)
{
	CRtspTestDlg * ptThis = (CRtspTestDlg *)nUser;

	cv::Mat srcMat1(nWidth, nHeight, CV_8UC3, (char *)lpRealFrame);
	//RtspClient::MirrorDIB((LPSTR)srcMat1.data, nWidth, nHeight, TRUE, 24);
	RtspClient::MirrorDIB((LPSTR)srcMat1.data, nWidth, nHeight, FALSE, 24);
	ptThis->DrawPicToHDC(srcMat1, IDC_VIDEO);
}