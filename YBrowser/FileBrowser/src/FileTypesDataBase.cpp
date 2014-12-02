
#include <BAUTILS.H>

#include "FileTypesDataBase.h"

#include "YuccaBrowser.h"
#include "YuccaBrowser.hrh"

#include "Common.h"
#ifdef SONE_VERSION
	#include <YFB_2002B0AA.rsg>
#else
	#ifdef LAL_VERSION

	#else
		#ifdef __YTOOLS_SIGNED
			#include <YuccaBrowser_2000713D.rsg>
		#else
			#include <YuccaBrowser.rsg>
		#endif
	#endif
#endif


_LIT(KTxSelectFrom		,"SELECT * FROM ");
_LIT(KTxWhere			," WHERE ");
_LIT(KtxEqualSign		," = ");

// all tables have index columsn as a firts one
_LIT(NColIndex				,"index");

// filetypes column names
_LIT(NCol1					,"name");
_LIT(NCol2					,"handler");
_LIT(NCol3					,"extensions");
_LIT(NCol4					,"typpe");
_LIT(NCol5					,"datta");

// plug-in disable column names
_LIT(NColPl1				,"type");
_LIT(NColPl2				,"uid");
_LIT(NColPl3				,"name");

// Handler filetypes column names
_LIT(NColHd1				,"type");
_LIT(NColHd2				,"uid");
_LIT(NColHd3				,"name");

//Handler default column names
_LIT(NColHdef1				,"name");
_LIT(NColHdef2				,"uid");

// database table name
_LIT(KtxtItemlist			,"ftass");
_LIT(KtxtRecognizerTable	,"recdis");
_LIT(KtxtHandlerAddTable	,"handleradd");

_LIT(KtxtHandlerDefault		,"defaulthand");
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
CMyDBClass::~CMyDBClass()
{
	iItemsDatabase.Close();
	
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/    
void CMyDBClass::ConstructL()
{
	TFileName ShortFil;
	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone != AppFile.FindByDir(KtxFTASSFileName, KNullDesC))
	{
		if(KErrNone ==CCoeEnv::Static()->FsSession().PrivatePath(ShortFil))
		{
			TFindFile privFile(CCoeEnv::Static()->FsSession());
			if(KErrNone == privFile.FindByDir(ShortFil, KNullDesC))
			{
				TParsePtrC hjelp(privFile.File());
				ShortFil.Copy(hjelp.Drive());
				ShortFil.Append(KtxFTASSFileName);
			}
		}
	}
	else
	{
		ShortFil.Copy(AppFile.File());
	}
			
	if(BaflUtils::FileExists(CEikonEnv::Static()->FsSession(),ShortFil))
	{
		TInt ePerror= iItemsDatabase.Open(CEikonEnv::Static()->FsSession(),ShortFil);
		
		User::LeaveIfError(ePerror);
	}
	else
	{	// no database exists so we make one
		User::LeaveIfError(iItemsDatabase.Create(CEikonEnv::Static()->FsSession(),ShortFil));     
		// and will create the onlt table needed for it
		CreateTypesTableL(iItemsDatabase);
		CreateDisableTableL(iItemsDatabase);
		CreateHandlerTypesL(iItemsDatabase);
		CreateHandlerDefaultL(iItemsDatabase);
	}
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMyDBClass::CreateHandlerTypesL(RDbDatabase& aDatabase) 
	{
	// Create a table definition
	CDbColSet* columns=CDbColSet::NewLC();
	
	// Add Columns
	TDbCol id(NColIndex,EDbColInt32);
	id.iAttributes=id.EAutoIncrement;// automatic indexing for items,it is our key field.
	columns->AddL(id);				 
	
	columns->AddL(TDbCol(NColHd1, EDbColText,250));
	columns->AddL(TDbCol(NColHd2, EDbColUint32));
	columns->AddL(TDbCol(NColHd3, EDbColText,250));
	
	// Create a table
	User::LeaveIfError(aDatabase.CreateTable(KtxtHandlerAddTable,*columns));
				
	// cleanup the column set
	CleanupStack::PopAndDestroy();
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMyDBClass::CreateHandlerDefaultL(RDbDatabase& aDatabase) 
	{
	// Create a table definition
	CDbColSet* columns=CDbColSet::NewLC();
	
	// Add Columns
	TDbCol id(NColIndex,EDbColInt32);
	id.iAttributes=id.EAutoIncrement;// automatic indexing for items,it is our key field.
	columns->AddL(id);				 
	
	columns->AddL(TDbCol(NColHdef1, EDbColText,250));
	columns->AddL(TDbCol(NColHdef2, EDbColUint32));
	
	// Create a table
	User::LeaveIfError(aDatabase.CreateTable(KtxtHandlerDefault,*columns));
				
	// cleanup the column set
	CleanupStack::PopAndDestroy();
}


/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMyDBClass::CreateDisableTableL(RDbDatabase& aDatabase) 
	{
	// Create a table definition
	CDbColSet* columns=CDbColSet::NewLC();
	
	// Add Columns
	TDbCol id(NColIndex,EDbColInt32);
	id.iAttributes=id.EAutoIncrement;// automatic indexing for items,it is our key field.
	columns->AddL(id);				 
	
	columns->AddL(TDbCol(NColPl1, EDbColInt32));
	columns->AddL(TDbCol(NColPl2, EDbColUint32));
	columns->AddL(TDbCol(NColPl3, EDbColText,250));
	
	// Create a table
	User::LeaveIfError(aDatabase.CreateTable(KtxtRecognizerTable,*columns));
				
	// cleanup the column set
	CleanupStack::PopAndDestroy();
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMyDBClass::CreateTypesTableL(RDbDatabase& aDatabase) 
	{
	// Create a table definition
	CDbColSet* columns=CDbColSet::NewLC();
	
	// Add Columns
	TDbCol id(NColIndex,EDbColInt32);
	id.iAttributes=id.EAutoIncrement;// automatic indexing for items,it is our key field.
	columns->AddL(id);				 
	
	columns->AddL(TDbCol(NCol1, EDbColText,250));
	columns->AddL(TDbCol(NCol2, EDbColText,250));
	columns->AddL(TDbCol(NCol3, EDbColText,250));
	columns->AddL(TDbCol(NCol4, EDbColInt32));
	columns->AddL(TDbCol(NCol5, EDbColText8,250));
	
	// Create a table
	User::LeaveIfError(aDatabase.CreateTable(KtxtItemlist,*columns));
				
	// cleanup the column set
	CleanupStack::PopAndDestroy();
}
/*
-----------------------------------------------------------------------------
*****************************************************************************
----------------------------------------------------------------------------
*/
TBool CMyDBClass::GetDefaultHandL(TDes& aName,TUint32& aUid)
{
	TBool Ret(EFalse);
	
	TFileName QueryBuffer;
	QueryBuffer.Copy(KTxSelectFrom);// just get all columns & rows
	QueryBuffer.Append(KtxtHandlerDefault);
	
	RDbView Myview;
	Myview.Prepare(iItemsDatabase,TDbQuery(QueryBuffer));
	CleanupClosePushL(Myview);
	Myview.EvaluateAll();
	Myview.FirstL();
	
	if(Myview.AtRow()) // Just delete one instance of the message           
	{
		Ret = ETrue;	
		Myview.GetL();		
		aName.Copy(Myview.ColDes(2));
        aUid = Myview.ColUint32(3);
	} 
	
	CleanupStack::PopAndDestroy(1); // Myview
	
	return Ret;
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMyDBClass::RemoveDefaultHandL(void)
{
	TFileName QueryBuffer;
	QueryBuffer.Copy(KTxSelectFrom);// just get all columns & rows
	QueryBuffer.Append(KtxtHandlerDefault);
		
	iItemsDatabase.Begin();
	
	RDbView Myview;
	// query buffr with index finds only the selected item row.
	Myview.Prepare(iItemsDatabase,TDbQuery(QueryBuffer));
	CleanupClosePushL(Myview);
	
	Myview.EvaluateAll();
	Myview.FirstL();
	// we have autoincrement in index so it should be unique
	// but just to make sure, we use 'while', instead of 'if'
	while(Myview.AtRow())            
	{	
		Myview.GetL();
		Myview.DeleteL();
		Myview.NextL();
	}
			
	CleanupStack::PopAndDestroy(1); // Myview
	iItemsDatabase.Commit();
	// compacts the databse, by physicaly removig deleted data.
	iItemsDatabase.Compact();
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/	
void CMyDBClass::SetDefaultHandL(const TDesC& aName, const TUint32& aUid)
{
	RemoveDefaultHandL();
	
	TFileName QueryBuffer;
	QueryBuffer.Copy(KTxSelectFrom);
	QueryBuffer.Append(KtxtHandlerDefault);

	iItemsDatabase.Begin();

	RDbView Myview;
	Myview.Prepare(iItemsDatabase,TDbQuery(QueryBuffer));
	CleanupClosePushL(Myview);

	Myview.InsertL(); 
	Myview.SetColL(2,aName);
	Myview.SetColL(3,aUid);
	Myview.PutL();  
	
	CleanupStack::PopAndDestroy(1); // Myview
	iItemsDatabase.Commit();
}

/*
-----------------------------------------------------------------------------
*****************************************************************************
----------------------------------------------------------------------------
*/
void CMyDBClass::GetDisabledL(RPointerArray<CDisabledItem>&	aArray)
{
	aArray.Reset();// first reset the array
	
	TFileName QueryBuffer;
	QueryBuffer.Copy(KTxSelectFrom);// just get all columns & rows
	QueryBuffer.Append(KtxtRecognizerTable);
	
	RDbView Myview;
	Myview.Prepare(iItemsDatabase,TDbQuery(QueryBuffer));
	CleanupClosePushL(Myview);
	Myview.EvaluateAll();
	Myview.FirstL();
	
	while(Myview.AtRow()) // Just delete one instance of the message           
	{	
		Myview.GetL();		

		CDisabledItem* NewItem = new(ELeave)CDisabledItem();	
		CleanupStack::PushL(NewItem);

		NewItem->iIndex = Myview.ColInt(1);
        
        if(Myview.ColInt(2) < 50)
			NewItem->iRecognizer= ETrue;
		else
			NewItem->iRecognizer= EFalse;
		
        NewItem->iUid 	= Myview.ColUint32(3);
        NewItem->iName	= Myview.ColDes(4).AllocL();
		
		CleanupStack::Pop(1);
		aArray.Append(NewItem);		
		Myview.NextL();
	} 
	
	CleanupStack::PopAndDestroy(1); // Myview
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TBool CMyDBClass::SetEnabledL(const TDesC& aName, const TUint32& aUid,const TBool& aRecognizer)
{
	TBool EnabledIt(EFalse);

	TFileName QueryBuffer;
	QueryBuffer.Copy(KTxSelectFrom);// just get all columns & rows
	QueryBuffer.Append(KtxtRecognizerTable);
		
	iItemsDatabase.Begin();
	
	RDbView Myview;
	// query buffr with index finds only the selected item row.
	Myview.Prepare(iItemsDatabase,TDbQuery(QueryBuffer));
	CleanupClosePushL(Myview);
	
	Myview.EvaluateAll();
	Myview.FirstL();
	// we have autoincrement in index so it should be unique
	// but just to make sure, we use 'while', instead of 'if'
	while(Myview.AtRow())            
	{	
		Myview.GetL();
		
		if((Myview.ColInt(2) < 50) && aRecognizer)
		{
			if(Myview.ColUint32(3) == aUid
			&& Myview.ColDes(4) == aName)
			{
				Myview.DeleteL();
				EnabledIt = ETrue;
			}
		}
		else if(!aRecognizer)
		{
			if(Myview.ColUint32(3) == aUid
			&& Myview.ColDes(4) == aName)
			{
				Myview.DeleteL();
				EnabledIt = ETrue;
			}
		}
			
		Myview.NextL();
	}
			
	CleanupStack::PopAndDestroy(1); // Myview
	iItemsDatabase.Commit();
	// compacts the databse, by physicaly removig deleted data.
	iItemsDatabase.Compact();
	
	return EnabledIt;
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMyDBClass::SetDisabledL(const TDesC& aName, const TUint32& aUid,const TBool& aRecognizer,TInt& aIndex)
{
	TFileName QueryBuffer;
	QueryBuffer.Copy(KTxSelectFrom);
	QueryBuffer.Append(KtxtRecognizerTable);

	iItemsDatabase.Begin();

	RDbView Myview;
	Myview.Prepare(iItemsDatabase,TDbQuery(QueryBuffer));
	CleanupClosePushL(Myview);

	Myview.InsertL(); 
		
	if(aRecognizer)
		Myview.SetColL(2,0x0);
	else
		Myview.SetColL(2,0x100);
	
	Myview.SetColL(3,aUid);
	Myview.SetColL(4,aName);
	
	Myview.PutL();  
	
	aIndex = Myview.ColInt(1);// autoincrement gives us unique index.
	
	CleanupStack::PopAndDestroy(1); // Myview
	iItemsDatabase.Commit();
}
/*
-----------------------------------------------------------------------------
****************************************************************************
----------------------------------------------------------------------------
*/
void CMyDBClass::GetHandlerTypesOWRL(const TDesC& aName, const TUint32& aUid,CDesCArray& aTypesArray)
{
	TFileName QueryBuffer;
	QueryBuffer.Copy(KTxSelectFrom);// just get all columns & rows
	QueryBuffer.Append(KtxtHandlerAddTable);
	
	RDbView Myview;
	Myview.Prepare(iItemsDatabase,TDbQuery(QueryBuffer));
	CleanupClosePushL(Myview);
	Myview.EvaluateAll();
	Myview.FirstL();
	
	while(Myview.AtRow()) // Just delete one instance of the message           
	{	
		Myview.GetL();		
		
		if(aName == Myview.ColDes(4) && aUid == Myview.ColUint32(3))
		{
			if(Myview.ColDes(2).Length())
			{
				aTypesArray.AppendL(Myview.ColDes(2));
			}
		}
			
		Myview.NextL();
	} 
	
	CleanupStack::PopAndDestroy(1); // Myview
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMyDBClass::SetHandlerTypesOWRL(const TDesC& aName, const TUint32& aUid,CDesCArray& aTypesArray)
{
	TFileName QueryBuffer;
	QueryBuffer.Copy(KTxSelectFrom);// just get all columns & rows
	QueryBuffer.Append(KtxtHandlerAddTable);
	
	iItemsDatabase.Begin();

	RDbView Myview;
	Myview.Prepare(iItemsDatabase,TDbQuery(QueryBuffer));
	CleanupClosePushL(Myview);

	for(TInt i=0; i < aTypesArray.MdcaCount(); i++)
	{
		Myview.InsertL(); 
		Myview.SetColL(2,aTypesArray.MdcaPoint(i));
		Myview.SetColL(3,aUid);
		Myview.SetColL(4,aName);
	
		Myview.PutL();  
	}
	
	CleanupStack::PopAndDestroy(1); // Myview
	iItemsDatabase.Commit();
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMyDBClass::DeleteHandlerTypesOWRL(const TDesC& aName, const TUint32& aUid)
{
	TFileName QueryBuffer;
	QueryBuffer.Copy(KTxSelectFrom);// just get all columns & rows
	QueryBuffer.Append(KtxtHandlerAddTable);
	
	RDbView Myview;
	Myview.Prepare(iItemsDatabase,TDbQuery(QueryBuffer));
	CleanupClosePushL(Myview);
	Myview.EvaluateAll();
	Myview.FirstL();
	
	while(Myview.AtRow()) // Just delete one instance of the message           
	{	
		Myview.GetL();		
		
		if(aName == Myview.ColDes(4) && aUid == Myview.ColUint32(3))
		{
			Myview.DeleteL();
		}
			
		Myview.NextL();
	} 
	
	CleanupStack::PopAndDestroy(1); // Myview
}

/*
-----------------------------------------------------------------------------
****************************************************************************
----------------------------------------------------------------------------
*/
_LIT(KTxMyExtensionSeparator	,"\t");

CDesCArrayFlat* CMyDBClass::GetExtensionArrayL(const TDesC& aData)
{
	CDesCArrayFlat* Ret = new(ELeave)CDesCArrayFlat(10);
	
	TInt RetErr(KErrNone),indexx(0);
	
	TPtrC ItName;
	
	do
	{
		RetErr = TextUtils::ColumnText(ItName,indexx,&aData);
		if(KErrNone == RetErr)
		{
			Ret->AppendL(ItName);
		}
		
		indexx++;
		
	}while(RetErr == KErrNone);
	
	if(aData.Length() && !Ret->MdcaCount())
	{
		Ret->AppendL(aData);
	}
	
	return Ret;
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMyDBClass::GetBufferFromArrayL(CDesCArray* aExtensions,TDes& aData)
{
	aData.Zero();
	
	if(aExtensions)
	{
		for(TInt i=0; i < aExtensions->MdcaCount(); i++)
		{
			if(aData.MaxLength() > (aData.Length() + aExtensions->MdcaPoint(i).Length()))
			{
				aData.Append(aExtensions->MdcaPoint(i));
				aData.Append(KTxMyExtensionSeparator);
			}
		}
	}
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMyDBClass::ReadDbItemsL(RPointerArray<CFtASSItem>& aItemArray)
{
	aItemArray.Reset();// first reset the array
	
	TFileName QueryBuffer;
	QueryBuffer.Copy(KTxSelectFrom);// just get all columns & rows
	QueryBuffer.Append(KtxtItemlist);
	
	RDbView Myview;
	Myview.Prepare(iItemsDatabase,TDbQuery(QueryBuffer));
	CleanupClosePushL(Myview);
	Myview.EvaluateAll();
	Myview.FirstL();
	
	while(Myview.AtRow()) // Just delete one instance of the message           
	{	
		Myview.GetL();		
		
		CFtASSItem* NewItem = new(ELeave)CFtASSItem();	
		CleanupStack::PushL(NewItem);

		NewItem->iIndex 	= Myview.ColInt(1);
        NewItem->iTypeName 	= Myview.ColDes(2).AllocL();
        NewItem->iHandler	= Myview.ColDes(3).AllocL();
        NewItem->iExtensions= GetExtensionArrayL(Myview.ColDes(4));
		
		if(Myview.ColInt(5) > 50)
			NewItem->iOwn = ETrue;
		else
			NewItem->iOwn = EFalse;
		
		CleanupStack::Pop(1);
		aItemArray.Append(NewItem);		
		Myview.NextL();
	} 
	
	CleanupStack::PopAndDestroy(1); // Myview
}


/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMyDBClass::ReadHandlersItemsL(RPointerArray<CFtASSItem>&	aItemArray)
{
	aItemArray.Reset();// first reset the array
	
	TFileName QueryBuffer;
	QueryBuffer.Copy(KTxSelectFrom);// just get all columns & rows
	QueryBuffer.Append(KtxtItemlist);
	
	RDbView Myview;
	Myview.Prepare(iItemsDatabase,TDbQuery(QueryBuffer));
	CleanupClosePushL(Myview);
	Myview.EvaluateAll();
	Myview.FirstL();
	
	while(Myview.AtRow()) // Just delete one instance of the message           
	{	
		Myview.GetL();		
		
		if(Myview.ColDes(3).Length() && Myview.ColDes(2).Length())
		{
			CFtASSItem* NewItem = new(ELeave)CFtASSItem();	
			CleanupStack::PushL(NewItem);
	
			NewItem->iIndex 	= Myview.ColInt(1);
        	NewItem->iTypeName 	= Myview.ColDes(2).AllocL();
        	NewItem->iHandler	= Myview.ColDes(3).AllocL();
			NewItem->iExtensions= GetExtensionArrayL(Myview.ColDes(4));
		
			if(Myview.ColInt(5) > 50)
				NewItem->iOwn = ETrue;
			else
				NewItem->iOwn = EFalse;
		
			CleanupStack::Pop(1);
			aItemArray.Append(NewItem);		
		}
		
		Myview.NextL();
	} 
	
	CleanupStack::PopAndDestroy(1); // Myview
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMyDBClass::ReadTypeItemsL(RPointerArray<CFtASSItem>&	aItemArray)
{
	aItemArray.Reset();// first reset the array
	
	TFileName QueryBuffer;
	QueryBuffer.Copy(KTxSelectFrom);// just get all columns & rows
	QueryBuffer.Append(KtxtItemlist);
	
	RDbView Myview;
	Myview.Prepare(iItemsDatabase,TDbQuery(QueryBuffer));
	CleanupClosePushL(Myview);
	Myview.EvaluateAll();
	Myview.FirstL();
	
	while(Myview.AtRow()) // Just delete one instance of the message           
	{	
		Myview.GetL();		
		
		if(Myview.ColDes(4).Length() && Myview.ColDes(2).Length())
		{
			CFtASSItem* NewItem = new(ELeave)CFtASSItem();	
			CleanupStack::PushL(NewItem);

			NewItem->iIndex 	= Myview.ColInt(1);
	        NewItem->iTypeName 	= Myview.ColDes(2).AllocL();
	        NewItem->iHandler	= Myview.ColDes(3).AllocL();
			NewItem->iExtensions= GetExtensionArrayL(Myview.ColDes(4));
			
			if(Myview.ColInt(5) > 50)
				NewItem->iOwn = ETrue;
			else
				NewItem->iOwn = EFalse;
		
			CleanupStack::Pop(1);
			aItemArray.Append(NewItem);		
		}
		
		Myview.NextL();
	} 
	
	CleanupStack::PopAndDestroy(1); // Myview
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMyDBClass::DeleteFromDatabaseL(TInt& aIndex)
{	
	TFileName QueryBuffer;
	QueryBuffer.Copy(KTxSelectFrom);
	QueryBuffer.Append(KtxtItemlist);
	
	if(aIndex >= 0)
	{
		QueryBuffer.Append(KTxWhere);
		QueryBuffer.Append(NColIndex);
		QueryBuffer.Append(KtxEqualSign);
		QueryBuffer.AppendNum(aIndex);
	}
	
	iItemsDatabase.Begin();
	
	RDbView Myview;
	// query buffr with index finds only the selected item row.
	Myview.Prepare(iItemsDatabase,TDbQuery(QueryBuffer));
	CleanupClosePushL(Myview);
	
	Myview.EvaluateAll();
	Myview.FirstL();
	// we have autoincrement in index so it should be unique
	// but just to make sure, we use 'while', instead of 'if'
	while(Myview.AtRow())            
	{	
		Myview.GetL();
		Myview.DeleteL();	
		Myview.NextL();
	}
			
	CleanupStack::PopAndDestroy(1); // Myview
	iItemsDatabase.Commit();
	// compacts the databse, by physicaly removig deleted data.
	iItemsDatabase.Compact();
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMyDBClass::UpdateDatabaseL(const TDesC& aTypeName,const TDesC& aHandler,CDesCArray* aExtensions,TBool aOwn,TInt& aIndex)
{	
	TFileName QueryBuffer;
	QueryBuffer.Copy(KTxSelectFrom);
	QueryBuffer.Append(KtxtItemlist);
	QueryBuffer.Append(KTxWhere);
	QueryBuffer.Append(NColIndex);
	QueryBuffer.Append(KtxEqualSign);
	QueryBuffer.AppendNum(aIndex);
	
	iItemsDatabase.Begin();
	
	RDbView Myview;
	Myview.Prepare(iItemsDatabase,TDbQuery(QueryBuffer));
	CleanupClosePushL(Myview);
	
	Myview.EvaluateAll();
	Myview.FirstL();
	
	if(Myview.AtRow())            
	{			
		Myview.UpdateL();
		Myview.SetColL(2,aTypeName);		
		Myview.SetColL(3,aHandler);
		
		QueryBuffer.Zero();
		GetBufferFromArrayL(aExtensions,QueryBuffer);
		Myview.SetColL(4,QueryBuffer);	
		
		if(aOwn)
			Myview.SetColL(5,0x100);	
		else
			Myview.SetColL(5,0x000);	
		
		Myview.PutL();
	}
			
	CleanupStack::PopAndDestroy(1); // Myview
	iItemsDatabase.Commit();
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMyDBClass::SaveToDatabaseL(const TDesC& aTypeName,const TDesC& aHandler,CDesCArray* aExtensions,TBool aOwn,TInt& aIndex)
{	
	TFileName QueryBuffer;
	QueryBuffer.Copy(KTxSelectFrom);
	QueryBuffer.Append(KtxtItemlist);

	iItemsDatabase.Begin();

	RDbView Myview;
	Myview.Prepare(iItemsDatabase,TDbQuery(QueryBuffer));
	CleanupClosePushL(Myview);

	Myview.InsertL(); 
		
	Myview.SetColL(2,aTypeName);
	Myview.SetColL(3,aHandler);
	
	QueryBuffer.Zero();
	GetBufferFromArrayL(aExtensions,QueryBuffer);
	Myview.SetColL(4,QueryBuffer);

	if(aOwn)
		Myview.SetColL(5,0x100);	
	else
		Myview.SetColL(5,0x000);
		
	Myview.PutL();  
	
	aIndex = Myview.ColInt(1);// autoincrement gives us unique index.
			
	CleanupStack::PopAndDestroy(1); // Myview
	iItemsDatabase.Commit();
}
