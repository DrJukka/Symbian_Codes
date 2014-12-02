/* Copyright (c) 2004, Nokia. All rights reserved */


// INCLUDE FILES
#include <bt_sock.h>

#include "ObjectExchangeServiceSearcher.h"
#include "ObjectExchangeProtocolConstants.h"

// ----------------------------------------------------------------------------
// TSdpAttributeParser::SSdpAttributeNode gObexProtocolListData[]
// Construct a TSdpAttributeParser
// ----------------------------------------------------------------------------
//
const TSdpAttributeParser::SSdpAttributeNode gObexProtocolListData[] = 
    {
        { TSdpAttributeParser::ECheckType, ETypeDES },
        { TSdpAttributeParser::ECheckType, ETypeDES },
        { TSdpAttributeParser::ECheckValue, ETypeUUID, KL2CAP },
        { TSdpAttributeParser::ECheckEnd },
        { TSdpAttributeParser::ECheckType, ETypeDES },
        { TSdpAttributeParser::ECheckValue, ETypeUUID, KRFCOMM },
        { TSdpAttributeParser::EReadValue, ETypeUint, KRfcommChannel },
        { TSdpAttributeParser::ECheckEnd },
        { TSdpAttributeParser::ECheckType, ETypeDES },
        { TSdpAttributeParser::ECheckValue, ETypeUUID, KBtProtocolIdOBEX },
        { TSdpAttributeParser::ECheckEnd },
        { TSdpAttributeParser::ECheckEnd },
        { TSdpAttributeParser::EFinished }
    };

// ----------------------------------------------------------------------------
// TStaticArrayC
// Provides a type and size, safe method of using static arrays.
// ----------------------------------------------------------------------------
//
const TStaticArrayC<TSdpAttributeParser::SSdpAttributeNode> gObexProtocolList =
CONSTRUCT_STATIC_ARRAY_C( gObexProtocolListData );

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CObjectExchangeServiceSearcher::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CObjectExchangeServiceSearcher* CObjectExchangeServiceSearcher
::NewL( MLog& aLog )
    {
    CObjectExchangeServiceSearcher* self = CObjectExchangeServiceSearcher
    ::NewLC( aLog );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CObjectExchangeServiceSearcher::NewLC()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CObjectExchangeServiceSearcher* CObjectExchangeServiceSearcher
::NewLC( MLog& aLog )
    {
    CObjectExchangeServiceSearcher* self = new ( 
        ELeave ) CObjectExchangeServiceSearcher( aLog );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// CObjectExchangeServiceSearcher::CObjectExchangeServiceSearcher()
// Constructor.
// ----------------------------------------------------------------------------
//
CObjectExchangeServiceSearcher::CObjectExchangeServiceSearcher( MLog& aLog )
: CBTServiceSearcher( aLog ),
  iServiceClass( KServiceClass ),
  iPort( -1 )
    {
    }

// ----------------------------------------------------------------------------
// CObjectExchangeServiceSearcher::~CObjectExchangeServiceSearcher()
// Destructor.
// ----------------------------------------------------------------------------
//
CObjectExchangeServiceSearcher::~CObjectExchangeServiceSearcher()
    {
    }

// ----------------------------------------------------------------------------
// CObjectExchangeServiceSearcher::ConstructL()
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CObjectExchangeServiceSearcher::ConstructL()
    {
    // no implementation required
    }

// ----------------------------------------------------------------------------
// CObjectExchangeServiceSearcher::ServiceClass()
// The service class to search.
// ----------------------------------------------------------------------------
//
const TUUID& CObjectExchangeServiceSearcher::ServiceClass() const
    {
    return iServiceClass;
    }

// ----------------------------------------------------------------------------
// CObjectExchangeServiceSearcher::ProtocolList()
// The list of Protocols required by the service
// ----------------------------------------------------------------------------
//
const TSdpAttributeParser::TSdpAttributeList& CObjectExchangeServiceSearcher
::ProtocolList() const
    {
    return gObexProtocolList;
    }

// ----------------------------------------------------------------------------
// CObjectExchangeServiceSearcher::FoundElementL()
// Read the data element.
// ----------------------------------------------------------------------------
//
void CObjectExchangeServiceSearcher::FoundElementL( TInt aKey, 
                                                    const CSdpAttrValue& aValue )
    {
  //  __ASSERT_ALWAYS( aKey == static_cast<TInt>( KRfcommChannel ), 
    //    Panic( EBTObjectExchangeProtocolRead ) );
    iPort = aValue.Uint();
    }

// ----------------------------------------------------------------------------
// CObjectExchangeServiceSearcher::Port()
// Port connection on the remote machine.
// ----------------------------------------------------------------------------
//
TInt CObjectExchangeServiceSearcher::Port()
    {
    return iPort;
    }

// End of File
