/* Copyright (c) 2001 - 2009 , J.P. Silvennoinen. All rights reserved */

#include "ContactReader.h"

#include <cntfield.h>
#include <cntdef.h> 
#include <cntitem.h>
#include <cntfldst.h>
#include <cntdb.h>

 
/*
-------------------------------------------------------------------------------
****************************  CContactsReader   *************************************
*/  

CContactsReader* CContactsReader::NewL(MContactReadNotify* aContactReadNotify)
    {
    CContactsReader* self = NewLC(aContactReadNotify);
    CleanupStack::Pop(self);
    return self;
    }

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/    
CContactsReader* CContactsReader::NewLC(MContactReadNotify* aContactReadNotify)
    {
    CContactsReader* self = new (ELeave) CContactsReader(aContactReadNotify);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CContactsReader::CContactsReader(MContactReadNotify* aContactReadNotify)
:CActive(0),iContactReadNotify(aContactReadNotify)
    {
    }
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CContactsReader::~CContactsReader()
{
	Cancel();
	delete iContactDb;
	iItemArray.ResetAndDestroy();
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CContactsReader::ConstructL(void)
{
	CActiveScheduler::Add(this);
	iContactDb = CContactDatabase::OpenL();	
	//
	CArrayFixFlat<CContactDatabase::TSortPref>* MySortOrder = new(ELeave)CArrayFixFlat<CContactDatabase::TSortPref>(1);
	
	MySortOrder->AppendL(CContactDatabase::TSortPref(KUidContactFieldGivenName));
	MySortOrder->AppendL(CContactDatabase::TSortPref(KUidContactFieldFamilyName));
	
	iContactDb->SortByTypeL(MySortOrder);
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CContactsReader::DoCancel()
{
	iCancel = ETrue;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/	
void CContactsReader::RunL()
{
	if(iCancel)
	{
		FinnishReadL();
	}
	else 
	{
		iCurrCount = iCurrCount + 1;
		
		if(iCurrCount % 10 == 0)
		{
			iContactReadNotify->ContactReadProgress(iCurrCount,iTotalCount);
		}
	
		iError = KErrNone;

		TContactIter CIter(*iContactDb);		

		TRAP(iError,
			CIter.GotoL(iCId);
			iCId = CIter.NextL());

		if(iCId != KNullContactId && iError == KErrNone)
		{
			TRAP(iError, DoOneReadL(iCId));
			if(iError != KErrNone)
			{
				FinnishReadL();
			}
			else
			{
				TRequestStatus* status=&iStatus;
				User::RequestComplete(status, KErrNone);
				SetActive();
			}
		}
		else
			FinnishReadL();
	}
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CContactsReader::ReReadContactL(TContactItemId aItem)
{
	TBool FoundMe(EFalse);
	
	for(TInt i=0; i < iItemArray.Count(); i++)
	{
		if(iItemArray[i])
		{
			if(iItemArray[i]->iId == aItem)
			{				
				CContactItem* cItem = iContactDb->ReadContactL(aItem);
				CleanupStack::PushL(cItem);
			
				ReadExtrasInfoL(*cItem,iItemArray[i]);

				CleanupStack::PopAndDestroy(1);//cItem
				
				FoundMe = ETrue;
			}
		}
	}	
	
	if(!FoundMe)
	{	// new item, lets add it to the list
		DoOneReadL(aItem);
	}
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CContactsReader::ReadExtrasInfoL(CContactItem& aContact,CMyCItem* aItem)
{
	if(aItem)
	{
		delete aItem->iNumbers;
		aItem->iNumbers = NULL;
		aItem->iNumbers = new(ELeave)CDesCArrayFlat(1);
		
		
		AppendNumbersL(aItem->iNumbers,KUidContactFieldPhoneNumber,aContact);
		AppendNumbersL(aItem->iNumbers,KUidContactFieldSms,aContact);
	}
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CContactsReader::AppendNumbersL(CDesCArray* aArray, TFieldType aFieldType,CContactItem& aItem)
{		
	TInt FiledIndex = aItem.CardFields().Find(aFieldType);
	
	if(FiledIndex != KErrNotFound)
	{
		TBuf<250> AddHjelper;
		AddHjelper.Copy(((aItem.CardFields())[FiledIndex]).TextStorage()->Text());
		
		AddHjelper.TrimAll();
		if(AddHjelper.Length())
		{
			aArray->AppendL(AddHjelper);
		}
		
		do
		{
			FiledIndex = aItem.CardFields().FindNext(aFieldType,FiledIndex+1);
								
			if (FiledIndex != KErrNotFound)
			{
				AddHjelper.Copy(((aItem.CardFields())[FiledIndex]).TextStorage()->Text());
				
				AddHjelper.TrimAll();
				
				if(AddHjelper.Length())
				{
					aArray->AppendL(AddHjelper);
				}
			}

		}while(FiledIndex != KErrNotFound);
	}
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CContactsReader::DoOneReadL(TContactItemId aItem)
{
	CContactItem* cItem = iContactDb->ReadContactL(aItem);
	CleanupStack::PushL(cItem);
	
	
	for(TInt m = 0; m < cItem->CardFields().Count(); m++)
	{
		TBuf<255> gggl;
		gggl.Copy(((cItem->CardFields())[m]).Label());
	}
	
	TInt cIndex = cItem->CardFields().Find(KUidContactFieldGivenName);
	TInt bIndex = cItem->CardFields().Find(KUidContactFieldFamilyName);

	if (cIndex != KErrNotFound || bIndex != KErrNotFound)
	{
		CMyCItem* NewItem = new(ELeave)CMyCItem();
		CleanupStack::PushL(NewItem);
		
		NewItem->iId = aItem;
	
		if (cIndex != KErrNotFound)
		{
			CContactTextField* NameField = ((cItem->CardFields())[cIndex]).TextStorage();					
			if(NameField)
			{
				if(NameField->Text().Length())
				{
					NewItem->iFirstName = NameField->Text().AllocL();
				}
			}
		}
		
		if (bIndex != KErrNotFound)
		{
			CContactTextField* NameField = ((cItem->CardFields())[bIndex]).TextStorage();					
			if(NameField)
			{
				if(NameField->Text().Length())
				{
					NewItem->iLastName= NameField->Text().AllocL();
				}
			}
		}

		ReadExtrasInfoL(*cItem,NewItem);

		CleanupStack::Pop();//NewItem
		iItemArray.Append(NewItem);
	}
	
	CleanupStack::PopAndDestroy(1);//cItem
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CContactsReader::FinnishReadL(void)
{
	iDone = ETrue;
	iContactReadNotify->ContactReadDone();
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CContactsReader::StartTheReadL(void)
{
	iError = KErrNone;
	iCancel = iDone = EFalse;
	iItemArray.ResetAndDestroy();
	
	iTotalCount = iCurrCount = 0;
	
	TContactIter CIter(*iContactDb);
	
	iTotalCount = iContactDb->CountL();
	
	iCId = CIter.FirstL();

	if(iCId != KNullContactId)
	{
		TRAP(iError, DoOneReadL(iCId));
		if(iError != KErrNone)
		{
			FinnishReadL();
		}
		else
		{
			TRequestStatus* status=&iStatus;
			User::RequestComplete(status, KErrNone);
			SetActive();
		}
	}
	else
		FinnishReadL();
}
