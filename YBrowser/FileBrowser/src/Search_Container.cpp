/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen
	All rights reserved 
*/
#include <coemain.h>
#include <aknnotewrappers.h> 
#include <aknutils.h>
#include <EIKPROGI.H>
#include <AknIconArray.h>
#include <eikapp.h>
#include <eikclbd.h>
#include <EIKLBV.H>
#include <eikmenup.h>
#include <eikmenub.h>
#include <APGCLI.H>
#include <f32file.h>
#include <AknGlobalNote.h>
#include <S32FILE.H>
#include <EIKCLB.H>
#include <stringloader.h> 
#include "Folder_reader.h"// for CFileItem
#include "Navipanel.h"
#include "Search_Container.h"

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

#include "ShowString.h"
#include "IconHandler.h"
#include "Public_Interfaces.h"



const TInt KAknExListFindBoxTextLength = 20;

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CSearchContainer* CSearchContainer::NewL(const TRect& aRect,CEikButtonGroupContainer* aCba,CSearchValues& aValues,CIconHandler& aIconHandler,const TDesC& aPath,CYuccaNavi* aYuccaNavi)
    {
    CSearchContainer* self = CSearchContainer::NewLC(aRect,aCba,aValues,aIconHandler,aPath,aYuccaNavi);
    CleanupStack::Pop(self);
    return self;
    }
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CSearchContainer* CSearchContainer::NewLC(const TRect& aRect,CEikButtonGroupContainer* aCba,CSearchValues& aValues,CIconHandler& aIconHandler,const TDesC& aPath,CYuccaNavi* aYuccaNavi)
    {
    CSearchContainer* self = new (ELeave) CSearchContainer(aCba,aValues,aIconHandler,aYuccaNavi);
    CleanupStack::PushL(self);
    self->ConstructL(aRect,aPath);
    return self;
    }
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CSearchContainer::CSearchContainer(CEikButtonGroupContainer* aCba,CSearchValues& aValues,CIconHandler& aIconHandler,CYuccaNavi* aYuccaNavi)
:iCba(aCba),iIconHandler(aIconHandler),iSearchValues(aValues),iYuccaNavi(aYuccaNavi)
    {
	
    }
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CSearchContainer::~CSearchContainer()
{
	if(iProgressDialog)
	{
		iProgressDialog->ProcessFinishedL(); 
	}

	iProgressDialog = NULL;
		
	delete iFileSearcher;
	iFileSearcher = NULL;
	
	delete iFindBox;
	iFindBox = NULL;
	
	delete iSelectionBox;
	iSelectionBox = NULL;
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CSearchContainer::ConstructL(const TRect& aRect,const TDesC& aPath)
{
    CreateWindowL();
    SetRect(aRect);
    ActivateL();

	if(iSearchValues.iItemArray.Count())
	{
		AddItemsToListL();
	}
	else
	{	
		iSearchTime.HomeTime();
		iFileSearcher = new(ELeave)CFileSearcher(*this,iSearchValues,iIconHandler,iIconHandler);
		iFileSearcher->ConstructL();
		iFileSearcher->StartTheSearchL(aPath);


		iProgressDialog = new (ELeave) CAknWaitDialog((REINTERPRET_CAST(CEikDialog**,&iProgressDialog)), ETrue);
		iProgressDialog->PrepareLC(R_WAIT_NOTE_SOFTKEY_CANCEL);
		iProgressDialog->SetCallback(this);
		iProgressDialog->RunLD();
	}
	SetMenuL();
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CSearchContainer::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    TPoint pointter(aPointerEvent.iPosition);
    TPoint relll(PositionRelativeToScreen());
            
    pointter.iY = (pointter.iY - relll.iY);
    pointter.iX = (pointter.iX - relll.iX);
    
    TBool forward(ETrue);
    
    switch (aPointerEvent.iType)
    {
        case TPointerEvent::EButton1Down:
            iStartPoint = pointter;
            break;
        case TPointerEvent::EButton1Up:
            if (iDragEvent)
            {
                HandleViewCommandL(EFolderViewMarkTogle);
                forward = EFalse;
            }
            iDragEvent = EFalse;
            break;
        case TPointerEvent::EDrag:
            {
                TInt heighP = (Rect().Height() / 10);
                TInt widthP = (Rect().Width() / 4);
                
                TInt huhuu(iStartPoint.iY - pointter.iY );
                
                if(huhuu < 0){
                    huhuu = (huhuu * -1);
                }
                
                if(heighP > huhuu){
                
                    TInt stufff(iStartPoint.iX - pointter.iX );
                
                    if(stufff < 0){
                        stufff = (stufff * -1);
                    }
                    
                    if(widthP < stufff){
                        iDragEvent = ETrue;
                    }else{
                        iDragEvent = EFalse;
                    }
                }else{
                    iDragEvent = EFalse;
                }
            }
            break;
        default:
            break;
    }
    
    if(forward){
        CCoeControl::HandlePointerEventL(aPointerEvent);
    }
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CSearchContainer::FileSearchProcessL(TInt aReadCount, const TDesC& aFolderName)
{
	if(iProgressDialog)
	{
		TBuf<15> HjelpName;
		TBuf<150> ProgressText1;
		
		if(aFolderName.Length() > 15)
		{
			HjelpName.Copy(aFolderName.Right(15));
		}
		else
		{
			HjelpName.Copy(aFolderName);
		}
		
		TBuf<60> Hjelpper;
		StringLoader::Load(Hjelpper,R_SH_STR_FILESFOUND);
		
		ProgressText1.Copy(HjelpName);
		ProgressText1.Append(_L("\n"));
		ProgressText1.AppendNum(aReadCount);
		ProgressText1.Append(Hjelpper);
		
		
		iProgressDialog->SetTextL(ProgressText1);
	}	
}
/*
-----------------------------------------------------------------------------

----------------------------------------------------------------------------
*/
TInt CSearchContainer::AddItemsToListL(void)
{
	TInt FileCount(0);
	
	if(iSelectionBox)
	{
		CDesCArray* itemArray = STATIC_CAST( CDesCArray* ,iSelectionBox->Model()->ItemTextArray());
				
		for(TInt i=(iSearchValues.iItemArray.Count() - 1); i >=0 ; i--)
		{
			if(iSearchValues.iItemArray[i])
			{
				if(iSearchValues.iItemArray[i]->iPath 
				&& iSearchValues.iItemArray[i]->iName)
				{
				
				}
				else
				{
					iSearchValues.iItemArray.Remove(i);
				}
			}
			else
			{
				iSearchValues.iItemArray.Remove(i);
			}
		}
		
		for(TInt ii= 0; ii < iSearchValues.iItemArray.Count(); ii++)
		{
			if(iSearchValues.iItemArray[ii])
			{
				if(iSearchValues.iItemArray[ii]->iPath 
				&& iSearchValues.iItemArray[ii]->iName)
				{
					AddFoundItemL(*iSearchValues.iItemArray[ii],*itemArray);
				}
			}
		}
		
		iSelectionBox->HandleItemAdditionL();
		
		FileCount = itemArray->Count();
		if(FileCount)
		{
			iSelectionBox->SetCurrentItemIndex(0);
		}
		
		UpdateScrollBar(iSelectionBox);
	}
	
	ReFreshNaviL();
	return FileCount;
}
/*
-----------------------------------------------------------------------------

----------------------------------------------------------------------------
*/
void CSearchContainer::FileSearchFinnishL(CFileSearcher* /*aObject*/)
{
	TInt FileCount = AddItemsToListL();
	
	if(iProgressDialog)
	{
		iProgressDialog->ProcessFinishedL(); 
	}

	iProgressDialog = NULL;

	TTime NowTime;
	NowTime.HomeTime();
	
	TTimeIntervalSeconds aInterval(0);
	NowTime.SecondsFrom(iSearchTime,aInterval);
	
	TBuf<150> TimeBuffer,Hjelpper;
	StringLoader::Load(Hjelpper,R_SH_STR_FOUNDENDDDD);
	
	TimeBuffer.Format(Hjelpper,FileCount,aInterval.Int());
		
	iIconHandler.ShowNoteL(TimeBuffer, EFalse);
	
	DrawNow();
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CSearchContainer::AddFoundItemL(CSFileItem& aItem,CDesCArray& aArray)
{
	TFileName Hjelpper,NameHjelp;
	
	NameHjelp.Copy(aItem.iName->Des());
	
	if(iExtendedInfoOn)
	{				
		TBuf<30> DateHelp;
		GetTimeBuffer(aItem.iModified.DateTime(),DateHelp,ETrue);
						
		if(aItem.iType == CFFileItem::EFFolderFile)
		{					
			Hjelpper.Format(KtxLongListFormatFolder,CFFileItem::EFFolderFile,&NameHjelp,&DateHelp,aItem.iFiles,aItem.iFolders);
		}
		else if(aItem.iType >= CFFileItem::EFJustFile)
		{							
			if(aItem.iSize > (1024* 1024 * 10))
			{
				
				Hjelpper.Format(KtxLongListFormatFileMb,aItem.iType,&NameHjelp,&DateHelp,(aItem.iSize / (1024* 1024)));
			}
			else if(aItem.iSize > (1024* 10))
			{
				Hjelpper.Format(KtxLongListFormatFileKb,aItem.iType,&NameHjelp,&DateHelp,(aItem.iSize / 1024));
			}
			else
			{
				Hjelpper.Format(KtxLongListFormatFileB,aItem.iType,&NameHjelp,&DateHelp,aItem.iSize);
			}
		}
		else //if(aItem.iType == CFFileItem::EFDriveFile)
		{		
			Hjelpper.Zero();
		}
	}
	else
	{
		Hjelpper.Format(KtxShortListFormat,aItem.iType,&NameHjelp);
	}				

	if(Hjelpper.Length())
	{
		aArray.AppendL(Hjelpper);
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSearchContainer::GetTimeBuffer(const TDateTime& aDateTime,TDes& aBuffer,TBool aDate)
{
	TBuf<2> HourHjlep,MinHjelp,SecHjelp;

	if(aDate)
	{
		if((aDateTime.Day() + 1) < 10)
		{
			HourHjlep.Num(0);
			HourHjlep.AppendNum((aDateTime.Day() + 1));
		}
		else
		{
			HourHjlep.Num((aDateTime.Day() + 1));
		}
		
		if((aDateTime.Month() + 1) < 10)
		{
			MinHjelp.Num(0);
			MinHjelp.AppendNum((aDateTime.Month() + 1));
		}
		else
		{
			MinHjelp.Num((aDateTime.Month() + 1));
		}
	
		aBuffer.Format(KtxFormatInfoDateList,&HourHjlep,&MinHjelp,aDateTime.Year());		
	}
	else
	{
		if(aDateTime.Hour() < 10)
		{
			HourHjlep.Num(0);
			HourHjlep.AppendNum(aDateTime.Hour());
		}
		else
		{
			HourHjlep.Num(aDateTime.Hour());
		}
		
		if(aDateTime.Minute() < 10)
		{
			MinHjelp.Num(0);
			MinHjelp.AppendNum(aDateTime.Minute());
		}
		else
		{
			MinHjelp.Num(aDateTime.Minute());
		}
		
		if(aDateTime.Second() < 10)
		{
			SecHjelp.Num(0);
			SecHjelp.AppendNum(aDateTime.Second());
		}
		else
		{
			SecHjelp.Num(aDateTime.Second());
		}
	

		aBuffer.Format(KtxFormatInfoTimeList,&HourHjlep,&MinHjelp,&SecHjelp);
	}
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CSearchContainer::DialogDismissedL (TInt /*aButtonId*/)
{		  		
	if(iFileSearcher)
	{
		iFileSearcher->Cancel();
	}
	iProgressDialog = NULL;
}
/*
-----------------------------------------------------------------------
CEikFormattedCellListBox 
-----------------------------------------------------------------------
*/
void CSearchContainer::DisplayListBoxL(void)
{	
	delete iFindBox;
	iFindBox = NULL;
	
	delete iSelectionBox;
	iSelectionBox = NULL;

	if(iExtendedInfoOn)
	{
	    iSelectionBox   = new( ELeave ) CAknDoubleGraphicStyleListBox();//CAknSingleGraphicStyleListBox();//CAknDoubleNumberStyleListBox();
	}
	else
	{
	    iSelectionBox   = new( ELeave ) CAknSingleGraphicStyleListBox();//CAknSingleGraphicStyleListBox();//CAknDoubleNumberStyleListBox();		
	}
	
	iSelectionBox->ConstructL(this,EAknListBoxMarkableList);
	
	
	CDesCArrayFlat* ListArray = new(ELeave)CDesCArrayFlat(1);
	iSelectionBox->Model()->SetItemTextArray(ListArray);
    iSelectionBox->Model()->SetOwnershipType(ELbmOwnsItemArray);
	iSelectionBox->CreateScrollBarFrameL( ETrue );
    iSelectionBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
	iSelectionBox->SetRect(Rect());
		
	
	TBuf<100> Hejlpper;
    StringLoader::Load(Hejlpper,R_SH_STR_NOFILES);
	
	iSelectionBox->View()->SetListEmptyTextL(Hejlpper);
	
	if(iExtendedInfoOn)
	{
	    STATIC_CAST(CAknDoubleGraphicStyleListBox*,iSelectionBox)->ItemDrawer()->ColumnData()->SetIconArray(iIconHandler.GetIconArrayL());
		STATIC_CAST(CAknDoubleGraphicStyleListBox*,iSelectionBox)->ItemDrawer()->ColumnData()->EnableMarqueeL(ETrue);
	}
	else
	{
		STATIC_CAST(CAknSingleGraphicStyleListBox*,iSelectionBox)->ItemDrawer()->ColumnData()->SetIconArray(iIconHandler.GetIconArrayL());
		STATIC_CAST(CAknSingleGraphicStyleListBox*,iSelectionBox)->ItemDrawer()->ColumnData()->EnableMarqueeL(ETrue);
	}
	
	if(iSearchOn)
	{
		iFindBox = CreateFindBoxL(iSelectionBox, iSelectionBox->Model(),CAknSearchField::ESearch);
		SizeChanged();
	}	
	
	iSelectionBox->ActivateL();
	UpdateScrollBar(iSelectionBox);
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CAknSearchField* CSearchContainer::CreateFindBoxL(CEikListBox* aListBox,CTextListBoxModel* aModel, CAknSearchField::TSearchFieldStyle aStyle )
{
    CAknSearchField* findbox = NULL;

    if ( aListBox && aModel )
	{
        // Gets pointer of CAknFilteredTextListBoxModel.
        CAknFilteredTextListBoxModel* model =STATIC_CAST( CAknFilteredTextListBoxModel*, aModel );
        // Creates FindBox.
        findbox = CAknSearchField::NewL( *this,aStyle,NULL,KAknExListFindBoxTextLength);
        CleanupStack::PushL(findbox);
        // Creates CAknListBoxFilterItems class.
        model->CreateFilterL( aListBox, findbox );
        //Filter can get by model->Filter();
        CleanupStack::Pop(findbox); // findbox
 	}

    return findbox;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CSearchContainer::UpdateScrollBar(CEikListBox* aListBox)
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
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CSearchContainer::GetCurrSelFileL(TDes& aFileName,TInt& aFileType)
{
	TInt CurrSel = GetSelectedIndexL();
	
	GetFileL(aFileName,aFileType,CurrSel);
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CSearchContainer::GetFileL(TDes& aFileName,TInt& aFileType,TInt aSeleted)
{
	if(aSeleted >=0  && aSeleted < iSearchValues.iItemArray.Count())
	{
		if(iSearchValues.iItemArray[aSeleted])
		{
			if(iSearchValues.iItemArray[aSeleted]->iPath 
			&& iSearchValues.iItemArray[aSeleted]->iName)
			{
				aFileName.Copy(iSearchValues.iItemArray[aSeleted]->iPath->Des());
				aFileName.Append(iSearchValues.iItemArray[aSeleted]->iName->Des());
			
				aFileType = iSearchValues.iItemArray[aSeleted]->iType;
			}
		}
	}
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CSearchContainer::AddSelectedFilesL(CDesCArray* aArray,TBool AlsoFolders)
{
	if(iSelectionBox && aArray)
	{
		TFileName AddFileName;
		TBool NotSelection(ETrue);
		TInt FileType(CFFileItem::EFDriveFile);
		
		const CListBoxView::CSelectionIndexArray* SelStuff = iSelectionBox->SelectionIndexes();
		if(SelStuff)
		{
			if(SelStuff->Count())
			{
				NotSelection = EFalse;
				
				for(TInt i=0; i < SelStuff->Count(); i++)
				{
					AddFileName.Zero();
					FileType = CFFileItem::EFDriveFile;
					
					GetFileL(AddFileName,FileType,SelStuff->At(i));
					
					if(FileType > CFFileItem::EFDriveFile
					&& AddFileName.Length())
					{
						if(!AlsoFolders && FileType == CFFileItem::EFFolderFile)
						{
							// do not add folders then
						}
						else
						{
							aArray->AppendL(AddFileName);
						}
					}
				}
			}	
		}
		
		if(NotSelection)
		{
			AddFileName.Zero();
			FileType = CFFileItem::EFDriveFile;
					
			GetCurrSelFileL(AddFileName,FileType);
			
			if(FileType > CFFileItem::EFDriveFile
			&& AddFileName.Length())
			{
				if(! AlsoFolders && FileType == CFFileItem::EFFolderFile)
				{
					// do not add folders then
				}
				else
				{
					aArray->AppendL(AddFileName);
				}
			}
		}
	}
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CSearchContainer::PageUpDown(TBool aUp)
{	
	if(iSelectionBox)
	{
		if(aUp)  
			iSelectionBox->View()->MoveCursorL(CListBoxView::ECursorPrevScreen,CListBoxView::ENoSelection);
		else
			iSelectionBox->View()->MoveCursorL(CListBoxView::ECursorNextScreen,CListBoxView::ENoSelection);
	    
	    iSelectionBox->UpdateScrollBarsL();
		iSelectionBox->DrawNow();
	}
	
	ReFreshNaviL();
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/

void CSearchContainer::HandleViewCommandL(TInt aCommand)
    {
   switch(aCommand)
        {
        case EExtrasSearchOn:
        	if(iExtendedInfoOn)
        	{
        		break;
        	}
		case EExtrasSearchOff:
			{
				ChangeViewTypeL(ETrue);
			}
			break;
        case EFolderViewMarkTogle:
			if(iSelectionBox && !iFindBox)
			{
				//TInt CurrSel = GetSelectedIndex();
				TInt CurrSel = iSelectionBox->CurrentItemIndex();
				
				if(CurrSel >= 0)
				{	
					TInt FilTyp = -1;
					
					if(iFindBox)
					{
						CAknFilteredTextListBoxModel* model = STATIC_CAST(CAknFilteredTextListBoxModel*,iSelectionBox->Model());		
		
						if(model)
						{
							GetFileType(FilTyp, model->Filter()->FilteredItemIndex(CurrSel)); 
						}
					}
					else
						GetFileType(FilTyp, CurrSel); 
						
					if(FilTyp > CFFileItem::EFFolderFile)	
					{		
						iSelectionBox->View()->ToggleItemL(CurrSel);
					}
				}
			}
			DrawNow();
			break;
		case EFolderViewMarkInverse:
			if(iSelectionBox && !iFindBox)
			{
				TInt FilTyp(-1);
					
				CDesCArray* itemArray= STATIC_CAST( CDesCArray* ,iSelectionBox->Model()->ItemTextArray());
			
				if(itemArray)
				{
					for(TInt i=0; i < itemArray->Count(); i++)
					{
						FilTyp = -1;
						GetFileType(FilTyp, i); 	
						if(FilTyp > CFFileItem::EFFolderFile)	
						{
							iSelectionBox->View()->ToggleItemL(i);
						}
					}
				}
			}
			DrawNow();
			break;
		case EFolderViewMarkAll:
			if(iSelectionBox && !iFindBox)
			{
				TInt FilTyp(-1);
					
				CDesCArray* itemArray= STATIC_CAST( CDesCArray* ,iSelectionBox->Model()->ItemTextArray());
			
				if(itemArray)
				{
					for(TInt i=0; i < itemArray->Count(); i++)
					{
						FilTyp = -1;
						GetFileType(FilTyp, i); 	
						if(FilTyp > CFFileItem::EFFolderFile)	
						{
							iSelectionBox->View()->SelectItemL(i);
						}
					}
				}
			
			}
			DrawNow();
			break;
		case EFolderViewUnMarkAll: 
 			if(iSelectionBox)
			{
				iSelectionBox->ClearSelection();
			}
			DrawNow();
        default:
            break;
        }
    }

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CSearchContainer::SelectionWithWildL(const TDesC& aWild)
{
	TInt FilTyp(-1);
		
	CDesCArray* itemArray= STATIC_CAST( CDesCArray* ,iSelectionBox->Model()->ItemTextArray());

	if(itemArray)
	{
		TFileName Hjelpper2;
		
		for(TInt i=0; i < itemArray->Count(); i++)
		{
			FilTyp = -1;
			GetFileType(FilTyp, i); 	
			if(FilTyp > CFFileItem::EFFolderFile)	
			{
				Hjelpper2.Zero();
				if(AppendFileName(Hjelpper2,i)) 
				{
					Hjelpper2.CopyLC(Hjelpper2);
					if(KErrNotFound != Hjelpper2.MatchF(aWild))
					{
						iSelectionBox->View()->SelectItemL(i);
					}
				}
			}
		}
	}
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
TBool CSearchContainer::AppendFileName(TDes& aFileName,TInt aIndex) 
{
	TBool Ret(EFalse);
	
	if(iSelectionBox)
	{
		CDesCArray* itemArray = STATIC_CAST( CDesCArray* ,iSelectionBox->Model()->ItemTextArray());		
	
		if(itemArray)
		{
			TInt CurrSel(aIndex);
			
			if(CurrSel < 0)
			{
				CurrSel = iSelectionBox->CurrentItemIndex();
			}
			
			if(itemArray->Count() > CurrSel && CurrSel >= 0)
			{
				TPtrC ItName,Item;
				Item.Set(itemArray->MdcaPoint(CurrSel));
				if(KErrNone == TextUtils::ColumnText(ItName,1,&Item))
				{
					Ret = ETrue;
					aFileName.Append(ItName);
					
					TInt CurrTypeId(CFFileItem::EFFolderFile);
					GetFileType(CurrTypeId,CurrSel);
					if(CurrTypeId == CFFileItem::EFFolderFile
					|| CurrTypeId == CFFileItem::EFDriveFile)
					{
						aFileName.Append(KtxDoubleSlash);
					}
				}
			}
		}
	}
	
	return Ret;
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CSearchContainer::GetFileType(TInt& aTypeId, TInt aIndex) 
{
	aTypeId = -1;
	
	if(iSelectionBox)
	{
		CDesCArray* itemArray = STATIC_CAST( CDesCArray* ,iSelectionBox->Model()->ItemTextArray());
				
		if(itemArray)
		{
			TInt CurrSel(aIndex);
			
			if(aIndex < 0)
			{
				CurrSel = GetSelectedIndexL();
			}
				
			if(itemArray->Count() > CurrSel && CurrSel >= 0)
			{
				TPtrC IdNumber,Item;
				Item.Set(itemArray->MdcaPoint(CurrSel));
				if(KErrNone == TextUtils::ColumnText(IdNumber,0,&Item))
				{
					TLex16 MyLex(IdNumber);

					if(KErrNone != MyLex.Val(aTypeId))
					{
						aTypeId = -1;
					}
				}
			}
		}
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSearchContainer::ChangeViewTypeL(TBool aSearchChange)
{
	TInt LastItemIndex(-1);
	if(iSelectionBox)
	{
		LastItemIndex = GetSelectedIndexL();
	}

	CArrayFixFlat<TInt>* SelStuff = new(ELeave)CArrayFixFlat<TInt>(10);
	CleanupStack::PushL(SelStuff);
	
	if(iSelectionBox && !iFindBox)
	{
		const CArrayFix<TInt>* SelStuffTmp = iSelectionBox->SelectionIndexes();
		for(TInt i=0; i < SelStuffTmp->Count(); i++)
		{
			SelStuff->AppendL(SelStuffTmp->At(i));
		}
	}
	
	if(aSearchChange)
	{
		if(iSearchOn)
		{
			iSearchOn = EFalse;
		}
		else
		{
			iSearchOn = ETrue;
		}
	}
	else
	{
		if(iExtendedInfoOn)
		{
			iExtendedInfoOn = EFalse;
		}
		else
		{
			iExtendedInfoOn = ETrue;
		}
	}
	
	DisplayListBoxL();
	AddItemsToListL();
	SetMenuL();
	
	if(SelStuff && iSelectionBox && !iFindBox)
	{
		for(TInt i=0; i < SelStuff->Count(); i++)
		{
			TInt FilTyp(CFFileItem::EFFolderFile);
			GetFileType(FilTyp, SelStuff->At(i)); 	
			if(FilTyp > CFFileItem::EFFolderFile)	
			{
				iSelectionBox->View()->SelectItemL(SelStuff->At(i));
			}
		}
	}
	
	CleanupStack::PopAndDestroy(SelStuff);
	
	if(LastItemIndex > 0 && iSelectionBox)
	{
		CDesCArray* itemArray = STATIC_CAST( CDesCArray* ,iSelectionBox->Model()->ItemTextArray());
	
		if(itemArray && LastItemIndex >= 0)
		{
			if(itemArray->Count() > LastItemIndex)
			{
				iSelectionBox->SetCurrentItemIndex(LastItemIndex);
				UpdateScrollBar(iSelectionBox);
			}
		}
	}   
	
	DrawNow();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSearchContainer::SizeChanged()
{
	if (iSelectionBox)
    {
        if (iFindBox)
        {
       	 	SizeChangedForFindBox();       
        }
        else
        {
            iSelectionBox->SetRect(Rect()); // Sets rectangle of lstbox.
    	}
    }
    else
    {
		DisplayListBoxL();
    }
    
   DrawNow();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSearchContainer::SizeChangedForFindBox()
    {
    if ( iSelectionBox&& iFindBox )
        {
    
        CAknColumnListBox* aknListBox = STATIC_CAST(CAknColumnListBox*, iSelectionBox);
        AknFind::HandleFixedFindSizeChanged(this, aknListBox, iFindBox);
        }
    }

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
TKeyResponse CSearchContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	TKeyResponse Ret = EKeyWasNotConsumed;
	
	switch (aKeyEvent.iCode)
    {
    case EKeyDevice3:
		{	
			CEikonEnv::Static()->EikAppUi()->HandleCommandL(EVBOpenFile);
			break;
		}
		break;
    case '#':
    	if(!iSearchOn)
    	{
    		ChangeViewTypeL(EFalse);
    	}
    	break;
    case 48:
	case 49:
	case 50:
	case 51:
	case 52:
	case 53:
	case 54:
	case 55:
	case 56:
	case 57:
		break;
	case 8:// c- use for delete 
		if(!iSearchOn && aKeyEvent.iCode == 8)
		{
			HandleViewCommandL(EFileDelete);
			break;
		}
	case EKeyRightArrow:
		if(!iSearchOn)
		{
			HandleViewCommandL(EFolderViewMarkTogle);
			break;
		}	
	case EKeyDownArrow:
	case EKeyUpArrow:
	case EKeyLeftArrow:
	default:
		if(iSelectionBox)
		{
			if ( iFindBox )
            {
            TBool needRefresh( EFalse );
            
            // Offers the key event to find box.
            if ( AknFind::HandleFindOfferKeyEventL( aKeyEvent, aType, this,
                                                    iSelectionBox, iFindBox,
                                                    EFalse,
                                                    needRefresh ) == EKeyWasConsumed )
                {
                	if ( needRefresh )
                    {
                	    SizeChanged();
                    	DrawNow();
                    }
					
					ReFreshNaviL();
                	return EKeyWasConsumed;
                }
            }
            
			Ret = iSelectionBox->OfferKeyEventL(aKeyEvent,aType);
			ReFreshNaviL();
		}
		break;
	}
	
	return Ret;
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CSearchContainer::ReFreshNaviL(void)
{
	if(iYuccaNavi)
	{
		TInt CurrSel = GetSelectedIndexL();

		if(CurrSel >=0  && CurrSel < iSearchValues.iItemArray.Count())
		{
			if(iSearchValues.iItemArray[CurrSel])
			{
				if(iSearchValues.iItemArray[CurrSel]->iPath)
				{
					iYuccaNavi->SetTextL(*iSearchValues.iItemArray[CurrSel]->iPath);
				}
			}
		}
	}
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
TInt CSearchContainer::GetSelectedIndexL(void)
{
	TInt Ret(-1);

	if(iSelectionBox)
	{
		TInt CurrItemInd = iSelectionBox->CurrentItemIndex();
		
		if(iFindBox)
		{
			CAknFilteredTextListBoxModel* model = STATIC_CAST(CAknFilteredTextListBoxModel*,iSelectionBox->Model());		
		
			if(model && CurrItemInd >= 0)
			{	
				Ret = model->Filter()->FilteredItemIndex(CurrItemInd);
			}
		}
		else
		{
			Ret = CurrItemInd;
		}
	}
	
	return Ret;
}




/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSearchContainer::SetMenuL(void)
{	
	CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(R_SEARCH_B_MENUBAR);	
	if(iCba)
	{
		if(iSearchOn)
		{
			iCba->SetCommandSetL(R_MYOBSR_CBA);
		}
		else
		{					
			iCba->SetCommandSetL(R_SEARCH_B_CBA);				
		}
		
		iCba->DrawDeferred();
	}
}

/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CSearchContainer::InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane)
{
	TBool HasStuff(EFalse),HasSelection(EFalse);
	TInt CurrTypeId(CFFileItem::EFDriveFile);
			
	if(iSelectionBox)
	{	
		TInt SelCur = GetSelectedIndexL();
		if(SelCur >= 0)
		{
			HasStuff = ETrue;
		}
		
		if(iFindBox)
		{
			CAknFilteredTextListBoxModel* model = STATIC_CAST(CAknFilteredTextListBoxModel*,iSelectionBox->Model());		

			if(model)
			{
				GetFileType(CurrTypeId, SelCur);//model->Filter()->FilteredItemIndex(SelCur)); 
			}
		}
		else
		{
			GetFileType(CurrTypeId, SelCur); 
		}
		
		const CListBoxView::CSelectionIndexArray* SelStuff = iSelectionBox->SelectionIndexes();
		if(SelStuff)
		{
			if(SelStuff->Count())
			{
				HasSelection = ETrue;
			}
		}
	}

		
	if(R_SEARCH_B_MENU == aResourceId)
	{
		if (iFindBox)
		{
			aMenuPane->SetItemDimmed(EFolderViewMark,ETrue);
			aMenuPane->SetItemDimmed(EExtrasSearchOn,ETrue);
		}
		else
		{
			if(iExtendedInfoOn)
			{
				aMenuPane->SetItemDimmed(EExtrasSearchOn,ETrue);
			}
			
			aMenuPane->SetItemDimmed(EExtrasSearchOff,ETrue);
		}
			
		
		if(CurrTypeId < CFFileItem::EFFolderFile || !HasStuff)
		{
			aMenuPane->SetItemDimmed(EEditMenu,ETrue);
			aMenuPane->SetItemDimmed(EFileMenu,ETrue);
			aMenuPane->SetItemDimmed(EFolderViewMark,ETrue);
		}
	}
	else if(R_EDITB_MENU == aResourceId)
	{	
		if(!HasStuff
		|| CurrTypeId < CFFileItem::EFJustFile)
		{
			aMenuPane->SetItemDimmed(EEditCopy,ETrue);
			aMenuPane->SetItemDimmed(EEditCut,ETrue);
		}
	}
	else if(R_FILEB_MENU == aResourceId)
	{
		if (iFindBox)
		{
			aMenuPane->SetItemDimmed(EFileDelete,ETrue);
			aMenuPane->SetItemDimmed(EFileRename,ETrue);
		}
	
		if(CurrTypeId < CFFileItem::EFFolderFile)
		{
			if(!HasStuff)
			{
				aMenuPane->SetItemDimmed(EFileDetails,ETrue);
			}
			
			aMenuPane->SetItemDimmed(EFileAttributes,ETrue);
			aMenuPane->SetItemDimmed(ESendFiles,ETrue);
			aMenuPane->SetItemDimmed(EFileDelete,ETrue);
			aMenuPane->SetItemDimmed(EFileRename,ETrue);
			aMenuPane->SetItemDimmed(EFileShowName,ETrue);
			aMenuPane->SetItemDimmed(EOpenWithViewer,ETrue);
		}
		else if(!HasStuff)
		{
			aMenuPane->SetItemDimmed(EFileAttributes,ETrue);
			aMenuPane->SetItemDimmed(ESendFiles,ETrue);
			aMenuPane->SetItemDimmed(EFileDetails,ETrue);
			aMenuPane->SetItemDimmed(EFileDelete,ETrue);
			aMenuPane->SetItemDimmed(EFileRename,ETrue);
			aMenuPane->SetItemDimmed(EFileShowName,ETrue);
			aMenuPane->SetItemDimmed(EOpenWithViewer,ETrue);
		}
		else if(CurrTypeId == CFFileItem::EFFolderFile)
		{
			aMenuPane->SetItemDimmed(EOpenWithViewer,ETrue);
			
			if(!HasSelection)
			{
				aMenuPane->SetItemDimmed(ESendFiles,ETrue);
			}
		}
	}
	else if(R_MARKB_MENU == aResourceId)
	{
		if(CurrTypeId < CFFileItem::EFJustFile)
		{
			aMenuPane->SetItemDimmed(EFolderViewMarkTogle,ETrue);
		}
	}

}
/*
-----------------------------------------------------------------------

-----------------------------------------------------------------------
*/
TInt CSearchContainer::CountComponentControls() const
{
	if(iSelectionBox)
	{
		if(iFindBox)
			return 2;
		else
			return 1;
	}
	else
		return 0;
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CCoeControl* CSearchContainer::ComponentControl(TInt aIndex) const
{	
	if(iFindBox && aIndex)
		return iFindBox;
	else
		return iSelectionBox;
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CSearchContainer::Draw(const TRect& /*aRect*/) const
{
    // Get the standard graphics context 
 //   CWindowGc& gc = SystemGc();
 //   gc.Clear(aRect);
}

