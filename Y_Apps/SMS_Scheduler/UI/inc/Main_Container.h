/* Copyright (c) 2001 - 2008, Dr. Jukka Silvennoinen. All rights reserved */

#ifndef __EZANS_MAINCONTAINER_H__
#define __EZANS_MAINCONTAINER_H__

#include <COECNTRL.H>
#include <AknLists.h>
#include <D32DBMS.H>
#include <EIKBTGPC.H>

#include "MsgDataBase.h"

/*
----------------------------------------------------------------
*/
class CEikButtonGroupContainer;
class CAknDoubleNumberStyleListBox;
class CEikMenuPane;
class CEikListBox;
class CProfileSettings;
class CGulIcon;
class CMyHelpContainer;
/*
----------------------------------------------------------------
*/

class CMainContainer : public CCoeControl
    {
public:
    CMainContainer(CEikButtonGroupContainer* aCba);
    void ConstructL(void);
     ~CMainContainer();
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
    void Draw(const TRect& aRect) const;
	CCoeControl* ComponentControl( TInt aIndex) const;
	TInt CountComponentControls() const;
public:
	void HandleViewCommandL(TInt aCommand);
	void SetMenuL(void);
	void InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane);
private:
	void ConvertFromUnicodeL(const TDes& aFrom,TInt& aUncorvetible);
	TBool CheckMessageForUnicodeL(HBufC* aMessage, TBool& aSaveNow);
	virtual void SizeChanged();
	virtual void HandleResourceChange(TInt aType);
	CDesCArray* GetProfilesArrayL(void);
	void MakeProfileBoxL(void);
	void UpdateScrollBar(CEikListBox* aListBox);
 private:
	CEikButtonGroupContainer* 		iCba;
	CAknDoubleTimeStyleListBox* 	iProfileBox;
	CProfileSettings*				iProfileSettings;
	RPointerArray<CMsgSched>		iItemArray;
	CMyHelpContainer*				iMyHelpContainer;
};



#endif // __EZANS_MAINCONTAINER_H__
