
// RtspTestDlg.h : ͷ�ļ�
//

#pragma once

#include "RtspClient.h"

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
public:
	static void __stdcall RealFrameCBK(LPBYTE lpRealFrame, UINT nSize, int nWidth, int nHeight, UINT nUser);
	void DrawPicToHDC(cv::Mat srcMat, UINT ID);
};
