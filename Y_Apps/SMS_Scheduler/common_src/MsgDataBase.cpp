/* Copyright (c) 2001 - 2008, Dr. Jukka Silvennoinen. All rights reserved */


#include "MsgDataBase.h"                     // own definitions
#include <BAUTILS.H>

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
CScheduleDB::~CScheduleDB()
{
	iItemsDatabase.Close();
	iFsSession.Close();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/  
void CScheduleDB::ConstructL()
{
	User::LeaveIfError(iFsSession.Connect());

	TFindFile PrivFolder(iFsSession);
	if(KErrNone == PrivFolder.FindByDir(KtxDatabaseName, KNullDesC))// finds the drive
	{
		TEntry anEntry;
		if(KErrNone == iFsSession.Entry(PrivFolder.File(),anEntry))
		{
			if(anEntry.iSize < 10)
			{
				iFsSession.Delete(PrivFolder.File());
			}
		}
		
		if(BaflUtils::FileExists(iFsSession,PrivFolder.File()))
		{
			User::LeaveIfError(iItemsDatabase.Open(iFsSession,PrivFolder.File()));
		}
		else
		{	// no database exists so we make one
			User::LeaveIfError(iItemsDatabase.Create(iFsSession,PrivFolder.File()));     
			// and will create the onlt table needed for it
			CreateTableL(iItemsDatabase);
			CreateMsgTableL(iItemsDatabase);
		}
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
void CScheduleDB::CreateTableL(RDbDatabase& aDatabase) 
	{
	// Create a table definition
	CDbColSet* columns=CDbColSet::NewLC();
	
	// Add Columns
	TDbCol id(NCol0,EDbColInt32);
	id.iAttributes=id.EAutoIncrement;// automatic indexing for items,it is our key field.
	columns->AddL(id);				 
	
	columns->AddL(TDbCol(NCol1, EDbColInt32));
	columns->AddL(TDbCol(NCol2, EDbColText16,150));
	columns->AddL(TDbCol(NCol3, EDbColText16,255));
	columns->AddL(TDbCol(NCol4, EDbColDateTime));
	columns->AddL(TDbCol(NCol5, EDbColInt32));
	columns->AddL(TDbCol(NCol6, EDbColInt32));
	columns->AddL(TDbCol(NCol7, EDbColInt32));
	
	// Create a table
	User::LeaveIfError(aDatabase.CreateTable(KtxtItemlist,*columns));
				
	// cleanup the column set
	CleanupStack::PopAndDestroy();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
void CScheduleDB::CreateMsgTableL(RDbDatabase& aDatabase) 
	{
	// Create a table definition
	CDbColSet* columns=CDbColSet::NewLC();
	
	// Add Columns
	TDbCol id(NCol0,EDbColInt32);
	id.iAttributes=id.EAutoIncrement;// automatic indexing for items,it is our key field.
	columns->AddL(id);				 
	columns->AddL(TDbCol(NCol3, EDbColText16,255));
	
	// Create a table
	User::LeaveIfError(aDatabase.CreateTable(KtxtItemlist2,*columns));
				
	// cleanup the column set
	CleanupStack::PopAndDestroy();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
void CScheduleDB::ReadDbItemsL(RPointerArray<CMsgSched>& aItemArray)
{
	aItemArray.ResetAndDestroy();// first reset the array
	
	TFileName QueryBuffer;
	QueryBuffer.Copy(_L("SELECT * FROM "));// just get all columns & rows
	QueryBuffer.Append(KtxtItemlist);
		
	RDbView Myview;
	Myview.Prepare(iItemsDatabase,TDbQuery(QueryBuffer));
	CleanupClosePushL(Myview);
	Myview.EvaluateAll();
	Myview.FirstL();
	
	while(Myview.AtRow()) // Just delete one instance of the message           
	{	
		Myview.GetL();		
		
		CMsgSched* NewItem = new(ELeave)CMsgSched();	
		aItemArray.Append(NewItem);
		
		NewItem->iIndex  = Myview.ColInt32(1);
        NewItem->iTime   = Myview.ColTime(5);
        NewItem->iRepeat = Myview.ColInt32(6);
        
        if(Myview.ColInt32(2) > 50)
        	NewItem->iUnicode = ETrue;
        else
        	NewItem->iUnicode = EFalse;
        
        NewItem->iNunmber = Myview.ColDes(3).AllocL();
		NewItem->iMessage = Myview.ColDes(4).AllocL();
		
		if(Myview.ColInt32(7) > 50)
		  	NewItem->iEnabled = ETrue;
		else
			NewItem->iEnabled = EFalse;

		if(Myview.ColInt32(8) > 50)
		  	NewItem->iFlashSMS = ETrue;
		else
			NewItem->iFlashSMS = EFalse;
				
		Myview.NextL();
	} 
	
	CleanupStack::PopAndDestroy(1); // Myview
}
      
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
void CScheduleDB::ReadDbItemsL(RPointerArray<CMsgStored>& aItemArray)
{
	aItemArray.ResetAndDestroy();// first reset the array
	
	TFileName QueryBuffer;
	QueryBuffer.Copy(_L("SELECT * FROM "));// just get all columns & rows
	QueryBuffer.Append(KtxtItemlist2);
		
	RDbView Myview;
	Myview.Prepare(iItemsDatabase,TDbQuery(QueryBuffer));
	CleanupClosePushL(Myview);
	Myview.EvaluateAll();
	Myview.FirstL();
	
	while(Myview.AtRow()) // Just delete one instance of the message           
	{	
		Myview.GetL();		
		
		CMsgStored* NewItem = new(ELeave)CMsgStored();	
		aItemArray.Append(NewItem);
		
		NewItem->iIndex = Myview.ColInt32(1);
		NewItem->iMessage = Myview.ColDes(2).AllocL();
				
		Myview.NextL();
	} 
	
	CleanupStack::PopAndDestroy(1); // Myview
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
void CScheduleDB::DeleteFromDatabaseL(TInt& aIndex,const TBool& aMessage)
{	
	TFileName QueryBuffer;
	QueryBuffer.Copy(_L("SELECT * FROM "));
	
	if(aMessage)
		QueryBuffer.Append(KtxtItemlist2);
	else
		QueryBuffer.Append(KtxtItemlist);
	
	QueryBuffer.Append(_L(" WHERE "));
	QueryBuffer.Append(NCol0);
	QueryBuffer.Append(_L(" = "));
	QueryBuffer.AppendNum(aIndex);
			
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
-----------------------------------------------------------------------------
*/ 
void CScheduleDB::UpdateDatabaseL(CMsgSched* aData)
{	
	if(aData)
	{
		TFileName QueryBuffer;
		QueryBuffer.Copy(_L("SELECT * FROM "));
		QueryBuffer.Append(KtxtItemlist);
		QueryBuffer.Append(_L(" WHERE "));
		QueryBuffer.Append(NCol0);
		QueryBuffer.Append(_L(" = "));
		QueryBuffer.AppendNum(aData->iIndex);

		iItemsDatabase.Begin();
		
		RDbView Myview;
		Myview.Prepare(iItemsDatabase,TDbQuery(QueryBuffer));
		CleanupClosePushL(Myview);
		
		Myview.EvaluateAll();
		Myview.FirstL();
		
		if(Myview.AtRow())            
		{			
			Myview.UpdateL();
			
			if(aData->iUnicode)
				Myview.SetColL(2,0x100);
			else
				Myview.SetColL(2,0x000);
		    
		    if(aData->iNunmber)
		    	Myview.SetColL(3,*aData->iNunmber);
		    
		    if(aData->iMessage)
		    	Myview.SetColL(4,*aData->iMessage);
			
			Myview.SetColL(5,aData->iTime);
			Myview.SetColL(6,aData->iRepeat);
			
			if(aData->iEnabled)
				Myview.SetColL(7,0x100);
			else
				Myview.SetColL(7,0x000);

			if(aData->iFlashSMS)
				Myview.SetColL(8,0x100);
			else
				Myview.SetColL(8,0x000);
				
			Myview.PutL();
		}
				
		CleanupStack::PopAndDestroy(1); // Myview
		iItemsDatabase.Commit();
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
void CScheduleDB::SaveToDatabaseL(CMsgStored* aData)
{	
	if(aData)
	{
		TFileName QueryBuffer;
		QueryBuffer.Copy(_L("SELECT * FROM "));
		QueryBuffer.Append(KtxtItemlist2);
		
		iItemsDatabase.Begin();

		RDbView Myview;
		Myview.Prepare(iItemsDatabase,TDbQuery(QueryBuffer));
		CleanupClosePushL(Myview);

		Myview.InsertL(); 

	    if(aData->iMessage)
	    	Myview.SetColL(2,*aData->iMessage);

		Myview.PutL();  
		
		aData->iIndex = Myview.ColInt(1);// autoincrement gives us unique index.
				
		CleanupStack::PopAndDestroy(1); // Myview
		iItemsDatabase.Commit();
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
void CScheduleDB::UpdateDatabaseL(CMsgStored* aData)
{	
	if(aData)
	{
		TFileName QueryBuffer;
		QueryBuffer.Copy(_L("SELECT * FROM "));
		QueryBuffer.Append(KtxtItemlist2);
		QueryBuffer.Append(_L(" WHERE "));
		QueryBuffer.Append(NCol0);
		QueryBuffer.Append(_L(" = "));
		QueryBuffer.AppendNum(aData->iIndex);

		iItemsDatabase.Begin();
		
		RDbView Myview;
		Myview.Prepare(iItemsDatabase,TDbQuery(QueryBuffer));
		CleanupClosePushL(Myview);
		
		Myview.EvaluateAll();
		Myview.FirstL();
		
		if(Myview.AtRow())            
		{			
			Myview.UpdateL();

		    if(aData->iMessage)
		    	Myview.SetColL(2,*aData->iMessage);

			Myview.PutL();
		}
				
		CleanupStack::PopAndDestroy(1); // Myview
		iItemsDatabase.Commit();
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
void CScheduleDB::SaveToDatabaseL(CMsgSched* aData)
{	
	if(aData)
	{
		TFileName QueryBuffer;
		QueryBuffer.Copy(_L("SELECT * FROM "));
		QueryBuffer.Append(KtxtItemlist);
		
		iItemsDatabase.Begin();

		RDbView Myview;
		Myview.Prepare(iItemsDatabase,TDbQuery(QueryBuffer));
		CleanupClosePushL(Myview);

		Myview.InsertL(); 
		
		if(aData->iUnicode)
			Myview.SetColL(2,0x100);
		else
			Myview.SetColL(2,0x000);
	    
	    if(aData->iNunmber)
	    	Myview.SetColL(3,*aData->iNunmber);
	    
	    if(aData->iMessage)
	    	Myview.SetColL(4,*aData->iMessage);
		
		Myview.SetColL(5,aData->iTime);
		Myview.SetColL(6,aData->iRepeat);
		
		if(aData->iEnabled)
			Myview.SetColL(7,0x100);
		else
			Myview.SetColL(7,0x000);		

		if(aData->iFlashSMS)
			Myview.SetColL(8,0x100);
		else
			Myview.SetColL(8,0x000);
		
		Myview.PutL();  
		
		aData->iIndex = Myview.ColInt(1);// autoincrement gives us unique index.
				
		CleanupStack::PopAndDestroy(1); // Myview
		iItemsDatabase.Commit();
	}
}
