/* Copyright (c) 2001 - 2009, Jukka Silvennoinen. All rights reserved */
 
#include "Contacts_Container.h"


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
CContacsContainer::CContacsContainer(RPointerArray<CMyCItem>& aItemArray,MContactClikked& aInterface)
:iItemArray(aItemArray),iInterface(aInterface)
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
	
	iListBox   = new( ELeave ) CAknSingleStyleListBox();	
	iListBox->ConstructL(this);//,EAknListBoxMarkableList);//EAknListBoxMultiselectionList
	
	CDesCArrayFlat* ListArray = new(ELeave)CDesCArrayFlat(1);

	TBuf<255> ConBuffer;
	for(TInt i=0; i < iItemArray.Count(); i++)
	{
		if(iItemArray[i])
		{
			ConBuffer.Copy(_L("\t"));
			
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
TBool CContacsContainer::GetSelectedNumberL(TDes& aName,TDes& aNumber)
{
	TBool Ret(EFalse);
	TInt CurrInd = GetSelectedIndexL();
	
	aName.Zero();
	aNumber.Zero();
	
	if(CurrInd < iItemArray.Count() && CurrInd >= 0)
	{
		if(iItemArray[CurrInd])
		{
			if(iItemArray[CurrInd]->iFirstName)
			{
				aName.Append(iItemArray[CurrInd]->iFirstName->Des());
			}
				
			if(iItemArray[CurrInd]->iLastName)
			{
				if(iItemArray[CurrInd]->iFirstName)
				{
					aName.Append(_L(" "));
				}
				
				aName.Append(iItemArray[CurrInd]->iLastName->Des());
			}
			
			aName.TrimAll();
			
			if(iItemArray[CurrInd]->iNumbers.Count())
			{
				if(iItemArray[CurrInd]->iNumbers.Count() > 1)
				{
					CDesCArrayFlat* NumbersArray = new(ELeave)CDesCArrayFlat(1);
					CleanupStack::PushL(NumbersArray);
					
					for(TInt f=0; f<iItemArray[CurrInd]->iNumbers.Count(); f++)
					{
						if(iItemArray[CurrInd]->iNumbers[f])
						{
							if(iItemArray[CurrInd]->iNumbers[f]->iNumber)
							{
								NumbersArray->AppendL(iItemArray[CurrInd]->iNumbers[f]->iNumber->Des());
							}
						}
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
				else if(iItemArray[CurrInd]->iNumbers[0])
				{
					if(iItemArray[CurrInd]->iNumbers[0]->iNumber)
					{
						aNumber.Copy(iItemArray[CurrInd]->iNumbers[0]->iNumber->Des());
					}
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
		iInterface.ContactClikkedL();
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
