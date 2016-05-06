// ARPLayer.h: interface for the CARPLayer class.
//
//////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"
#include "EthernetLayer.h"
#include "IPLayer.h"
#include "NILayer.h"
#include <list>
#include <pcap.h>
#include <Packet32.h>

class CARPLayer : public CBaseLayer
{
private:
	inline void		ResetHeader( );

public:
	unsigned short ARP_REQUEST;
	unsigned short ARP_REPLY;

	CARPLayer(char* pName);
	virtual ~CARPLayer(void);

	typedef struct _ARP_HEADER
	{
		unsigned short arpHardwareType;
		unsigned short arpProtocolType;
		unsigned char arpHardwareAddrSize;
		unsigned char arpProtocolAddrSize;
		unsigned short arpOperationType;
		unsigned char arpSenderHardwareAddress[6];
		unsigned char arpSenderIPAddress[4];
		unsigned char arpTargetHardwareAddress[6];
		unsigned char arpTargetIPAddress[4];
		unsigned char arpData[ARP_DATA_SIZE];

	} ARP_HEADER, *PARP_HEADER;

	typedef struct _ARP_CACHE_RECORD
	{
		CString arpInterface;
		unsigned char ipAddress[4];
		unsigned char ethernetAddress[6];
		BOOL isComplete;
	} ARP_CACHE_RECORD, *PARP_CACHE_RECORD;

	void setARPOperationType(unsigned char operationType);
	void setSenderIPAddress(unsigned char* senderIP);
	void setSenderHardwareAddress(unsigned char* senderHard);
	void setTargetIPAddress(unsigned char* targetIP);
	void setTargetHardwareAddress(unsigned char* targetHard);
	void setAdapter(CString adapter);
	void OnTimer(UINT nIDEvent) ;
	list<ARP_CACHE_RECORD> getARPCacheTable(void);

	BOOL Send(unsigned char* ppayload, int length);
	BOOL Receive(unsigned char* ppayload);

	list<ARP_CACHE_RECORD> arpCacheTable;
	list<ARP_CACHE_RECORD> arpProxyTable;

protected:
	ARP_HEADER arpHeader;
	CString adapter;
	unsigned char ownMACAddress[6];
	unsigned char ownIPAddress[4];
	unsigned char targetIPAddress[4];
};

