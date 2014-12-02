/* Copyright (c) 2001 - 2009, Dr. Jukka Silvennoinen. All rights reserved */


#ifndef __SCHEDULEDB_HEADER__
#define __SCHEDULEDB_HEADER__

#include <e32base.h>
#include <S32FILE.H>
#include <D32DBMS.H>
#include "Common.h"


class CKeyCapcap : public CBase
    {
    public:
    	CKeyCapcap(): iNunmber(NULL),iName(NULL){};
        ~CKeyCapcap(){delete iNunmber; delete iName;};
    public:
        TBuf<50>	iLabel;
        HBufC*		iNunmber;
        HBufC*		iName;
        TBool 		iEnabled;
        TUint32		iDelay;
    };


// database column names
_LIT(NCol0					,"index");
_LIT(NCol1					,"unicode");
_LIT(NCol2					,"number");
_LIT(NCol3					,"name");
_LIT(NCol4					,"time");
_LIT(NCol5					,"repeat");
_LIT(NCol6					,"enabled");

// database table name
_LIT(KtxtItemlist			,"schedule");
_LIT(KtxtItemlist2			,"messages");


class CMsgSched : public CBase
    {
    public:
    	CMsgSched(): iIndex(-1),iNunmber(NULL),iName(NULL){};
        ~CMsgSched(){delete iNunmber; delete iName;};
    public:
        TInt 	iIndex;
        HBufC*	iNunmber;
        HBufC*	iName;
        TBool 	iEnabled;
        TTime	iTime;	
        TInt	iRepeat;
    };


class CScheduleDB : public CBase
    {
public : 
	void ConstructL();
    ~CScheduleDB();
public:
	void DeleteFromDatabaseL(TInt& aIndex);
	void UpdateDatabaseL(CMsgSched* aData);
	void SaveToDatabaseL(CMsgSched* aData);
	void ReadDbItemsL(RPointerArray<CMsgSched>&	aItemArray);
private:
	void CreateTableL(RDbDatabase& aDatabase);
private:
	RDbNamedDatabase 		iItemsDatabase;
	RFs						iFsSession;
    };


	
#endif //__EXAMPLESNAMEDDB_HEADER__
