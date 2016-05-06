#include "StdAfx.h"
#include "ARPLayer.h"

CARPLayer::CARPLayer(char* pName)
: CBaseLayer( pName ), ARP_REQUEST(0x100), ARP_REPLY(0x200)
{
	ResetHeader();
}


CARPLayer::~CARPLayer(void)
{
}

void CARPLayer::ResetHeader()
{
	arpHeader.arpHardwareType = 0x0100;
	arpHeader.arpProtocolType = 0x0008;
	arpHeader.arpHardwareAddrSize = 0x6;
	arpHeader.arpProtocolAddrSize = 0x4;
	arpHeader.arpOperationType = 0;
	memset(arpHeader.arpSenderHardwareAddress, 0, 6);
	memset(arpHeader.arpSenderIPAddress, 0, 4);
	memset(arpHeader.arpTargetHardwareAddress, 0, 6);
	memset(arpHeader.arpTargetIPAddress, 0, 4);
	memset(ownMACAddress, 0, 6);
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
	memcpy(arpHeader.arpSenderHardwareAddress, senderHard, 6);
	memcpy(ownMACAddress, senderHard, 6);
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
	memcpy(arpHeader.arpTargetHardwareAddress, targetHard, 6);
}

list<CARPLayer::ARP_CACHE_RECORD> CARPLayer::getARPCacheTable(void)
{
	return arpCacheTable;
}

void CARPLayer::setAdapter(CString adapter)
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

	//if cache is vaild and complete record
	if((isCacheAvailable == TRUE) && ((*cacheIter).isComplete == TRUE))
	{
		setTargetHardwareAddress((*cacheIter).ethernetAddress);
		((CEthernetLayer*)GetUnderLayer())->SetEnetDstAddress((*cacheIter).ethernetAddress);
	}
	//it is not valid record
	else
	{
		memset(arpHeader.arpTargetHardwareAddress, 0, 6);
		((CEthernetLayer*)GetUnderLayer())->SetEnetDstAddress(BROADCAST_ADDR);
	}
	
	arpHeader.arpHardwareType = 0x0100;
	arpHeader.arpProtocolType = 0x0008;
	arpHeader.arpHardwareAddrSize = 0x6;
	arpHeader.arpProtocolAddrSize = 0x4;
	arpHeader.arpOperationType = ARP_REQUEST;
	memcpy(arpHeader.arpSenderHardwareAddress, ownMACAddress, 6);
	memcpy(arpHeader.arpSenderIPAddress, ownIPAddress, 4);
	memcpy(arpHeader.arpTargetIPAddress, targetIPAddress, 4);
	
	ARP_CACHE_RECORD newRecord;
	newRecord.arpInterface = this->adapter;
	memset(newRecord.ethernetAddress, 0, 6);
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

	unsigned char* receivedARPTargetIPAddress = (unsigned char*)pARPFrame->arpTargetIPAddress;
	unsigned char* receivedARPSenderIPAddress = (unsigned char*)pARPFrame->arpSenderIPAddress;
	unsigned char* receivedARPSenderHardwareAddress = (unsigned char*)pARPFrame->arpSenderHardwareAddress;
	
	if ( (receivedARPTargetIPAddress[0] == ownIPAddress[0]) &&
		 (receivedARPTargetIPAddress[1] == ownIPAddress[1]) &&
		 (receivedARPTargetIPAddress[2] == ownIPAddress[2]) &&
		 (receivedARPTargetIPAddress[3] == ownIPAddress[3]))
	{
		BOOL isARPRecordExist = FALSE;
		list<ARP_CACHE_RECORD>::iterator arpIter = arpCacheTable.begin();
		for(arpIter; arpIter != arpCacheTable.end(); arpIter++)
		{
			if(memcmp((*arpIter).ipAddress,receivedARPTargetIPAddress, 4) == 0)
			{
				isARPRecordExist = TRUE;
				memcpy((*arpIter).ethernetAddress, receivedARPSenderHardwareAddress, 6);
				(*arpIter).isComplete = TRUE;
				break;
			}
		}
		if(ntohs(pARPFrame->arpOperationType) == ntohs(ARP_REQUEST))
		{
			if(isARPRecordExist == FALSE)
			{
				ARP_CACHE_RECORD newRecord;
				newRecord.arpInterface = adapter;
				memcpy(newRecord.ethernetAddress, receivedARPSenderHardwareAddress, 6);
				memcpy(newRecord.ipAddress, receivedARPSenderIPAddress, 4);
				newRecord.isComplete = TRUE;

				arpCacheTable.push_back(newRecord);
			}
		
			unsigned char tempHardwareAddress[6];
			unsigned char tempIPAddress[4];
			memset(tempHardwareAddress, 0, 6);
			memset(tempIPAddress, 0, 4);

			memcpy(tempHardwareAddress, receivedARPSenderHardwareAddress, 6);
			memcpy(tempIPAddress, receivedARPSenderIPAddress, 4);

			memcpy(arpHeader.arpSenderHardwareAddress, ownMACAddress, 6);
			memcpy(arpHeader.arpTargetHardwareAddress, tempHardwareAddress, 6);
			memcpy(arpHeader.arpSenderIPAddress, ownIPAddress, 4);
			memcpy(arpHeader.arpTargetIPAddress, tempIPAddress, 4);
			
			arpHeader.arpHardwareType = 0x0100;
			arpHeader.arpProtocolType = 0x0008;
			arpHeader.arpHardwareAddrSize = 0x6;
			arpHeader.arpProtocolAddrSize = 0x4;
			arpHeader.arpOperationType = ARP_REPLY;
			memset(arpHeader.arpData, 0, 1);

			((CEthernetLayer*)GetUnderLayer())->SetEnetDstAddress(arpHeader.arpTargetHardwareAddress);
			((CEthernetLayer*)GetUnderLayer())->SetEnetSrcAddress(arpHeader.arpSenderHardwareAddress);
		
			bSuccess = mp_aUpperLayer[0]->Receive((unsigned char*)pARPFrame->arpData);
			bSuccess = mp_UnderLayer->Send((unsigned char*)&arpHeader, ARP_HEADER_SIZE);
		}
		return bSuccess;
	}
	else
	{
		//discard this message.
		return bSuccess;
	}
}


void CARPLayer::OnTimer(UINT nIDEvent) 
{
	
}