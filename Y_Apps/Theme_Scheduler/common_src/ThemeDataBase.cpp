/* Copyright (c) 2001 - 2008 , Dr Jukka Silvennoinen. All rights reserved */

#include "ThemeDataBase.h"                     // own definitions
#include <BAUTILS.H>
#include <e32math.h>
#include <aknssrvclient.h>
#include <AknSkinsInternalCRKeys.h>
#include <aknglobalnote.h> 
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
			CreateRandomTableL(iItemsDatabase);
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
	columns->AddL(TDbCol(NCol2, EDbColInt32));
	columns->AddL(TDbCol(NCol3, EDbColUint32));
	columns->AddL(TDbCol(NCol4, EDbColDateTime));
	columns->AddL(TDbCol(NCol5, EDbColInt32));
	columns->AddL(TDbCol(NCol6, EDbColInt32));
	columns->AddL(TDbCol(NCol7, EDbColText));
	
	// Create a table
	User::LeaveIfError(aDatabase.CreateTable(KtxtItemlist,*columns));
				
	// cleanup the column set
	CleanupStack::PopAndDestroy();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
void CScheduleDB::CreateRandomTableL(RDbDatabase& aDatabase) 
	{
	// Create a table definition
	CDbColSet* columns=CDbColSet::NewLC();
	
	// Add Columns
	TDbCol id(NCol0,EDbColInt32);
	id.iAttributes=id.EAutoIncrement;// automatic indexing for items,it is our key field.
	columns->AddL(id);				 
	
	columns->AddL(TDbCol(NCol3, EDbColUint32));
	
	// Create a table
	User::LeaveIfError(aDatabase.CreateTable(KtxtRandomLst,*columns));
				
	// cleanup the column set
	CleanupStack::PopAndDestroy();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
void CScheduleDB::GetRandomProfilesL(CArrayFixFlat<TUint32>& aProfArray)
{
	aProfArray.Reset();// first reset the array
	
	TFileName QueryBuffer;
	QueryBuffer.Copy(_L("SELECT * FROM "));// just get all columns & rows
	QueryBuffer.Append(KtxtRandomLst);
		
	RDbView Myview;
	Myview.Prepare(iItemsDatabase,TDbQuery(QueryBuffer));
	CleanupClosePushL(Myview);
	Myview.EvaluateAll();
	Myview.FirstL();
	
	while(Myview.AtRow()) // Just delete one instance of the message           
	{	
		Myview.GetL();		
		
		aProfArray.AppendL(Myview.ColUint32(2));		
		
		Myview.NextL();
	} 
	
	CleanupStack::PopAndDestroy(1); // Myview
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
void CScheduleDB::SetRandomProfilesL(CArrayFixFlat<TUint32>& aProfArray)
{
	DeleteRandomProfilesL();
	
	TFileName QueryBuffer;
	QueryBuffer.Copy(_L("SELECT * FROM "));// just get all columns & rows
	QueryBuffer.Append(KtxtRandomLst);
		
	iItemsDatabase.Begin();

	RDbView Myview;
	Myview.Prepare(iItemsDatabase,TDbQuery(QueryBuffer));
	CleanupClosePushL(Myview);

	for(TInt i=0; i < aProfArray.Count(); i++)
	{
		Myview.InsertL(); 
	
		Myview.SetColL(2,aProfArray.At(i));

		Myview.PutL();  
	}
		
	CleanupStack::PopAndDestroy(1); // Myview
	iItemsDatabase.Commit();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
void CScheduleDB::ReadDbItemsL(RArray<TThemeSched>&	aItemArray)
{
	aItemArray.Reset();// first reset the array
	
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
		
		TThemeSched NewItem;	
		NewItem.iIndex = Myview.ColInt32(1);
        
        if(Myview.ColInt(2) > 0x50)
        	NewItem.iType = ETrue;
        else
        	NewItem.iType = EFalse;
        
        NewItem.iProfileId = Myview.ColInt32(3);
		NewItem.iThemeId = Myview.ColUint32(4);   
        
		NewItem.iTime   = Myview.ColTime(5);
		NewItem.iRepeat = Myview.ColInt32(6);
		
		if(Myview.ColInt32(7) > 50)
			NewItem.iEnabled = ETrue;
		else
			NewItem.iEnabled = EFalse;
		
		NewItem.iImageFile.Copy(Myview.ColDes(8));
		
		aItemArray.Append(NewItem);		
		Myview.NextL();
	} 
	
	CleanupStack::PopAndDestroy(1); // Myview
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CScheduleDB::ShowNoteL(const TDesC& aMessage)
{
	CAknGlobalNote* dialog = CAknGlobalNote::NewLC();
	TInt NoteId = dialog->ShowNoteL(EAknGlobalInformationNote,aMessage);
	User::After(5000000);
	dialog->CancelNoteL(NoteId);
	CleanupStack::PopAndDestroy(dialog);;
}  
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
void CScheduleDB::DeleteRandomProfilesL(void)
{	
	TFileName QueryBuffer;
	QueryBuffer.Copy(_L("SELECT * FROM "));
	QueryBuffer.Append(KtxtRandomLst);
			
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
void CScheduleDB::DeleteFromDatabaseL(TInt& aIndex)
{	
	TFileName QueryBuffer;
	QueryBuffer.Copy(_L("SELECT * FROM "));
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
void CScheduleDB::UpdateDatabaseL(TBool aType, TInt aProfile, TUint32 aTheme,TInt aRepeat, TTime aTime,TInt& aIndex,TBool& aEnabled, TDes& aFileName)
{	
	TFileName QueryBuffer;
	QueryBuffer.Copy(_L("SELECT * FROM "));
	QueryBuffer.Append(KtxtItemlist);
	QueryBuffer.Append(_L(" WHERE "));
	QueryBuffer.Append(NCol0);
	QueryBuffer.Append(_L(" = "));
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
		
		if(aType)
			Myview.SetColL(2,0x100);
		else
			Myview.SetColL(2,0x000);
		
	    Myview.SetColL(3,aProfile);
			
		Myview.SetColL(4,aTheme);
		Myview.SetColL(5,aTime);
		Myview.SetColL(6,aRepeat);
		
		if(aEnabled)
			Myview.SetColL(7,0x100);
		else
			Myview.SetColL(7,0x000);
		
		Myview.SetColL(8,aFileName);
		
		Myview.PutL();
	}
			
	CleanupStack::PopAndDestroy(1); // Myview
	iItemsDatabase.Commit();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
void CScheduleDB::SaveToDatabaseL(TBool aType, TInt aProfile, TUint32 aTheme,TInt aRepeat, TTime aTime,TInt& aIndex,TBool& aEnabled, TDes& aFileName)
{	
	TFileName QueryBuffer;
	QueryBuffer.Copy(_L("SELECT * FROM "));
	QueryBuffer.Append(KtxtItemlist);
	
	iItemsDatabase.Begin();

	RDbView Myview;
	Myview.Prepare(iItemsDatabase,TDbQuery(QueryBuffer));
	CleanupClosePushL(Myview);

	Myview.InsertL(); 
	
	if(aType)
		Myview.SetColL(2,0x100);
	else
		Myview.SetColL(2,0x000);
	
    Myview.SetColL(3,aProfile);
		
	Myview.SetColL(4,aTheme);
	Myview.SetColL(5,aTime);
	Myview.SetColL(6,aRepeat);

	if(aEnabled)
		Myview.SetColL(7,0x100);
	else
		Myview.SetColL(7,0x000);
	
	Myview.SetColL(8,aFileName);

	Myview.PutL();  
	
	aIndex = Myview.ColInt(1);// autoincrement gives us unique index.
			
	CleanupStack::PopAndDestroy(1); // Myview
	iItemsDatabase.Commit();
}
