/* 
	Copyright (c) 2006-09, 
	Jukka Silvennoinen. 
	All rights reserved 
*/


#ifndef __MAINSERVERCOMMON_H__
#define __MAINSERVERCOMMON_H__

#include <e32base.h>
#include <APMSTD.H>
#include "Common.h"


//the server version. A version must be specified when 
//creating a session with the server
const TUint SServMajorVersionNumber=0;
const TUint SServMinorVersionNumber=1;
const TUint SServBuildVersionNumber=1;

const TInt KMaxSoneClientMessage=512;


//opcodes used in message passing between client and server
enum TSoneClientMessages
	{
	EMainServSetFileMapping,
	EMainServKillTask,
	EMainServKillProcess,
	EMainServKillThread,
	EMainServGetEInfo
    };
 
 #ifdef __SERIES60_3X__

#else
	//opcodes used by server to indicate which asynchronous service
	// 2nd edition only
	enum TSProtServRqstComplete
		{
		EMainServSetFileMappingComplete = 1,
		EMainServKillTaskComplete,
		EMainServKillProcessComplete,
		EMainServKillThreadComplete,
		EMainServGetEInfoComplete
	};
#endif

    
/*
----------------------------- TExampleItem ----------------------------------
*/  
class TFileMappingItem
    {
    public:
    	TUid 		 	  iUid;
        TDataType 		  iDataType;
        TDataTypePriority iPriority;
    };

class TExtraInfoItem
    {
    public:
    	TUint 		iAreaCode;
		TUint 		iCellId;
		TBuf<100>	iCountryCode;
		TBuf<100>	iNwId;
		TBuf<40>	iImsi;
    };

/*
----------------------------- etxra stuff ----------------------------------
*/

    
#endif // __CLIENTSERVERCOMMON_H__

