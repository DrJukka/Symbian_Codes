/* Copyright (c) 2001 - 2009, Dr. Jukka Silvennoinen. All rights reserved */


#ifndef __SCHEDULEDB_HEADER__
#define __SCHEDULEDB_HEADER__

#include <e32base.h>
#include <S32FILE.H>
#include <D32DBMS.H>
#include "Common.h"



// database column names
_LIT(NCol0					,"index");
_LIT(NCol1					,"unicode");
_LIT(NCol2					,"number");
_LIT(NCol3					,"message");
_LIT(NCol4					,"time");
_LIT(NCol5					,"repeat");
_LIT(NCol6					,"enabled");
_LIT(NCol7					,"typppe");

// database table name
_LIT(KtxtItemlist			,"schedule");
_LIT(KtxtItemlist2			,"messages");


class CMsgStored : public CBase
    {
    public:
    	CMsgStored(): iIndex(-1),iMessage(NULL){};
        ~CMsgStored(){delete iMessage;};
    public:
        TInt 	iIndex;
        HBufC*	iMessage;		
    };

class CMsgSched : public CBase
    {
    public:
    	CMsgSched(): iIndex(-1),iNunmber(NULL),iMessage(NULL){};
        ~CMsgSched(){delete iNunmber; delete iMessage;};
    public:
        TInt 	iIndex;
        HBufC*	iNunmber;
        HBufC*	iMessage;
        TBool 	iUnicode;
        TBool 	iEnabled;
        TBool 	iFlashSMS;        
        TTime	iTime;	
        TInt	iRepeat;
    };


class CScheduleDB : public CBase
    {
public : 
	void ConstructL();
    ~CScheduleDB();
public:
	void DeleteFromDatabaseL(TInt& aIndex,const TBool& aMessage);
	void UpdateDatabaseL(CMsgSched* aData);
	void UpdateDatabaseL(CMsgStored* aData);
	void SaveToDatabaseL(CMsgStored* aData);
	void SaveToDatabaseL(CMsgSched* aData);
	void ReadDbItemsL(RPointerArray<CMsgSched>&	aItemArray);
	void ReadDbItemsL(RPointerArray<CMsgStored>& aItemArray);
private:
	void CreateTableL(RDbDatabase& aDatabase);
	void CreateMsgTableL(RDbDatabase& aDatabase);
private:
	RDbNamedDatabase 		iItemsDatabase;
	RFs						iFsSession;
    };


	
#endif //__EXAMPLESNAMEDDB_HEADER__
