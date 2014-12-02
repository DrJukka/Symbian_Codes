/* 
	Copyright (c) 2001 - 2008 ,
	Jukka Silvennoinen, 
	All rights reserved 
*/
#include <avkon.hrh>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <uri8.h>
#include <http.h>
#include <chttpformencoder.h>
#include <HttpStringConstants.h>
#include <http\RHTTPTransaction.h>
#include <http\RHTTPSession.h>
#include <http\RHTTPHeaders.h>
#include <COMMDB.H> 
#include <apselect.h> 

#include <COMMDB.H> 		//Communications database 
#include <CDBPREFTABLE.H>	//Connection Preferences table
#include <CommDbConnPref.h>

#include "HTTP_Engine.h"

// Used user agent for requests
_LIT8(KUserAgent, "Y-Update 1.0");

// This client accepts all content types.
// (change to e.g. "text/plain" for plain text only)
_LIT8(KAccept, "*/*");

_LIT8(KNoCache, "no-cache");

const TInt KStatustextBufferSize 	= 32;
const TInt KInfotextBufferSize 		= 64;
const TInt KURIBufferSize 			= 128;
const TInt KDefaultBufferSize 		= 256;


_LIT8(KContentTypeURLEncoded		,"application/x-www-form-urlencoded\0");

_LIT8(KContentTypeMultipart			,"multipart/form-data\0");


/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CHTTPEngine* CHTTPEngine::NewL(MClientObserver& aObserver)
	{
    CHTTPEngine* self = CHTTPEngine::NewLC(aObserver);
    CleanupStack::Pop(self);
    return self;
	}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CHTTPEngine* CHTTPEngine::NewLC(MClientObserver& aObserver)
	{
    CHTTPEngine* self = new (ELeave) CHTTPEngine(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
	}


/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CHTTPEngine::CHTTPEngine(MClientObserver& aObserver)
:iObserver(aObserver),iPostData(NULL),iRunning(EFalse),iIapSet(EFalse)
{
}


/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CHTTPEngine::~CHTTPEngine()
{	
	iSession.Close();
    
    delete iIAPConnector;
	iIAPConnector = NULL;
    
    iSocketServ.Close();

	delete iUrl;
	delete iPostData;
	
	iFileToWrite.Close();
	iFileSession.Close();
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CHTTPEngine::ConstructL()
{
	User::LeaveIfError(iFileSession.Connect());

	// Open RHTTPSession with default protocol ("HTTP/TCP")
	TRAPD(err, iSession.OpenL());
	if(err != KErrNone) 
	{
//		HBufC* Buff2 = StringLoader::LoadLC(R_STR_CANNOTCON);
		iObserver.ClientEvent(_L("Session"),_L("Error"),err);
//		CleanupStack::PopAndDestroy(Buff2);	
		User::Leave(err);
	}

	// Install this class as the callback for authentication requests. When
	// page requires authentication the framework calls GetCredentialsL to get
	// user name and password.
	InstallAuthenticationL(iSession);
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CHTTPEngine::SetIap(TUint32 aIap)
{
	iIap = aIap;
	iIapSet = ETrue;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CHTTPEngine::SetHeaderL(RHTTPHeaders aHeaders,TInt aHdrField,const TDesC8& aHdrValue)
{
	RStringF valStr = iSession.StringPool().OpenFStringL(aHdrValue);
	CleanupClosePushL(valStr);
	
	THTTPHdrVal val(valStr);
	aHeaders.SetFieldL(iSession.StringPool().StringF(aHdrField,RHTTPSession::GetTable()), val);
	
	CleanupStack::PopAndDestroy(); // valStr
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CHTTPEngine::IapConnectionDoneL(TInt aError)
{
	if(aError == KErrNone)
	{
		TBool OkToGo(EFalse);
		iConnectionSetupDone = ETrue;
		
		if(iDataReceiver == NULL)
		{
			iFileSession.Delete(iFileName);
			
			TInt FileErr = iFileToWrite.Create(iFileSession,iFileName,EFileWrite|EFileShareAny);
			if(FileErr == KErrNone)
			{
				OkToGo = ETrue;
			}
			else
			{
				aError = FileErr;
			}	
		}
		else
		{
			OkToGo = ETrue;
		}
		
		if(OkToGo)
		{
			// Parse string to URI
			TUriParser8 uri;
			
			if(iUrl)
			{
				uri.Parse(*iUrl);
			}
				
			if(iPost)
			{
				// Get request method string for HTTP POST
				RStringF method = iSession.StringPool().StringF(HTTP::EPOST,RHTTPSession::GetTable());

				// Open transaction with previous method and parsed uri. This class will
				// receive transaction events in MHFRunL and MHFRunError.
				iTransaction = iSession.OpenTransactionL(uri, *this, method);

				// Set headers for request; user agent, accepted content type and body's
				// content type.
				RHTTPHeaders hdr = iTransaction.Request().GetHeaderCollection();
				SetHeaderL(hdr, HTTP::EUserAgent, KUserAgent);
				SetHeaderL(hdr, HTTP::EAccept, KAccept);
				SetHeaderL(hdr, HTTP::EPragma,KNoCache);
				SetHeaderL(hdr, HTTP::ECacheControl,KNoCache);
				
				switch(iContType)
				{
				case EURLEncoded:
					SetHeaderL(hdr, HTTP::EContentType, KContentTypeURLEncoded);	
					break;
				case EMultiPart:
					SetHeaderL(hdr, HTTP::EContentType, KContentTypeMultipart);
					break;
				default://ENoContent
					break;
				};
				
				// Set this class as an data supplier. Inherited MHTTPDataSupplier methods
				// are called when framework needs to send body data.
				MHTTPDataSupplier* dataSupplier = this;
				iTransaction.Request().SetBody(*dataSupplier);
			}
			else
			{
				// Get request method string for HTTP GET
				RStringF method = iSession.StringPool().StringF(HTTP::EGET,RHTTPSession::GetTable());

				// Open transaction with previous method and parsed uri. This class will
				// receive transaction events in MHFRunL and MHFRunError.
				
				iTransaction = iSession.OpenTransactionL(uri, *this, method);

				// Set headers for request; user agent and accepted content type
				RHTTPHeaders hdr = iTransaction.Request().GetHeaderCollection();
				SetHeaderL(hdr, HTTP::EUserAgent, KUserAgent);
				SetHeaderL(hdr, HTTP::EAccept, KAccept);
				SetHeaderL(hdr, HTTP::EPragma,KNoCache);
				SetHeaderL(hdr, HTTP::ECacheControl,KNoCache);	
			}
			
			
			// Submit the transaction. After this the framework will give transaction
			// events via MHFRunL and MHFRunError.
			iTransaction.SubmitL();

			iRunning = ETrue;
//			HBufC* Buff2 = StringLoader::LoadLC(R_STR_CONNECTINSRV);
			iObserver.ClientEvent(_L("Connecting"),_L("Server"),KErrNone);
//			CleanupStack::PopAndDestroy(Buff2);	
		}
		else
		{	
		//	HBufC* Buff2 = StringLoader::LoadLC(R_STR_GENERALERROR);
			iObserver.ClientEvent(_L("Server"),_L("Error"),aError);
		//	CleanupStack::PopAndDestroy(Buff2);	
		}
	}
	else if(aError != KErrCancel)
	{
//		HBufC* Buff2 = StringLoader::LoadLC(R_STR_GENERALERROR);
		iObserver.ClientEvent(_L("Server"),_L("Error"),aError);
//		CleanupStack::PopAndDestroy(Buff2);
	}
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CHTTPEngine::IssueHTTPGetL(const TDesC8& aUri, MDataReceiver* aDataReceiver, TBool aAskForIAP)
{
	iDataReceiver = aDataReceiver;
	iFileName.Zero();
	
	if(iFileToWrite.SubSessionHandle())
	{
		iFileToWrite.Close();
	}

	delete iUrl;
	iUrl = NULL;
	iUrl = aUri.AllocL();

	iPost = EFalse;
	if(SetupConnectionL(aAskForIAP))
	{
	  // wait till IAP completed, will continue automatically	
	}
	else
	{
		IapConnectionDoneL(KErrNone);
	}	
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CHTTPEngine::IssueHTTPGetL(const TDesC8& aUri, const TDesC& aFileName, TBool aAskForIAP)
{	
	iDataReceiver = NULL;

	if(iFileToWrite.SubSessionHandle())
	{
		iFileToWrite.Close();
	}
	
	iFileName.Copy(aFileName);
	
	delete iUrl;
	iUrl = NULL;
	iUrl = aUri.AllocL();

	iPost = EFalse;
	if(SetupConnectionL(aAskForIAP))
	{
	  // wait till IAP completed, will continue automatically	
	}
	else
	{
		IapConnectionDoneL(KErrNone);
	}	
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CHTTPEngine::IssueHTTPPostL(const TDesC8& aUri,
								 const TDesC8& aBody,
								 MDataReceiver* aDataReceiver,
								 TContType aContType,
								 TBool aAskForIAP)
{

	iDataReceiver = aDataReceiver;
	iContType = aContType;

	if(iFileToWrite.SubSessionHandle())
	{
		iFileToWrite.Close();
	}
	
	iFileName.Zero();
	
	delete iUrl;
	iUrl = NULL;
	iUrl = aUri.AllocL();
    
	// Copy data to be posted into member variable; iPostData is used later in
	// methods inherited from MHTTPDataSupplier.
	delete iPostData;
	iPostData = aBody.AllocL();
	
	iPost = ETrue;
	if(SetupConnectionL(aAskForIAP))
	{
	  // wait till IAP completed, will continue automatically	
	}
	else
	{
		IapConnectionDoneL(KErrNone);
	}
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CHTTPEngine::IssueHTTPPostL(const TDesC8& aUri,
								 const TDesC8& aBody,
								 const TDesC& aFileName,
								 TContType aContType, 
								 TBool aAskForIAP)
{

	iDataReceiver = NULL;
	iContType = aContType;

	if(iFileToWrite.SubSessionHandle())
	{
		iFileToWrite.Close();
	}
	
	iFileName.Copy(aFileName);
	
	delete iUrl;
	iUrl = NULL;
	iUrl = aUri.AllocL();
    
	// Copy data to be posted into member variable; iPostData is used later in
	// methods inherited from MHTTPDataSupplier.
	delete iPostData;
	iPostData = aBody.AllocL();
	
	iPost = ETrue;
	if(SetupConnectionL(aAskForIAP))
	{
	  // wait till IAP completed, will continue automatically	
	}
	else
	{
		IapConnectionDoneL(KErrNone);
	}
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CHTTPEngine::CancelTransaction()
{
	if(iRunning)
	{
		// Close() also cancels transaction (Cancel() can also be used but
		// resources allocated by transaction must be still freed with Close())
		iTransaction.Close();

		// Not running anymore
		iRunning = EFalse;
		
//		HBufC* Buff2 = StringLoader::LoadLC(R_STR_TRANSCANCELLED);
		iObserver.ClientEvent(_L("Download"),_L("Cancelled"),KErrNone);
//		CleanupStack::PopAndDestroy(Buff2);	
		
		if(iFileToWrite.SubSessionHandle())
		{
			iFileToWrite.Close();
		}
		
		iObserver.ClientCancelled(iFileName,KErrCancel);
	}
	else if(!iConnectionSetupDone)
	{
		delete iIAPConnector;
		iIAPConnector = NULL;
	}
}


/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CHTTPEngine::MHFRunL(RHTTPTransaction aTransaction,const THTTPEvent& aEvent)
	{
	switch (aEvent.iStatus)
		{
		case THTTPEvent::EGotResponseHeaders:
			{
			// HTTP response headers have been received. Use
			// aTransaction.Response() to get the response. However, it's not
			// necessary to do anything with the response when this event occurs.

			// Get HTTP status code from header (e.g. 200)
			RHTTPResponse resp = aTransaction.Response();
		//	TInt status = resp.StatusCode();
			
			iReceived = iOverAllSize = 0;	
			if(resp.HasBody())
			{
				if(resp.Body())
				{
					iOverAllSize = resp.Body()->OverallDataSize();
				}
			}
			// Get status text (e.g. "OK")
			TBuf<KStatustextBufferSize> statusText;
			statusText.Copy(resp.StatusText().DesC());

			TBuf<KDefaultBufferSize> text;
			
//			HBufC* Buff2 = StringLoader::LoadLC(R_STR_RECHEADER);
//			text.Format(*Buff2, status, &statusText);
//			CleanupStack::PopAndDestroy(Buff2);
					
			iObserver.ClientEvent(_L("Receiving"),_L(""),iReceived,iOverAllSize);
			}
			break;

		case THTTPEvent::EGotResponseBodyData:
			{
			// Part (or all) of response's body data received. Use
			// aTransaction.Response().Body()->GetNextDataPart() to get the actual
			// body data.
	
			// Get the body data supplier
			MHTTPDataSupplier* body = aTransaction.Response().Body();
			
			if(body && iOverAllSize <= 0)
			{
				iOverAllSize = body->OverallDataSize();
			}

			TPtrC8 dataChunk;
			// GetNextDataPart() returns ETrue, if the received part is the last
			// one.
			TBool isLast = body->GetNextDataPart(dataChunk);
		
			if(iFileToWrite.SubSessionHandle())
			{	
				iFileToWrite.Write(dataChunk,dataChunk.Length());
			}
			else if(iDataReceiver)
			{
				iDataReceiver->HTMLDataReceivedL(dataChunk,dataChunk.Length());
			}
			
			TBuf<KInfotextBufferSize> text;
			
			iReceived = iReceived + dataChunk.Length();
			
			if(iOverAllSize > 0)
			{
			//	HBufC* Buff2 = StringLoader::LoadLC(R_STR_RECEIVED1);
			//	text.Format(*Buff2,iReceived,iOverAllSize);
			//	CleanupStack::PopAndDestroy(Buff2);	
			}
			else
			{	
			//	HBufC* Buff3 = StringLoader::LoadLC(R_STR_RECEIVED2);
			//	text.Format(*Buff3,iReceived);
			//	CleanupStack::PopAndDestroy(Buff3);	
			}
			
			iObserver.ClientEvent(_L("Receiving"),_L(""),iReceived,iOverAllSize);
			// Always remember to release the body data.
			body->ReleaseData();
			
			// NOTE: isLast may not be ETrue even if last data part received.
			// (e.g. multipart response without content length field)
			// Use EResponseComplete to reliably determine when body is completely
			// received.
			if (isLast)
			{
//				HBufC* Buff4 = StringLoader::LoadLC(R_STR_RECEIVEDALL);
				iObserver.ClientEvent(_L("Download"),_L("Finished"),KErrNone);
//				CleanupStack::PopAndDestroy(Buff4);	
			}

			}
			break;
		case THTTPEvent::EResponseComplete:
			{
			// Indicates that header & body of response is completely received.
			// No further action here needed.
			//	HBufC* Buff2 = StringLoader::LoadLC(R_STR_TRANSCOMPLETE);
				iObserver.ClientEvent(_L("Download"),_L("Finished"),KErrNone);
			//	CleanupStack::PopAndDestroy(Buff2);	
			}
			break;
		case THTTPEvent::ESucceeded:
			{
			
				// Indicates that transaction succeeded.
			//	HBufC* Buff2 = StringLoader::LoadLC(R_STR_TRANSSUCCESS);
				iObserver.ClientEvent(_L("Download"),_L("Succesfull"),KErrNone);
			//	CleanupStack::PopAndDestroy(Buff2);	
					
			if(iFileToWrite.SubSessionHandle())
			{
				iFileToWrite.Close();
			}
			
			// Transaction can be closed now. It's not needed anymore.
			aTransaction.Close();
			iRunning = EFalse;
			
			iObserver.ClientBodyReceived(iFileName);
			}
			break;
		case THTTPEvent::EFailed:
			{
			// Transaction completed with failure.
		//	HBufC* Buff2 = StringLoader::LoadLC(R_STR_TRANSFAILED);
			iObserver.ClientEvent(_L("Download"),_L("Failed"),KErrNone);
		//	CleanupStack::PopAndDestroy(Buff2);	
					
			if(iFileToWrite.SubSessionHandle())
			{
				iFileToWrite.Close();
			}
			aTransaction.Close();
			iRunning = EFalse;
			
			iObserver.ClientCancelled(iFileName,KErrGeneral);
			
			}
			break;
		default:
			// There are more events in THTTPEvent, but they are not usually
			// needed. However, event status smaller than zero should be handled
			// correctly since it's error.
			{
		
			if(iFileToWrite.SubSessionHandle())
			{
				iFileToWrite.Close();
			}
			
			if (aEvent.iStatus < 0)
			{
				// Just close the transaction on errors
				aTransaction.Close();
				iRunning = EFalse;
				
			//	iObserver.ClientEvent(KErrorStr,aEvent.iStatus);
			} 
			else 
			{
				// Other events are not errors (e.g. permanent and temporary
				// redirections)
			//	iObserver.ClientEvent(KUnrecognisedEvent,aEvent.iStatus);
			}
			
			iObserver.ClientCancelled(iFileName,aEvent.iStatus);
			}
			break;
		}


}


/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
TInt CHTTPEngine::MHFRunError(TInt aError,
							  RHTTPTransaction /*aTransaction*/,
							  const THTTPEvent& /*aEvent*/)
	{
	// Just notify about the error and return KErrNone.
	
//	HBufC* Buff2 = StringLoader::LoadLC(R_STR_GENERALERROR);
	iObserver.ClientEvent(_L("Download"),_L("Error"),aError);
//	CleanupStack::PopAndDestroy(Buff2);
	return KErrNone;
	}


/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
TBool CHTTPEngine::GetNextDataPart(TPtrC8& aDataPart)
	{
	if(iPostData)
		{
		// Provide pointer to next chunk of data (return ETrue, if last chunk)
		// Usually only one chunk is needed, but sending big file could require
		// loading the file in small parts.
		aDataPart.Set(iPostData->Des());
		}
	return ETrue;
	}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CHTTPEngine::ReleaseData()
	{
	// It's safe to delete iPostData now.
	delete iPostData;
	iPostData = NULL;
	}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
TInt CHTTPEngine::Reset()
	{
	// Nothing needed since iPostData still exists and contains all the data.
	// (If a file is used and read in small parts we should seek to beginning
	// of file and provide the first chunk again in GetNextDataPart() )
	return KErrNone;
	}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
TInt CHTTPEngine::OverallDataSize()
{
	if(iPostData)
		return iPostData->Length();
	else
		return KErrNotFound ;
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
TBool CHTTPEngine::GetCredentialsL(const TUriC8& /*aURI*/,RString /*aRealm*/,RStringF /*aAuthenticationType*/,RString& /*aUsername*/,RString& /*aPassword*/)
{
	return ETrue;;
}
	
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
TBool CHTTPEngine::SetupConnectionL(TBool aAskForIAP)
{
	TBool DoWait(EFalse);
	
    if(!iConnectionSetupDone )
    {
//    	HBufC* Buff2 = StringLoader::LoadLC(R_STR_CONNECTININTRN);
		iObserver.ClientEvent(_L("Connecting"),_L("Internet"),KErrNone);
  //  	CleanupStack::PopAndDestroy(Buff2);
    	
	    if(!iSocketServ.Handle())
	    {
		    //open socket server and start the connection
		    User::LeaveIfError(iSocketServ.Connect());
	    }
	    
	    DoWait = ETrue;
	    
	    delete iIAPConnector;
	    iIAPConnector = NULL;
	    
	    iIAPConnector = new(ELeave)CIAPConnector(this,iSession,iSocketServ);
	    iIAPConnector->ConstructL();
		
		if(iIapSet)
		{
			iIAPConnector->SetIap(iIap);
		}
		
		iIAPConnector->SetupIAPConnectionL(aAskForIAP);
    }
    
    return DoWait;
}

/*
-------------------------------------------------------------------------------
****************************  CIAPConnector   *************************************
-------------------------------------------------------------------------------
*/  

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CIAPConnector::CIAPConnector(MIAPConObserver* aUpdate,RHTTPSession& aSession,RSocketServ& aServ)
:CActive(0),iUpdate(aUpdate),iSession(aSession),iSocketServ(aServ),iIapSet(EFalse)
    {
    }
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CIAPConnector::~CIAPConnector()
{
	Cancel();
	
	// and finally close handles
    iConnection.Close();
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CIAPConnector::ConstructL(void)
{
	CActiveScheduler::Add(this);
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CIAPConnector::SetIap(TUint32 aIap)
{
	iIap = aIap;
	iIapSet = ETrue;
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CIAPConnector::DoCancel()
{
	iConnection.Stop();	
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/	
void CIAPConnector::RunL()
{
	if(iStatus.Int() == KErrNone)
	{
		//set the sessions connection info
	    RStringPool strPool = iSession.StringPool();
	    RHTTPConnectionInfo connInfo = iSession.ConnectionInfo();
	    
	    //to use our socket server and connection
	    connInfo.SetPropertyL ( strPool.StringF(HTTP::EHttpSocketServ,
	        RHTTPSession::GetTable() ), THTTPHdrVal (iSocketServ.Handle()) );

	    connInfo.SetPropertyL ( strPool.StringF(HTTP::EHttpSocketConnection,
	        RHTTPSession::GetTable() ), 
	        THTTPHdrVal (REINTERPRET_CAST(TInt, &(iConnection))) );
	}
	
	if(iUpdate)
	{
		iUpdate->IapConnectionDoneL(iStatus.Int());
	}
}

        
/*
-------------------------------------------------------------------------------


-------------------------------------------------------------------------------
*/
void CIAPConnector::SetupIAPConnectionL(TBool aAskForIAP)
{
   	User::LeaveIfError(iConnection.Open(iSocketServ));
  
    // open the IAP communications database 
	CCommsDatabase* commDB = CCommsDatabase::NewL(EDatabaseTypeIAP);
	CleanupStack::PushL(commDB);

	// initialize a view 
	CCommsDbConnectionPrefTableView* commDBView = 
	commDB->OpenConnectionPrefTableInRankOrderLC(ECommDbConnectionDirectionUnknown);

	// go to the first record 
	User::LeaveIfError(commDBView->GotoFirstRecord());

	// Declare a prefTableView Object.
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref;

	// read the connection preferences 
	commDBView->ReadConnectionPreferenceL(pref);
//	TUint32 iapID = pref.iBearer.iIapId; 

	// pop and destroy the IAP View 
	CleanupStack::PopAndDestroy(commDBView);
	// pop and destroy the database object
	CleanupStack::PopAndDestroy(commDB);

	// Now we have the iap Id. Use it to connect for the connection.
	// Create a connection preference variable.
	
	if(iIapSet)
	{
		iConnectPref.SetIapId(iIap);
		iConnectPref.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
	}
	else if(!aAskForIAP)
	{
		iConnectPref.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
	}


	iConnection.Start(iConnectPref,iStatus);
	SetActive();
}

// end of file
