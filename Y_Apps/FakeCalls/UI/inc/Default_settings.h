/* Copyright (c) 2001 - 2009 , Dr Jukka Silvennoinen. All rights reserved */

#ifndef __DEFFAULT_SETTTINGS_H__
#define __DEFFAULT_SETTTINGS_H__

#include <COECNTRL.H>
#include <AknLists.h>
#include <D32DBMS.H>
#include <AknSettingItemList.h>

#include "Common.h"

class CMyHelpContainer;


/*
----------------------------------------------------------------
*/
class CEikButtonGroupContainer;


	class CDefaultSettingsListListbox : public CAknSettingItemList
		{
		public:
			CAknSettingItem* CreateSettingItemL(TInt identifier);
			void HandleListBoxEventL(CEikListBox* aListBox,TListBoxEvent aEventType);
		private:
			virtual void SizeChanged();
		public:
			TInt			iSnoozeTime,iMaxTime;
			TBuf<50>		iOperator,iSoundFile,iSnoozeTimeText,iMaxTimeText,iSnoozeLabel;
			TFileName		iFullFileName;
		};
	
/*
----------------------------------------------------------------
*/
class CDefaultSettings : public CCoeControl
    {
public:
	CDefaultSettings(CEikButtonGroupContainer* aCba);
    void ConstructL(void);
     ~CDefaultSettings();
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
    void Draw(const TRect& aRect) const;
	CCoeControl* ComponentControl( TInt aIndex) const;
	TInt CountComponentControls() const;
public:
	void SetMenuL(void);
	void SaveValuesL();	
	void HandleViewCommandL(TInt aCommand);
	void InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane);	
private:
	virtual void SizeChanged();
	virtual void HandleResourceChange(TInt aType);
	void MakeListboxL(TInt aSelected);
	void GetValuesFromStoreL(void);
	void GetValuesL(CDictionaryFileStore* aDStore,TInt aUID,TDes& aValue);
	void GetValuesL(CDictionaryFileStore* aDStore,TInt aUID,TUint32& aValue);
	void SaveValuesL(CDictionaryFileStore* aDStore,TInt aUID, TUint32 aValue);
	void SaveValuesL(CDictionaryFileStore* aDStore,TInt aUID, const TDesC& aValue);
 private:
	CEikButtonGroupContainer* 		iCba;
	CDefaultSettingsListListbox*	iListBox;	
	CMyHelpContainer*				iMyHelpContainer;
};



#endif // __DEFFAULT_SETTTINGS_H__
