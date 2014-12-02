/* 	
	Copyright (c) 2001 - 2007, 
	J.P. Silvennoinen.
	All rights reserved 
*/

#include <ECom.h>
#include "Ecom_Handler.h"

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CEcomHandler::CEcomHandler(MTasksHandlerExit* aTasksHandlerExit)
:iTasksHandlerExit(aTasksHandlerExit)
{
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CEcomHandler::~CEcomHandler()
{
	iHandlers.ResetAndDestroy();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CEcomHandler* CEcomHandler::NewL(MTasksHandlerExit* aTasksHandlerExit)
{
    CEcomHandler *me = new (ELeave) CEcomHandler(aTasksHandlerExit);
    CleanupStack::PushL(me);
	me->ConstructL();
	CleanupStack::Pop();
    return me;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CEcomHandler::ConstructL()
{
	ReFreshHandlersL();
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CEcomHandler::ReFreshHandlersL()
{		
	iHandlers.ResetAndDestroy();

	RImplInfoPtrArray infoArray;
	REComSession::ListImplementationsL(KUidYTaskHandUID, infoArray);

	TFileName CurrName;

	for ( TInt i = 0; i < infoArray.Count(); i++ )
	{
		CurrName.Zero();		
		CurrName.Copy(infoArray[i]->DisplayName());
		CurrName.TrimAll();
		
		TInt YStart = infoArray[i]->OpaqueData().Find(KtxStrtYTools);
		TInt YEnd 	= infoArray[i]->OpaqueData().Find(KtxEnddYTools);
		if(YStart != KErrNotFound
		&& YEnd != KErrNotFound)
		{	
			YStart = YStart + KtxStrtYTools().Length();
			YEnd = (YEnd - YStart);
			if(YEnd > 0 )
			{
				if(CurrName.Length() && CheckVersionL(infoArray[i]->OpaqueData().Mid(YStart,YEnd)))
				{
					CHandlerItem* newItem = new(ELeave)CHandlerItem();
					CleanupStack::PushL(newItem);
				
					newItem->iUid = infoArray[i]->ImplementationUid();	
					newItem->iName = CurrName.AllocL();
				
					newItem->iServerName = ParseInfoL(infoArray[i]->OpaqueData().Mid(YStart,YEnd),KtxStrtSERVER,KtxEnddSERVER);
					newItem->iIconfile = ParseInfoL(infoArray[i]->OpaqueData().Mid(YStart,YEnd),KtxStrtICONFILE,KtxEnddICONFILE);
				
					CleanupStack::Pop(newItem);
					
					iHandlers.Append(newItem);
				}
			}
		}
	}
	
	infoArray.ResetAndDestroy();
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
TBool CEcomHandler::CheckVersionL(const TDesC8& aData)
{
	TBool Ret(EFalse);
	
	TInt VerStart 	= aData.Find(KtxStrtVERSION);
	TInt VerEnd 	= aData.Find(KtxEnddVERSION);
		
	if(VerStart != KErrNotFound
	&& VerEnd != KErrNotFound)
	{	
		VerStart = VerStart + KtxStrtVERSION().Length();
		VerEnd = (VerEnd - VerStart);
		
		if(VerEnd > 0)
		{
			if(aData.Mid(VerStart,VerEnd) == KtxCurrVERSION)
			{
				Ret = ETrue;
			}
		}
	}
	
	return Ret;
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/

HBufC* CEcomHandler::ParseInfoL(const TDesC8& aData,const TDesC8& aStartTag,const TDesC8& aEndTag)
{
	HBufC* Ret(NULL);
	
	TInt dataStart 	= aData.Find(aStartTag);
	TInt dataEnd 	= aData.Find(aEndTag);
		
	if(dataStart != KErrNotFound
	&& dataEnd != KErrNotFound)
	{	
		dataStart = dataStart + aStartTag.Length();
		dataEnd = (dataEnd - dataStart);
		
		if(dataEnd > 0)
		{
			Ret = HBufC::NewL(dataEnd);
			Ret->Des().Copy(aData.Mid(dataStart,dataEnd));
		}
	}
	
	return Ret;
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CYTasksContainer* CEcomHandler::GetHandlerL(CHandlerItem& aHandler,CEikButtonGroupContainer* aCba)
{
	CYTasksContainer* Ret(NULL);

	TAny *impl;  // pointer to interface implementation
	RImplInfoPtrArray infoArray;		
			
	REComSession::ListImplementationsL(KUidYTaskHandUID, infoArray);
	
	TFileName CurrName;				
	for ( TInt i = 0; i < infoArray.Count(); i++ )
	{
		CurrName.Zero();		
		CurrName.Copy(infoArray[i]->DisplayName());
		CurrName.TrimAll();
		
		if(aHandler.iUid == infoArray[i]->ImplementationUid() 
		&& CurrName.Length()
		&& aHandler.iName)
		{
			if(CurrName == aHandler.iName->Des())	
			{
				impl = REComSession::CreateImplementationL(infoArray[i]->ImplementationUid(), _FOFF(CYTasksContainer,iDestructorIDKey));
				if ( impl )
				{
					Ret = ((CYTasksContainer*)impl);
					Ret->iTasksHandlerExit = iTasksHandlerExit;				
					CleanupStack::PushL(Ret);
					Ret->ConstructL(aCba);
					CleanupStack::Pop();//Ret
					break;
				}
				
				impl = NULL;
			}
		}
	}

	infoArray.ResetAndDestroy();

	return Ret;
}

