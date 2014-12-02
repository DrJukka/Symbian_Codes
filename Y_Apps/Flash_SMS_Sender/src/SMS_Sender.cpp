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


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CSMSSender* CSMSSender::NewL(MMsvObserver& aObserver)
    {
    CSMSSender* self = CSMSSender::NewLC(aObserver);
    CleanupStack::Pop();
    return self;
    }

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
    
CSMSSender* CSMSSender::NewLC(MMsvObserver& aObserver)
    {
    CSMSSender* self = new (ELeave) CSMSSender(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CSMSSender::CSMSSender(MMsvObserver& aObserver) 
    : CActive(EPriorityStandard), iPhase(EIdle), iOperation(NULL), iSession(NULL), iMtm(NULL), 
      iMtmRegistry(NULL), iObserver(aObserver)
    {
    // No implementation required
    }




/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CSMSSender::~CSMSSender()
{
   	Cancel(); // Causes call to DoCancel() if active
    
    delete iOperation;
    iOperation = NULL;

    delete iMtm;
    iMtm = NULL;

    delete iMtmRegistry;
    iMtmRegistry = NULL;

    delete iSession;    // session must be deleted last (and constructed first)
    iSession = NULL;

	delete iDetails;
	iDetails = NULL;
    delete iDescription;
    iDescription = NULL;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSMSSender::ConstructL()
{
 	CActiveScheduler::Add(this);	
    iSession = CMsvSession::OpenSyncL(*this);
    iMtmRegistry = CClientMtmRegistry::NewL(*iSession);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSMSSender::SendMsgL(const TDesC& aMessage,const TDesC& aRecipients,TBool aUnicodeEnabled,TBool aFlashSMS)
{

	iMessageMade = iMessageSent = iMessageReceived = EFalse;
	iUnicodeEnabled = aUnicodeEnabled;
	iFlashSMS = aFlashSMS;
	
	Cancel();
	
	if(iUnicodeEnabled && (aMessage.Length() > 70))
		iMessage.Copy(aMessage.Left(70));
	else if(aMessage.Length() > 160)
		iMessage.Copy(aMessage.Left(160));
	else
		iMessage.Copy(aMessage);
	
	iRecipients.Copy(aRecipients);
  	
	if(iUnicodeEnabled)
		iCharSet = TSmsDataCodingScheme::ESmsAlphabetUCS2;
	else
		iCharSet = TSmsDataCodingScheme::ESmsAlphabet7Bit;
  	
	CreateNewMessageL();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSMSSender::DoCancel()
{
    if (iOperation)
	{
        iOperation->Cancel();
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
// from CActive
void CSMSSender::RunL()
{
    iMessageMade = ETrue;

    if(iStatus.Int() != KErrNone){
		iObserver.HandleError(iStatus.Int());
    }else{    
		// Determine the current operations progress
		// Note : This class is only appropriate to Locally acting operations
		//        such as Create, Move, Delete.
		//        For the CBaseMtm::InvokeAsyncFunctionL, used to schedule the messange for sending,
		//        no status is available as this is not a local operation, so we use the fact
		//        that sent messages are moved to the 'Sent' folder which we can detect using
		//        the HandleSessionEventL
		TMsvLocalOperationProgress progress;
		TUid mtmUid = iOperation->Mtm();
		if (mtmUid == KUidMsvLocalServiceMtm)
		{
			progress = McliUtils::GetLocalProgressL(*iOperation);
	
			if(progress.iError != KErrNone){
				iObserver.HandleError(MMsvObserver::EFatalServerError);
				return;
			}
		}
		else
		{
			if (iPhase != EWaitingForScheduled)
			{
				iObserver.HandleError(MMsvObserver::EScheduleFailed);
				return;
			}
		}
	
		delete iOperation;
		iOperation = NULL;
	
		switch (iPhase)
		{
		case EWaitingForCreate:
			{
				iObserver.HandleStatusChange(MMsvObserver::ECreated);
				SetMtmEntryL(progress.iId);
				if (InitializeMessageL())
				{
					if (MoveMessageEntryL(KMsvGlobalOutBoxIndexEntryId))
					{
						iObserver.HandleStatusChange(MMsvObserver::ECreated);
					
						iPhase = EWaitingForMove;
					}
					else
					{
						iPhase = EIdle;
					}
				}
				else
				{
					iPhase = EIdle;
				}
			}
			break;
		case EWaitingForMove:
			{
				iObserver.HandleStatusChange(MMsvObserver::EMovedToOutBox);
				// We must create an entry selection for message copies 
				// (although now we only have one message in selection)
				CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection;
				CleanupStack::PushL(selection);
	
				selection->AppendL(progress.iId);
	
				// schedule the sending with the active scheduler
				SetScheduledSendingStateL(*selection);
	
				CleanupStack::PopAndDestroy(selection);
				iPhase = EWaitingForScheduled;
			}
			break;
		case EWaitingForScheduled:
			{
				const TMsvEntry& msvEntry = iMtm->Entry().Entry();
				TInt state = msvEntry.SendingState();
				if (state != KMsvSendStateScheduled)
				{
					iObserver.HandleError(MMsvObserver::EScheduleFailed);
					iPhase = EIdle;
					return;
				}
				else
				{
					iObserver.HandleStatusChange(MMsvObserver::EScheduledForSend);
		
					iPhase = EWaitingForSent;
				}
			}
			break;
		case EWaitingForDeleted:
			{    
				iObserver.HandleStatusChange(MMsvObserver::EDeleted);
				iPhase = EIdle;
			}
			break;
	   case EWaitingForSent:  // We handle this in HandleSessionEventL
	   case EIdle:            // Shouldn't get triggered in this state
	   default:
		   break;
	   }
    }
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSMSSender::CreateNewMessageL()
{
	Cancel();
	iMessageMade = iMessageSent = iMessageReceived = EFalse;
	
	delete iMtm;
	iMtm = NULL;

    TMsvEntry newEntry;              // This represents an entry in the Message Server index
    newEntry.iMtm = KUidMsgTypeSMS;                         // message type is SMS
	newEntry.iType = KUidMsvMessageEntry;                   // this defines the type of the entry: message 
	newEntry.iServiceId = KMsvLocalServiceIndexEntryId;     // ID of local service (containing the standard folders)
	newEntry.iDate.UniversalTime();                              // set the date of the entry to home time
	newEntry.SetInPreparation(ETrue);                       // a flag that this message is in preparation
   	newEntry.SetReadOnly(EFalse);
   	
    // - CMsvEntry accesses and acts upon a particular Message Server entry.
    // - NewL() does not create a new entry, but simply a new object to access an existing entry.
    // - It takes in as parameters the client's message server session,
    //   ID of the entry to access and initial sorting order of the children of the entry. 
    CMsvEntry* entry = CMsvEntry::NewL(*iSession, KMsvDraftEntryIdValue, TMsvSelectionOrdering());
    CleanupStack::PushL(entry);

    delete iOperation;
    iOperation = NULL;
    iOperation = entry->CreateL(newEntry, iStatus);
    CleanupStack::PopAndDestroy(entry);

    iPhase = EWaitingForCreate;
    SetActive();    
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

TBool CSMSSender::SetupSmsHeaderL()
{
    if (iMtm)
 	{ 
        // To handle the sms specifics we start using SmsMtm
        CSmsClientMtm* smsMtm = static_cast<CSmsClientMtm*>(iMtm);
        // 
        smsMtm->RestoreServiceAndSettingsL();

        // CSmsHeader encapsulates data specific for sms messages,
        // like service center number and options for sending.
        CSmsHeader&   header          = smsMtm->SmsHeader();
        CSmsSettings& serviceSettings = smsMtm->ServiceSettings();

        CSmsSettings* sendOptions = CSmsSettings::NewL();
        CleanupStack::PushL(sendOptions);
        sendOptions->CopyL(serviceSettings); // restore existing settings
		sendOptions->SetCharacterSet(iCharSet);
		sendOptions->SetDelivery(ESmsDeliveryImmediately);      // set to be delivered immediately
		
		if(iFlashSMS){
			header.Message().SmsPDU().SetClass(ETrue,TSmsDataCodingScheme::ESmsClass0);
		}
		
        header.SetSmsSettingsL(*sendOptions);

        CleanupStack::PopAndDestroy(sendOptions);
    
   	#ifdef __WINS__
		
	#else	
        // let's check if there's a service centre address
        if (header.Message().ServiceCenterAddress().Length() == 0)
        {
            // no, there isn't. We assume there is at least one sc number set and use
            // the default SC number. 

		#ifdef __SERIES60_3X__
			const TInt numSCAddresses = serviceSettings.ServiceCenterCount();
		#else
			const TInt numSCAddresses = serviceSettings.NumSCAddresses();
		#endif

			if (numSCAddresses)
			{
				TInt scIndex=0;
	
			#ifdef __SERIES60_3X__
				scIndex = serviceSettings.DefaultServiceCenter();
			#else
				scIndex = serviceSettings.DefaultSC();
			#endif
				
				if ((scIndex < 0) || (scIndex >= numSCAddresses))
					{
					scIndex = 0; //???
					}
				//get the service center number:
			#ifdef __SERIES60_3X__
				TPtrC serviceCentreNumber = serviceSettings.GetServiceCenter(scIndex).Address();
			#else
				TPtrC serviceCentreNumber = serviceSettings.SCAddress(scIndex).Address();
			#endif				

				//This can only be used on SMS-SUBMIT type messages. The message PDU
				//type can be checked by using the CSmsHeader::Type API.
				header.SetServiceCenterAddressL(serviceCentreNumber);
			}
            else
            {
                // here there could be a dialog in which user can add sc number
                iObserver.HandleError(MMsvObserver::ENoServiceCentre);
                return EFalse;
            }
        }
    #endif
        return ETrue;
    }
    else
    {
        return EFalse;
    }
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSMSSender::PopulateMessageL(TMsvEntry& aMsvEntry,TDes& aDescription,TDes& aDetails)
    {
    ASSERT(iMtm);

	// We get the message body from Mtm and insert a bodytext
	CRichText& mtmBody = iMtm->Body();
	mtmBody.Reset();
	mtmBody.InsertL(0, iMessage);   // insert our msg tag as the body text

	if(iDescription)
	{	
		if(iDescription->Des().Length() > 30)
			aDescription.Copy(iDescription->Des().Left(30));
		else
			aDescription.Copy(iDescription->Des());
	}
	else
	{	
		if(iMessage.Length() > 30)
			aDescription.Copy(iMessage.Left(30));
		else
			aDescription.Copy(iMessage);
	}	    	    	
	
    if(iDetails)
    {
    	if(iDetails->Des().Length() > 30)
			aDetails.Copy(iDetails->Des().Left(30));
		else
			aDetails.Copy(iDetails->Des());
    }
    else
	{ 
		if(iRecipients.Length() > 30)
			aDetails.Copy(iRecipients.Left(30));
		else
			aDetails.Copy(iRecipients);	
	}
	
	aMsvEntry.SetInPreparation(EFalse);            
	aMsvEntry.SetScheduled(EFalse);
	aMsvEntry.iDate.UniversalTime();
	aMsvEntry.SetSendingState(KMsvSendStateWaiting); 
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TBool CSMSSender::InitializeMessageL()
    {
    ASSERT(iMtm);

    TMsvEntry msvEntry = (iMtm->Entry()).Entry();

    TBuf<30> MyDescription,MyDetails;
    PopulateMessageL(msvEntry,MyDescription,MyDetails);

	msvEntry.iDescription.Set(MyDescription);
	msvEntry.iDetails.Set(MyDetails);

    // Set SMS specific Mtm fields
    if (!SetupSmsHeaderL())
        {
        return EFalse;
        }

    // Add our recipient to the list, takes in two TDesCs, first is real address and second is an alias
    // works also without the alias parameter.
    iMtm->AddAddresseeL(iRecipients, msvEntry.iDetails);
    
    // save message
    CMsvEntry& entry = iMtm->Entry();
    entry.ChangeL(msvEntry);              // commit index changes
    iMtm->SaveMessageL();                 // commit message to message server

    return ETrue;
    }

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TBool CSMSSender::MoveMessageEntryL(TMsvId aTarget)
    {
    ASSERT(iMtm);

    TMsvEntry msvEntry((iMtm->Entry()).Entry());

    if (msvEntry.Parent() != aTarget)
        {
        TMsvSelectionOrdering sort;
        sort.SetShowInvisibleEntries(ETrue);    // we want to also handle the invisible entries
        // Take a handle to the parent entry
        CMsvEntry* parentEntry = CMsvEntry::NewL(iMtm->Session(), msvEntry.Parent(), sort);
        CleanupStack::PushL(parentEntry);

        // Move original from the parent to the new location
        iOperation = parentEntry->MoveL(msvEntry.Id(), aTarget, iStatus);

        CleanupStack::PopAndDestroy(parentEntry);
        SetActive();

        return ETrue;
        }
    return EFalse;
    }


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSMSSender::SetScheduledSendingStateL(CMsvEntrySelection& aSelection)
    {
    ASSERT(iMtm);

    // Add entry to task scheduler
    TBuf8<1> dummyParams;
    // invoking async schedule copy command on our mtm
    iOperation = iMtm->InvokeAsyncFunctionL(
                        ESmsMtmCommandScheduleCopy,
                        aSelection,
                        dummyParams,
                        iStatus);
    SetActive();
    }


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSMSSender::HandleChangedEntryL(TMsvId aEntryId)
{
    // if we've no Mtm then we can't be in the middle of sending a message
    if (iMtm)
 	{
        TMsvEntry msvEntry((iMtm->Entry()).Entry());
        if (msvEntry.Id() == aEntryId)
        {
            if (msvEntry.SendingState() == KMsvSendStateSent)
            {
            	iMessageSent = ETrue;
            	iObserver.HandleStatusChange(MMsvObserver::ESent);
            }
            else if (msvEntry.SendingState() == KMsvSendStateFailed)
            {
                iPhase = EIdle;
                iObserver.HandleError(MMsvObserver::ESendFailed);
            }
        }
    }
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSMSSender::HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* /*aArg2*/, TAny* /*aArg3*/)
    {
    switch (aEvent)
        {
        case EMsvEntriesChanged:
            {
            // Find out what happened to the message we scheduled for sending
            if (iPhase == EWaitingForSent)
                {
                // We take the moved entries into a selection
                CMsvEntrySelection* entries = static_cast<CMsvEntrySelection*>(aArg1);

                 //Process each created entry, one at a time.
                for(TInt i = 0; i < entries->Count(); i++)
                    {
                    HandleChangedEntryL(entries->At(i));       
                    }
                }
            }
            break;
        case EMsvEntriesCreated:        // A new entry has been created in the message server
		case EMsvEntriesMoved:      // this event is given when message entries are moved
			{// 

			}
			break;
        case EMsvServerReady:
        
            break;
        case EMsvCloseSession:
            iSession->CloseMessageServer();
            break;
        case EMsvServerTerminated:
            iSession->CloseMessageServer();
            break;
        case EMsvGeneralError:
        case EMsvServerFailedToStart:
            iObserver.HandleError(MMsvObserver::EFatalServerError);
            break;
        default:
            // All other events are ignored
            break;
        }
    }


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSMSSender::SetMtmEntryL(TMsvId aEntryId)
    {
    // Get the server entry from our session
    CMsvEntry* entry = iSession->GetEntryL(aEntryId);
    CleanupStack::PushL(entry);
    
    // Check if our mtm is different from the mtm set to our entry
    if ((iMtm == NULL) || (entry->Entry().iMtm != (iMtm->Entry()).Entry().iMtm))
        {
        // If so, we delete the old... 
        delete iMtm;
        iMtm = NULL;
        
        // ...and get a new one from the MtmRegistry
        iMtm = iMtmRegistry->NewMtmL(entry->Entry().iMtm);
        }

    iMtm->SetCurrentEntryL(entry);
    CleanupStack::Pop(entry); 
    }

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSMSSender::DeleteEntryL(TMsvEntry& aMsvEntry)
    {
    // Take a handle to the parent entry
    CMsvEntry* parentEntry = CMsvEntry::NewL(*iSession, aMsvEntry.Parent(), TMsvSelectionOrdering());

    CleanupStack::PushL(parentEntry);

    // here parentEntry is the Sent folder (must be so that we can call DeleteL) 
    iOperation = parentEntry->DeleteL(aMsvEntry.Id(), iStatus);

    CleanupStack::PopAndDestroy(parentEntry);

    SetActive();
    }
