
// DC_ARP_01Dlg.cpp : ±¸Çö ÆÄÀÏ
//

#include "stdafx.h"
#include "DC_ARP_01.h"
#include "DC_ARP_01Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ÀÀ¿ë ÇÁ·Î±×·¥ Á¤º¸¿¡ »ç¿ëµÇ´Â CAboutDlg ´ëÈ­ »óÀÚÀÔ´Ï´Ù.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();
//¤¾¤¾È÷ÆR
// ´ëÈ­ »óÀÚ µ¥ÀÌÅÍÀÔ´Ï´Ù.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV Áö¿øÀÔ´Ï´Ù.

// ±¸ÇöÀÔ´Ï´Ù.
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


// CDC_ARP_01Dlg ´ëÈ­ »óÀÚ




CDC_ARP_01Dlg::CDC_ARP_01Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDC_ARP_01Dlg::IDD, pParent),
	CBaseLayer( "ArpDlg" ),
	m_bSendReady(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_LayerMgr.AddLayer( this ) ;
	m_LayerMgr.AddLayer( new CNILayer( "NI" ) ) ;
	m_LayerMgr.AddLayer( new CEthernetLayer( "Ethernet" ) ) ;
	m_LayerMgr.AddLayer( new CARPLayer( "ARP" ) );
	m_LayerMgr.AddLayer( new CIPLayer( "IP" ) );
	m_LayerMgr.AddLayer( new CApplicationLayer( "APP" ) ) ;

	m_LayerMgr.ConnectLayers("NI ( *Ethernet ( *ARP ( *IP ( *APP ) ) ) )");

	m_APP = (CApplicationLayer *)m_LayerMgr.GetLayer("APP");
	m_IP = (CIPLayer *)m_LayerMgr.GetLayer("IP");
	m_ARP = (CARPLayer *)m_LayerMgr.GetLayer("ARP");
	m_ETH = (CEthernetLayer *)m_LayerMgr.GetLayer("Ethernet");
	m_NI = (CNILayer *)m_LayerMgr.GetLayer("NI");
}

void CDC_ARP_01Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ARP_CACHE_TABLE_LIST, m_ArpTable);
	DDX_Control(pDX, IDC_ARP_SEND_IP, m_DstIPAddr);
}

BEGIN_MESSAGE_MAP(CDC_ARP_01Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_ARP_ITEM_DELETE_BUTTON, &CDC_ARP_01Dlg::OnBnClickedArpItemDeleteButton)
	ON_BN_CLICKED(IDC_ARP_ALL_DELETE_BUTTON, &CDC_ARP_01Dlg::OnBnClickedArpAllDeleteButton)
	ON_BN_CLICKED(IDC_ARP_SEND_BUTTON, &CDC_ARP_01Dlg::OnBnClickedArpSendButton)
	ON_BN_CLICKED(IDC_WINDOW_OK_BUTTON, &CDC_ARP_01Dlg::OnBnClickedWindowOkButton)
END_MESSAGE_MAP()


// CDC_ARP_01Dlg ¸Þ½ÃÁö Ã³¸®±â

BOOL CDC_ARP_01Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ½Ã½ºÅÛ ¸Þ´º¿¡ "Á¤º¸..." ¸Þ´º Ç×¸ñÀ» Ãß°¡ÇÕ´Ï´Ù.

	// IDM_ABOUTBOX´Â ½Ã½ºÅÛ ¸í·É ¹üÀ§¿¡ ÀÖ¾î¾ß ÇÕ´Ï´Ù.
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

	// ÀÌ ´ëÈ­ »óÀÚÀÇ ¾ÆÀÌÄÜÀ» ¼³Á¤ÇÕ´Ï´Ù. ÀÀ¿ë ÇÁ·Î±×·¥ÀÇ ÁÖ Ã¢ÀÌ ´ëÈ­ »óÀÚ°¡ ¾Æ´Ò °æ¿ì¿¡´Â
	//  ÇÁ·¹ÀÓ¿öÅ©°¡ ÀÌ ÀÛ¾÷À» ÀÚµ¿À¸·Î ¼öÇàÇÕ´Ï´Ù.
	SetIcon(m_hIcon, TRUE);			// Å« ¾ÆÀÌÄÜÀ» ¼³Á¤ÇÕ´Ï´Ù.
	SetIcon(m_hIcon, FALSE);		// ÀÛÀº ¾ÆÀÌÄÜÀ» ¼³Á¤ÇÕ´Ï´Ù.

	// TODO: ¿©±â¿¡ Ãß°¡ ÃÊ±âÈ­ ÀÛ¾÷À» Ãß°¡ÇÕ´Ï´Ù.

	return TRUE;  // Æ÷Ä¿½º¸¦ ÄÁÆ®·Ñ¿¡ ¼³Á¤ÇÏÁö ¾ÊÀ¸¸é TRUE¸¦ ¹ÝÈ¯ÇÕ´Ï´Ù.
}

void CDC_ARP_01Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// ´ëÈ­ »óÀÚ¿¡ ÃÖ¼ÒÈ­ ´ÜÃß¸¦ Ãß°¡ÇÒ °æ¿ì ¾ÆÀÌÄÜÀ» ±×¸®·Á¸é
//  ¾Æ·¡ ÄÚµå°¡ ÇÊ¿äÇÕ´Ï´Ù. ¹®¼­/ºä ¸ðµ¨À» »ç¿ëÇÏ´Â MFC ÀÀ¿ë ÇÁ·Î±×·¥ÀÇ °æ¿ì¿¡´Â
//  ÇÁ·¹ÀÓ¿öÅ©¿¡¼­ ÀÌ ÀÛ¾÷À» ÀÚµ¿À¸·Î ¼öÇàÇÕ´Ï´Ù.

void CDC_ARP_01Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ±×¸®±â¸¦ À§ÇÑ µð¹ÙÀÌ½º ÄÁÅØ½ºÆ®ÀÔ´Ï´Ù.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Å¬¶óÀÌ¾ðÆ® »ç°¢Çü¿¡¼­ ¾ÆÀÌÄÜÀ» °¡¿îµ¥¿¡ ¸ÂÃä´Ï´Ù.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ¾ÆÀÌÄÜÀ» ±×¸³´Ï´Ù.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// »ç¿ëÀÚ°¡ ÃÖ¼ÒÈ­µÈ Ã¢À» ²ô´Â µ¿¾È¿¡ Ä¿¼­°¡ Ç¥½ÃµÇµµ·Ï ½Ã½ºÅÛ¿¡¼­
//  ÀÌ ÇÔ¼ö¸¦ È£ÃâÇÕ´Ï´Ù.
HCURSOR CDC_ARP_01Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CDC_ARP_01Dlg::OnBnClickedArpItemDeleteButton()
{
	// TODO: ¿©±â¿¡ ÄÁÆ®·Ñ ¾Ë¸² Ã³¸®±â ÄÚµå¸¦ Ãß°¡ÇÕ´Ï´Ù.

}


void CDC_ARP_01Dlg::OnBnClickedArpAllDeleteButton()
{
	// TODO: ¿©±â¿¡ ÄÁÆ®·Ñ ¾Ë¸² Ã³¸®±â ÄÚµå¸¦ Ãß°¡ÇÕ´Ï´Ù.

}


void CDC_ARP_01Dlg::OnBnClickedArpSendButton()
{
	// TODO: ¿©±â¿¡ ÄÁÆ®·Ñ ¾Ë¸² Ã³¸®±â ÄÚµå¸¦ Ãß°¡ÇÕ´Ï´Ù.

}


void CDC_ARP_01Dlg::OnBnClickedWindowOkButton()
{
	// TODO: ¿©±â¿¡ ÄÁÆ®·Ñ ¾Ë¸² Ã³¸®±â ÄÚµå¸¦ Ãß°¡ÇÕ´Ï´Ù.
	AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_APP_EXIT,0);
}
