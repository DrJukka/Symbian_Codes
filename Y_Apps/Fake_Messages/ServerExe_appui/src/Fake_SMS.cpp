/* 
	Copyright (c) 2001 - 2008, 
	Dr. Jukka Silvennoinen. 
	All rights reserved 
*/

#include <e32svr.h>
#include <e32math.h>
#include <apacmdln.h>
#include <APGCLI.H>
#include <w32std.h>
#include <apgwgnam.h>
#include <txtrich.h>
#include <smutset.h>
#include <smuthdr.h>
#include <mmsconst.h>
 
#include "Fake_SMS.h" 

/* 
-----------------------------------------------------------------------------
LIBRARY         gsmu.lib
LIBRARY         smcm.lib
LIBRARY         msgs.lib 
-----------------------------------------------------------------------------
*/
CFakeSMSSender::CFakeSMSSender(void)
    {
    // Implementation not required
    }
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CFakeSMSSender::~CFakeSMSSender()
{
	delete iMtmReg;
    iMtmReg = NULL;
    
	delete iMsvSession; // delete always last, and make first4

	if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("Closing now...."));
	  	iFile.Close();
	}
	
	iFsSession.Close();
}
      
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CFakeSMSSender* CFakeSMSSender::NewL()
    {
    CFakeSMSSender* SALockServer = CFakeSMSSender::NewLC();
    CleanupStack::Pop(SALockServer) ;
    return SALockServer;
    }
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CFakeSMSSender* CFakeSMSSender::NewLC()
    {
	CFakeSMSSender* SALockServer = new (ELeave) CFakeSMSSender();
    CleanupStack::PushL(SALockServer) ;
    SALockServer->ConstructL() ;
    return SALockServer;
    }


_LIT(KRecFilename					,"C:\\SFakeSMSDbg.txt");
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CFakeSMSSender::ConstructL()
{
	User::LeaveIfError(iFsSession.Connect());
    
//	iFsSession.Delete(KRecFilename);		
//	iFile.Create(iFsSession,KRecFilename,EFileWrite|EFileShareAny);

	if(iFile.SubSessionHandle())
	{
		TTime CurrTime;
		CurrTime.HomeTime();	
			
		TBuf8<200> MsgBuffer(_L8("Started: "));
		
		MsgBuffer.AppendNum(CurrTime.DateTime().Day());
					
		TInt MyMonth = (CurrTime.DateTime().Month() + 1);
		MsgBuffer.Append(_L("."));
		if(MyMonth < 10)
			MsgBuffer.AppendNum(0);
					
		MsgBuffer.AppendNum(MyMonth);
					
		MsgBuffer.Append(_L("."));						
		MsgBuffer.AppendNum(CurrTime.DateTime().Year());		
					
		MsgBuffer.Append(_L(" - "));
		
		MsgBuffer.AppendNum(CurrTime.DateTime().Hour());
		MsgBuffer.Append(_L(":"));
		if(CurrTime.DateTime().Minute() < 10)
			MsgBuffer.AppendNum(0);
		
		MsgBuffer.AppendNum(CurrTime.DateTime().Minute());
		
		MsgBuffer.Append(_L("."));
		if(CurrTime.DateTime().Second() < 10)
			MsgBuffer.AppendNum(0);
		
		MsgBuffer.AppendNum(CurrTime.DateTime().Second());
		MsgBuffer.Append(_L("\n"));
		
		iFile.Write(MsgBuffer);
	}
	
	//iMsvSession = CMsvSession::OpenAsyncL(*this);
	iMsvSession = CMsvSession::OpenSyncL(*this);
	iMtmReg = CClientMtmRegistry::NewL(*iMsvSession);
	
	

}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CFakeSMSSender::HandleSessionEventL(TMsvSessionEvent aEvent, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
{
	switch (aEvent)
    {
    case EMsvServerReady:// This event tells us that the session has been opened
		
        
        if(iFile.SubSessionHandle())
		{
			iFile.Write(_L8("Create Me\n"));
		}
        
        break;
    default:  
        break;
    }
}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CFakeSMSSender::CreateSMSMessageL(HBufC* aNunmber,HBufC* aName,HBufC* aMessage,TBool aUnicodeMsg,TTime aMsgTime)
{ 
	if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("CreateNewMessageL\n"));
	}
	
	TTime hejlpper1;
	TTime hejlpper2;
				
	hejlpper1.HomeTime();
	hejlpper2.UniversalTime();
	    	
	TTimeIntervalMinutes myInterval;
	hejlpper2.MinutesFrom(hejlpper1,myInterval);
	
	TTime useTime = aMsgTime + myInterval;
	
	if(aName && aNunmber && aMessage)
	{
	    TMsvId MsvId = CreateNewMessageL(useTime);
	    
	    if(iFile.SubSessionHandle())
		{
			iFile.Write(_L8("AppendPDUDataL\n"));
		}
		
	    AppendPDUDataL(MsvId,*aNunmber,aUnicodeMsg,*aMessage);

		if(iFile.SubSessionHandle())
		{
			iFile.Write(_L8("AppendMessageDataL\n"));
		}
		AppendMessageDataL(MsvId,*aNunmber);
		
		if(iFile.SubSessionHandle())
		{
			iFile.Write(_L8("FinalizeMessageL\n"));
		}
		FinalizeMessageL(MsvId,*aMessage,*aName,useTime);
	}

	if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("all done ok.\n"));
	}
}


/* 
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/

TMsvId CFakeSMSSender::CreateNewMessageL(TTime aMsgTime)
{
    CMsvEntry* Draftentry = CMsvEntry::NewL(*iMsvSession, KMsvDraftEntryIdValue ,TMsvSelectionOrdering());
  	CleanupStack::PushL(Draftentry);

    CMsvOperationWait* wait = CMsvOperationWait::NewLC();
    wait->Start();
    
	TMsvEntry newEntry;              // This represents an entry in the Message Server index
    newEntry.iMtm = KUidMsgTypeSMS;                         // message type is SMS
 	newEntry.iType = KUidMsvMessageEntry; //KUidMsvServiceEntry                  // this defines the type of the entry: message 
 	newEntry.iServiceId = KMsvLocalServiceIndexEntryId; //    // ID of local service (containing the standard folders)
   	newEntry.iDate = aMsgTime;                              // set the date of the entry to home time
	newEntry.SetInPreparation(ETrue);                       // a flag that this message is in preparation
	newEntry.SetReadOnly(EFalse);					
		
	CMsvOperation* oper = Draftentry->CreateL(newEntry,wait->iStatus);
    CleanupStack::PushL(oper);
    CActiveScheduler::Start();

    // ...and keep track of the progress of the create operation.
    TMsvLocalOperationProgress progress = McliUtils::GetLocalProgressL(*oper);
    User::LeaveIfError(progress.iError);
//	Draftentry->MoveL(progress.iId,KMsvGlobalInBoxIndexEntryId);

	CleanupStack::PopAndDestroy(3);//Draftentry,wait,oper
	
    return progress.iId;
 }

/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CFakeSMSSender::AppendPDUDataL(TMsvId aEntryId, const TDesC& aRecipient,TBool aUnicodeMsg,const TDesC& aBody)
{
 	CMsvEntry* entry = iMsvSession->GetEntryL(aEntryId);
    CleanupStack::PushL(entry);				
	
	CMsvStore* srore= entry->EditStoreL();
	CleanupStack::PushL(srore);
	if(srore)
	{	
		CSmsBuffer* smsBuffer = CSmsBuffer::NewL();	
		CSmsMessage* smsMsg = CSmsMessage::NewL(iFsSession,CSmsPDU::ESmsDeliver, smsBuffer);
		
		CleanupStack::PushL(smsMsg);
				
		smsMsg->SetTime(entry->Entry().iDate);
		
		if(aRecipient.Length() > 20)
			smsMsg->SetToFromAddressL(aRecipient.Left(20));
		else
			smsMsg->SetToFromAddressL(aRecipient);
		
		smsBuffer->InsertL(0,aBody);	
		
		CSmsDeliver& DSMSPDU = STATIC_CAST(CSmsDeliver&,smsMsg->SmsPDU());
		DSMSPDU.SetMoreMessagesToSend(EFalse);
		DSMSPDU.SetReplyPath(EFalse);
		DSMSPDU.SetStatusReportIndication(EFalse);

		TInt Hours(0x20);
		DSMSPDU.SetServiceCenterTimeStamp(entry->Entry().iDate,Hours);
		DSMSPDU.SetTextCompressed(EFalse);
		DSMSPDU.SetPIDType(TSmsProtocolIdentifier::ESmsPIDShortMessageType);	
		
		//DSMSPDU.SetTextCompressed(TBool aCompressed);
		//DSMSPDU.SetAlphabet(TSmsDataCodingScheme::TSmsAlphabet aAlphabet);
		
		CSmsHeader* MyHeader = CSmsHeader::NewL(smsMsg);
		CleanupStack::Pop(1);//smsMsg
		CleanupStack::PushL(MyHeader);
		
		CSmsSettings*  MySetting = CSmsSettings::NewL();
		CleanupStack::PushL(MySetting);
		
		if(aUnicodeMsg)
			MySetting->SetCharacterSet(TSmsDataCodingScheme::ESmsAlphabetUCS2);
		else
			MySetting->SetCharacterSet(TSmsDataCodingScheme::ESmsAlphabet7Bit);
			
		//MySetting->StoreL(*srore);//InternalizeL
		MyHeader->SetSmsSettingsL(*MySetting);
			
		MyHeader->StoreL(*srore);
		srore->CommitL();
		
		CleanupStack::PopAndDestroy(2);//MySetting,MyHeader,
	}	

	CleanupStack::PopAndDestroy(2);//entry,srore
}

/* 
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CFakeSMSSender::AppendMessageDataL(TMsvId aEntryId,const TDesC& aRecipient)
{	
	CMsvEntry* entry = iMsvSession->GetEntryL(aEntryId);
    CleanupStack::PushL(entry);
	
	CSmsClientMtm* MySmsMtm = static_cast<CSmsClientMtm*>(iMtmReg->NewMtmL(KUidMsgTypeSMS));
    MySmsMtm->SetCurrentEntryL(entry);
    CleanupStack::Pop(); //entry
    entry = NULL;
  	CleanupStack::PushL(MySmsMtm);

	MySmsMtm->RestoreServiceAndSettingsL();
	
	CMsvStore* srore = MySmsMtm->Entry().EditStoreL();
	CleanupStack::PushL(srore);
		
	if(srore)
	{
		MySmsMtm->SmsHeader().RestoreL(*srore);
	}
	
	CleanupStack::PopAndDestroy(srore);
    CSmsHeader& header = MySmsMtm->SmsHeader();

	if (header.Message().ServiceCenterAddress().Length() == 0)
	{
		CSmsSettings* serviceSettings = &(MySmsMtm->ServiceSettings());
    	
    	const TInt numSCAddresses = serviceSettings->ServiceCenterCount();

    	if (numSCAddresses)
		{
			TInt scIndex=0;

			scIndex = serviceSettings->DefaultServiceCenter();

	        if ((scIndex < 0) || (scIndex >= numSCAddresses))
		        {
		        scIndex = 0; //???
		        }
	        //get the service center number:
	        TPtrC serviceCentreNumber = serviceSettings->GetServiceCenter(scIndex).Address();

	        //This can only be used on SMS-SUBMIT type messages. The message PDU
	        //type can be checked by using the CSmsHeader::Type API.
	        header.SetServiceCenterAddressL(serviceCentreNumber);
		}
	}	

    MySmsMtm->SmsHeader().SetFromAddressL(aRecipient);
    MySmsMtm->SaveMessageL();
    CleanupStack::PopAndDestroy(1);//MySmsMtm
}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CFakeSMSSender::FinalizeMessageL(TMsvId aEntryId,const TDesC& aBody,const TDesC& aRecipientName,TTime aMsgTime)
{
 	CMsvEntry* entry = iMsvSession->GetEntryL(aEntryId);
    CleanupStack::PushL(entry);
	
	TMsvEntry newEntry(entry->Entry());              // This represents an entry in the Message Server index
   	newEntry.SetInPreparation(EFalse);                       // a flag that this message is in preparation
	newEntry.iDetails.Set(aRecipientName);
	newEntry.SetUnread(ETrue);
	newEntry.iDate = aMsgTime;
	newEntry.SetReadOnly(ETrue);				

	CMsvStore* srore = entry->EditStoreL();
	CleanupStack::PushL(srore);
	if(srore)
	{		
		CParaFormatLayer* paraFormatLayer = CParaFormatLayer::NewL();
		CleanupStack::PushL(paraFormatLayer);
		CCharFormatLayer* charFormatLayer = CCharFormatLayer::NewL();
		CleanupStack::PushL(charFormatLayer);
		CRichText* messageBodyContent = CRichText::NewL(paraFormatLayer, charFormatLayer);
		CleanupStack::PushL(messageBodyContent);
					
		messageBodyContent->InsertL(0,aBody);
		
		srore->StoreBodyTextL(*messageBodyContent);
		srore->CommitL();
			
		CleanupStack::PopAndDestroy(3); // messageBodyContent, charFormatLayer,paraFormatLayer

		if(aBody.Length() > 50)
			newEntry.iDescription.Set(aBody.Left(50));
		else
			newEntry.iDescription.Set(aBody);
	}
	
	CleanupStack::PopAndDestroy(srore);
	
	entry->ChangeL(newEntry);	
	entry->MoveL(aEntryId,KMsvGlobalInBoxIndexEntryId);

	CleanupStack::PopAndDestroy(entry);
}
