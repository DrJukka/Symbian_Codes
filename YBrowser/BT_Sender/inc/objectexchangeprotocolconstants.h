/* Copyright (c) 2004, Nokia. All rights reserved */


#ifndef __CBTOBEX_SDP_PROTOCOL__
#define __CBTOBEX_SDP_PROTOCOL__

// INCLUDES
#include <e32def.h>
#include <bt_sock.h>
#include "Definitions.h"

// CONSTANTS

const TUint KBtProtocolIdOBEX = 0x0008;
const TUint KRfcommChannel = 1;
const TUint KServiceClass = 0x1105;

_LIT( KServiceName,"OBEX" );
_LIT( KServerTransportName,"RFCOMM" );

// Used by ObjectExchangeServer.h
const TInt KBufferGranularity = 1024;
const TInt KSimultainousSocketsOpen = 8;

_LIT( KStrRFCOMM,"RFCOMM" );

#endif //    __CBTOBEX_SDP_PROTOCOL__

// End of File
