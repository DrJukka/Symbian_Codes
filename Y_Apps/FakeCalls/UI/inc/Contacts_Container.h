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

class MContactClikked
	{
	public:
		virtual void ContactClikkedL(void) = 0;
		};

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
class CContacsContainer : public CCoeControl
    {
public:
	~CContacsContainer();
	CContacsContainer(RPointerArray<CMyCItem>& aItemArray,MContactClikked& aInterface);
    void ConstructL(void);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	CCoeControl* ComponentControl( TInt aIndex) const;
	TInt CountComponentControls() const;
	void SizeChanged();
	TInt GetSelectedIndexL(void);
	TBool GetSelectedNumberL(TDes& aName,TDes& aNumber);
private:
	TBool AskWithList(CDesCArray* aArray,TInt& aSelected, const TDesC& aTitle);
	void SizeChangedForFindBox();
	CAknSearchField* CreateFindBoxL(CEikListBox* aListBox,CTextListBoxModel* aModel, CAknSearchField::TSearchFieldStyle aStyle);
	void UpdateScrollBar(CEikListBox* aListBox);
	void ConstructListBoxL(void);
	void Draw(const TRect& aRect) const;
private:
	CAknSingleStyleListBox*		iListBox;
	CAknSearchField* 			iFindBox;
	RPointerArray<CMyCItem> 	iItemArray;
	MContactClikked& 			iInterface;
};

  
#endif // __SCCOPY_CONTACTSCONT_H__
