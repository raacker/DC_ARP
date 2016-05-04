
// DC_ARP_01Dlg.h : 헤더 파일
//

#pragma once
#include "LayerManager.h"	// Added by ClassView
#include "ApplicationLayer.h"	// Added by ClassView
#include "EthernetLayer.h"	// Added by ClassView
#include "IPLayer.h"		// Added by ClassView
#include "NILayer.h"		// Added by ClassView
#include "afxwin.h"
#include "afxcmn.h"

// CDC_ARP_01Dlg 대화 상자
class CDC_ARP_01Dlg : public CDialogEx,
	public CBaseLayer
{
// 생성입니다.
public:
	CDC_ARP_01Dlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DC_ARP_01_DIALOG };
	CListBox		m_ArpTable;
	CIPAddressCtrl	m_DstIPAddr;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedArpItemDeleteButton();
	afx_msg void OnBnClickedArpAllDeleteButton();
	afx_msg void OnBnClickedArpSendButton();
	afx_msg void OnBnClickedWindowOkButton();

private:
	CLayerManager	m_LayerMgr;
	BOOL			m_bSendReady;

	CApplicationLayer m_APP;
	CARPLayer*		m_ARP;
	CIPLayer*		m_IP;
	CEthernetLayer*	m_ETH;
	CNILayer*		m_NI;
};
