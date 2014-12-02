/* Copyright (c) 2001 - 2005 , J.P. Silvennoinen. All rights reserved */

#include "ContactReader.h"

#include <cntfield.h>
#include <cntdef.h> 
#include <cntitem.h>
#include <cntfldst.h>
#include <cntdb.h>
#include <COEMAIN.H>
#include <AknQueryDialog.h>

/*
-------------------------------------------------------------------------------
****************************  CContactsReader   *************************************
-------------------------------------------------------------------------------
*/  

CContactsReader* CContactsReader::NewL(MContactReadUpdate* aUpdate)
    {
    CContactsReader* self = NewLC(aUpdate);
    CleanupStack::Pop(self);
    return self;
    }

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/    
CContactsReader* CContactsReader::NewLC(MContactReadUpdate* aUpdate)
    {
    CContactsReader* self = new (ELeave) CContactsReader(aUpdate);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CContactsReader::CContactsReader(MContactReadUpdate* aUpdate)
:CActive(0),iUpdate(aUpdate)
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
			iUpdate->ReadProgress(iCurrCount,iTotalCount);
		}
		
		iError = KErrNone;

		TContactIter CIter(*iContactDb);		

		TContactItemId CId(KNullContactId);
		
		TRAP(iError,
			if(iLastCId != KNullContactId)
			{
				CIter.GotoL(iLastCId);
			}
			CId = CIter.NextL());

		if(CId != KNullContactId && iError == KErrNone)
		{
			iLastCId = CId;
			TRAP(iError, DoOneReadL(CId));
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
TBool CContactsReader::DoReReadL(CMyCItem* aItem)
{
	TBool IsOk(EFalse);
	
	if(aItem)
	{
		CContactItem* cItem = iContactDb->ReadContactL(aItem->iId);
		CleanupStack::PushL(cItem);
		TInt cIndex = cItem->CardFields().Find(KUidContactFieldGivenName);
		TInt bIndex = cItem->CardFields().Find(KUidContactFieldFamilyName);

		if (cIndex != KErrNotFound || bIndex != KErrNotFound)
		{
			aItem->iNumbers.ResetAndDestroy();
			
			delete aItem->iFirstName;
			aItem->iFirstName = NULL;
			
			delete aItem->iLastName;
			aItem->iLastName = NULL;
			
			if (cIndex != KErrNotFound)
			{
				CContactTextField* NameField = ((cItem->CardFields())[cIndex]).TextStorage();					
				if(NameField)
				{
					aItem->iFirstName = NameField->Text().AllocL();
				}
			}
			
			if (bIndex != KErrNotFound)
			{
				CContactTextField* NameField = ((cItem->CardFields())[bIndex]).TextStorage();					
				if(NameField)
				{
					aItem->iLastName= NameField->Text().AllocL();
				}
			}
			
			CDesCArray* Numbers = GetAllNumbersLC(cItem);
			
			if(Numbers->MdcaCount())
			{
				for(TInt i=0; i < Numbers->MdcaCount(); i++)
				{
					CNumItem* NewNumber = new(ELeave)CNumItem();
					CleanupStack::PushL(NewNumber);
					
					NewNumber->iNumber = Numbers->MdcaPoint(i).AllocL();
					//NewNumber->iMobile = STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->EikAppUi())->IsMobileNum(*NewNumber->iNumber);
					
					CleanupStack::Pop(NewNumber);
					aItem->iNumbers.Append(NewNumber);
				}
				
				IsOk = ETrue;
			}
			
			CleanupStack::PopAndDestroy(Numbers);
		}
		
		CleanupStack::PopAndDestroy(1);//cItem
	}
	
	return IsOk;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CContactsReader::DoOneReadL(TContactItemId aCId)
{
	CContactItem* cItem = iContactDb->ReadContactL(aCId);
	CleanupStack::PushL(cItem);
	TInt cIndex = cItem->CardFields().Find(KUidContactFieldGivenName);
	TInt bIndex = cItem->CardFields().Find(KUidContactFieldFamilyName);

	if (cIndex != KErrNotFound || bIndex != KErrNotFound)
	{
		CMyCItem* NewItem = new(ELeave)CMyCItem();
		CleanupStack::PushL(NewItem);
		NewItem->iId = aCId;
	
		if (cIndex != KErrNotFound)
		{
			CContactTextField* NameField = ((cItem->CardFields())[cIndex]).TextStorage();					
			if(NameField)
			{
				NewItem->iFirstName = NameField->Text().AllocL();
			}
		}
		
		if (bIndex != KErrNotFound)
		{
			CContactTextField* NameField = ((cItem->CardFields())[bIndex]).TextStorage();					
			if(NameField)
			{
				NewItem->iLastName= NameField->Text().AllocL();
			}
		}
		
		CDesCArray* Numbers = GetAllNumbersLC(cItem);
		
		if(Numbers->MdcaCount())
		{
			for(TInt i=0; i < Numbers->MdcaCount(); i++)
			{
				CNumItem* NewNumber = new(ELeave)CNumItem();
				CleanupStack::PushL(NewNumber);
				
				NewNumber->iNumber = Numbers->MdcaPoint(i).AllocL();
				//NewNumber->iMobile = STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->EikAppUi())->IsMobileNum(*NewNumber->iNumber);
				
				CleanupStack::Pop(NewNumber);
				NewItem->iNumbers.Append(NewNumber);
			}
			
			CleanupStack::PopAndDestroy(Numbers);
			CleanupStack::Pop(NewItem);
			
			iItemArray.Append(NewItem);
		}
		else
		{
			CleanupStack::PopAndDestroy(Numbers);
			CleanupStack::PopAndDestroy(NewItem);
		}
	}
	
	CleanupStack::PopAndDestroy(1);//cItem
}


/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CDesCArray* CContactsReader::GetAllNumbersLC(CContactItem* aItem)
{
	CDesCArrayFlat* NumberArray = new(ELeave)CDesCArrayFlat(1);
	CleanupStack::PushL(NumberArray);
	
	AppendNumbersL(NumberArray,KUidContactFieldPhoneNumber,aItem);
	AppendNumbersL(NumberArray,KUidContactFieldSms,aItem);
					
	return NumberArray;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CContactsReader::AppendNumbersL(CDesCArray* aArray, TFieldType aFieldType,CContactItem* aItem)
{		
	TInt FiledIndex = aItem->CardFields().Find(aFieldType);
	
	if(FiledIndex != KErrNotFound)
	{
		TBuf<250> AddHjelper;
		AddHjelper.Copy(((aItem->CardFields())[FiledIndex]).TextStorage()->Text());
		
		AddHjelper.TrimAll();		
		if(AddHjelper.Length())
		{
			aArray->AppendL(AddHjelper);
		}
		
		do
		{
			FiledIndex = aItem->CardFields().FindNext(aFieldType,FiledIndex+1);
								
			if (FiledIndex != KErrNotFound)
			{
				AddHjelper.Copy(((aItem->CardFields())[FiledIndex]).TextStorage()->Text());
				
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
void CContactsReader::FinnishReadL(void)
{
	iDone = ETrue;

	iUpdate->ReadProgress(100, 100);
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CContactsReader::StartTheReadL(void)
{
	iLastCId = KNullContactId;

	iTotalCount = iCurrCount = 0;
	
	iError = KErrNone;
	iCancel = iDone = EFalse;
	iItemArray.ResetAndDestroy();
	
	TContactIter CIter(*iContactDb);
	
	iTotalCount = iContactDb->CountL();
	
	TContactItemId CId = CIter.FirstL();

	if(CId != KNullContactId)
	{
		iLastCId = CId;
		TRAP(iError, DoOneReadL(CId));
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
	{
		TRequestStatus* status=&iStatus;
		User::RequestComplete(status, KErrNone);
		SetActive();
	}
}
