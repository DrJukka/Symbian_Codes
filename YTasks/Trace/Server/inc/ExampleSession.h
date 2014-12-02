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
	void GetItemsCountL(const RMessage2& aMessage);
	void GetItemValueL(const RMessage2& aMessage);
	void ShutMeDownNowL(const RMessage2& aMessage);
	void ClearListItemsL(const RMessage2& aMessage);
	void StartTraceL(const RMessage2& aMessage);
	void StopTraceL(const RMessage2& aMessage);
	void GetCPUMaxL(const RMessage2& aMessage);
	void ReReadSettingsL(const RMessage2& aMessage);
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
