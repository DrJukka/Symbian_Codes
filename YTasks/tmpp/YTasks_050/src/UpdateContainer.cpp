
#include "UpdateContainer.h"
#include "YTools_A000257B.hrh" 
#include <YTools_A000257B.rsg>

#include <AknIconArray.h>
#include <eikclbd.h>
#include <AknGlobalNote.h>
#include <AknQueryDialog.h>
#include <EIKPROGI.H>
_LIT(KtxUpdateDataNow			,"Update list");

_LIT(KtxDownloadFile			,"C:\\Jukka.txt");
_LIT(KtxDrJukkaUrl1				,"http://www.drjukka.com/Me_Echo.php");
_LIT(KtxDrJukkaUrl2				,"http://www.drjukka.com/Me_Echo2.php");

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
CUpdateContainer::CUpdateContainer(CEikButtonGroupContainer* aCba,CInfoGetter& aInfoGetter)
:iCba(aCba),iInfoGetter(aInfoGetter)
{		

}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
CUpdateContainer::~CUpdateContainer()
{  	
	if(iProgressDialog)
	{
		iProgressDialog->ProcessFinishedL(); 
	}

	iProgressDialog = NULL;
	iProgressInfo= NULL;
	
	delete iEngine;
	iEngine = NULL;
	
    delete iMyListBox;
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CUpdateContainer::ConstructL()
{
	CreateWindowL();	
	SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
		
	ActivateL();
	SetMenuL();
	DrawNow();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CUpdateContainer::ShowNoteL(const TDesC& aMessage)
{
	CAknGlobalNote* dialog = CAknGlobalNote::NewLC();
	TInt NoteId = dialog->ShowNoteL(EAknGlobalInformationNote,aMessage);
	User::After(2000000);
	dialog->CancelNoteL(NoteId);
	CleanupStack::PopAndDestroy(dialog);;
}
 /*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CUpdateContainer::DialogDismissedL (TInt aButtonId)
{		  	
	iProgressDialog = NULL;
	iProgressInfo = NULL;
	
	if(iEngine && aButtonId < 0)
	{
		iEngine->CancelTransaction();
	}
} 
/*
------------------------------------------------------------------------
------------------------------------------------------------------------
*/
TBool CUpdateContainer::GetFeedL(const TDesC& aUrl,const TDesC& aFileName, TBool aGet)
{
	TBool Ret(ETrue);
		
	iProgressDialog = new (ELeave) CAknProgressDialog((REINTERPRET_CAST(CEikDialog**,&iProgressDialog)),EFalse);
	iProgressDialog->PrepareLC(R_PROGRESS_NOTE);
	iProgressInfo = iProgressDialog->GetProgressInfoL();
	iProgressDialog->SetCallback(this);
	iProgressDialog->RunLD();
	iProgressInfo->SetFinalValue(5);
		
	if(!iEngine)
  	{
  		iEngine = CHTTPEngine::NewL(*this);
  	}
				
	TBuf8<255> Urlhjelp;	
	
	if(aUrl.Length() > 255)
		Urlhjelp.Copy(aUrl.Left(255));
	else
		Urlhjelp.Copy(aUrl);
	
	HBufC8* BodyBuf(NULL);
	
/*	if(aGet)
	{
*/		BodyBuf = iInfoGetter.InfoBufferLC(_L8("jukkas testing time password string"),ETrue);
/*	}
	else
	{
		BodyBuf = iInfoGetter.InfoBufferLC(_L8(""),ETrue);
	}
*/	
/*	if(aGet)
	{
		iEngine->IssueHTTPGetL(Urlhjelp,*BodyBuf,aFileName, ETrue);		
	}
	else
*/	{
		iEngine->IssueHTTPPostL(Urlhjelp,*BodyBuf,aFileName,CHTTPEngine::EMultiPart, ETrue);
	}
	
 	CleanupStack::PopAndDestroy(BodyBuf);
 	
	return Ret;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CUpdateContainer::ClientEvent(const TDesC& aEventDescription,TInt /*aError*/)
{
	if(iProgressDialog)
	{
		iProgressDialog->SetTextL(aEventDescription);
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CUpdateContainer::ClientEvent(const TDesC& aEventDescription,TInt aProgress,TInt aFinal)
{
	ClientEvent(aEventDescription,KErrNone);
	
	if(iProgressInfo && aFinal > 0)
	{
		iProgressInfo->SetFinalValue(aFinal);
		iProgressInfo->SetAndDraw(aProgress);
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CUpdateContainer::ClientCancelled(const TDesC& /*aFileName*/,TInt aError)
{
	if(iProgressDialog)
	{
		iProgressDialog->ProcessFinishedL(); 
	}

	iProgressDialog = NULL;
	iProgressInfo= NULL;
	

	TBuf<100> ErrorMsg(_L("Cancelled"));
	if(aError == 7373)
	{
		ErrorMsg.Copy(_L("Invalid URL "));	
	}
	else if(aError != KErrCancel)
	{
		ErrorMsg.Copy(_L("Download failed with error: "));
		ErrorMsg.AppendNum(aError);
	}

	ShowNoteL(ErrorMsg);
	
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CUpdateContainer::ClientBodyReceived(const TDesC& aFileName)
{
	TFileName SaveName(aFileName);
	
	if(iProgressDialog)
	{
		iProgressDialog->ProcessFinishedL(); 
	}

	iProgressDialog = NULL;
	iProgressInfo= NULL;
	
	ShowNoteL(_L("Got stufff"));

}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CUpdateContainer::SizeChanged()
{
	MakeListBoxL();	
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CUpdateContainer::MakeListBoxL()
{
	TInt MySetIndex(0);
	
	if(iMyListBox)
	{
		MySetIndex = iMyListBox->CurrentItemIndex();
	}
    
	delete iMyListBox;
	iMyListBox = NULL;

    iMyListBox   = new( ELeave ) CAknSingleLargeStyleListBox();
	iMyListBox->ConstructL(this,EAknListBoxSelectionList);

	CArrayPtr<CGulIcon>* icons =new( ELeave ) CAknIconArray(1);
	CleanupStack::PushL(icons);
	
	iMyListBox->Model()->SetItemTextArray(GetArrayL(icons));
   	iMyListBox->Model()->SetOwnershipType(ELbmOwnsItemArray);
    
    CleanupStack::Pop(icons);
	iMyListBox->ItemDrawer()->ColumnData()->SetIconArray(icons);
	
	iMyListBox->CreateScrollBarFrameL( ETrue );
    iMyListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
	iMyListBox->SetRect(Rect());
	
	iMyListBox->View()->SetListEmptyTextL(KtxNoData);

	iMyListBox->ActivateL();

	TInt ItemsCount = iMyListBox->Model()->ItemTextArray()->MdcaCount();
	
	if(ItemsCount > MySetIndex && MySetIndex >= 0)
		iMyListBox->SetCurrentItemIndex(MySetIndex);
	else if(ItemsCount > 0)
		iMyListBox->SetCurrentItemIndex(0);
		
	UpdateScrollBar(iMyListBox);
	iMyListBox->DrawNow();
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
CDesCArray* CUpdateContainer::GetArrayL(CArrayPtr<CGulIcon>* aIcon)
{	
	CDesCArrayFlat* MyArray = new(ELeave)CDesCArrayFlat(1);
	CleanupStack::PushL(MyArray);
	
	// Append Text string to MyArray in here as "1\tMytext", where 1 is the zero based icon index
	// also remember to load and add icons in here... listbox will automatically re-size icons
	// thus no need to set any size for them.
	
	CleanupStack::Pop(MyArray);
	return MyArray;
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CUpdateContainer::UpdateScrollBar(CEikListBox* aListBox)
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
----------------------------------------------------------------------------
*/
void CUpdateContainer::Draw(const TRect& /*aRect*/) const
{
	CWindowGc& gc = SystemGc();
	gc.Clear(Rect());
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

void CUpdateContainer::HandleViewCommandL(TInt aCommand)
{
	switch(aCommand)
	{
	case EUpdateList1:
		{
			GetFeedL(KtxDrJukkaUrl1,KtxDownloadFile, EFalse);
		}
		break;
	case EUpdateList2:
		{
			GetFeedL(KtxDrJukkaUrl2,KtxDownloadFile, ETrue);
		}
		break;
	default:

		break;
	}	
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CUpdateContainer::SetMenuL(void)
{
	CEikonEnv::Static()->AppUiFactory()->MenuBar()->StopDisplayingMenuBar();

	{
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(R_UPDATE_MENUBAR);
		
		if(iCba)
		{
			iCba->SetCommandSetL(R_UPDATE_CBA);
			iCba->DrawDeferred();
		}
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CUpdateContainer::InitMenuPanelL(TInt aResourceId, CEikMenuPane* /*aMenuPane*/)
{
	if(aResourceId == R_UPDATE_MENU)
	{
/*		TBool Dim(ETrue);
	
		if(iProfileBox)
		{
			TInt CurrInd = iProfileBox->CurrentItemIndex();
			if(CurrInd >= 0)
			{
				Dim = EFalse;
			}
		}
		
		if(Dim)
		{
		//	aMenuPane->SetItemDimmed(EOpenNewsShow,ETrue);
		}*/
	}
	
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TKeyResponse CUpdateContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aEventCode)
{
	TKeyResponse Ret = EKeyWasNotConsumed;	
	
	if(iMyListBox)
	{
		Ret = iMyListBox->OfferKeyEventL(aKeyEvent,aEventCode);
	}
	
	return Ret;
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
CCoeControl* CUpdateContainer::ComponentControl( TInt /*aIndex*/) const
{
	return iMyListBox;
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TInt CUpdateContainer::CountComponentControls() const
{	
	if(iMyListBox)
		return 1;
	else
		return 0;
}
