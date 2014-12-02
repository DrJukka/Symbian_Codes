/* 
	Copyright (c) 2001 - 2008, 
	Dr. Jukka Silvennoinen. 
	All rights reserved 
*/

#ifndef __FAKES__MSSENDER__
#define __FAKES__MSSENDER__

#include <e32base.h>
#include <S32FILE.H>
#include <D32DBMS.H>
#include <msvapi.h>
#include <mtclreg.h>
#include <smut.h>
#include <smsclnt.h>

/*! 
  @class CSProtServer
  
  @discussion An instance of class CSProtServer is the main server class
  for the ClientServerAsync example application
  */
	
class CFakeSMSSender : public CBase, MMsvSessionObserver
    {
public : 
    static CFakeSMSSender* NewL();
    static CFakeSMSSender* NewLC();
    ~CFakeSMSSender();
    void CreateSMSMessageL(const TDesC& aNunmber,const TDesC& aName,const TDesC& aMessage,TBool aUnicodeMsg,TTime aMsgTime);
protected:
	void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);
private: 
	TMsvId CreateNewMessageL(TTime aMsgTime);
	void AppendPDUDataL(TMsvId aEntryId, const TDesC& aRecipient,TBool aUnicodeMsg,const TDesC& aBody);
	void FinalizeMessageL(TMsvId aEntryId,const TDesC& aBody,const TDesC& aRecipientName,TTime aMsgTime);
	void AppendMessageDataL(TMsvId aEntryId,const TDesC& aRecipient);
private: 
    CFakeSMSSender(void);
    void ConstructL() ;
private:
	CMsvSession* 		iMsvSession;
    CClientMtmRegistry*	iMtmReg;
    RFile				iFile;
	RFs 				iFsSession;
};


#endif //__FAKES__MSSENDER__

