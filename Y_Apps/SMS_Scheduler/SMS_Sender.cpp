/* Copyright (c) 2001 - 2009, Dr. Jukka Silvennoinen, All rights reserved */


#include "SMS_Sender.h"

#include <msvids.h>
#include <mtclreg.h>
#include <mtclbase.h>
#include <txtrich.h>
#include <smsclnt.h>
#include <smuthdr.h>
#include <smutset.h>
#include <smscmds.h>
#include <msvstd.h>
#include <smut.h>
#include <mtmuibas.h>
#include <mtmdef.h>


// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CSMSSender::CSMSSender()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CSMSSender::CSMSSender(MMsvObserver& aObserver)
: CActive( CActive::EPriorityStandard ),iObserver(aObserver)
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CSMSSender::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSMSSender::ConstructL()
    {
    // Session to message server is opened asynchronously.
    iSession = CMsvSession::OpenAsyncL( *this );
    }

// -----------------------------------------------------------------------------
// CSMSSender::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSMSSender* CSMSSender::NewL(MMsvObserver& aObserver)
    {
    CSMSSender* self = NewLC(aObserver);
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CSMSSender::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSMSSender* CSMSSender::NewLC(MMsvObserver& aObserver)
    {
    CSMSSender* self = new ( ELeave ) CSMSSender(aObserver);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------
// CSMSSender::~CSMSSender()
// Destructor.
// ----------------------------------------------------------
//
CSMSSender::~CSMSSender()
    {
    Cancel();           // cancel any outstanding request

    delete iOperation;
	delete iSmsMtm;
    delete iMtmRegistry;
    delete iSession;    // session must be deleted last
    }

// -----------------------------------------------------------------------------
// CSMSSender::DoCancel()
// Cancels a request.
// -----------------------------------------------------------------------------
//
void CSMSSender::DoCancel()
    {
    if ( iOperation )
        {
        iOperation->Cancel();
        }
    }

// -----------------------------------------------------------------------------
// CSMSSender::RunL()
// Handles request completion events.
// -----------------------------------------------------------------------------
//
void CSMSSender::RunL()
    {
    User::LeaveIfError( iStatus != KErrNone );

    // Determine the current operations progress.
    // ProgressL returns an 8 bit descriptor.
    TBufC8<KMsvProgressBufferLength> progress( iOperation->ProgressL() );
    _LIT8( KCompare, "KErrNone" );
    User::LeaveIfError( !progress.Compare( KCompare ) );

    // The pointer to the current CMsvOperation object is no longer needed.
    delete iOperation;
    iOperation = NULL;

    // Determine which request has finished.
    switch ( iState )
        {
        case EWaitingForMoving:
            // Once a message is moved to Outbox it is scheduled for sending.
        	 iObserver.HandleStatusChange(MMsvObserver::EMovedToOutBox);
        	ScheduleL();
            break;

        case EWaitingForScheduling:
            {
            TMsvEntry entry( iSmsMtm->Entry().Entry() );
            TInt state( entry.SendingState() );

            if ( state == KMsvSendStateWaiting || state == KMsvSendStateScheduled)
                {
                }

            iObserver.HandleStatusChange(MMsvObserver::ESent);
            
            break;
            }

        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CSMSSender::HandleSessionEventL()
// Handles notifications of events from the Message Server.
// -----------------------------------------------------------------------------
//
void CSMSSender::HandleSessionEventL( TMsvSessionEvent aEvent,
                                      TAny* aArg1, TAny* aArg2, TAny* /*aArg3*/)
	{
	switch ( aEvent )
		{
        // Session to server established
		case EMsvServerReady:
            {
				delete iMtmRegistry;
				iMtmRegistry = NULL;
				iMtmRegistry = CClientMtmRegistry::NewL( *iSession );
				
				delete iSmsMtm;
				iSmsMtm = NULL;
				iSmsMtm = STATIC_CAST( CSmsClientMtm*, iMtmRegistry->NewMtmL( KUidMsgTypeSMS ) );
            }
			break;
        case EMsvGeneralError:
        case EMsvServerFailedToStart:
        	iObserver.HandleError(MMsvObserver::EFatalServerError);
            break;
        default:
            break;
        };
    }


// -----------------------------------------------------------------------------
// CSMSSender::SendL()
// Starts the process of creating and sending an SMS message.
// -----------------------------------------------------------------------------
//
TBool CSMSSender::SendMsgL(HBufC* aMessage,HBufC* aRecipients,TBool aUnicodeEnabled)
    {
    
    if(aMessage)
   	{
   		if(iUnicodeEnabled && (aMessage->Des().Length() > 70))
   			iMessageText.Copy(aMessage->Des().Left(70));
   		else if(aMessage->Des().Length() > 160)
   			iMessageText.Copy(aMessage->Des().Left(160));
   		else
   			iMessageText.Copy(*aMessage);
   	}
   	else
   		iMessageText.Zero();
    
	if(aRecipients)
		iRecipientNumber.Copy(*aRecipients);
	else
		iRecipientNumber.Zero();
    
	iUnicodeEnabled = aUnicodeEnabled;
	
	if(iUnicodeEnabled)
		iCharSet = TSmsDataCodingScheme::ESmsAlphabetUCS2;
	else
		iCharSet = TSmsDataCodingScheme::ESmsAlphabet7Bit;
	
    if ( CreateMsgL() )
    {
        return ETrue;
    }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSMSSender::CreateMsgL()
// Create an SMS message.
// -----------------------------------------------------------------------------
//
TBool CSMSSender::CreateMsgL()
    {
    // Current entry is the Draft folder.
    iSmsMtm->SwitchCurrentEntryL( KMsvDraftEntryId );

    // Create a new SMS message entry as a child of the current context.
    iSmsMtm->CreateMessageL( KUidMsgTypeSMS.iUid );

    CMsvEntry& serverEntry = iSmsMtm->Entry();
    TMsvEntry entry( serverEntry.Entry() );

    CRichText& body = iSmsMtm->Body();   // the body of the message
    body.Reset();
    // Insert the message text gotten as input from user.
    body.InsertL( 0, iMessageText );

    // Message will be sent immediately.
    entry.SetSendingState( KMsvSendStateWaiting );
	entry.iDate.UniversalTime(); // insert current time //Solution for HomeTime()

    // Set the SMS message settings for the message.
    CSmsHeader& header = iSmsMtm->SmsHeader();
    CSmsSettings* settings = CSmsSettings::NewL();
    CleanupStack::PushL( settings );

    settings->CopyL( iSmsMtm->ServiceSettings() );    // restore settings
    settings->SetDelivery( ESmsDeliveryImmediately ); // to be delivered immediately
	settings->SetDeliveryReport(ETrue);
	settings->SetCharacterSet(iCharSet);
	
    header.SetSmsSettingsL( *settings );              // new settings

	// Let's check if there is a service center address.
	if ( header.Message().ServiceCenterAddress().Length() == 0 )
		{
		// No, there isn't. We assume there is at least one service center
        // number set and use the default service center number.
		CSmsSettings* serviceSettings = &( iSmsMtm->ServiceSettings() );

        // Check if number of service center addresses in the list is null.
		if ( !serviceSettings->ServiceCenterCount() )
            {
			return EFalse;     // quit creating the message
            }

		else
			{
				CSmsNumber* smsCenter= CSmsNumber::NewL();
				CleanupStack::PushL(smsCenter);
				smsCenter->SetAddressL((serviceSettings->GetServiceCenter( serviceSettings->DefaultServiceCenter())).Address());
				header.Message().SetServiceCenterAddressL( smsCenter->Address() );
				CleanupStack::PopAndDestroy(smsCenter);
		    }
        }

	CleanupStack::PopAndDestroy( settings );

    // Recipient number is displayed also as the recipient alias.
    entry.iDetails.Set( iRecipientNumber );
    // Add addressee.
    iSmsMtm->AddAddresseeL( iRecipientNumber, entry.iDetails );

    // Validate message.
    if ( !ValidateL() )
        {
        return EFalse;
        }

    entry.SetVisible( ETrue );          // set message as visible
    entry.SetInPreparation( EFalse );   // set together with the visibility flag
    serverEntry.ChangeL( entry );       // commit changes		
    iSmsMtm->SaveMessageL();            // save message

    TMsvSelectionOrdering selection;
	CMsvEntry* parentEntry = CMsvEntry::NewL( iSmsMtm->Session(), KMsvDraftEntryId, selection );
    CleanupStack::PushL( parentEntry );

    // Move message to Outbox.
    iOperation =parentEntry->MoveL( entry.Id(), KMsvGlobalOutBoxIndexEntryId, iStatus );

    CleanupStack::PopAndDestroy( parentEntry );

    iState = EWaitingForMoving;
    SetActive();

    return ETrue;
  }

// -----------------------------------------------------------------------------
// CSMSSender::ValidateL()
// Validate an SMS message.
// -----------------------------------------------------------------------------
//
TBool CSMSSender::ValidateL()
    {
    // Empty part list to hold the result.
    TMsvPartList result( KMsvMessagePartNone );

    // Validate message body.
    result = iSmsMtm->ValidateMessage( KMsvMessagePartBody );

    if ( result != KMsvMessagePartNone )
        {
        return EFalse;
        }

    // Validate recipient.
    result = iSmsMtm->ValidateMessage( KMsvMessagePartRecipient );

    if ( result != KMsvMessagePartNone )
        {
        return EFalse;
        }

    return ETrue;
    }

// -----------------------------------------------------------------------------
// CSMSSender::ScheduleL()
// Schedule an SMS message for sending.
// -----------------------------------------------------------------------------
//
void CSMSSender::ScheduleL()
    {
    CMsvEntrySelection* selection = new ( ELeave ) CMsvEntrySelection;
    CleanupStack::PushL( selection );
    selection->AppendL( iSmsMtm->Entry().EntryId() ); // add message to selection

    // Add entry to task scheduler.
    TBuf8<1> dummyParams;   // dummy parameters needed for InvokeAsyncFunctionL
    iOperation = iSmsMtm->InvokeAsyncFunctionL( ESmsMtmCommandScheduleCopy,*selection, dummyParams, iStatus );

    CleanupStack::PopAndDestroy( selection );

    iState = EWaitingForScheduling;
    SetActive();
    }

