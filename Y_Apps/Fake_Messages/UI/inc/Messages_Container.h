/* Copyright (c) 2001 - 2008, Dr. Jukka Silvennoinen. All rights reserved */

#ifndef __MEZZAGESS_MAINCONTAINER_H__
#define ___MEZZAGESS_MAINCONTAINER_H__

#include <COECNTRL.H>
#include <AknLists.h>
#include <D32DBMS.H>
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

/*
----------------------------------------------------------------
*/
class CMessagesContainer : public CCoeControl
    {
public:
	CMessagesContainer(CEikButtonGroupContainer* aCba);
    void ConstructL(void);
     ~CMessagesContainer();
public:
	void HandleViewCommandL(TInt aCommand);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	CCoeControl* ComponentControl( TInt aIndex) const;
	TInt CountComponentControls() const;
	void InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane);
	void SetMenuL(void);
	void GetMessageL(TDes& aMessage);
private:
	virtual void SizeChanged();
	void Draw(const TRect& aRect) const;
	CDesCArray* GetArrayL(void);
	void MakeListBoxL(void);
	void UpdateScrollBar(CEikListBox* aListBox);
 private:
	CEikButtonGroupContainer* 		iCba;
	CAknSingleNumberStyleListBox* 	iMyListBox;
	RPointerArray<CMsgStored>		iItemArray;
};
	


#endif // ___MEZZAGESS_MAINCONTAINER_H__
