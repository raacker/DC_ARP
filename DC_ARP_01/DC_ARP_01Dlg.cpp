
// DC_ARP_01Dlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "DC_ARP_01.h"
#include "DC_ARP_01Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();
// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CDC_ARP_01Dlg 대화 상자




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
	DDX_Control(pDX, IDC_ARP_SEND_IP, m_unDstIPAddr);
	DDX_Control(pDX, IDC_NICARD_COMBO, m_ComboEnetName);
	DDX_Control(pDX, IDC_OWN_IP_ADDRESS, m_unSrcIPAddr);
	DDX_Control(pDX, IDC_GRATUITOUS_ADDRESS_BOX, m_unGratuitousAddresss);
	DDX_Control(pDX, IDC_PROXY_ARP_ENTRY_LIST, m_proxyARPEntry);
}

BEGIN_MESSAGE_MAP(CDC_ARP_01Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	ON_BN_CLICKED(IDC_ARP_ITEM_DELETE_BUTTON, &CDC_ARP_01Dlg::OnBnClickedArpItemDeleteButton)
	ON_BN_CLICKED(IDC_ARP_ALL_DELETE_BUTTON, &CDC_ARP_01Dlg::OnBnClickedArpAllDeleteButton)
	ON_BN_CLICKED(IDC_ARP_SEND_BUTTON, &CDC_ARP_01Dlg::OnBnClickedArpSendButton)
	ON_BN_CLICKED(IDC_WINDOW_OK_BUTTON, &CDC_ARP_01Dlg::OnBnClickedWindowOkButton)
	ON_BN_CLICKED(IDC_ARP_SETTING_BUTTON, &CDC_ARP_01Dlg::OnBnClickedArpSettingButton)
	ON_BN_CLICKED(IDC_GRATUITOUS_SEND_BUTTON, &CDC_ARP_01Dlg::OnBnClickedGratuitousSendButton)
	ON_CBN_SELCHANGE(IDC_NICARD_COMBO, &CDC_ARP_01Dlg::OnCbnSelchangeNicardCombo)

	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDC_ARP_01Dlg 메시지 처리기

BOOL CDC_ARP_01Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	SetRegstryMessage( ) ;
	SetDlgState(IPC_INITIALIZING);
	SetDlgState(CFT_COMBO_SET);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CDC_ARP_01Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ( nID == SC_CLOSE )
	{
		if ( MessageBox( "Are you sure ?", 
			"Question", 
			MB_YESNO | MB_ICONQUESTION ) 
			== IDNO )
			return ;
		else EndofProcess( ) ;
	}

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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CDC_ARP_01Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CDC_ARP_01Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDC_ARP_01Dlg::OnSendMessage()  
{
	// TODO: Add your control notification handler code here
	UpdateData( TRUE ) ;

	SetTimer(1,3000,NULL);

	SendData( ) ;

	UpdateData( FALSE ) ;
}
void CDC_ARP_01Dlg::OnButtonAddrSet() 
{
	// TODO: Add your control notification handler code here
	UpdateData( TRUE ) ;
	unsigned char src_ip[4];
	unsigned char dst_ip[4];
	unsigned char src_mac[12];
	unsigned char dst_mac[12];


	if ( !m_unDstEnetAddr || 
		!m_unSrcEnetAddr  )
	{
		MessageBox( "주소를 설정 오류발생", 
			"경고", 
			MB_OK | MB_ICONSTOP ) ;

		return ;
	}

	if ( m_bSendReady ){
		SetDlgState( IPC_ADDR_RESET ) ;
		SetDlgState( IPC_INITIALIZING ) ;
	}
	else{
		m_unSrcIPAddr.GetAddress(src_ip[0],src_ip[1],src_ip[2],src_ip[3]);
		m_unDstIPAddr.GetAddress(dst_ip[0],dst_ip[1],dst_ip[2],dst_ip[3]);

		m_IP->SetSrcIPAddress(src_ip);
		m_IP->SetDstIPAddress(dst_ip);

		sscanf(m_unSrcEnetAddr, "%02x%02x%02x%02x%02x%02x", &src_mac[0],&src_mac[1],&src_mac[2],&src_mac[3],&src_mac[4],&src_mac[5]);
		sscanf(m_unDstEnetAddr, "%02x%02x%02x%02x%02x%02x", &dst_mac[0],&dst_mac[1],&dst_mac[2],&dst_mac[3],&dst_mac[4],&dst_mac[5]);

		m_ETH->SetEnetSrcAddress(src_mac);
		m_ETH->SetEnetDstAddress(dst_mac);

		int nIndex = m_ComboEnetName.GetCurSel();
		m_NI->SetAdapterNumber(nIndex);

		m_NI->PacketStartDriver();

		SetDlgState( IPC_ADDR_SET ) ;
		SetDlgState( IPC_READYTOSEND ) ;		
	}

	m_bSendReady = !m_bSendReady ;
}

void CDC_ARP_01Dlg::SendData()
{
	CString MsgHeader ; 
	if ( m_unDstEnetAddr == (unsigned int)0xff )
		MsgHeader.Format( "[%s:BROADCAST] ", m_unSrcEnetAddr ) ;
	else
		MsgHeader.Format( "[%s:%s] ", m_unSrcEnetAddr, m_unDstEnetAddr ) ;
	MsgHeader.Format();

	int nlength = m_stMessage.GetLength();
	unsigned char* ppayload = new unsigned char[nlength+1];
	memcpy(ppayload,(unsigned char*)(LPCTSTR)m_stMessage,nlength);
	ppayload[nlength] = '\0';

	m_APP->Send(ppayload,m_stMessage.GetLength());
}

BOOL CDC_ARP_01Dlg::Receive(unsigned char *ppayload)
{
	CString Msg;
	int len_ppayload = strlen((char *)ppayload); 

	unsigned char *GetBuff = (unsigned char *)malloc(len_ppayload);
	memset(GetBuff,0,len_ppayload);
	memcpy(GetBuff,ppayload,len_ppayload);
	GetBuff[len_ppayload] = '\0';
	
	if( m_unSrcEnetAddr == (unsigned int)0xff )
		Msg.Format("[%s:BROADCAST] %s",m_unDstEnetAddr,(char *)GetBuff);
	else
		Msg.Format("[%s:%s] %s",m_unDstEnetAddr,m_unSrcEnetAddr,(char *)GetBuff);

	KillTimer(1);
	return TRUE ;
}

void CDC_ARP_01Dlg::SetDlgState(int state) // 다이얼로그 초기화 부분
{
	UpdateData( TRUE ) ;
	int i;
	CString device_description;

	CButton*			pARPItemDeleteButton = (CButton*) GetDlgItem( IDC_ARP_ITEM_DELETE_BUTTON ) ;
	CButton*			pARPAllDeleteButton = (CButton*) GetDlgItem( IDC_ARP_ALL_DELETE_BUTTON ) ;
	CButton*			pWindowOkButton = (CButton*) GetDlgItem( IDC_WINDOW_OK_BUTTON ) ;
	CButton*			pWindowCloseButton = (CButton*) GetDlgItem( IDC_WINDOW_CLOSE_BUTTON ) ;
	CButton*			pARPSendButton = (CButton*) GetDlgItem( IDC_ARP_SEND_BUTTON ) ;
	CButton*			pProxyDeleteButton = (CButton*) GetDlgItem( IDC_PROXY_DELETE_BUTTON ) ;
	CButton*			pProxyAddButton = (CButton*) GetDlgItem( IDC_PROXY_ADD_BUTTON  ) ;
	CButton*			pGratuitousSendButton = (CButton*) GetDlgItem( IDC_GRATUITOUS_SEND_BUTTON ) ;
	CButton*			pARPSettingButton = (CButton*) GetDlgItem( IDC_ARP_SETTING_BUTTON ) ;

	CIPAddressCtrl*		pARPSendIP = (CIPAddressCtrl*) GetDlgItem( IDC_ARP_SEND_IP );
	CIPAddressCtrl*		pOWNIPAddress = (CIPAddressCtrl*) GetDlgItem( IDC_OWN_IP_ADDRESS );

	CEdit*				pGratuitousARPAddress = (CEdit*) GetDlgItem( IDC_GRATUITOUS_ADDRESS_BOX ) ;

	CComboBox*			pEnetNameCombo = (CComboBox*)GetDlgItem( IDC_NICARD_COMBO );

	switch( state )
	{
	case IPC_INITIALIZING : // 첫 화면 세팅
		pARPAllDeleteButton->EnableWindow( FALSE ) ;
		pARPItemDeleteButton->EnableWindow( FALSE ) ;
		pWindowOkButton->EnableWindow( FALSE );
		pARPSendButton->EnableWindow( FALSE );
		pProxyDeleteButton->EnableWindow( FALSE );
		pProxyAddButton->EnableWindow( FALSE );
		pGratuitousSendButton->EnableWindow( FALSE );
		pARPSettingButton->EnableWindow( FALSE );
		pARPSendIP->EnableWindow( FALSE );
		pOWNIPAddress->EnableWindow( FALSE );
		pGratuitousARPAddress->EnableWindow ( FALSE );
		m_proxyARPEntry.EnableWindow( FALSE );
		m_ArpTable.EnableWindow( FALSE ) ;
		break ;
	case IPC_READYTOSEND : // Send(S)버튼을 눌렀을 때 세팅
		pARPAllDeleteButton->EnableWindow( TRUE ) ;
		pARPItemDeleteButton->EnableWindow( TRUE ) ;
		pWindowOkButton->EnableWindow( TRUE );
		pARPSendButton->EnableWindow( TRUE );
		pProxyDeleteButton->EnableWindow( TRUE );
		pProxyAddButton->EnableWindow( TRUE );
		pGratuitousSendButton->EnableWindow( TRUE );
		pARPSettingButton->EnableWindow( TRUE );
		pARPSendIP->EnableWindow( TRUE );
		pOWNIPAddress->EnableWindow( TRUE );
		pGratuitousARPAddress->EnableWindow ( TRUE );
		m_proxyARPEntry.EnableWindow( TRUE );
		m_ArpTable.EnableWindow( TRUE ) ;
		break ;
	case IPC_WAITFORACK :	break ;
	case IPC_ERROR :		break ;
	case IPC_ADDR_SET :	// Setting 버튼을 눌렀을 때
		pARPSettingButton->SetWindowText( "Reset" ) ; 
		pARPSendIP->EnableWindow( FALSE ) ;
		pOWNIPAddress->EnableWindow( FALSE ) ;
		pEnetNameCombo->EnableWindow( FALSE );
		m_NI->m_thrdSwitch = TRUE;
		break ;
	case IPC_ADDR_RESET : // Reset 버튼을 눌렀을 때
		pARPSettingButton->SetWindowText( "Setting" ) ; 
		pSrcEdit->EnableWindow( TRUE ) ;
		if ( !pChkButton->GetCheck( ) )
			pDstEdit->EnableWindow( TRUE ) ;
		pChkButton->EnableWindow( TRUE ) ;
		pSrcIPEdit->EnableWindow( TRUE );
		pDstIPEdit->EnableWindow( TRUE );
		pEnetNameCombo->EnableWindow( TRUE );
		m_NI->m_thrdSwitch = FALSE;
		break ;
	case CFT_COMBO_SET :
		for(i=0;i<NI_COUNT_NIC;i++){
			if(!m_NI->GetAdapterObject(i))
				break;
			device_description = m_NI->GetAdapterObject(i)->description;
			device_description.Trim();
			pEnetNameCombo->AddString(device_description);
			pEnetNameCombo->SetCurSel(0);

			PPACKET_OID_DATA OidData;
			OidData = (PPACKET_OID_DATA)malloc(sizeof(PACKET_OID_DATA));
			OidData->Oid = 0x01010101;
			OidData->Length = 6;

			LPADAPTER adapter = PacketOpenAdapter(m_NI->GetAdapterObject(0)->name);
			PacketRequest(adapter, FALSE, OidData);

			m_unSrcEnetAddr.Format("%.2X%.2X%.2X%.2X%.2X%.2X",OidData->Data[0],OidData->Data[1],OidData->Data[2],OidData->Data[3],OidData->Data[4],OidData->Data[5]) ;
		}
		break;
	}

	UpdateData( FALSE ) ;
}
void CDC_ARP_01Dlg::OnBnClickedArpItemDeleteButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int index = m_ArpTable.GetCurSel();
	if(index != LB_ERR) {
		m_ArpTable.DeleteString(index);
	}
}


void CDC_ARP_01Dlg::OnBnClickedArpAllDeleteButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_ArpTable.ResetContent();
}


void CDC_ARP_01Dlg::OnBnClickedArpSendButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SendData();
}


void CDC_ARP_01Dlg::OnBnClickedWindowOkButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_APP_EXIT,0);
}


void CDC_ARP_01Dlg::OnBnClickedArpSettingButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDC_ARP_01Dlg::OnBnClickedGratuitousSendButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDC_ARP_01Dlg::OnCbnSelchangeNicardCombo()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
