/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen. 
	All rights reserved 
*/

#ifndef __EXAMPLESESSION_HEADER__
#define __EXAMPLESESSION_HEADER__

#include <e32base.h>
#include <S32FILE.H>
#include "ExampleServer.h"


// ----------------------------------------------------------------------------------------
// Session
// ----------------------------------------------------------------------------------------
class CExPolicy_ServerSession : public CSession2
	{
public:
	CExPolicy_ServerSession();
	void CreateL();
 private:
	~CExPolicy_ServerSession();
	inline CExPolicy_Server& Server();
	void ServiceL(const RMessage2& aMessage);
	void ServiceError(const RMessage2& aMessage,TInt aError);
private:// internal functions to handle client requests
	void GetItemValueL(const RMessage2& aMessage,TBool aImsi);
	void GetListItemValueAndNameL(const RMessage2& aMessage,TBool aImsi);
	void SetItemValueL(const RMessage2& aMessage,TBool aImsi);
	void DeleteValueL(const RMessage2& aMessage,TBool aImsi);
	void GetItemCountL(const RMessage2& aMessage,TBool aImsi);
	void SetItemValueAndNameL(const RMessage2& aMessage,TBool aImsi);
	
	void GetEnabledStatusL(const RMessage2& aMessage,TBool aProtect);
	void SetEnabledStatusL(const RMessage2& aMessage,TBool aProtect);
	
	void SetGetPasswordL(const RMessage2& aMessage,TBool aSetIt);
	
	void IsServerInitializedL(const RMessage2& aMessage);
	void CheckRegStatusL(const RMessage2& aMessage);
	void DoRegNowL(const RMessage2& aMessage,TBool aThai);

	void IsCodeOk(const RMessage2& aMessage);

	void LockScreenStatusL(const RMessage2& aMessage);
	void LockScreenOnOffL(const RMessage2& aMessage,TBool aSetOn);

private:
	
	};
	
/*
----------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------
*/
inline CExPolicy_ServerSession::CExPolicy_ServerSession()
	{
	}
/*
----------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------
*/	
inline CExPolicy_Server& CExPolicy_ServerSession::Server()
	{
	return *static_cast<CExPolicy_Server*>(const_cast<CServer2*>(CSession2::Server()));
	}
	
#endif //__EXAMPLESESSION_HEADER__
