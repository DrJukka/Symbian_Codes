/* 	
	Copyright (c) 2006, 
	Jukka Silvennoinen. 
	All rights reserved 
*/

#ifndef __EXAMPLESERVERSESSION_H__
#define __EXAMPLESERVERSESSION_H__

#include <e32base.h>
#include "Common.h"

	
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
	TInt GetItemCount(TInt& aItemCount);
	TInt GetItemL(TExampleItem& aItemBuffer);
	TInt ShutServerDownNow(void);
	TInt ClearItemList(void);
	TInt StopTrace(void);
	TInt StartTrace(void);
	TInt GetCPUMax(TUint32& aItemCount);
	TInt ReReadSettings(void);
private:	
	};

#endif // __EXAMPLESERVERSESSION_H__
