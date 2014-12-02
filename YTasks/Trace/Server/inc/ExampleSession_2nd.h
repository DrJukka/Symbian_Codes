/* Copyright (c) 2001 - 2004 , Jukka Silvennoinen Solution One Telecom LTd. All rights reserved */


#ifndef __SPROTSESSION__
#define __SPROTSESSION__

#include <e32base.h>
#include "ExampleServer.h"

/*! 
  @class CExPolicy_ServerSession
  
  @discussion An instance of class CExPolicy_ServerSession is created for each client
  */
class CExPolicy_ServerSession : public CSession
    {

public: // New methods
    static CExPolicy_ServerSession* NewL(RThread& aClient, CExPolicy_Server& aServer);
    static CExPolicy_ServerSession* NewLC(RThread& aClient, CExPolicy_Server& aServer);
    ~CExPolicy_ServerSession();
 public: // from CSession
    void ServiceL(const RMessage& aMessage);
private: // New methods
	CExPolicy_Server& Server(){ return iServer;};
    CExPolicy_ServerSession(RThread& aClient, CExPolicy_Server& aServer);
    void ConstructL() ;
    void PanicClient(TInt aPanic) const;
private:
	void GetItemValueL(const RMessage& aMessage,TBool aImsi);
	void GetListItemValueAndNameL(const RMessage& aMessage,TBool aImsi);
	void SetItemValueL(const RMessage& aMessage,TBool aImsi);
	void DeleteValueL(const RMessage& aMessage,TBool aImsi);
	void GetItemCountL(const RMessage& aMessage,TBool aImsi);
	void SetItemValueAndNameL(const RMessage& aMessage,TBool aImsi);
	
	void GetEnabledStatusL(const RMessage& aMessage,TBool aProtect);
	void SetEnabledStatusL(const RMessage& aMessage,TBool aProtect);
	
	void SetGetPasswordL(const RMessage& aMessage,TBool aSetIt);
	
	void IsServerInitializedL(const RMessage& aMessage);
	void CheckRegStatusL(const RMessage& aMessage);
	void DoRegNowL(const RMessage& aMessage,TBool aThai);

	void IsCodeOk(const RMessage& aMessage);

	void LockScreenStatusL(const RMessage& aMessage);
	void LockScreenOnOffL(const RMessage& aMessage,TBool aSetOn);
	
	void CheckSIMCard(void);
private: // Data
    RMessage        	iMessage;
    CExPolicy_Server&   iServer;
    TBool 				iGotShutDownCommand;
    };

#endif //__SPROTSESSION__