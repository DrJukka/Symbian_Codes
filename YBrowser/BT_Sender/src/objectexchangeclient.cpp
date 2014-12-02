/* Copyright (c) 2004, Nokia. All rights reserved */


// INCLUDE FILES

#include "ObjectExchangeClient.h"
#include "ObjectExchangeServiceSearcher.h"
#include "Log.h"

#include <stringloader.h>

#include "Common.h"
#ifdef SONE_VERSION
    #include <YFB_2002B0AA.rsg>
#else
    #ifdef LAL_VERSION

    #else
        #ifdef __YTOOLS_SIGNED
            #include <YuccaBrowser_2000713D.rsg>
        #else
            #include <YuccaBrowser.rsg>
        #endif
    #endif
#endif

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CObjectExchangeClient::NewL()
// Symbian two-phased constructor.
// ----------------------------------------------------------------------------
//
CObjectExchangeClient* CObjectExchangeClient::NewL( MLog& aLog, const TDesC& aFilename)
    {
    CObjectExchangeClient* self = NewLC( aLog,aFilename);
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CObjectExchangeClient::NewLC()
// Symbian two-phased constructor.
// ----------------------------------------------------------------------------
//
CObjectExchangeClient* CObjectExchangeClient::NewLC( MLog& aLog , const TDesC& aFilename)
    {
    CObjectExchangeClient* self = new ( ELeave ) CObjectExchangeClient( aLog );
    CleanupStack::PushL( self );
    self->ConstructL(aFilename);
    return self;
    }

// ----------------------------------------------------------------------------
// CObjectExchangeClient::CObjectExchangeClient()
// Constructor.
// ----------------------------------------------------------------------------
//
CObjectExchangeClient::CObjectExchangeClient( MLog& aLog )
: CActive( CActive::EPriorityStandard ),
  iState( EWaitingToGetDevice ),
  iLog( aLog )
    {
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// CObjectExchangeClient::~CObjectExchangeClient()
// Destructor.
// ----------------------------------------------------------------------------
//
CObjectExchangeClient::~CObjectExchangeClient()
    {

    if ( iState != EWaitingToGetDevice && iClient )
    {
        iClient->Abort();
        iStatus = KErrNone;
    }

    Cancel();
    
    delete iCurrObject;
    iCurrObject = NULL;

    delete iServiceSearcher;
    iServiceSearcher = NULL;

    delete iClient;
    iClient = NULL;
    }

// ----------------------------------------------------------------------------
// CObjectExchangeClient::ConstructL()
// Perform second phase construction of this object.
// ----------------------------------------------------------------------------
//
void CObjectExchangeClient::ConstructL(const TDesC& aFilename)
{
	iFileToSend.Copy(aFilename);
	
    iServiceSearcher = CObjectExchangeServiceSearcher::NewL( iLog );
}

// ----------------------------------------------------------------------------
// CObjectExchangeClient::DoCancel()
// Cancel any outstanding requests.
// ----------------------------------------------------------------------------
//
void CObjectExchangeClient::DoCancel()
{
	if(iState == EGettingDevice && iServiceSearcher)
	{
		iServiceSearcher->Cancel();
	}
}

// ----------------------------------------------------------------------------
// CObjectExchangeClient::RunL()
// Respond to an event.
// ----------------------------------------------------------------------------
//
void CObjectExchangeClient::RunL()
    {
    TFileName Hjrlppp;
    
    if ( iStatus != KErrNone )
    {
        switch ( iState )
            {
            case EGettingDevice:
                if ( iStatus == KErrCancel )
                {
                    StringLoader::Load(Hjrlppp,R_STR_OBJNODEVUCE);
                    iLog.LogL(Hjrlppp);
                    
                }
                iState = EWaitingToGetDevice;
                break;
            case EGettingService:
            case EGettingConnection:
            case EDisconnecting:
                {
                    StringLoader::Load(Hjrlppp,R_STR_OBJCONNFAIL);
                    iLog.LogL(Hjrlppp, iStatus.Int() );
                    iState = EWaitingToGetDevice;
                }
                break;
            case EWaitingToSend:
                {
                    StringLoader::Load(Hjrlppp,R_STR_OBJSENDFAIL);
                
                    iLog.LogL(Hjrlppp, iStatus.Int() );
                    iState = EWaitingToGetDevice;
                }
                break;
            default:
             //   Panic( EBTObjectExchangeUnexpectedLogicState );
                break;
            }
    }
    else 
    {
        switch ( iState )
            {
            case EGettingDevice:
                // found a device now search for a suitable service
                iLog.LogL( iServiceSearcher->ResponseParams().DeviceName() );
                iState = EGettingService;
                iStatus = KRequestPending; 
                // this means that the RunL can not be called until
                // this program does something to iStatus
                iServiceSearcher->FindServiceL( iStatus );
                SetActive();
                break;

            case EGettingService:
                {
                    StringLoader::Load(Hjrlppp,R_STR_OBJCONNECTING);
                
                    iLog.LogL(Hjrlppp);
                    iState = EGettingConnection;
                    ConnectToServerL();
                }
                break;
            case EGettingConnection:
                {
                	TParsePtrC Hjelpp(iFileToSend);
                	
                	StringLoader::Load(Hjrlppp,R_STR_OBJSENDING);
                	
                	Hjrlppp.Append(Hjelpp.NameAndExt());
                	
                	iLog.LogL(Hjrlppp);
	                iState = EWaitingToSend;
	                
	                SendObjectL(iFileToSend);
                }
                break;
            case EDisconnecting:
                {
                    StringLoader::Load(Hjrlppp,R_STR_OBJDISCONNECTED);
                    
                    iLog.LogL(Hjrlppp);
                    iState = EWaitingToGetDevice;
                }
                break;
			case EWaitingToSend:
                iLog.DoneL(KErrNone,iFileToSend);
                break;
            default:
               //// Panic( EBTObjectExchangeSdpRecordDelete );
                break;
            };
        }
    }

// ----------------------------------------------------------------------------
// CObjectExchangeClient::ConnectL()
// Connect to a service.
// ----------------------------------------------------------------------------
//
void CObjectExchangeClient::ConnectL()
{
    TFileName Hjrlppp;
    
    if ( iState == EWaitingToGetDevice && !IsActive() )
    {
        iServiceSearcher->SelectDeviceByDiscoveryL( iStatus );
        
        StringLoader::Load(Hjrlppp,R_STR_OBJFINDDEV);

        iLog.LogL(Hjrlppp);
        iState = EGettingDevice;
        SetActive();
    }
    else
    {
        StringLoader::Load(Hjrlppp,R_STR_OBJBTBUSY);
    
        iLog.LogL(Hjrlppp);
        User::Leave( KErrInUse );
    }
}

// ----------------------------------------------------------------------------
// CObjectExchangeClient::ConnectToServerL()
// Connect to the server.
// ----------------------------------------------------------------------------
//
void CObjectExchangeClient::ConnectToServerL()
    {
    TObexBluetoothProtocolInfo protocolInfo;

    protocolInfo.iTransport.Copy( KServerTransportName );
    protocolInfo.iAddr.SetBTAddr( iServiceSearcher->BTDevAddr() );
    protocolInfo.iAddr.SetPort( iServiceSearcher->Port() );

    if ( iClient )
        {
        delete iClient;
        iClient = NULL;
        }
    iClient = CObexClient::NewL( protocolInfo );
    
    iClient->Connect( iStatus );
    SetActive();
    }

// ----------------------------------------------------------------------------
// CObjectExchangeClient::SendObjectL()
// Send a message to a service on a remote machine.
// ----------------------------------------------------------------------------
//
void CObjectExchangeClient::SendObjectL(const TDesC& aName)
    {
    if ( iState != EWaitingToSend )
        {
        User::Leave( KErrDisconnected );
        }
    else if ( IsActive() ) 
        {
        User::Leave( KErrInUse );
        }
    
    delete iCurrObject;
    iCurrObject = NULL;
    iCurrObject = CObexFileObject::NewL(aName);
    
    TParsePtrC parsePtr (aName);        
    TPtrC ptr = parsePtr.NameAndExt();
    
    iCurrObject->SetNameL( ptr );
        
    iClient->Put( *iCurrObject, iStatus );
    SetActive();
    }

// ----------------------------------------------------------------------------
// CObjectExchangeClient::StopL()
// Aborts command.
// ----------------------------------------------------------------------------
//
void CObjectExchangeClient::StopL()
{
    if ( iClient && iClient->IsConnected() )
    {
    	iState = EWaitingToGetDevice;
        iClient->Abort();
	}
}

// ----------------------------------------------------------------------------
// CObjectExchangeClient::DisconnectL()
// Disconnects from the remote machine.
// ----------------------------------------------------------------------------
//
void CObjectExchangeClient::DisconnectL()
    {
    if ( iState == EWaitingToGetDevice )
        {
        return;
        }
    if ( iState == EWaitingToSend )
    {
        TFileName Hjrlppp;
        StringLoader::Load(Hjrlppp,R_STR_OBJDISCONTING);
        iLog.LogL(Hjrlppp);
        
        iState = EDisconnecting;
        iClient->Disconnect( iStatus );
        SetActive();
    }
    else 
        {
        User::Leave( KErrInUse );
        }
    }

// ----------------------------------------------------------------------------
// CObjectExchangeClient::IsBusy()
// True, if the client is performing some operation..
// ----------------------------------------------------------------------------
//
TBool CObjectExchangeClient::IsBusy()
    {
    return IsActive();
    }

// ----------------------------------------------------------------------------
// CObjectExchangeClient::IsConnected()
// True, if the client is performing some operation..
// ----------------------------------------------------------------------------
//
TBool CObjectExchangeClient::IsConnected()
    {
    return iState == EWaitingToSend;
    }

// End of File
