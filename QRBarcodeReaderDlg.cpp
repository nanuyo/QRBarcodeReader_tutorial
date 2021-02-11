
// QRBarcodeReaderDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "QRBarcodeReader.h"
#include "QRBarcodeReaderDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CQRBarcodeReaderDlg dialog



CQRBarcodeReaderDlg::CQRBarcodeReaderDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_QRBARCODEREADER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CQRBarcodeReaderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, mEdt1);
}

BEGIN_MESSAGE_MAP(CQRBarcodeReaderDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(ID_BTN_URL, &CQRBarcodeReaderDlg::OnBnClickedBtnUrl)
END_MESSAGE_MAP()


// CQRBarcodeReaderDlg message handlers

BOOL CQRBarcodeReaderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CQRBarcodeReaderDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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
		StartCamera();
		CDialogEx::OnPaint();
	}
	
	
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CQRBarcodeReaderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}






int CQRBarcodeReaderDlg::CreateBitmapInfo(int w, int h, int bpp)
{
	if (m_pBitmapInfo != NULL)
	{
		delete[] m_pBitmapInfo;
		m_pBitmapInfo = NULL;
	}

	if (bpp == 8)
		m_pBitmapInfo = (BITMAPINFO*)new BYTE[sizeof(BITMAPINFO) + 255 * sizeof(RGBQUAD)];
	else // 24 or 32bit
		m_pBitmapInfo = (BITMAPINFO*)new BYTE[sizeof(BITMAPINFO)];

	m_pBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_pBitmapInfo->bmiHeader.biPlanes = 1;
	m_pBitmapInfo->bmiHeader.biBitCount = bpp;
	m_pBitmapInfo->bmiHeader.biCompression = BI_RGB;
	m_pBitmapInfo->bmiHeader.biSizeImage = 0;
	m_pBitmapInfo->bmiHeader.biXPelsPerMeter = 0;
	m_pBitmapInfo->bmiHeader.biYPelsPerMeter = 0;
	m_pBitmapInfo->bmiHeader.biClrUsed = 0;
	m_pBitmapInfo->bmiHeader.biClrImportant = 0;

	if (bpp == 8)
	{
		for (int i = 0; i < 256; i++)
		{
			m_pBitmapInfo->bmiColors[i].rgbBlue = (BYTE)i;
			m_pBitmapInfo->bmiColors[i].rgbGreen = (BYTE)i;
			m_pBitmapInfo->bmiColors[i].rgbRed = (BYTE)i;
			m_pBitmapInfo->bmiColors[i].rgbReserved = 0;
		}
	}

	m_pBitmapInfo->bmiHeader.biWidth = w;
	m_pBitmapInfo->bmiHeader.biHeight = -h;
	return 0;
}


int CQRBarcodeReaderDlg::DrawImage()
{
	CClientDC dc(GetDlgItem(IDC_PICTURE));
	CRect rect;
	
	GetDlgItem(IDC_PICTURE)->GetClientRect(&rect);

	SetStretchBltMode(dc.GetSafeHdc(), COLORONCOLOR);
	StretchDIBits(dc.GetSafeHdc(), 0, 0, rect.Width(), rect.Height(), 0, 0, m_matImage.cols, m_matImage.rows, m_matImage.data, m_pBitmapInfo, DIB_RGB_COLORS, SRCCOPY);

	// create a reader
	ImageScanner scanner;

	// configure the reader
	//scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);

	Mat grey;
	CString a;
	cvtColor(m_matImage, grey, COLOR_BGR2GRAY);
	uchar* raw = (uchar*)grey.data;
	int width = m_matImage.cols;
	int height = m_matImage.rows;

	// wrap image data
	Image image(width, height, "Y800", raw, width * height);

	// scan the image for barcodes
	int n = scanner.scan(image);
	dc.SetTextColor(RGB(255, 25, 2));
	a.Format(_T("No Code"));
	// extract results
	for (Image::SymbolIterator symbol = image.symbol_begin();
		symbol != image.symbol_end();
		++symbol)
	{
		dc.SetTextColor(RGB(2, 25, 255));
		string str = symbol->get_data();
		mEdt1.SetWindowTextW(CString(str.c_str()));
		a.Format(_T("OK: %s"), CString(str.c_str()));
	}

	// clean up
	image.set_data(NULL, 0);
	dc.DrawText(a, -1, &rect, DT_LEFT | DT_WORDBREAK);
	

	return 0;
}


void CQRBarcodeReaderDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (!capture->isOpened())
	{
		return;
	}

	if (nIDEvent == 1000)
	{
		if (!capture->read(m_matImage))
		{
			return;
		}

		CreateBitmapInfo(m_matImage.cols, m_matImage.rows, m_matImage.channels() * 8);
		DrawImage();
	}

	CDialogEx::OnTimer(nIDEvent);
}


int CQRBarcodeReaderDlg::StartCamera()
{
	capture = new VideoCapture(0);

	if (!capture->isOpened())
	{
		printf("캠을 열수 없습니다. \n");
		return FALSE;
	}

	SetTimer(1000, 100, NULL);
	return 0;
}


void CQRBarcodeReaderDlg::OnBnClickedBtnUrl()
{
	CString edtStr;
	mEdt1.GetWindowTextW(edtStr);
	ShellExecute(NULL, TEXT("open"), edtStr, NULL, NULL, SW_SHOW);
}
