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
	memset(arpHeader.arpSenderHardwareAddress, 0, 12);
	memset(arpHeader.arpSenderIPAddress, 0, 4);
	memset(arpHeader.arpTargetHardwareAddress, 0, 12);
	memset(arpHeader.arpTargetIPAddress, 0, 4);
	memset(ownMACAddress, 0, 12);
	memset(ownIPAddress, 0, 4);
}


void CARPLayer::setARPOperationType(unsigned char operationType)
{
	arpHeader.arpOperationType = operationType;
}


void CARPLayer::setSenderIPAddress(unsigned char* senderIP)
{
	arpHeader.arpSenderIPAddress[0] = ownIPAddress[0] = senderIP[0];
	arpHeader.arpSenderIPAddress[1] = ownIPAddress[1] = senderIP[1];
	arpHeader.arpSenderIPAddress[2] = ownIPAddress[2] = senderIP[2];
	arpHeader.arpSenderIPAddress[3] = ownIPAddress[3] = senderIP[3];
}


void CARPLayer::setSenderHardwareAddress(unsigned char* senderHard)
{
	memcpy(arpHeader.arpSenderHardwareAddress, senderHard, 12);
	memcpy(ownMACAddress, senderHard, 12);
}


void CARPLayer::setTargetIPAddress(unsigned char* targetIP)
{
	arpHeader.arpTargetIPAddress[0] = targetIPAddress[0] = targetIP[0];
	arpHeader.arpTargetIPAddress[1] = targetIPAddress[1] = targetIP[1];
	arpHeader.arpTargetIPAddress[2] = targetIPAddress[2] = targetIP[2];
	arpHeader.arpTargetIPAddress[3] = targetIPAddress[3] = targetIP[3];
}


void CARPLayer::setTargetHardwareAddress(unsigned char* targetHard)
{
	memcpy(arpHeader.arpTargetHardwareAddress, targetHard, 12);
}

list<CARPLayer::ARP_CACHE_RECORD> CARPLayer::getARPCacheTable(void)
{
	return arpCacheTable;
}

void CARPLayer::setAdapter(LPADAPTER adapter)
{
	this->adapter = adapter;
}

BOOL CARPLayer::Send(unsigned char* ppayload, int length)
{
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
		((CEthernetLayer*)GetUnderLayer())->SetEnetDstAddress(BROADCAST_ADDR);
	}
	
	arpHeader.arpOperationType = 0x1;
	
	ARP_CACHE_RECORD newRecord;
	newRecord.arpInterface = this->adapter;
	memset(newRecord.ethernetAddress, 0, 12);
	memcpy(newRecord.ipAddress, targetIPAddress, 4);
	newRecord.isComplete = FALSE;

	arpCacheTable.push_back(newRecord);

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
				memcpy((*arpIter).ethernetAddress, ownMACAddress, 12);
				break;
			}
		}
		if(pARPFrame->arpOperationType == 0x1)
		{
			if(isARPRecordExist == FALSE)
			{
				ARP_CACHE_RECORD newRecord;
				newRecord.arpInterface = adapter;
				memcpy(newRecord.ethernetAddress, receivedARPTargetHardwareAddress, 12);
				memcpy(newRecord.ipAddress, receivedARPTargetIPAddress, 4);
				newRecord.isComplete = TRUE;

				arpCacheTable.push_back(newRecord);
			}
		
			unsigned char* tempHardwareAddress = NULL;
			unsigned char* tempIPAddress = NULL;
			memset(tempHardwareAddress, 0, 12);
			memset(tempIPAddress, 0, 4);

			memcpy(tempHardwareAddress, arpHeader.arpSenderHardwareAddress, 12);
			memcpy(tempIPAddress, arpHeader.arpSenderIPAddress, 4);
			
			memcpy(arpHeader.arpSenderHardwareAddress, arpHeader.arpTargetHardwareAddress, 12);
			memcpy(arpHeader.arpSenderIPAddress, arpHeader.arpTargetIPAddress, 4);
			memcpy(arpHeader.arpTargetHardwareAddress, tempHardwareAddress, 12);
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

