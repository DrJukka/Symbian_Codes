/* 	
	Copyright (c) 2006, 
	Jukka Silvennoinen. 
	All rights reserved 
*/

#ifndef __EXAMPLESERVERSESSION_H__
#define __EXAMPLESERVERSESSION_H__

#include <e32base.h>

/*
  RExampleServerClient
  
  this class provides the client-side interface to the server session
*/
// ----------------------------------------------------------------------------------------
// Client
// ----------------------------------------------------------------------------------------

class RExampleServerClient : public RSessionBase
	{
public:
	TInt Connect();
	void Close();
	// get, set & delete items data functions.
	TInt GetItemValueAndName(TDes& aName,TDes& aValue,TInt& aIndex,TBool aImsi);
	TInt GetListItemValueAndName(TDes& aName,TDes& aValue,TInt& aListIndex,TBool aImsi);
	TInt SetItemValueAndName(const TDesC& aName,const TDesC& aValue,TInt& aIndex,TBool aImsi);
	TInt SetItemValue(const TDesC& aValue,TInt& aIndex,TBool aImsi);
	TInt DeleteItem(const TDesC& aName,const TDesC& aValue,TInt& aIndex,TBool aImsi);
	TInt GetItemCount(TInt& aItemCount,TBool aImsi);

	TBool GetStatus(TBool aProtect);
	TInt SetStatus(TBool aProtect,TBool& On);

	TInt SetPassword(const TDesC& aPassword);
	TInt GetPassword(TDes& aPassword);

	TBool ServerInitialized(void);
	TInt CheckRegStatusL(TBool& aFirstStart, TInt& aDaysLeft, TUint& aResult,TDes& aOperator);
	TInt TryRegisterApp(TDesC& aBuffer, TUint& aResult, TTime& aStartTime);
	TInt TryRegisterApp(TUint& aResult);
	
	TBool IsCodeOk(const TDesC& aCode);
	
	TBool LastLockScreenStatus(void);
	TBool SetLockScreenStatus(TBool aStatus);
	
	TInt DoSIMCheckNow(void);
private:	
	};

#endif // __EXAMPLESERVERSESSION_H__
