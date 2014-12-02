/* Copyright (c) 2001 - 2008, Dr. Jukka Silvennoinen. All rights reserved */


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
	void ReReadValuesNow(void);
private:	
	};

#endif // __EXAMPLESERVERSESSION_H__
