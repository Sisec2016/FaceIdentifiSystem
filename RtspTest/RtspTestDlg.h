
// RtspTestDlg.h : ͷ�ļ�
//

#pragma once

#include "RtspClient.h"
#include "afxwin.h"

// CRtspTestDlg �Ի���
class CRtspTestDlg : public CDialogEx
{
// ����
public:
	CRtspTestDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_RTSPTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
private:
	RtspClient *m_rtspClient;
	CString m_IP;
	bool  m_bStart;
public:
	static void __stdcall RealFrameCBK(const char *  lpRealFrame, UINT nSize, int nWidth, int nHeight, UINT nUser);
	void DrawPicToHDC(cv::Mat srcMat, UINT ID);
	afx_msg void OnBnClickedButton2();
	afx_msg void OnDestroy();	
	
private:
	bool checkIp(CString IP);
public:
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
};
