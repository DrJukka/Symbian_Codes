/* ====================================================================
 * File: MainListView.cpp
 * Created: 01/07/07
 * Author: Jukka Silvennoinen
 * Copyright (c): 2007, All rights reserved
 * ==================================================================== */

#include <coemain.h>
#include <aknappui.h>
#include <EIKENV.H>
#include <AknUtils.h>
#include <AknIconArray.h>
#include <eikclbd.h>
#include <APGCLI.H>
#include <GULICON.H>
#include <aknmessagequerydialog.h>
#include <BAUTILS.H>
#include "YTools_A0000F5D.hrh"

#include "MainServerSession.h"
#include "NewTypesContainer.h"
#include "MainContainer.h"
#include "AddType_Settings.h"

_LIT(KtxtItemlist	,"RecFiles");
_LIT(NCol0			,"index");
_LIT(NCol1			,"typpe");
_LIT(NCol2			,"data");
_LIT(NCol3			,"ext");

#include "Common.h"

#ifdef SONE_VERSION
	#include "YTools_2002B0A8_res.rsg"
#else
	#ifdef LAL_VERSION
		#include "YTools_A0000F5D_res.rsg"
	#else
		#include "YTools_A0000F5D_res.rsg"
	#endif
#endif



_LIT8(KtxYFileType	,"ytasks/yfile");

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMyTepesView* CMyTepesView::NewL(CEikButtonGroupContainer& aCba)
    {
    CMyTepesView* self = CMyTepesView::NewLC(aCba);
    CleanupStack::Pop(self);
    return self;
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMyTepesView* CMyTepesView::NewLC(CEikButtonGroupContainer& aCba)
    {
    CMyTepesView* self = new (ELeave) CMyTepesView(aCba);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMyTepesView::CMyTepesView(CEikButtonGroupContainer& aCba)
:iCba(aCba)
    {
	// no implementation required
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMyTepesView::~CMyTepesView()
{
	delete iImeiSettings;
	iImeiSettings = NULL;

	delete iListBox;
	iArray.ResetAndDestroy();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyTepesView::ConstructL()
{
    CreateWindowL();

    SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
   
   	ActivateL();
    SetMenuL();
    DrawNow();
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMyTepesView::MakeListBoxL()
{
	TInt MySetIndex(0);
	
	if(iListBox)
	{
		MySetIndex = GetSelectedIndexL();
	}
    
	delete iListBox;
	iListBox = NULL;
	
    iListBox   = new( ELeave ) CAknDoubleStyleListBox();
	iListBox->ConstructL(this,EAknListBoxSelectionList);
	
	iListBox->Model()->SetItemTextArray(GetListBoxArrayL());
    iListBox->Model()->SetOwnershipType(ELbmOwnsItemArray);
	iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
	iListBox->SetRect(Rect());
	
	iListBox->ActivateL();

	TInt ItemsCount = iListBox->Model()->ItemTextArray()->MdcaCount();
	
	if(ItemsCount > MySetIndex && MySetIndex >= 0)
		iListBox->SetCurrentItemIndex(MySetIndex);
	else if(ItemsCount > 0)
		iListBox->SetCurrentItemIndex(0);
	
	UpdateScrollBar(iListBox);
	iListBox->DrawNow();
	DrawNow();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CDesCArray* CMyTepesView::GetListBoxArrayL(void)
{
	CDesCArrayFlat* MyArray = new(ELeave)CDesCArrayFlat(10);
	CleanupStack::PushL(MyArray);
	
	TRAPD(Err,GetTypesArrayL());
	
	TFileName Hjelpper, SmallHjelp;
	for(TInt i=0; i < iArray.Count(); i++)
	{
		Hjelpper.Copy(_L("\t"));
	
		if(iArray[i])
		{
			if(iArray[i]->iType)
			{
				SmallHjelp.Copy(*iArray[i]->iType);
				Hjelpper.Append(SmallHjelp);	
			}
			
			Hjelpper.Append(_L("\t"));	
		
			if(iArray[i]->iExt)
			{
				Hjelpper.Append(*iArray[i]->iExt);
			}
			else if(iArray[i]->iData)
			{
				SmallHjelp.Copy(*iArray[i]->iData);
				Hjelpper.Append(SmallHjelp);
			}
		}
		
		MyArray->AppendL(Hjelpper);
	}

	CleanupStack::Pop(MyArray);
	return MyArray;
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
TInt CMyTepesView::GetTypesArrayL(void)
{
	iArray.ResetAndDestroy();

	RFs FsSession;
	if(FsSession.Connect() == KErrNone)
	{
		TFindFile PrivFolder(FsSession);
		if(KErrNone == PrivFolder.FindByDir(KFilleDbFileName, KNullDesC))// finds the drive
		{
			TEntry anEntry;
			if(KErrNone == FsSession.Entry(PrivFolder.File(),anEntry))
			{
				if(anEntry.iSize < 10)
				{
					FsSession.Delete(PrivFolder.File());
									
					RDbNamedDatabase Database;
					User::LeaveIfError(Database.Create(FsSession,PrivFolder.File()));     
					CreateTableL(Database);
				}
			}
		
			RDbNamedDatabase Database;
			if(KErrNone == Database.Open(FsSession,PrivFolder.File()))
			{
				TFileName QueryBuffer;
				QueryBuffer.Copy(_L("SELECT * FROM "));// just get all columns & rows
				QueryBuffer.Append(KtxtItemlist);
				
				RDbView Myview;
				Myview.Prepare(Database,TDbQuery(QueryBuffer));
				CleanupClosePushL(Myview);
				Myview.EvaluateAll();
				Myview.FirstL();
				
				while(Myview.AtRow()) // Just delete one instance of the message           
				{	
					Myview.GetL();		
					
					if(Myview.ColDes8(2).Length())
					{
						if(Myview.ColDes8(3).Length()
						|| Myview.ColDes(4).Length())
						{
							CMyRecItem* NewIttem = new(ELeave)CMyRecItem();
							iArray.Append(NewIttem);
							
							NewIttem->iIndex = Myview.ColInt(1);
							NewIttem->iType= Myview.ColDes8(2).AllocL();
							
							if(Myview.ColDes8(3).Length())
							{
								NewIttem->iData= Myview.ColDes8(3).AllocL();
							}
							
							if(Myview.ColDes(4).Length())
							{
								NewIttem->iExt = Myview.ColDes(4).AllocL();
							}
						}
					}
						
					Myview.NextL();
				} 
				
				CleanupStack::PopAndDestroy(1); // Myview
				
				Database.Close();
			}
		}
		else
		{
			TFindFile AppFile(CCoeEnv::Static()->FsSession());
			if(KErrNone == AppFile.FindByDir(KFilleResourceFileName, KNullDesC))
			{
				TParsePtrC Hjelppp(AppFile.File());
				TFileName DBFileName;
				
				DBFileName.Copy(Hjelppp.Drive());
				DBFileName.Append(KFilleDbFileName);
				
				BaflUtils::EnsurePathExistsL(FsSession,DBFileName);
				
				RDbNamedDatabase Database;
				User::LeaveIfError(Database.Create(FsSession,DBFileName));     
				CreateTableL(Database);
			}
		}
	}
	
	FsSession.Close();
	
	return iArray.Count();
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMyTepesView::SaveToDatabaseL(const TDesC8& aType,const TDesC8& aData,const TDesC& aExt,TInt& aIndex)
{	
	TFindFile PrivFolder(CCoeEnv::Static()->FsSession());
	if(KErrNone == PrivFolder.FindByDir(KFilleDbFileName, KNullDesC))// finds the drive
	{
		RDbNamedDatabase Database;
		if(KErrNone == Database.Open(CCoeEnv::Static()->FsSession(),PrivFolder.File()))
		{
			TFileName QueryBuffer;
			QueryBuffer.Copy(_L("SELECT * FROM "));
			QueryBuffer.Append(KtxtItemlist);

			Database.Begin();

			RDbView Myview;
			Myview.Prepare(Database,TDbQuery(QueryBuffer));
			CleanupClosePushL(Myview);

			Myview.InsertL(); 
				
			Myview.SetColL(2,aType);		
			Myview.SetColL(3,aData);
			Myview.SetColL(4,aExt);

			Myview.PutL();  
			
			aIndex = Myview.ColInt(1);// autoincrement gives us unique index.
					
			CleanupStack::PopAndDestroy(1); // Myview
			Database.Commit();
			Database.Close();
		}
	}
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMyTepesView::UpdateDatabaseL(const TDesC8& aType,const TDesC8& aData,const TDesC& aExt,TInt& aIndex)
{
	TFindFile PrivFolder(CCoeEnv::Static()->FsSession());
	if(KErrNone == PrivFolder.FindByDir(KFilleDbFileName, KNullDesC))// finds the drive
	{
		RDbNamedDatabase Database;
		if(KErrNone == Database.Open(CCoeEnv::Static()->FsSession(),PrivFolder.File()))
		{	
			TFileName QueryBuffer;
			QueryBuffer.Copy(_L("SELECT * FROM "));
			QueryBuffer.Append(KtxtItemlist);
			QueryBuffer.Append(_L(" WHERE "));
			QueryBuffer.Append(NCol0);
			QueryBuffer.Append(_L(" = "));
			QueryBuffer.AppendNum(aIndex);
			
			Database.Begin();
			
			RDbView Myview;
			Myview.Prepare(Database,TDbQuery(QueryBuffer));
			CleanupClosePushL(Myview);
			
			Myview.EvaluateAll();
			Myview.FirstL();
			
			if(Myview.AtRow())            
			{			
				Myview.UpdateL();
				Myview.SetColL(2,aType);		
				Myview.SetColL(3,aData);
				Myview.SetColL(4,aExt);	
				Myview.PutL();
			}
					
			CleanupStack::PopAndDestroy(1); // Myview
			Database.Commit();
			Database.Close();
		}
	}
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMyTepesView::DeleteFromDatabaseL(TInt& aIndex)
{	
	TFindFile PrivFolder(CCoeEnv::Static()->FsSession());
	if(KErrNone == PrivFolder.FindByDir(KFilleDbFileName, KNullDesC))// finds the drive
	{
		RDbNamedDatabase Database;
		if(KErrNone == Database.Open(CCoeEnv::Static()->FsSession(),PrivFolder.File()))
		{
			TFileName QueryBuffer;
			QueryBuffer.Copy(_L("SELECT * FROM "));
			QueryBuffer.Append(KtxtItemlist);
			QueryBuffer.Append(_L(" WHERE "));
			QueryBuffer.Append(NCol0);
			QueryBuffer.Append(_L(" = "));
			QueryBuffer.AppendNum(aIndex);
				
			Database.Begin();
			
			RDbView Myview;
			// query buffr with index finds only the selected item row.
			Myview.Prepare(Database,TDbQuery(QueryBuffer));
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
			Database.Commit();
			// compacts the databse, by physicaly removig deleted data.
			Database.Compact();
			Database.Close();
		}
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyTepesView::CreateTableL(RDbDatabase& aDatabase) 
	{
	// Create a table definition
	CDbColSet* columns=CDbColSet::NewLC();
	
	// Add Columns
	TDbCol id(NCol0,EDbColInt32);
	id.iAttributes=id.EAutoIncrement;// automatic indexing for items,it is our key field.
	columns->AddL(id);				 
	
	columns->AddL(TDbCol(NCol1, EDbColText8,200));
	columns->AddL(TDbCol(NCol2, EDbColText8,200));
	columns->AddL(TDbCol(NCol3, EDbColText,200));
	
	// Create a table
	User::LeaveIfError(aDatabase.CreateTable(KtxtItemlist,*columns));
				
	// cleanup the column set
	CleanupStack::PopAndDestroy();
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMyTepesView::UpdateScrollBar(CEikListBox* aListBox)
    {
    if (aListBox)
        {   
        TInt pos(aListBox->View()->CurrentItemIndex());
        if (aListBox->ScrollBarFrame())
            {
            aListBox->ScrollBarFrame()->MoveVertThumbTo(pos);
            }
        }
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyTepesView::SizeChanged()
{	
	delete iImeiSettings;
	iImeiSettings = NULL;

	MakeListBoxL();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CMyTepesView::CMyTepesView::GetSelectedIndexL(void)
{
	TInt Ret(-1);

	if(iListBox)
	{
		TInt CurrItemInd = iListBox->CurrentItemIndex();
		
		CAknFilteredTextListBoxModel* model = STATIC_CAST(CAknFilteredTextListBoxModel*,iListBox->Model());		
		
		if(model && CurrItemInd >= 0)
		{	
			Ret = model->Filter()->FilteredItemIndex(CurrItemInd);
		}
	}
	
	return Ret;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyTepesView::HandleViewCommandL(TInt aCommand)
    {
   switch(aCommand)
        {
        case ESettingsOk:
        	if(iImeiSettings)
        	{	
        		TInt Indde(-1);
        		TBuf8<200> Typp,Datt;
				TBuf<200> Enttt;
						
        		iImeiSettings->GetDataL(Typp,Datt,Enttt,Indde);
        		
        		if(Indde >=0)
        		{
        			UpdateDatabaseL(Typp,Datt,Enttt,Indde);	
        		}
        		else
        		{
	        		SaveToDatabaseL(Typp,Datt,Enttt,Indde);	
	        		
	        		TBuf8<255> TypHjelp;
	        		TypHjelp.Copy(Typp);
	        		
	        		TFileMappingItem SetMe;
					SetMe.iDataType  = TDataType(TypHjelp);
					SetMe.iUid.iUid = (TInt32)0xA000312E;
					SetMe.iPriority = KDataTypePriorityHigh;
					
					RMainServerClient MainServerClient;
					MainServerClient.Connect();
					MainServerClient.SetFileMappingL(SetMe);
					MainServerClient.Close();
	        		
        		}
        	}
		case ESettingsBack:
			{
				delete iImeiSettings;
				iImeiSettings = NULL;
			}
			SetMenuL();
			MakeListBoxL();
			break;
        case EMyTypeNew:
			{
				delete iImeiSettings;
				iImeiSettings = NULL;
				iImeiSettings = new(ELeave)CImeiSettings();
				iImeiSettings->ConstructL(KtxYFileType,KNullDesC8,KNullDesC,-1);
				
			}
			SetMenuL();
			DrawNow();
			break;
		case EMyTypeMod:
			{
				TInt Indd = GetSelectedIndexL();
				if(Indd >= 0 && Indd < iArray.Count())
				{
					if(iArray[Indd])
					{
						TBuf8<200> Typp,Datt;
						TBuf<200> Enttt;
						
						if(iArray[Indd]->iType)
						{
							Typp.Copy(*iArray[Indd]->iType);
						}
						
						if(iArray[Indd]->iExt)
						{
							Enttt.Copy(*iArray[Indd]->iExt);
						}
						
						if(iArray[Indd]->iData)
						{
							Datt.Copy(*iArray[Indd]->iData);
						}
						
						delete iImeiSettings;
						iImeiSettings = NULL;
						iImeiSettings = new(ELeave)CImeiSettings();
						iImeiSettings->ConstructL(Typp,Datt,Enttt,iArray[Indd]->iIndex);
					}
				}	
			}
			SetMenuL();
			DrawNow();
			break;
		case EMyTypeDel:
			{
				TInt Indd = GetSelectedIndexL();
				if(Indd >= 0 && Indd < iArray.Count())
				{
					if(iArray[Indd])
					{
						DeleteFromDatabaseL(iArray[Indd]->iIndex);
					}
				}
			}
			MakeListBoxL();
			break;
        default:
            break;
        }
    }

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TKeyResponse CMyTepesView::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aEventCode)
{
	TKeyResponse Ret = EKeyWasNotConsumed;	

	if(iImeiSettings)
	{
		Ret = iImeiSettings->OfferKeyEventL(aKeyEvent,aEventCode);
	}
	else
	{
		switch (aKeyEvent.iCode)
	    {
	    case EKeyDevice3:
			CEikonEnv::Static()->AppUiFactory()->MenuBar()->TryDisplayMenuBarL();
			Ret = EKeyWasConsumed;
			break;
		case '2':
			if(iListBox)
			{
				iListBox->View()->MoveCursorL(CListBoxView::ECursorPreviousPage,CListBoxView::ENoSelection);
				iListBox->UpdateScrollBarsL();
				iListBox->DrawNow();
				Ret = EKeyWasConsumed;
			}
			DrawNow();
			break;
		case '8':
			if(iListBox)
			{
				iListBox->View()->MoveCursorL(CListBoxView::ECursorNextPage,CListBoxView::ENoSelection);
				iListBox->UpdateScrollBarsL();
				iListBox->DrawNow();
				Ret = EKeyWasConsumed;
			}
			DrawNow();
			break;
		default:
			if(iListBox)
			{        
				Ret = iListBox->OfferKeyEventL(aKeyEvent,aEventCode);
			}
			break;
		}
	}
	
	return Ret;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
// Draw this application's view to the screen
void CMyTepesView::Draw(const TRect& /*aRect*/) const
{

}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CCoeControl* CMyTepesView::ComponentControl( TInt /*aIndex*/) const
{
	if(iImeiSettings)
	{
		return iImeiSettings;
	}
	else
	{
		return iListBox;
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CMyTepesView::CountComponentControls() const
{
	if(iImeiSettings)
	{
		return 1;
	}
	else if(iListBox)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyTepesView::SetMenuL(void)
{
	CEikonEnv::Static()->AppUiFactory()->MenuBar()->StopDisplayingMenuBar();		
	
	if(iImeiSettings)
	{
		iCba.SetCommandSetL(R_MYSET_CBA);
		iCba.DrawDeferred();
	}
	else
	{
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(R_MYTYPE_MENUBAR);

		iCba.SetCommandSetL(R_MYTYPE_CBA);
		iCba.DrawDeferred();
	}
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyTepesView::InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane)
{	
	if(R_MYTYPE_MENU == aResourceId)
	{
		TBool DimSel(ETrue);
		if(iListBox)
		{
			if(GetSelectedIndexL() >= 0)
			{
				DimSel= EFalse;
			}
		}
		
		if(DimSel)
		{
			aMenuPane->SetItemDimmed(EMyTypeMod,ETrue);
			aMenuPane->SetItemDimmed(EMyTypeDel,ETrue);
		}
	}
}


