/* Copyright (c) 2001 - 2008, Dr. Jukka Silvennoinen. All rights reserved */


#ifndef __SCHEDULEDB_HEADER__
#define __SCHEDULEDB_HEADER__

#include <e32base.h>
#include <S32FILE.H>
#include <D32DBMS.H>

#include "Common.h"



// database column names
_LIT(NCol0					,"index");
_LIT(NCol1					,"profile");
_LIT(NCol2					,"time");
_LIT(NCol3					,"repeat");
_LIT(NCol4					,"enabled");


// database column names
_LIT(NColRan0					,"index");
_LIT(NColRan1					,"profile");

// database table name
_LIT(KtxtItemlist			,"schedule");
_LIT(KtxtRandomLst			,"random");


class TProfSched 
    {
    public:
        TProfSched(): iIndex(-1){};
    public:
        TInt 		iIndex;
        TInt		iProfileId;
        TInt		iRepeat;
        TTime		iTime;	
        TBool		iEnabled;
    };


class CScheduleDB : public CBase
    {
public : 
	void ConstructL();
    ~CScheduleDB();
public:
	void DeleteFromDatabaseL(TInt& aIndex);
	void UpdateDatabaseL(TInt aProfile, TInt aRepeat, TTime aTime,TInt& aIndex,TBool& aEnabled);
	void SaveToDatabaseL(TInt aProfile, TInt aRepeat, TTime aTime,TInt& aIndex,TBool& aEnabled);
	void ReadDbItemsL(RArray<TProfSched>&	aItemArray);
	
	void GetRandomProfilesL(CArrayFixFlat<TInt>& aProfArray);
	void SetRandomProfilesL(CArrayFixFlat<TInt>& aProfArray);
private:
	void DeleteRandomProfilesL(void);
	void CreateRandomTableL(RDbDatabase& aDatabase);
	void CreateTableL(RDbDatabase& aDatabase);
private:
	RDbNamedDatabase 		iItemsDatabase;
	RFs						iFsSession;
    };


	
#endif //__EXAMPLESNAMEDDB_HEADER__
