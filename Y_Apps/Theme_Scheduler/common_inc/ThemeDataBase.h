/* Copyright (c) 2001 - 2008 , Dr Jukka Silvennoinen. All rights reserved */


#ifndef __SCHEDULEDB_HEADER__
#define __SCHEDULEDB_HEADER__

#include <e32base.h>
#include <S32FILE.H>
#include <D32DBMS.H>

#include "Common.h"

// database column names
_LIT(NCol0					,"index");
_LIT(NCol1					,"type");
_LIT(NCol2					,"profile");
_LIT(NCol3					,"Theme");
_LIT(NCol4					,"time");
_LIT(NCol5					,"repeat");
_LIT(NCol6					,"enabled");
_LIT(NCol7					,"image");

// database table name
_LIT(KtxtItemlist			,"schedule");
_LIT(KtxtRandomLst			,"random");

class TThemeSched 
    {
    public:
        TThemeSched(): iIndex(-1){};
    public:
        TInt 		iIndex;
        TBool		iType;
        TInt		iProfileId;
        TUint32		iThemeId;
        TInt		iRepeat;
        TTime		iTime;	
        TBool		iEnabled;
        TFileName	iImageFile;
    };


class CScheduleDB : public CBase
    {
public : 
	void ConstructL();
    ~CScheduleDB();
public:
	void DeleteFromDatabaseL(TInt& aIndex);
	void UpdateDatabaseL(TBool aType, TInt aProfile, TUint32 aTheme,TInt aRepeat, TTime aTime,TInt& aIndex,TBool& aEnabled, TDes& aFileName);
	void SaveToDatabaseL(TBool aType, TInt aProfile, TUint32 aTheme,TInt aRepeat, TTime aTime,TInt& aIndex,TBool& aEnabled, TDes& aFileName);
	void ReadDbItemsL(RArray<TThemeSched>&	aItemArray);
	void SetRandomProfilesL(CArrayFixFlat<TUint32>& aProfArray);
	void GetRandomProfilesL(CArrayFixFlat<TUint32>& aProfArray);
	void DeleteRandomProfilesL(void);
private:
	void ShowNoteL(const TDesC& aMessage);
	void CreateTableL(RDbDatabase& aDatabase);
	void CreateRandomTableL(RDbDatabase& aDatabase);
private:
	RDbNamedDatabase 		iItemsDatabase;
	RFs						iFsSession;
	
    };


	
#endif //__EXAMPLESNAMEDDB_HEADER__
