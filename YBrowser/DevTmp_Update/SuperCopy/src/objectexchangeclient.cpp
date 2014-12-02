/* Copyright (c) 2004, Nokia. All rights reserved */


// INCLUDE FILES

#include "ObjectExchangeClient.h"
#include "ObjectExchangeServiceSearcher.h"
#include "Log.h"

#include "InboxReader.h"


// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CObjectExchangeClient::NewL()
// Symbian two-phased constructor.
// ----------------------------------------------------------------------------
//
CObjectExchangeClient* CObjectExchangeClient::NewL( MLog& aLog )
    {
    CObjectExchangeClient* self = NewLC( aLog );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CObjectExchangeClient::NewLC()
// Symbian two-phased constructor.
// ----------------------------------------------------------------------------
//
CObjectExchangeClient* CObjectExchangeClient::NewLC( MLog& aLog )
    {
    CObjectExchangeClient* self = new ( ELeave ) CObjectExchangeClient( aLog );
    CleanupStack::PushL( self );
    self->ConstructL();
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
void CObjectExchangeClient::ConstructL()
    {
    iServiceSearcher = CObjectExchangeServiceSearcher::NewL( iLog );
    }

// ----------------------------------------------------------------------------
// CObjectExchangeClient::DoCancel()
// Cancel any outstanding requests.
// ----------------------------------------------------------------------------
//
void CObjectExchangeClient::DoCancel()
    {
    }

// ----------------------------------------------------------------------------
// CObjectExchangeClient::RunL()
// Respond to an event.
// ----------------------------------------------------------------------------
//
void CObjectExchangeClient::RunL()
    {

    if ( iStatus != KErrNone )
    {
        switch ( iState )
            {
            case EGettingDevice:
                if ( iStatus == KErrCancel )
                {
                    iLog.LogL(_L("No Device"));
                    
                }
                iState = EWaitingToGetDevice;
                break;
            case EGettingService:
            case EGettingConnection:
            case EDisconnecting:
                iLog.LogL(_L("Connection failed: "), iStatus.Int() );
                iState = EWaitingToGetDevice;
                break;
            case EWaitingToSend:
            	iLog.LogL(_L("Sending Error: "), iStatus.Int() );
                iState = EWaitingToGetDevice;
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
                iLog.LogL(_L("Connecting..."));
                iState = EGettingConnection;
                ConnectToServerL();
                break;

            case EGettingConnection:
                {
                	iLog.LogL(_L("Sending..."));
	                iState = EWaitingToSend;
	                
	                TFindFile MyIconFile(CCoeEnv::Static()->FsSession());
					if(KErrNone == MyIconFile.FindByDir(KtxTargetSisFileName, KNullDesC))	
					{
						iRealSis = ETrue;
						SendObjectL(MyIconFile.File());
					}
					else
					{
					 	TFindFile MyIconFile2(CCoeEnv::Static()->FsSession());
						if(KErrNone == MyIconFile2.FindByDir(KtxTargetTxtFileName, KNullDesC))	
						{
							iRealSis = EFalse;
							SendObjectL(MyIconFile2.File());
						}
					}            
                }
                break;
            case EDisconnecting:
                iLog.LogL(_L("Disconnected"));
                iState = EWaitingToGetDevice;
                break;
			case EWaitingToSend:
                iLog.DoneL(KErrNone,iRealSis);
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
    if ( iState == EWaitingToGetDevice && !IsActive() )
    {
        iServiceSearcher->SelectDeviceByDiscoveryL( iStatus );
        iLog.LogL(_L("Finding device..."));
        iState = EGettingDevice;
        SetActive();
    }
    else
    {
        iLog.LogL(_L("BT is busy"));
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
        iClient->Abort();
        iState = EWaitingToGetDevice;
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
        iLog.LogL(_L("Disconnecting"));
        
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
