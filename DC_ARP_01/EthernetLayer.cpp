// EthernetLayer.cpp: implementation of the CEthernetLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DC_ARP_01.h"
#include "EthernetLayer.h"
#include "ARPLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEthernetLayer::CEthernetLayer( char* pName )
: CBaseLayer( pName )
{
	ResetHeader( ) ;
}

CEthernetLayer::~CEthernetLayer()
{
}

void CEthernetLayer::ResetHeader()
{
	memset( m_sHeader.enet_dstaddr.addrs, 0, 6 ) ;
	memset( m_sHeader.enet_srcaddr.addrs, 0, 6 ) ;
	memset( m_sHeader.enet_data, 0, ETHER_MAX_DATA_SIZE ) ;
	m_sHeader.enet_type = 0x0008 ; // 0x0800
}

unsigned char* CEthernetLayer::GetEnetDstAddress() 
{
	return m_sHeader.enet_srcaddr.addrs;
}

unsigned char* CEthernetLayer::GetEnetSrcAddress()
{
	return m_sHeader.enet_dstaddr.addrs;
}

void CEthernetLayer::SetEnetSrcAddress(unsigned char *pAddress)
{
	memcpy( &m_sHeader.enet_srcaddr.addrs, pAddress, 6 ) ;

}

void CEthernetLayer::SetEnetDstAddress(unsigned char *pAddress)
{

	memcpy( &m_sHeader.enet_dstaddr.addrs, pAddress, 6 ) ;
}

BOOL CEthernetLayer::Send(unsigned char *ppayload, int nlength)
{
	memcpy( m_sHeader.enet_data, ppayload, nlength ) ;

	BOOL bSuccess = FALSE ;
	bSuccess = mp_UnderLayer->Send((unsigned char*) &m_sHeader,nlength+ETHER_HEADER_SIZE);

	return bSuccess ;
}
	
BOOL CEthernetLayer::Receive( unsigned char* ppayload )
{
	PETHERNET_HEADER pFrame = (PETHERNET_HEADER) ppayload ;

	BOOL bSuccess = FALSE ;

	if( memcmp((char *)pFrame->enet_srcaddr.S_un.s_ether_addr,(char *)m_sHeader.enet_srcaddr.S_un.s_ether_addr,6) != 0)
	{
		if ( memcmp((char *)pFrame->enet_dstaddr.S_un.s_ether_addr,(char *)m_sHeader.enet_srcaddr.S_un.s_ether_addr,6) == 0 ||
			 memcmp((char *)pFrame->enet_dstaddr.S_un.s_ether_addr,BROADCAST_ADDR, 6) == 0)
		{
			if(ntohs(pFrame->enet_type) == ntohs(m_sHeader.enet_type))
			{
				bSuccess = mp_aUpperLayer[0]->Receive((unsigned char*) pFrame->enet_data);
			}
		}
	}
	return bSuccess ;
}
