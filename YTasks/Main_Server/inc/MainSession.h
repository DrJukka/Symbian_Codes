/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen. 
	All rights reserved 
*/

#ifndef __MAINSESSION_HEADER__
#define __MAINSESSION_HEADER__

#include <e32base.h>
#include <S32FILE.H>
#include "MainServer.h"


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
	void SetFileMappingL(const RMessage2& aMessage);
	void KillTaskL(const RMessage2& aMessage);
	void KillThreadL(const RMessage2& aMessage);
	void KillProcessL(const RMessage2& aMessage);
	void GetExtraInfoL(const RMessage2& aMessage);

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
	
#endif //__MAINSESSION_HEADER__
