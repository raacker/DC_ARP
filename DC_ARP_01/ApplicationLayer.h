#pragma once

#include "BaseLayer.h"

class CApplicationLayer
: public CBaseLayer  
{
private:
	inline void		ResetHeader( ) ;
	CObject* mp_Dlg ;

public:
	unsigned char	*m_ppayload;
	int				m_length;

	BOOL			Receive( unsigned char* ppayload );
	BOOL			Send( unsigned char*, int );

	CApplicationLayer( char* pName );
	virtual ~CApplicationLayer();

	typedef struct _APP_HEADER {

		unsigned int	app_dstaddr	; // destination address of application layer
		unsigned int	app_srcaddr	; // source address of application layer
		unsigned short	app_length	; // total length of the data
		unsigned char	app_type	; // type of application data
		unsigned char	app_data[ APP_DATA_SIZE ] ; // application data

	} _APP_HEADER, *PAPP_HEADER ;
};

