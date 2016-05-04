#include "StdAfx.h"
#include "ARPLayer.h"

CARPLayer::CARPLayer(char* pName)
: CBaseLayer( pName )
{
	ResetHeader();
}


CARPLayer::~CARPLayer(void)
{
}

void CARPLayer::ResetHeader()
{
	arpHeader.arpHardwareType = 0x1;
	arpHeader.arpProtocolType = 0x0800;
	arpHeader.arpHardwareAddrSize = 0x6;
	arpHeader.arpProtocolAddrSize = 0x4;
	arpHeader.arpOperationType = 0;
	memset(arpHeader.arpSenderHardwareAddress, 0, 6);
	memset(arpHeader.arpSenderIPAddress, 0, 4);
	memset(arpHeader.arpTargetHardwareAddress, 0, 6);
	memset(arpHeader.arpTargetIPAddress, 0, 4);
}


void CARPLayer::setARPOperationType(unsigned char operationType)
{
	arpHeader.arpOperationType = operationType;
}


void CARPLayer::setSenderIPAddress(unsigned char* senderIP)
{
	memcpy(arpHeader.arpSenderIPAddress, senderIP, 4);
}


void CARPLayer::setSenderHardwareAddress(unsigned char* senderHard)
{
	memcpy(arpHeader.arpSenderHardwareAddress, senderHard, 6);
}


void CARPLayer::setTargetIPAddress(unsigned char* targetIP)
{
	memcpy(arpHeader.arpTargetIPAddress, targetIP, 4);
}


void CARPLayer::setTargetHardwareAddress(unsigned char* targetHard)
{
	memcpy(arpHeader.arpTargetHardwareAddress, targetHard, 6);
}


BOOL CARPLayer::Send(unsigned char* ppayload, int length)
{
	// ip address 받아와서 확인하는 작업 안되어있다.
	CIPLayer::IPLayer_HEADER ipHeader;

	memcpy( arpHeader.arpData, ppayload, length );

	BOOL isCacheAvailable = FALSE;
	list<ARP_CACHE_RECORD>::iterator cacheIter = arpCacheTable.begin();
	for(cacheIter; cacheIter != arpCacheTable.end(); cacheIter++)
	{
		if(memcmp((*cacheIter).ipAddress, targetIPAddress, 4) == 0)
		{
			isCacheAvailable = TRUE;
			break;
		}
	}

	if(isCacheAvailable == TRUE)
	{
		setTargetHardwareAddress((*cacheIter).ethernetAddress);
	}
	else
	{
		setTargetHardwareAddress(NULL);
		((CEthernetLayer*)GetUnderLayer())->SetEnetDstAddress(BROADCAST_ADDR);
	}
	setTargetIPAddress(ipHeader.ip_dst);

	BOOL bSuccess = FALSE ;
	bSuccess = mp_UnderLayer->Send((unsigned char*)&arpHeader,length+ARP_HEADER_SIZE);

	return bSuccess;
}


BOOL CARPLayer::Receive(unsigned char* ppayload)
{

}
