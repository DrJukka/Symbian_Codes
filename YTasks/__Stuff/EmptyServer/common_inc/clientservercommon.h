/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen. 
	All rights reserved 
*/


#ifndef __CLIENTSERVERCOMMON_H__
#define __CLIENTSERVERCOMMON_H__

#include <e32base.h>

//Client & Server UID values.
const TUid KExapmpleServerUid3	={0x102013AB};
const TUid KExapmpleClientUid3	={0x102013AE};


//server name
_LIT(KExapmpleServerName			,"NSrv_102013AB");
// server's EXE file name
_LIT(KExapmpleServerImg				,"NSrv_102013AB");		

_LIT(KExapmpleServerSemaphoreName	,"NSrv_102013ABsmp");



//the server version. A version must be specified when 
//creating a session with the server
const TUint SServMajorVersionNumber=0;
const TUint SServMinorVersionNumber=1;
const TUint SServBuildVersionNumber=1;

const TInt KMaxSoneClientMessage=512;


//opcodes used in message passing between client and server
enum TSoneClientMessages
	{
	EExamplePServGetCount,
	EExamplePServReadValueAndName,
	EExamplePServSetValue,
	EExamplePServSetValueAndName,
	EExamplePServDeleteItem, //5
	EGetItemListItem,
	
	EExamplePServGetCount2,
	EExamplePServReadValueAndName2,
	EExamplePServSetValue2,
	EExamplePServSetValueAndName2, //10
	EExamplePServDeleteItem2,
	EGetItemListItem2,
	
	EGetEnabledProtect,
	EGetEnabledScreen,
	ESetEnabledProtect, //15
	ESetEnabledScreen,
	
	ESetProtectPassword,
	EGetProtectPassword,
	
	EServerInitializedStatus,
	EServerRegStatus, //20
	EServerRegisterNow,
	
	EServerLockScreenOn,
	EServerLockScreenOff,
	EServerLockScreenStatus,
	
	EServerRegisterNowThai,
	EServerCheckCodeOk,
	EServerCheckSIM
    };
 
 #ifdef __SERIES60_3X__

#else
	//opcodes used by server to indicate which asynchronous service
	// 2nd edition only
	enum TSProtServRqstComplete
		{
		EExamplePServGetCountComplete = 1,
		EExamplePServReadValueAndNameComplete,
		EExamplePServSetValueComplete,
		EExamplePServSetValueAndNameComplete,
		EExamplePServDeleteItemComplete, //5
		EGetItemListItemComplete,
		
		EExamplePServGetCount2Complete,
		EExamplePServReadValueAndName2Complete,
		EExamplePServSetValue2Complete,
		EExamplePServSetValueAndName2Complete, //10
		EExamplePServDeleteItem2Complete,
		EGetItemListItem2Complete,
		
		EGetEnabledProtectComplete,
		EGetEnabledScreenComplete,
		ESetEnabledProtectComplete, //15
		ESetEnabledScreenComplete,
		
		ESetProtectPasswordComplete,
		EGetProtectPasswordComplete,
		
		EServerInitializedStatusComplete,
		EServerRegStatusComplete, //20
		EServerRegisterNowComplete,
		
		EServerLockScreenOnComplete,
		EServerLockScreenOffComplete,
		EServerLockScreenStatusComplete,
		
		EServerRegisterNowThaiComplete,
		EServerCheckCodeOkComplete,  
		EServerCheckSIMComplete
	};
#endif
/*
----------------------------- TNProtRegItem ----------------------------------
*/  
class TNProtRegItem
    {
    public:
        TNProtRegItem():iFirstRun(ETrue), iDays(-1),iStatus(0){};
    public:
        TBool		iFirstRun;
        TInt 		iDays;
        TUint		iStatus;
        TTime		iStartTime;
        TBuf<30>	iCode;
    };
 
    
/*
----------------------------- TExampleItem ----------------------------------
*/  
class TExampleItem
    {
    public:
        TExampleItem(): iIndex(-1){};
    public:
        TInt 		iIndex;
        TBuf<100> 	iName,iValue;
    };
    
/*
----------------------------- etxra stuff ----------------------------------
*/

    
#endif // __CLIENTSERVERCOMMON_H__

