/* Copyright (c) 2001 - 2008, Dr. Jukka Silvennoinen. All rights reserved */

#ifndef __EZANS_MAINCONTAINER_H__
#define __EZANS_MAINCONTAINER_H__

#include <COECNTRL.H>
#include <AknLists.h>
#include <D32DBMS.H>
#include <EIKBTGPC.H>

#include "MsgDataBase.h"
#include "Ps_Observer.h"
/*
----------------------------------------------------------------
*/
class CEikButtonGroupContainer;
class CAknDoubleNumberStyleListBox;
class CEikMenuPane;
class CEikListBox;
class CProfileSettings;
class CGulIcon;
class CDefaultSettings;
class CMyHelpContainer;
/*
----------------------------------------------------------------
*/

class CMainContainer : public CCoeControl,MMyPsObsCallBack
    {
public:
    CMainContainer(CEikButtonGroupContainer* aCba,const TBool& aDimPreview);
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
protected:
	void PsValueChangedL(TInt aValue,TInt aError){};
	void PsValueChangedL(const TDesC8& aValue,TInt aError){};
private:
	void SaveValuesL(CDictionaryFileStore* aDStore,TInt aUID, TUint32 aValue);
	void SaveValuesL(CDictionaryFileStore* aDStore,TInt aUID, const TDesC& aValue);
	void SaveValuesL(void);
	void GetValuesL(CDictionaryFileStore* aDStore,TInt aUID,TUint32& aValue);
	void GetValuesL(CDictionaryFileStore* aDStore,TInt aUID,TDes& aValue);
	void GetValuesFromStoreL(void);
private:
	void ShowMenuItemsL(TBool& aEnable, TBool& aDisable, TBool& aRemove, TBool& aPreview);
	TBool HandlePopUpMenuSelectionL(TInt& aCommand);
	virtual void SizeChanged();
	virtual void HandleResourceChange(TInt aType);
	CDesCArray* GetProfilesArrayL(void);
	void MakeProfileBoxL(void);
	void UpdateScrollBar(CEikListBox* aListBox);
 private:
	CEikButtonGroupContainer* 		iCba;
	TBool                           iDimPreview;
	CAknDoubleTimeStyleListBox* 	iProfileBox;
	CProfileSettings*				iProfileSettings;
	RPointerArray<CMsgSched>		iItemArray;
	RPointerArray<CKeyCapcap>		iKeyArray;
	CDefaultSettings*				iDefaultSettings;
	CMyHelpContainer*				iMyHelpContainer;
};



#endif // __EZANS_MAINCONTAINER_H__
