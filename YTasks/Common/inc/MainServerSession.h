/* 	
	Copyright (c) 2006, 
	Jukka Silvennoinen. 
	All rights reserved 
*/

#ifndef __MAINSERVERSESSION_H__
#define __MAINSERVERSESSION_H__

#include <e32base.h>
#include "Mainservercommon.h"
/*
  RMainServerClient
  
  this class provides the client-side interface to the server session
*/
// ----------------------------------------------------------------------------------------
// Client
// ----------------------------------------------------------------------------------------

class RMainServerClient : public RSessionBase
	{
public:
	TInt Connect();
	void Close();
	// get, set & delete items data functions.
	TInt SetFileMappingL(TFileMappingItem& aDataMapping);
	TInt KillTaskL(TUid& aUid);
	TInt KillProcessL(TDes8& aName);
	TInt KillThreadL(TDes8& aName);
	TInt GetExtraInfoL(TExtraInfoItem& aItemBuffer);

private:	
	};

#endif // __MAINSERVERSESSION_H__
