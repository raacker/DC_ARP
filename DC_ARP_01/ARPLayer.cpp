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
	arpHeader.arpProtocolType = 0x3412;
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

list<CARPLayer::ARP_CACHE_RECORD> CARPLayer::getARPCacheTable(void)
{
	return arpCacheTable;
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
		((CEthernetLayer*)GetUnderLayer())->SetEnetDstAddress((*cacheIter).ethernetAddress);
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
	PARP_HEADER pARPFrame = (PARP_HEADER)ppayload;
	
	
	BOOL bSuccess = FALSE ;

	char* receivedARPTargetIPAddress = (char*)pARPFrame->arpTargetIPAddress;
	char* receivedARPTargetHardwareAddress = (char*)pARPFrame->arpTargetHardwareAddress;
	if ( memcmp(receivedARPTargetIPAddress, ownIPAddress, 4) == 0)
	{
		BOOL isARPRecordExist = FALSE;
		list<ARP_CACHE_RECORD>::iterator arpIter = arpCacheTable.begin();
		for(arpIter; arpIter != arpCacheTable.end(); arpIter++)
		{
			if(memcmp((*arpIter).ipAddress,receivedARPTargetIPAddress, 4) == 0)
			{
				isARPRecordExist = TRUE;
				memcpy((*arpIter).ethernetAddress, ownMACAddress, 6);
				break;
			}
		}
		if(pARPFrame->arpOperationType == 0x1)
		{
			if(isARPRecordExist == FALSE)
			{
				ARP_CACHE_RECORD newRecord;
				newRecord.arpInterface = adapter;
				memcpy(newRecord.ethernetAddress, receivedARPTargetHardwareAddress, 6);
				memcpy(newRecord.ipAddress, receivedARPTargetIPAddress, 4);
				newRecord.isComplete = TRUE;

				arpCacheTable.push_back(newRecord);
			}
		
			unsigned char* tempHardwareAddress = NULL;
			unsigned char* tempIPAddress = NULL;
			memset(tempHardwareAddress, 0, 6);
			memset(tempIPAddress, 0, 4);

			memcpy(tempHardwareAddress, arpHeader.arpSenderHardwareAddress, 6);
			memcpy(tempIPAddress, arpHeader.arpSenderIPAddress, 4);
			
			memcpy(arpHeader.arpSenderHardwareAddress, arpHeader.arpTargetHardwareAddress, 6);
			memcpy(arpHeader.arpSenderIPAddress, arpHeader.arpTargetIPAddress, 4);
			memcpy(arpHeader.arpTargetHardwareAddress, tempHardwareAddress, 6);
			memcpy(arpHeader.arpTargetIPAddress, tempIPAddress, 4);
		
			arpHeader.arpOperationType = 0x2;
		
			((CEthernetLayer*)GetUnderLayer())->SetEnetDstAddress(arpHeader.arpTargetHardwareAddress);
			((CEthernetLayer*)GetUnderLayer())->SetEnetSrcAddress(arpHeader.arpSenderHardwareAddress);
		
			bSuccess = mp_aUpperLayer[0]->Receive((unsigned char*)pARPFrame->arpData);
			bSuccess = mp_UnderLayer->Send((unsigned char*)&arpHeader, sizeof(ppayload));
		}
		return bSuccess;
		// ARP reply 헤더 만들기!!!!!!!!!!!!!!!!!!
		// char* 캐스팅 하는 부분들이 존재할 꺼 같다.
	}
	else
	{
		//discard this message.
		return bSuccess;
	}
}

