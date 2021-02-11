
// QRBarcodeReaderDlg.h : header file
//

#pragma once


// CQRBarcodeReaderDlg dialog
class CQRBarcodeReaderDlg : public CDialogEx
{
// Construction
public:
	CQRBarcodeReaderDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_QRBARCODEREADER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	BITMAPINFO* m_pBitmapInfo;
	Mat m_matImage;
	VideoCapture* capture;
	int CreateBitmapInfo(int w, int h, int bpp);
	int DrawImage();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	int StartCamera();
	CEdit mEdt1;
	afx_msg void OnBnClickedBtnUrl();
};
