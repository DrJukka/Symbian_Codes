/* Copyright (c) 2001-2005, Jukka Silvennoinen, S-One Telecom co., Ltd.. All rights reserved */

#ifndef __CCOPY_CONTACT_READER_H__
#define __CCOPY_CONTACT_READER_H__

#include <e32base.h>
#include <coecntrl.h>
#include <w32std.h>
#include <e32std.h>
#include <cntdef.h>
#include <cntdb.h> 
#include <cpbkcontactchangenotifier.h> 
#include <mpbkcontactdbobserver.h> 
#include <cpbkcontactengine.h> 

	class MContactReadNotify
	{
	public:
		virtual void ContactReadDone() = 0;
		virtual void ContactReadProgress(TInt aProgress, TInt aTotal) = 0;
		};


	class CMyCItem :public CBase
		{
	public:
		~CMyCItem(){delete iFirstName;delete iLastName;delete iNumbers;};
		CMyCItem(){iFirstName = NULL;iLastName = NULL;iNumbers = NULL;};
	public:
		HBufC*			iFirstName;
		HBufC*			iLastName;
		CDesCArrayFlat*	iNumbers;
		TContactItemId	iId;
	};

	class CContactsReader : public CActive
	{
	public:
	    static CContactsReader* NewL(MContactReadNotify* aContactReadNotify);
	    static CContactsReader* NewLC(MContactReadNotify* aContactReadNotify);
	    ~CContactsReader();
	    void StartTheReadL(void);
	    TBool HasFinishedReading(void){ return iDone;};
	    TInt  ErrorCode(void){ return iError;};
	    RPointerArray<CMyCItem>& ItemArray(void){ return iItemArray;};
		void ReReadContactL(TContactItemId aItem);
	protected:
		void AppendNumbersL(CDesCArray* aArray, TFieldType aFieldType,CContactItem& aItem);
		void DoCancel();
		void RunL();
		void DoOneReadL(TContactItemId aItem);
		void FinnishReadL(void);
		void ReadExtrasInfoL(CContactItem& aContact,CMyCItem* aItem);
	    CContactsReader(MContactReadNotify* aContactReadNotify);
	    void ConstructL(void);
	private:
		CContactDatabase* 			iContactDb;
		TBool 						iDone,iCancel;
		TContactItemId 				iCId;
		TInt 						iError;
		RPointerArray<CMyCItem> 	iItemArray;
		MContactReadNotify*			iContactReadNotify;
		TInt						iTotalCount,iCurrCount;
	};


#endif // __CCOPY_CONTACT_READER_H__

