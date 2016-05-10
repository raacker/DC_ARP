
// DC_ARP_01Dlg.cpp : ���� ����
//

#include "stdafx.h"
#include "DC_ARP_01.h"
#include "DC_ARP_01Dlg.h"
#include "afxdialogex.h"
#include "ARPLayer.h"
#include "proxyDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();
// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
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


// CDC_ARP_01Dlg ��ȭ ����




CDC_ARP_01Dlg::CDC_ARP_01Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDC_ARP_01Dlg::IDD, pParent),
	CBaseLayer( "ArpDlg" ),
	m_bSendReady(FALSE)
	, m_unGratuitousAddressstes(_T(""))
{

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_LayerMgr.AddLayer( this ) ;
	m_LayerMgr.AddLayer( new CNILayer( "NI" ) ) ;
	m_LayerMgr.AddLayer( new CEthernetLayer( "Ethernet" ) ) ;
	m_LayerMgr.AddLayer( new CARPLayer( "ARP" ) );
	m_LayerMgr.AddLayer( new CIPLayer( "IP" ) );
	m_LayerMgr.AddLayer( new CApplicationLayer( "APP" ) ) ;

	m_LayerMgr.ConnectLayers("NI ( *Ethernet ( *ARP ( *IP ( *APP ( *ArpDlg ) ) ) ) )");

	m_APP = (CApplicationLayer *)mp_UnderLayer;
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
	DDX_Text(pDX, IDC_GRATUITOUS_ADDRESS_BOX, m_unGratuitousAddressstes);
}

BEGIN_MESSAGE_MAP(CDC_ARP_01Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	ON_BN_CLICKED(IDC_ARP_ITEM_DELETE_BUTTON, &CDC_ARP_01Dlg::OnBnClickedArpItemDeleteButton)
	ON_BN_CLICKED(IDC_ARP_ALL_DELETE_BUTTON, &CDC_ARP_01Dlg::OnBnClickedArpAllDeleteButton)
	ON_BN_CLICKED(IDC_ARP_SEND_BUTTON, OnSendMessage)
	ON_BN_CLICKED(IDC_WINDOW_OK_BUTTON, &CDC_ARP_01Dlg::OnBnClickedWindowOkButton)
	ON_BN_CLICKED(IDC_ARP_SETTING_BUTTON, OnButtonAddrSet)
	ON_BN_CLICKED(IDC_GRATUITOUS_SEND_BUTTON, &CDC_ARP_01Dlg::OnBnClickedGratuitousSendButton)
	ON_CBN_SELCHANGE(IDC_NICARD_COMBO, OnComboEnetAddr)

	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_GRATUITOUS_ADDRESS_BOX, &CDC_ARP_01Dlg::OnEnChangeGratuitousAddressBox)
	ON_BN_CLICKED(IDC_PROXY_ADD_BUTTON, &CDC_ARP_01Dlg::OnBnClickedProxyAddButton)
	ON_BN_CLICKED(IDC_PROXY_DELETE_BUTTON, &CDC_ARP_01Dlg::OnBnClickedProxyDeleteButton)
	ON_CBN_SELCHANGE(IDC_PROXY_INTERFACE_COMBO, &CDC_ARP_01Dlg::OnCbnSelchangeProxyInterfaceCombo)
	ON_LBN_SELCHANGE(IDC_PROXY_ARP_ENTRY_LIST, &CDC_ARP_01Dlg::OnLbnSelchangeProxyArpEntryList)
END_MESSAGE_MAP()


// CDC_ARP_01Dlg �޽��� ó����

BOOL CDC_ARP_01Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
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

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	SetRegstryMessage( ) ;
	SetTimer(2, 2000, NULL);
	SetTimer(3, 6000, NULL);
	SetTimer(4, 20000, NULL);
	SetDlgState(IPC_INITIALIZING);
	SetDlgState(CFT_COMBO_SET);

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CDC_ARP_01Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CDC_ARP_01Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDC_ARP_01Dlg::OnSendMessage()  
{
	// TODO: Add your control notification handler code here
	UpdateData( TRUE ) ;

	SetTimer(1,3000,NULL);
	m_ARP->setTargetIPAddress((unsigned char*)dstIPAddrString);
	
	SendData( ) ;
	
	UpdateData( FALSE ) ;
}
void CDC_ARP_01Dlg::OnButtonAddrSet() //���ù�ư ������ ��.
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
		MessageBox( "�ּҸ� ���� �����߻�", 
			"���", 
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
		m_ARP->setSenderIPAddress(src_ip);
		m_ARP->setTargetIPAddress(dst_ip);

		sscanf(m_unSrcEnetAddr, "%02x%02x%02x%02x%02x%02x", &src_mac[0],&src_mac[1],&src_mac[2],&src_mac[3],&src_mac[4],&src_mac[5]);
		sscanf(m_unDstEnetAddr, "%02x%02x%02x%02x%02x%02x", &dst_mac[0],&dst_mac[1],&dst_mac[2],&dst_mac[3],&dst_mac[4],&dst_mac[5]);
		AfxMessageBox(m_unSrcEnetAddr);

		m_ETH->SetEnetSrcAddress(src_mac);
		m_ETH->SetEnetDstAddress(dst_mac);
		m_ARP->setSenderHardwareAddress(src_mac);

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

	m_stMessage.SetString("Hello World!");
	int nlength = m_stMessage.GetLength();
	unsigned char* ppayload = new unsigned char[nlength+1];
	memcpy(ppayload,(unsigned char*)(LPCTSTR)m_stMessage,nlength);
	ppayload[nlength] = '\0';
	
	m_ARP->setSenderIPAddress((unsigned char*)srcIPAddrString);

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
	
	KillTimer(1);
	
	return TRUE ;
}

void CDC_ARP_01Dlg::SetDlgState(int state) // ���̾�α� �ʱ�ȭ �κ�
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
	case IPC_INITIALIZING : // ù ȭ�� ����
		pWindowOkButton->EnableWindow( TRUE );
		pARPSendButton->EnableWindow( FALSE );
		pGratuitousSendButton->EnableWindow( TRUE );
		pARPSettingButton->EnableWindow( TRUE );
		pARPSendIP->EnableWindow( TRUE );
		pOWNIPAddress->EnableWindow( TRUE );
		pGratuitousARPAddress->EnableWindow ( TRUE );
		m_proxyARPEntry.EnableWindow( FALSE );
		m_ArpTable.EnableWindow( FALSE ) ;
		break ;

	case IPC_READYTOSEND : // Send ��ư�� ������ �� ����
		pARPSendButton->EnableWindow( TRUE );
		pGratuitousSendButton->EnableWindow( TRUE );
		pARPSettingButton->EnableWindow( TRUE );
		pARPSendIP->EnableWindow( TRUE );
		pOWNIPAddress->EnableWindow( TRUE );
		pGratuitousARPAddress->EnableWindow ( TRUE );
		m_proxyARPEntry.EnableWindow( TRUE );

		DWORD dwIP;
		pARPSendIP->GetAddress(dstIPAddrString[0],dstIPAddrString[1],dstIPAddrString[2],dstIPAddrString[3] );
		//�ؽ�Ʈ�� ���� �� ������°�.
		m_ArpTable.EnableWindow( TRUE ) ;
		break ;

	case IPC_WAITFORACK :	break ;
	case IPC_ERROR :		break ;
	case IPC_ADDR_SET :	// Setting ��ư�� ������ ��
		
		pARPSettingButton->SetWindowText( "Reset" ) ; 
		pARPSendIP->EnableWindow( FALSE ) ;
		pEnetNameCombo->EnableWindow( FALSE );
		pARPSendButton->EnableWindow( TRUE );

		pOWNIPAddress->GetAddress(srcIPAddrString[0],srcIPAddrString[1],srcIPAddrString[2],srcIPAddrString[3]) ;

		pOWNIPAddress->EnableWindow( FALSE ) ;
		m_NI->m_thrdSwitch = TRUE;
		break;
		
	case IPC_ADDR_RESET : // Reset ��ư�� ������ ��
		
		pARPSettingButton->SetWindowText( "Setting" ) ; 
		pARPSendIP->EnableWindow( TRUE ) ;
		pOWNIPAddress->EnableWindow( TRUE ) ;
		pEnetNameCombo->EnableWindow( TRUE );
		pARPSendButton->EnableWindow( FALSE );
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

void CDC_ARP_01Dlg::OnComboEnetAddr()
{
	UpdateData(TRUE);

	int nIndex = m_ComboEnetName.GetCurSel();
	m_NI->GetAdapterObject(nIndex)->name; //�� �������.

	PPACKET_OID_DATA OidData; //�����Ҵ�
	OidData = (PPACKET_OID_DATA)malloc(sizeof(PACKET_OID_DATA));
	OidData->Oid = 0x01010101;
	OidData->Length = 6;

	LPADAPTER adapter = PacketOpenAdapter(m_NI->GetAdapterObject(nIndex)->name);
	CString device_description = m_NI->GetAdapterObject(nIndex)->description;
	device_description.Trim();
	m_ARP->setAdapter(device_description);

	PacketRequest(adapter, FALSE, OidData);

	m_unSrcEnetAddr.Format("%.2X%.2X%.2X%.2X%.2X%.2X",OidData->Data[0],OidData->Data[1],OidData->Data[2],OidData->Data[3],OidData->Data[4],OidData->Data[5]) ;

	UpdateData(FALSE);
}

void CDC_ARP_01Dlg::EndofProcess()
{
	m_LayerMgr.DeAllocLayer( ) ;
}

void CDC_ARP_01Dlg::SetRegstryMessage()
{

}
void CDC_ARP_01Dlg::OnTimer(UINT nIDEvent) 
{
	switch(nIDEvent)
	{
	case 1:{
		KillTimer( 1 ) ;
		break;
		   }
	case 2:{
		m_ArpTable.ResetContent();
		list<CARPLayer::ARP_CACHE_RECORD>::iterator cacheIter = m_ARP->arpCacheTable.begin();
		for(cacheIter; cacheIter != m_ARP->arpCacheTable.end(); cacheIter++)
		{
			CString record;
			CString ipAddress;
			ipAddress.Format(" %3d.%3d.%3d.%3d ", (unsigned char)(*cacheIter).ipAddress[0],(unsigned char)(*cacheIter).ipAddress[1],
									(unsigned char)(*cacheIter).ipAddress[2],(unsigned char)(*cacheIter).ipAddress[3] );
			
			record.Append(ipAddress);
			record.Append(getMACAddressString((*cacheIter).ethernetAddress));
			record.Append(getCompleteString((*cacheIter).isComplete));
			m_ArpTable.AddString(record.GetString());
		}
				m_ArpTable.UpdateData(TRUE);
				break;
		   }
	case 3:{
			list<CARPLayer::ARP_CACHE_RECORD>::iterator cacheIter = m_ARP->arpCacheTable.begin();
			for(cacheIter; cacheIter != m_ARP->arpCacheTable.end();){
				if ((*cacheIter).isComplete == FALSE)
				{
					cacheIter = m_ARP->arpCacheTable.erase(cacheIter);
				}
				else
					cacheIter++;
			}
		break;
		}
	case 4:{
				list<CARPLayer::ARP_CACHE_RECORD>::iterator cacheIter = m_ARP->arpCacheTable.begin();
				for(cacheIter; cacheIter != m_ARP->arpCacheTable.end();){
					if ((*cacheIter).isComplete == FALSE)
						cacheIter++;
					else{
						cacheIter = m_ARP->arpCacheTable.erase(cacheIter);
					}
				}
				break;
			}
		}
	

	CDialog::OnTimer(nIDEvent);
}

CString CDC_ARP_01Dlg::getCompleteString(BOOL isComplete)
{
	if(isComplete == TRUE)
		return CString(" Complete");
	else
		return CString(" Incomplete");
}

CString CDC_ARP_01Dlg::getMACAddressString(unsigned char* macAddress)
{
	if((macAddress[0] == 0) && (macAddress[1] == 0) &&
		(macAddress[2] == 0) && (macAddress[9] == 0) && 
		(macAddress[10] == 0) && (macAddress[11] == 0))
		return CString(" ??:??:??:??:??:?? ");
	else
	{
		CString returnString;
		returnString.Format(" %2X:%2X:%2X:%2X:%2X:%2X ",
				macAddress[0],macAddress[1],macAddress[2],
				macAddress[3],macAddress[4],macAddress[5]);
		return returnString.GetString();
	}
}

void CDC_ARP_01Dlg::OnBnClickedArpItemDeleteButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int i=0;
	int index = m_ArpTable.GetCurSel();
	if(index != LB_ERR) {
		list<CARPLayer::ARP_CACHE_RECORD>::iterator cacheIter = m_ARP->arpCacheTable.begin();
		for(cacheIter; cacheIter != m_ARP->arpCacheTable.end(); cacheIter++)
		{
			if(i == index){
 				cacheIter = m_ARP->arpCacheTable.erase(cacheIter);
				return;
			}
			else{
				i++;
			}
		}
	}
}


void CDC_ARP_01Dlg::OnBnClickedArpAllDeleteButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_ArpTable.ResetContent();
}

void CDC_ARP_01Dlg::OnBnClickedWindowOkButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_APP_EXIT,0);
}






void CDC_ARP_01Dlg::OnIpnFieldchangedArpSendIp(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
}


void CDC_ARP_01Dlg::OnIpnFieldchangedOwnIpAddress(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	*pResult = 0;
}

void CDC_ARP_01Dlg::OnBnClickedGratuitousSendButton() //gratuitous ��ư ������ ��.
{
	UpdateData( TRUE ) ;
	
	SetTimer(1,3000,NULL);
	
	SendDataEditMac( ) ;
	
	UpdateData( FALSE ) ;
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

void CDC_ARP_01Dlg::SendDataEditMac(void)
{
	unsigned char src_mac[12];
	unsigned char dst_mac[12];
	m_stMessage.SetString("Hello World!");
	int nlength = m_stMessage.GetLength();
	unsigned char* ppayload = new unsigned char[nlength+1];
	memcpy(ppayload,(unsigned char*)(LPCTSTR)m_stMessage,nlength);
	ppayload[nlength] = '\0';
	sscanf(m_unGratuitousAddressstes, "%02x%02x%02x%02x%02x%02x", &src_mac[0],&src_mac[1],&src_mac[2],&src_mac[3],&src_mac[4],&src_mac[5]);//Mac�ּ� �о��.
	m_unDstEnetAddr.Format("%.2x%.2x%.2x%.2x%.2x%.2x",0xff,0xff,0xff,0xff,0xff,0xff) ;
	
	sscanf(m_unDstEnetAddr, "%02x%02x%02x%02x%02x%02x", &dst_mac[0],&dst_mac[1],&dst_mac[2],&dst_mac[3],&dst_mac[4],&dst_mac[5]);
	m_IP->SetSrcIPAddress((unsigned char*)srcIPAddrString);
	m_IP->SetDstIPAddress((unsigned char*)srcIPAddrString);
	m_ARP->setSenderIPAddress((unsigned char*)srcIPAddrString);
	m_ARP->setTargetIPAddress((unsigned char*)srcIPAddrString);

	m_ETH->SetEnetSrcAddress(src_mac);

	m_ARP->setSenderHardwareAddress((unsigned char*)src_mac);
	
	
	m_APP->Send(ppayload,m_stMessage.GetLength());
}

void CDC_ARP_01Dlg::OnEnChangeGratuitousAddressBox()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// __super::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CDC_ARP_01Dlg::OnBnClickedProxyAddButton()
{
	CString recordtext;
	CString recordipAddress;
	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	proxyDlg dlg(this);
	int result = dlg.DoModal();
	if(result == IDOK)
	{
		CARPLayer::ARP_CACHE_RECORD newRecord;
		
		newRecord.arpInterface = dlg.selectedDevice;
		memcpy(newRecord.ethernetAddress, dlg.proxyMACAddr, 6);
		memcpy(newRecord.ipAddress, dlg.proxyIPAddrString, 4);
		newRecord.isComplete = TRUE;
		recordtext.Append(newRecord.arpInterface);
		recordipAddress.Format(" %3d.%3d.%3d.%3d ", newRecord.ipAddress[0],newRecord.ipAddress[1],
									newRecord.ipAddress[2],newRecord.ipAddress[3] );
		recordtext.Append(recordipAddress);
		recordtext.Append(getMACAddressString(newRecord.ethernetAddress));
		m_proxyARPEntry.AddString(recordtext);

		m_ARP->arpProxyTable.push_back(newRecord);
	}
}


void CDC_ARP_01Dlg::OnBnClickedProxyDeleteButton()
{
	int i=0;
	int index = m_proxyARPEntry.GetCurSel();
	if(index != LB_ERR) {
		list<CARPLayer::ARP_CACHE_RECORD>::iterator cacheIter = m_ARP->arpProxyTable.begin();
		for(cacheIter; cacheIter != m_ARP->arpProxyTable.end(); cacheIter++)
		{
			if(i == index){
 				cacheIter = m_ARP->arpProxyTable.erase(cacheIter);
				break;
			}
			else{
				i++;
			}
		}
	
	
		m_proxyARPEntry.ResetContent();
		cacheIter = m_ARP->arpProxyTable.begin();
		for(cacheIter; cacheIter != m_ARP->arpProxyTable.end(); cacheIter++)
		{
			CString record;
			CString saveIpaddress;
			record.Append((*cacheIter).arpInterface);
			saveIpaddress.Format(" %3d.%3d.%3d.%3d ", (*cacheIter).ipAddress[0],(*cacheIter).ipAddress[1],
									(*cacheIter).ipAddress[2],(*cacheIter).ipAddress[3] );
			record.Append(saveIpaddress);
			record.Append(getMACAddressString((*cacheIter).ethernetAddress));
			record.Append(getCompleteString((*cacheIter).isComplete));
			m_proxyARPEntry.AddString(record.GetString());
		}
		m_proxyARPEntry.UpdateData(TRUE);
	}
	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}



void CDC_ARP_01Dlg::OnCbnSelchangeProxyInterfaceCombo()
{
	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CDC_ARP_01Dlg::OnLbnSelchangeProxyArpEntryList()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}
