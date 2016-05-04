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
}

CApplicationLayer::~CApplicationLayer()
{
}

void CApplicationLayer::ResetHeader( )
{
	m_sHeader.capp_totlen  = 0x0000 ;
	m_sHeader.capp_type    = 0x00 ;

	memset( m_sHeader.capp_data, 0, APP_DATA_SIZE ) ;
}