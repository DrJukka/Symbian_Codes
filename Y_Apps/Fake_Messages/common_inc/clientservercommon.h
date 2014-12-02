/* Copyright (c) 2001 - 2009, Dr. Jukka Silvennoinen. All rights reserved */


#ifndef __CLIENTSERVERCOMMON_H__
#define __CLIENTSERVERCOMMON_H__

#include <e32base.h>
#include "Common.h"

//Client & Server UID values.
const TUid KExapmpleServerUid3	= KUidTOneServer;
const TUid KExapmpleClientUid3	= KUidTOneViewer;


//the server version. A version must be specified when 
//creating a session with the server
const TUint SServMajorVersionNumber=0;
const TUint SServMinorVersionNumber=1;
const TUint SServBuildVersionNumber=1;

const TInt KMaxSoneClientMessage=512;


//opcodes used in message passing between client and server
enum TSoneClientMessages
	{
	EExampleReReadValues,
	EExampleDummy1,
	EExampleDummy2,
	EExampleDummy3,
	EExampleDummy4, //5

    };
 


    
#endif // __CLIENTSERVERCOMMON_H__

