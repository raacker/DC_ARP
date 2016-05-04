#include "StdAfx.h"
#include "ApplicationLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CApplicationLayer::CApplicationLayer( char* pName )
: CBaseLayer( pName ),
	mp_Dlg( NULL )
{
	ResetHeader();
}

CApplicationLayer::~CApplicationLayer()
{
}

void CApplicationLayer::ResetHeader( )
{
	m_sHeader.app_totlen  = 0x0000 ;
	m_sHeader.app_type    = 0x00 ;

	memset( m_sHeader.app_data, 0, APP_DATA_SIZE ) ;
}