#include "stdafx.h"
#include "OperatorWorker.h"


OperatorWorker::OperatorWorker():
_activity(this, &OperatorWorker::getPhotography)
{
	m_pCap = NULL;
	m_nFeatureSize = 0;
}


OperatorWorker::~OperatorWorker()
{
}

void OperatorWorker::InitCapture()
{
	m_nFeatureSize = EF_Size();

	m_capParam.szCap.cx = 640;
	m_capParam.szCap.cy = 480;
	m_capParam.eType = CAP_WDM;
	m_capParam.lIndex = 0;

	m_pCap = CCapture::Create(&m_capParam);

	if (NULL == m_pCap)
	{
		std::cout << "Open camera device failed." << endl;
		return;
	}
	BOOL bOK = m_pCap->InitCapture();
	if (!bOK)
	{
		delete m_pCap;
		m_pCap = NULL;
		return;
	}
}

void OperatorWorker::startCapture()
{
	m_pCap->Play();
}

void OperatorWorker::start()
{
	_activity.start();
}
void OperatorWorker::stop()
{
	_activity.stop();
	_activity.wait();
}

void OperatorWorker::stopCapture()
{
	m_pCap->Stop();
}

void OperatorWorker::getPhotography()
{
	while (!_activity.isStopped())
	{
		long nWidth = m_capParam.szCap.cx;
		long nHeight = m_capParam.szCap.cy;
		long lLength = nWidth*nHeight * 3;

		BYTE* pDataBuf = NULL;

		BYTE* pCamBuf = new BYTE[lLength];
		BOOL b = m_pCap->GetFrame(pCamBuf, lLength);
		if (!b)
		{
			delete[]pCamBuf;
			return;
		}
		MirrorDIB((LPSTR)pCamBuf, nWidth, nHeight, TRUE, 24);
		MirrorDIB((LPSTR)pCamBuf, nWidth, nHeight, FALSE, 24);

		BYTE* pFeature = new BYTE[m_nFeatureSize];

		THFI_FacePos ptfp[1];
		int k;
		for (k = 0; k < 1; k++)
		{
			ptfp[k].dwReserved = (DWORD)new BYTE[512];
		}
		int nFace = THFI_DetectFace(0, pCamBuf, 24, nWidth, nHeight, ptfp, 1);//only process one face

		//CString str;
		//
		//
		//if (nFace > 0)
		//{
		//	RECT* pFace = new RECT[nFace];
		//	BOOL bMouth = TRUE;
		//	for (int i = 0; i<nFace; ++i)
		//	{
		//		pFace[i] = ptfp[i].rcFace;
		//	}
		//
		//	if (m_bEnroll)//enroll
		//	{
		//		int ret = EF_Extract(0, pCamBuf, nWidth, nHeight, 3, (DWORD)&ptfp[0], m_pEnrollFeature);
		//
		//		m_bEnroll = FALSE;
		//		UpdateData(FALSE);
		//		AfxMessageBox("Enroll OK.");
		//	}
		//	else if (m_bIdentify)//compare
		//	{
		//		int ret = EF_Extract(0, pCamBuf, nWidth, nHeight, 3, (DWORD)&ptfp[0], pFeature);
		//
		//		float score = EF_Compare(pFeature, m_pEnrollFeature);
		//
		//		CString str;
		//		str.Format("score=%d", (int)(score * 100));
		//		SetDlgItemText(IDC_SCORE, str);
		//	}
		//
		//
		//	//draw all face rectangle
		//	//TiDrawFaceRects(pCamBuf, nWidth, nHeight, pFace, nFace, RGB(0, 255, 0), 4);
		//
		//	//only draw the first face(eye,mouth...) 
		//	RECT rcLeft, rcRight, rcMouth, rcNose;
		//	int offset = 5;
		//	//left eye rectangle
		//	rcLeft.left = ptfp[0].ptLeftEye.x - offset;
		//	rcLeft.top = ptfp[0].ptLeftEye.y - offset;
		//	rcLeft.right = ptfp[0].ptLeftEye.x + offset;
		//	rcLeft.bottom = ptfp[0].ptLeftEye.y + offset;
		//
		//	//right eye rectangle
		//	rcRight.left = ptfp[0].ptRightEye.x - offset;
		//	rcRight.top = ptfp[0].ptRightEye.y - offset;
		//	rcRight.right = ptfp[0].ptRightEye.x + offset;
		//	rcRight.bottom = ptfp[0].ptRightEye.y + offset;
		//
		//	//mouth rectangle
		//	rcMouth.left = ptfp[0].ptMouth.x - offset;
		//	rcMouth.top = ptfp[0].ptMouth.y - offset;
		//	rcMouth.right = ptfp[0].ptMouth.x + offset;
		//	rcMouth.bottom = ptfp[0].ptMouth.y + offset;
		//
		//	//nose rectangle
		//	rcNose.left = ptfp[0].ptNose.x - offset;
		//	rcNose.top = ptfp[0].ptNose.y - offset;
		//	rcNose.right = ptfp[0].ptNose.x + offset;
		//	rcNose.bottom = ptfp[0].ptNose.y + offset;
		//
		//	//draw
		//	if (0)
		//	{
		//		//TiDrawFaceRects(pCamBuf, nWidth, nHeight, &rcLeft, 1, RGB(0, 255, 0), 4);
		//		//TiDrawFaceRects(pCamBuf, nWidth, nHeight, &rcRight, 1, RGB(0, 255, 0), 4);
		//		//TiDrawFaceRects(pCamBuf, nWidth, nHeight, &rcMouth, 1, RGB(0, 255, 0), 4);
		//		//TiDrawFaceRects(pCamBuf, nWidth, nHeight, &rcNose, 1, RGB(0, 255, 0), 4);
		//	}
		//	else
		//	{
		//		RECT temp[4];
		//		temp[0] = rcLeft;
		//		temp[1] = rcRight;
		//		temp[2] = rcMouth;
		//		temp[3] = rcNose;
		//
		//		TiDrawFaceRects(pCamBuf, nWidth, nHeight, temp, 4, RGB(0, 255, 0), 4);
		//	}
		//
		//	delete[]pFace;
		//
		//} //if
		//else
		//{
		//	//	SetWindowText("No Face!");
		//}

		for (k = 0; k < 1; k++)
		{
			delete[](BYTE*)ptfp[k].dwReserved;
		}

		MirrorDIB((LPSTR)pCamBuf, nWidth, nHeight, FALSE, 24);
		//�ͷ���Դ,��ʾ��Ƶ֡
		BITMAPINFOHEADER bih;
		ContructBih(nWidth, nHeight, bih);

		Picture picture;
		picture.m_pCamBuf = pCamBuf;
		picture.m_bih = bih;
		
		m_DataQueue.push(picture);

		//	DrawBmpBuf(bih, pCamBuf, hFrameWnd, TRUE);
		// 	delete[]pCamBuf;
	 	delete[]pFeature;
		

		Thread::sleep(50);
	}//while	
}

// void OperatorWorker::OnTimer(UINT nIDEvent)
// {
// 	long nWidth = m_capParam.szCap.cx;
// 	long nHeight = m_capParam.szCap.cy;
// 	long lLength = nWidth*nHeight * 3;
// 
// 	HWND hFrameWnd;
// 	GetDlgItem(IDC_VIDEO, &hFrameWnd);
// 
// 
// 	BYTE* pDataBuf = NULL;
// 
// 	BYTE* pCamBuf = new BYTE[lLength];
// 	BOOL b = m_pCap->GetFrame(pCamBuf, lLength);
// 	if (!b)
// 	{
// 		delete[]pCamBuf;
// 		return;
// 	}
// 	MirrorDIB((LPSTR)pCamBuf, nWidth, nHeight, TRUE, 24);
// 	MirrorDIB((LPSTR)pCamBuf, nWidth, nHeight, FALSE, 24);
// 
// 	BYTE* pFeature = new BYTE[m_nFeatureSize];
// 
// 	THFI_FacePos ptfp[1];
// 	int k;
// 	for (k = 0; k < 1; k++)
// 	{
// 		ptfp[k].dwReserved = (DWORD)new BYTE[512];
// 	}
// 	int nFace = THFI_DetectFace(0, pCamBuf, 24, nWidth, nHeight, ptfp, 1);//only process one face
// 
// 	CString str;
// 
// 
// 	if (nFace > 0)
// 	{
// 		RECT* pFace = new RECT[nFace];
// 		BOOL bMouth = TRUE;
// 		for (int i = 0; i < nFace; ++i)
// 		{
// 			pFace[i] = ptfp[i].rcFace;
// 		}
// 
// 		if (m_bEnroll)//enroll
// 		{
// 			int ret = EF_Extract(0, pCamBuf, nWidth, nHeight, 3, (DWORD)&ptfp[0], m_pEnrollFeature);
// 
// 			m_bEnroll = FALSE;
// 			UpdateData(FALSE);
// 			AfxMessageBox("Enroll OK.");
// 		}
// 		else if (m_bIdentify)//compare
// 		{
// 			int ret = EF_Extract(0, pCamBuf, nWidth, nHeight, 3, (DWORD)&ptfp[0], pFeature);
// 
// 			float score = EF_Compare(pFeature, m_pEnrollFeature);
// 
// 			CString str;
// 			str.Format("score=%d", (int)(score * 100));
// 			SetDlgItemText(IDC_SCORE, str);
// 		}
// 
// 
// 		//draw all face rectangle
// 		TiDrawFaceRects(pCamBuf, nWidth, nHeight, pFace, nFace, RGB(0, 255, 0), 4);
// 
// 		//only draw the first face(eye,mouth...) 
// 		RECT rcLeft, rcRight, rcMouth, rcNose;
// 		int offset = 5;
// 		//left eye rectangle
// 		rcLeft.left = ptfp[0].ptLeftEye.x - offset;
// 		rcLeft.top = ptfp[0].ptLeftEye.y - offset;
// 		rcLeft.right = ptfp[0].ptLeftEye.x + offset;
// 		rcLeft.bottom = ptfp[0].ptLeftEye.y + offset;
// 
// 		//right eye rectangle
// 		rcRight.left = ptfp[0].ptRightEye.x - offset;
// 		rcRight.top = ptfp[0].ptRightEye.y - offset;
// 		rcRight.right = ptfp[0].ptRightEye.x + offset;
// 		rcRight.bottom = ptfp[0].ptRightEye.y + offset;
// 
// 		//mouth rectangle
// 		rcMouth.left = ptfp[0].ptMouth.x - offset;
// 		rcMouth.top = ptfp[0].ptMouth.y - offset;
// 		rcMouth.right = ptfp[0].ptMouth.x + offset;
// 		rcMouth.bottom = ptfp[0].ptMouth.y + offset;
// 
// 		//nose rectangle
// 		rcNose.left = ptfp[0].ptNose.x - offset;
// 		rcNose.top = ptfp[0].ptNose.y - offset;
// 		rcNose.right = ptfp[0].ptNose.x + offset;
// 		rcNose.bottom = ptfp[0].ptNose.y + offset;
// 
// 		//draw
// 		if (0)
// 		{
// 			TiDrawFaceRects(pCamBuf, nWidth, nHeight, &rcLeft, 1, RGB(0, 255, 0), 4);
// 			TiDrawFaceRects(pCamBuf, nWidth, nHeight, &rcRight, 1, RGB(0, 255, 0), 4);
// 			TiDrawFaceRects(pCamBuf, nWidth, nHeight, &rcMouth, 1, RGB(0, 255, 0), 4);
// 			TiDrawFaceRects(pCamBuf, nWidth, nHeight, &rcNose, 1, RGB(0, 255, 0), 4);
// 		}
// 		else
// 		{
// 			RECT temp[4];
// 			temp[0] = rcLeft;
// 			temp[1] = rcRight;
// 			temp[2] = rcMouth;
// 			temp[3] = rcNose;
// 
// 			TiDrawFaceRects(pCamBuf, nWidth, nHeight, temp, 4, RGB(0, 255, 0), 4);
// 		}
// 
// 		delete[]pFace;
// 
// 	}
// 	else
// 	{
// 		//	SetWindowText("No Face!");
// 	}
// 
// 	for (k = 0; k < 1; k++)
// 	{
// 		delete[](BYTE*)ptfp[k].dwReserved;
// 	}
// 
// 	MirrorDIB((LPSTR)pCamBuf, nWidth, nHeight, FALSE, 24);
// 	//�ͷ���Դ,��ʾ��Ƶ֡
// 	BITMAPINFOHEADER bih;
// 	ContructBih(nWidth, nHeight, bih);
// 
// 	DrawBmpBuf(bih, pCamBuf, hFrameWnd, TRUE);
// 	delete[]pCamBuf;
// 	delete[]pFeature;
// 
// 	CDialog::OnTimer(nIDEvent);
// }


#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)
BOOL OperatorWorker::MirrorDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, BOOL bDirection, int nImageBits)
{
	// ָ��Դͼ���ָ��
	LPSTR	lpSrc;
	// ָ��Ҫ���������ָ��
	LPSTR	lpDst;
	// ָ����ͼ���ָ��
	LPSTR	lpBits;
	HLOCAL	hBits;
	// ѭ������
	LONG	i;
	LONG	j;
	int nBits;//ÿ����ռ��λ��
	// ͼ��ÿ�е��ֽ���
	LONG lLineBytes;
	// ����ͼ��ÿ�е��ֽ���
	lLineBytes = WIDTHBYTES(lWidth *nImageBits);
	// ��ʱ�����ڴ棬�Ա���һ��ͼ��
	hBits = LocalAlloc(LHND, lLineBytes);
	if (hBits == NULL)
	{
		// �����ڴ�ʧ��
		return FALSE;
	}
	// �����ڴ�
	lpBits = (char *)LocalLock(hBits);
	int nStep = nImageBits / 8;
	long lCenter = lWidth / 2 * nStep;
	// �жϾ���ʽ
	if (bDirection)
	{
		// ˮƽ����
		// ���ͼ��ÿ�н��в���
		for (i = 0; i < lHeight; i++)
		{
			// ���ÿ��ͼ����벿�ֽ��в���
			for (j = 0; j < lCenter; j += nStep)
			{
				for (nBits = 0; nBits < nStep; nBits++)
				{
					lpSrc = (char *)lpDIBBits + lLineBytes * i + lCenter - j + nBits;
					lpDst = (char *)lpDIBBits + lLineBytes * i + lCenter + j + nBits;
					*lpBits = *lpDst;
					*lpDst = *lpSrc;
					*lpSrc = *lpBits;
				}

			}

		}
	}
	else
	{
		// ��ֱ����
		// ����ϰ�ͼ����в���
		for (i = 0; i < lHeight / 2; i++)
		{
			// ָ������i����������ָ��
			lpSrc = (char *)lpDIBBits + lLineBytes * i;
			// ָ���i����������ָ��
			lpDst = (char *)lpDIBBits + lLineBytes * (lHeight - i - 1);
			// ����һ�У����ΪlWidth
			memcpy(lpBits, lpDst, lLineBytes);
			// ��������i�����ظ��Ƶ���i��
			memcpy(lpDst, lpSrc, lLineBytes);
			// ����i�����ظ��Ƶ�������i��
			memcpy(lpSrc, lpBits, lLineBytes);

		}
	}
	// �ͷ��ڴ�
	LocalUnlock(hBits);
	LocalFree(hBits);
	// ����
	return TRUE;
}

void OperatorWorker::ContructBih(int nWidth, int nHeight, BITMAPINFOHEADER& bih)
{
	bih.biSize = 40; 						// header size
	bih.biWidth = nWidth;
	bih.biHeight = nHeight;
	bih.biPlanes = 1;
	bih.biBitCount = 24;					// RGB encoded, 24 bit
	bih.biCompression = BI_RGB;			// no compression
	bih.biSizeImage = nWidth*nHeight * 3;
	bih.biXPelsPerMeter = 0;
	bih.biYPelsPerMeter = 0;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;
}

queue<Picture> OperatorWorker::getDataQueue()const
{
	return m_DataQueue;
}


