/* Copyright (c) 2001 - 2009, Jukka Silvennoinen. All rights reserved */

#ifndef __SCCOPY_CONTACTSCONT_H__
#define __SCCOPY_CONTACTSCONT_H__

#include <coecntrl.h>       // CCoeControl
#include <eikbtgpc.h>
#include <eikmenup.h>
#include <EIKLBX.H>
#include <aknlists.h> 
#include <aknsfld.h> 
#include <CNTDEF.H>
#include <aknnavi.h> 
#include <aknnavide.h> 
#include <akntabgrp.h> 
#include "ContactReader.h"

class CContactItem;

    class CMyContact :public CBase
        {
    public:
        ~CMyContact(){delete iName;delete iNumber;};
        CMyContact(){iName = NULL;iNumber = NULL;iId = -1;};
    public:
        HBufC*          iName;
        HBufC*          iNumber;
        TContactItemId  iId;
    };
    
//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
class CContacsContainer : public CCoeControl
    {
public:
	~CContacsContainer();
	CContacsContainer(CEikButtonGroupContainer* aCba,RPointerArray<CMyCItem>& aItemArray);
    void ConstructL(void);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	CCoeControl* ComponentControl( TInt aIndex) const;
	TInt CountComponentControls() const;
	void SizeChanged();
	TInt GetSelectedIndexL(void);
	TBool GetSelectedNumberL(TDes& aName,TDes& aNumber, const TInt aIndex);
	
	void HandleViewCommandL(TInt aCommand);
	void SetMenuL(void);
	void GetSelectedArrayLC(RPointerArray<CMyContact>& aArray);
private:
	void SelecUnSelectAllL(TBool aSelect);
	void SelecUnSelectL(TInt aItem);
	
	TBool AskWithList(CDesCArray* aArray,TInt& aSelected, const TDesC& aTitle);
	void SizeChangedForFindBox();
	CAknSearchField* CreateFindBoxL(CEikListBox* aListBox,CTextListBoxModel* aModel, CAknSearchField::TSearchFieldStyle aStyle);
	void UpdateScrollBar(CEikListBox* aListBox);
	void ConstructListBoxL(void);
	void Draw(const TRect& aRect) const;
private:
	CAknSingleGraphicStyleListBox*		iListBox;
	CAknSearchField* 			iFindBox;
	CEikButtonGroupContainer*   iCba;
	RPointerArray<CMyCItem> 	iItemArray;
};

  
#endif // __SCCOPY_CONTACTSCONT_H__
