/* Copyright (c) 2001-2005, Jukka Silvennoinen, S-One Telecom co., Ltd.. All rights reserved */

#ifndef __CCOPY_CONTACT_READER_H__
#define __CCOPY_CONTACT_READER_H__

#include <e32base.h>
#include <coecntrl.h>
#include <w32std.h>
#include <e32std.h>
#include <cntdef.h>
#include <cntdb.h> 
	
	class CNumItem :public CBase
		{
	public:
		~CNumItem(){delete iNumber;};
		CNumItem(){iNumber = NULL;};
	public:
		HBufC*	iNumber;
	//	TBool	iMobile;
		};
		
		
	class CMyCItem :public CBase
		{
	public:
		~CMyCItem(){delete iFirstName;delete iLastName;iNumbers.ResetAndDestroy();};
		CMyCItem(){iFirstName = NULL;iLastName = NULL;};
	public:
		HBufC*					iFirstName;
		HBufC*					iLastName;
		TContactItemId			iId;
		RPointerArray<CNumItem> iNumbers;
	};

	class MContactReadUpdate
	{
	public:
		virtual void ReadProgress(TInt aCurrent, TInt aCount) = 0;
		};
		
	class CContactsReader : public CActive
	{
	public:
	    static CContactsReader* NewL(MContactReadUpdate* aUpdate);
	    static CContactsReader* NewLC(MContactReadUpdate* aUpdate);
	    ~CContactsReader();
	    void StartTheReadL(void);
	    TBool HasFinishedReading(void){ return iDone;};
	    TInt  ErrorCode(void){ return iError;};
	    RPointerArray<CMyCItem>& ItemArray(void){ return iItemArray;};
		CDesCArray* GetAllNumbersLC(CContactItem* aItem);

	protected:
		TBool DoReReadL(CMyCItem* aItem);
		void AppendNumbersL(CDesCArray* aArray, TFieldType aFieldType,CContactItem* aItem);
		void DoCancel();
		void RunL();
		void DoOneReadL(TContactItemId aCId);
		void FinnishReadL(void);
	    CContactsReader(MContactReadUpdate* aUpdate);
	    void ConstructL(void);
	private:
		MContactReadUpdate* 		iUpdate;
		CContactDatabase* 			iContactDb;
		TBool 						iDone,iCancel;
		TInt 						iError,iTotalCount,iCurrCount;
		RPointerArray<CMyCItem> 	iItemArray;
		TContactItemId 				iLastCId;
	};




#endif // __CCOPY_CONTACT_READER_H__

