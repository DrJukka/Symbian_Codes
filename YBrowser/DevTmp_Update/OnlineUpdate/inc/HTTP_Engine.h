/* 
	Copyright (c) 2001 - 2008 ,
	Jukka Silvennoinen, 
	All rights reserved 
*/

#ifndef __CLIENTHTTPENGINE_H__
#define __CLIENTHTTPENGINE_H__

#include <coecntrl.h>
#include <http\mhttpdatasupplier.h>
#include <http\mhttptransactioncallback.h>
#include <http\mhttpauthenticationcallback.h>
#include <es_sock.h>
#include <CommDbConnPref.h>


/*
* Forward declarations
*/
class RHTTPSession;
class RHTTPTransaction;


class MIAPConObserver
	{
	public:
		virtual void IapConnectionDoneL(TInt aError) = 0;
	};

	class CIAPConnector : public CActive
	{
	public:
	 	CIAPConnector(MIAPConObserver* aUpdate,RHTTPSession& aSession,RSocketServ& aServ);
	    void ConstructL(void);
	    ~CIAPConnector();
		void SetupIAPConnectionL(TBool aAskForIAP);
		void SetIap(TUint32 aIap);
	protected:
		void DoCancel();
		void RunL();
	private:
		MIAPConObserver* 		iUpdate;
		RHTTPSession&			iSession;
		RSocketServ& 			iSocketServ;
		TCommDbConnPref 		iConnectPref;	
    	RConnection 			iConnection;
    	TUint32					iIap;
		TBool					iIapSet;
	};


/*
* MClientObserver
* CHTTPEngine passes events and responses body data with this interface.
* An instance of this class must be provided for construction of CHTTPEngine.
*/
class MDataReceiver
	{
	public:
		virtual void HTMLDataReceivedL(const TDesC8& aData, TInt aLength) = 0;
	};
	
class MClientObserver
	{
	public:
		virtual void ClientEvent(const TDesC& aText1,const TDesC& aText2,TInt aProgress,TInt aFinal) = 0;
		virtual void ClientEvent(const TDesC& aText1,const TDesC& aText2,TInt aError) = 0;
		virtual void ClientBodyReceived(const TDesC& aFileName) = 0;
		virtual void ClientCancelled(const TDesC& aFileName,TInt aError) = 0;
	};

/*
* Provides simple interface to HTTP Client API.
*/
class CHTTPEngine :	public CBase,public MHTTPTransactionCallback,
public MHTTPDataSupplier,public MHTTPAuthenticationCallback,MIAPConObserver
	{
public:
	enum TContType
		{
		EURLEncoded,
		EMultiPart 
		};

	static CHTTPEngine* NewL(MClientObserver& iObserver);
	static CHTTPEngine* NewLC(MClientObserver& iObserver);
	~CHTTPEngine();

	void IssueHTTPGetL(const TDesC8& aUri, MDataReceiver* aDataReceiver, TBool aAskForIAP = EFalse);
	void IssueHTTPGetL(const TDesC8& aUri, const TDesC& aFileName, TBool aAskForIAP = EFalse);
	void IssueHTTPPostL(const TDesC8& aUri,const TDesC8& aBody,MDataReceiver* aDataReceiver,TContType aContType = EURLEncoded, TBool aAskForIAP = EFalse);
	void IssueHTTPPostL(const TDesC8& aUri,const TDesC8& aBody, const TDesC& aFileName,TContType aContType = EURLEncoded, TBool aAskForIAP = EFalse);

	void CancelTransaction();
	inline TBool IsRunning() { return iRunning; };
	void SetIap(TUint32 aIap);
protected:
	void IapConnectionDoneL(TInt aError);
private:
	void ConstructL();
	CHTTPEngine(MClientObserver& iObserver);
	void SetHeaderL(RHTTPHeaders aHeaders, TInt aHdrField,const TDesC8& aHdrValue);
private:
	void MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent& aEvent);
	TInt MHFRunError(TInt aError,RHTTPTransaction aTransaction,const THTTPEvent& aEvent);
private:
	void ReleaseData();
	TBool GetNextDataPart(TPtrC8& aDataPart);
	TInt Reset();
	TInt OverallDataSize();
	TBool SetupConnectionL(TBool aAskForIAP);
private:
	TBool GetCredentialsL(	const TUriC8& aURI,RString aRealm,RStringF aAuthenticationType,RString& aUsername,RString& aPassword);
private:
	// declare members
	TContType 				iContType;
	CIAPConnector*			iIAPConnector;
    RSocketServ 			iSocketServ;
	RHTTPSession			iSession;
	RHTTPTransaction		iTransaction;
	MClientObserver&		iObserver;	// Used for passing body data and.
	HBufC8*					iPostData;	// Data for HTTP POST
	TBool					iRunning;	// ETrue, if transaction running
	TBool 					iConnectionSetupDone;
	RFs						iFileSession;		
	RFile					iFileToWrite;
	TFileName				iFileName;
	TInt 					iOverAllSize,iReceived;
	TBool					iPost;
	HBufC8*					iUrl;
	MDataReceiver*			iDataReceiver;
	TUint32					iIap;
	TBool					iIapSet;
};

#endif // __CLIENTHTTPENGINE_H__
