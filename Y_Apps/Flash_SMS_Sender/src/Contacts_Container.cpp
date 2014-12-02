/* Copyright (c) 2001 - 2009, Jukka Silvennoinen. All rights reserved */
 
#include "Contacts_Container.h"

#include "App_2002B0B0.hrh" 
#include <App_2002B0B0.rsg> 
#include "App_2002B0B0.h" 

#include <EIKLBV.H>
#include <GULICON.H>
#include <EIKENV.H>
#include <EIKAPPUI.H>
#include <AknQueryDialog.h>
#include <eikmenub.h>
#include <eikclbd.h>
#include <aknmessagequerydialog.h> 
#include <akniconarray.h> 
#include <eikapp.h>
#include <AknUtils.h>
#include <CNTITEM.H>
#include <CNTFLDST.H>
// CContacsContainer
//

const TInt KAknExListFindBoxTextLength = 20;

/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CContacsContainer::CContacsContainer(CEikButtonGroupContainer* aCba,RPointerArray<CMyCItem>& aItemArray)
:iCba(aCba),iItemArray(aItemArray)
{	
}

CContacsContainer::~CContacsContainer()
{	
	delete iListBox;
	iListBox = NULL;
	delete iFindBox;
	iFindBox = NULL;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CContacsContainer::ConstructL(void)
    {
    CreateWindowL();
	SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	
	ConstructListBoxL();

	ActivateL();
	DrawNow();
}	
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CContacsContainer::ConstructListBoxL(void)
{
	delete iListBox;
	iListBox = NULL;
	
	iListBox   = new( ELeave ) CAknSingleGraphicStyleListBox();	
	iListBox->ConstructL(this);//,EAknListBoxMarkableList);//EAknListBoxMultiselectionList
	
	CDesCArrayFlat* ListArray = new(ELeave)CDesCArrayFlat(1);

	TBuf<255> ConBuffer;
	for(TInt i=0; i < iItemArray.Count(); i++)
	{
		if(iItemArray[i])
		{
			ConBuffer.Copy(_L("0\t"));
			
			if(iItemArray[i]->iFirstName)
			{
				ConBuffer.Append(iItemArray[i]->iFirstName->Des());
			}
				
			if(iItemArray[i]->iLastName)
			{
				if(iItemArray[i]->iFirstName)
				{
					ConBuffer.Append(_L(" "));
				}
				
				ConBuffer.Append(iItemArray[i]->iLastName->Des());
			}
			
			ListArray->AppendL(ConBuffer);
		}
	}
	
	iListBox->Model()->SetItemTextArray(ListArray);
    iListBox->Model()->SetOwnershipType(ELbmOwnsItemArray);
    
    CArrayPtr<CGulIcon>* icons =new( ELeave ) CAknIconArray(1);
    CleanupStack::PushL(icons); 

    #ifdef __SERIES60_3X__  
        _LIT(KtxAvkonMBM    ,"Z:\\resource\\apps\\avkon2.mif");
                
        CFbsBitmap* FrameMsk2(NULL);
        CFbsBitmap* FrameImg2(NULL);
        CFbsBitmap* FrameMsk(NULL);
        CFbsBitmap* FrameImg(NULL);
                
        AknIconUtils::CreateIconL(FrameImg2,FrameMsk2,KtxAvkonMBM,EMbmAvkonQgn_indi_checkbox_on,EMbmAvkonQgn_indi_checkbox_on_mask);
        AknIconUtils::CreateIconL(FrameImg,FrameMsk,KtxAvkonMBM,EMbmAvkonQgn_indi_checkbox_off,EMbmAvkonQgn_indi_checkbox_off_mask);

        icons->AppendL(CGulIcon::NewL(FrameImg,FrameMsk));
        icons->AppendL(CGulIcon::NewL(FrameImg2,FrameMsk2));

    #else 
        _LIT(KtxAvkonMBM    ,"z:\\system\\data\\avkon.mbm");
            
        icons->AppendL(LoadImageL(KtxAvkonMBM,EMbmAvkonQgn_indi_checkbox_off,EMbmAvkonQgn_indi_checkbox_off_mask));
        icons->AppendL(LoadImageL(KtxAvkonMBM,EMbmAvkonQgn_indi_checkbox_on,EMbmAvkonQgn_indi_checkbox_on_mask));   
    #endif
    CleanupStack::Pop();//icons
    iListBox->ItemDrawer()->ColumnData()->SetIconArray(icons);
                
	iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

   
	delete iFindBox;
	iFindBox = NULL;
	iFindBox = CreateFindBoxL(iListBox, iListBox->Model(),CAknSearchField::ESearch);
	SizeChanged();
	iListBox->ActivateL();
	
	iListBox->UpdateScrollBarsL();	
	iListBox->HandleItemAdditionL();
	
	UpdateScrollBar(iListBox);	
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CAknSearchField* CContacsContainer::CreateFindBoxL(CEikListBox* aListBox,CTextListBoxModel* aModel, CAknSearchField::TSearchFieldStyle aStyle )
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
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TBool CContacsContainer::GetSelectedNumberL(TDes& aName,TDes& aNumber, const TInt aIndex)
{
	TBool Ret(EFalse);
	
	aName.Zero();
	aNumber.Zero();
	
	if(aIndex < iItemArray.Count() && aIndex >= 0)
	{
		if(iItemArray[aIndex])
		{
			if(iItemArray[aIndex]->iFirstName)
			{
				aName.Append(iItemArray[aIndex]->iFirstName->Des());
			}
				
			if(iItemArray[aIndex]->iLastName)
			{
				if(iItemArray[aIndex]->iFirstName)
				{
					aName.Append(_L(" "));
				}
				
				aName.Append(iItemArray[aIndex]->iLastName->Des());
			}
			
			aName.TrimAll();
			
			if(iItemArray[aIndex]->iNumbers->Count())
			{
				if(iItemArray[aIndex]->iNumbers->Count() > 1)
				{
					CDesCArrayFlat* NumbersArray = new(ELeave)CDesCArrayFlat(1);
					CleanupStack::PushL(NumbersArray);
					
					for(TInt f=0; f<iItemArray[aIndex]->iNumbers->Count(); f++)
					{
						 NumbersArray->AppendL(iItemArray[aIndex]->iNumbers->MdcaPoint(f));							
					}
					
					TInt ArrSel(-1);
					if(AskWithList(NumbersArray,ArrSel,aName))
					{
						if(ArrSel < NumbersArray->MdcaCount() && ArrSel >= 0)
						{
							aNumber.Copy(NumbersArray->MdcaPoint(ArrSel));
						}
					}

					CleanupStack::PopAndDestroy(NumbersArray);
				}
				else if(iItemArray[aIndex]->iNumbers->Count() == 1)
				{
					aNumber.Copy(iItemArray[aIndex]->iNumbers->MdcaPoint(0));    
				}
			}
		}
	}
	
	if(!aNumber.Length())
	{
		aName.Zero();
	}
	else
	{
		Ret = ETrue;
	}
	
	return Ret;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
TInt CContacsContainer::GetSelectedIndexL(void)
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
TBool CContacsContainer::AskWithList(CDesCArray* aArray,TInt& aSelected, const TDesC& aTitle)
{
	TBool Ret = ETrue;
	aSelected = -1;

    CAknSinglePopupMenuStyleListBox* list = new(ELeave)CAknSinglePopupMenuStyleListBox;
    CleanupStack::PushL(list);

    CAknPopupList* popupList = CAknPopupList::NewL(list, R_AVKON_SOFTKEYS_OK_BACK,AknPopupLayouts::EMenuWindow);  
    CleanupStack::PushL(popupList);

    list->ConstructL(popupList, 0);
    list->CreateScrollBarFrameL(ETrue);
    list->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);
    list->Model()->SetItemTextArray(aArray);
    list->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);

	popupList->SetTitleL(aTitle);

	// Show popup list and then show return value.
    TInt popupOk = popupList->ExecuteLD();
	if (popupOk)
    {
		aSelected = list->CurrentItemIndex();
	}
	else
		Ret = EFalse;

    CleanupStack::Pop();            // popuplist
	CleanupStack::PopAndDestroy(1);  // list
	return Ret;
}


/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CContacsContainer::UpdateScrollBar(CEikListBox* aListBox)
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
TKeyResponse CContacsContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	TKeyResponse Ret = EKeyWasNotConsumed;

	switch (aKeyEvent.iCode)
    {
	case EKeyDevice3:
        HandleViewCommandL(EContMarkTogle);
        DrawNow();
        break;
	case EKeyRightArrow:
	case EKeyLeftArrow:
	case EKeyDownArrow:
	case EKeyUpArrow:
	default:
		if(iListBox)
		{
			if ( iFindBox )
            {
            TBool needRefresh( EFalse );
            
            // Offers the key event to find box.
            if ( AknFind::HandleFindOfferKeyEventL( aKeyEvent, aType, this,
                                                    iListBox, iFindBox,
                                                    EFalse,
                                                    needRefresh ) == EKeyWasConsumed )
                {
                	if ( needRefresh )
                    {
                	    SizeChanged();
                    	DrawNow();
                    }

                	return EKeyWasConsumed;
                }
            }
            
			Ret = iListBox->OfferKeyEventL(aKeyEvent,aType);
		}
		break;
	}
	
	return Ret;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CContacsContainer::SizeChanged()
{
	if (iListBox)
    {
        if (iFindBox)
        {
        	CAknColumnListBox* aknListBox = STATIC_CAST(CAknColumnListBox*, iListBox);
        	AknFind::HandleFixedFindSizeChanged(this, aknListBox, iFindBox);
        
        }
        else
        {
            iListBox->SetRect(Rect()); // Sets rectangle of lstbox.
    	}
    }
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CContacsContainer::SizeChangedForFindBox()
    {
    if ( iListBox && iFindBox )
        {
        CAknColumnListBox* aknListBox = STATIC_CAST(CAknColumnListBox*, iListBox);
        AknFind::HandleFixedFindSizeChanged(this, aknListBox, iFindBox);
        }
    }

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CContacsContainer::CountComponentControls() const
{
	TInt Controls(0);
	if(iListBox && iFindBox)
	{
		Controls = 2;
	}
	
	return Controls;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CCoeControl* CContacsContainer::ComponentControl( TInt aIndex) const
{	
	CCoeControl* RetCont(NULL);

	if(iFindBox && aIndex)
		RetCont = iFindBox;
	else
		RetCont = iListBox;
	
	return RetCont;
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CContacsContainer::Draw(const TRect& aRect) const
{
	CWindowGc& gc = SystemGc();
	gc.Clear(aRect);
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CContacsContainer::GetSelectedArrayLC(RPointerArray<CMyContact>& aArray)
{
    TBuf<150> name,number;
    if(iListBox)
    {
        const CListBoxView::CSelectionIndexArray* Arrr = iListBox->SelectionIndexes() ;
        if(Arrr)
        {
            for(TInt i =0; i < Arrr->Count(); i ++)
            {
                name.Zero();
                number.Zero();
                
                if(GetSelectedNumberL(name,number, Arrr->At(i))){
                    CMyContact* tmpp = new(ELeave)CMyContact();
                    aArray.Append(tmpp);
                    tmpp->iId    = -1;
                    tmpp->iName  = name.AllocL();
                    tmpp->iNumber= number.AllocL();
                }
            }
        }
    }   
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

void CContacsContainer::SelecUnSelectL(TInt aItem)
{
    CDesCArray* itemArray = STATIC_CAST(CDesCArray* ,iListBox->Model()->ItemTextArray());
    if(itemArray)
    {
        if (aItem >= 0 && itemArray->Count() > aItem)
        {
            TBuf<200> Hjelper;
            
            if(iListBox->View()->ItemIsSelected(aItem)) 
            {
                Hjelper.Num(0); 
                iListBox->View()->DeselectItem(aItem);
            }
            else
            {
                Hjelper.Num(1);
                iListBox->View()->SelectItemL(aItem);
            }
            
            TPtrC ItemName;
            TPtrC Item(itemArray->MdcaPoint(aItem));
            if(KErrNone == TextUtils::ColumnText(ItemName,1,&Item))
            {
                Hjelper.Append(_L("\t"));
                Hjelper.Append(ItemName);
            
                itemArray->Delete(aItem);
                itemArray->InsertL(aItem,Hjelper);
                iListBox->View()->DrawItem(aItem); 
            }   
        }
    }
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CContacsContainer::SelecUnSelectAllL(TBool aSelect)
{
    CDesCArray* itemArray = STATIC_CAST(CDesCArray* ,iListBox->Model()->ItemTextArray());
    if(itemArray)
    {
        TBuf<200> Hjelper;
        TPtrC ItemName;
            
        for (TInt i = 0;i <itemArray->Count(); i++)
        {
            TPtrC Item(itemArray->MdcaPoint(i));
            if(KErrNone == TextUtils::ColumnText(ItemName,1,&Item))
            {
                if(aSelect) 
                {
                    Hjelper.Num(1);
                    iListBox->View()->SelectItemL(i);
                }
                else
                {
                    Hjelper.Num(0); 
                    iListBox->View()->DeselectItem(i);
                }               
                
                Hjelper.Append(_L("\t"));
                Hjelper.Append(ItemName);
                
                itemArray->Delete(i);
                itemArray->InsertL(i,Hjelper);
            }   
        }
    }
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CContacsContainer::HandleViewCommandL(TInt aCommand) 
{
    switch(aCommand)
    {
    case EContMarkTogle:
        if(iListBox)
        {
            CAknFilteredTextListBoxModel* model = STATIC_CAST(CAknFilteredTextListBoxModel*,iListBox->Model());     
            TInt CurrInd = model->Filter()->FilteredItemIndex(iListBox->CurrentItemIndex());
            if (CurrInd >= 0)
            {
                SelecUnSelectL(CurrInd);
            }
        }
        DrawNow();
        break;
    case EContMarkAll:
        {
            SelecUnSelectAllL(ETrue);
        }
        DrawNow();
        break;
    case EContUnMarkAll:
        {
            SelecUnSelectAllL(EFalse);
        }
        DrawNow();
        break;
    default:
        break;
    }
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CContacsContainer::SetMenuL(void)
{
    CEikonEnv::Static()->AppUiFactory()->MenuBar()->StopDisplayingMenuBar();                

    if(iCba)
    {
        CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(R_CONT_MENUBAR);
        
        iCba->SetCommandSetL(R_CONT_CBA);
        iCba->DrawDeferred();
    }
}
