/* Copyright (c) 2001 - 2009 , Dr Jukka Silvennoinen. All rights reserved */

#ifndef __EZANS_MAINCONTAINER_H__
#define __EZANS_MAINCONTAINER_H__

#include <COECNTRL.H>
#include <AknLists.h>
#include <D32DBMS.H>
#include <AknSettingItemList.h>

#include "Common.h"

class CMyHelpContainer;

#ifdef LAL_VERSION    
	#include <MAdtronicApiClient.h>
#endif 

/*
----------------------------------------------------------------
*/
class CEikButtonGroupContainer;


	class CAknSettingsListListbox : public CAknSettingItemList
		{
		public:
			CAknSettingItem* CreateSettingItemL(TInt identifier);
			void HandleListBoxEventL(CEikListBox* aListBox,TListBoxEvent aEventType);
			void GetColorName(const TRgb& aColor, TDes& aName);
			TBool SelectFontBGColorL(TRgb& aColor, const TDesC& aTitle);
		private:
			virtual void SizeChanged();
			void SelectFontL(void);
			TBool SelectColorL(TRgb& aColor);
		public:
			TBool			iOn,iLockEnabled,iAddIsValid;
			TInt 			iLightOn,iOrienta,iLockTime,iLightVal,iLightTime,iLightVal2,iShowDate;
			TBuf<255>		iFont;
			TBuf<50>		iLockTimetxt,iLightTimetxt,iBgColor,iFontColor;
			TRgb			iBgColorRgb,iFontColorRgb;
		};
	
/*
----------------------------------------------------------------
*/
class CMainContainer : public CCoeControl
#ifdef LAL_VERSION    
,public MAdtronicStatus
#endif 
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
	void SaveValuesL();	
	void HandleViewCommandL(TInt aCommand);
	void InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane);
protected:
	void installationValid();
	void installationInvalid();
	void adtronicNotInstalled();	
private:
	virtual void SizeChanged();
	virtual void HandleResourceChange(TInt aType);
	void MakeListboxL(TInt aSelected);
	void GetValuesFromStoreL(void);
	void GetValuesL(CDictionaryFileStore* aDStore,TInt aUID,TDes& aValue);
	void GetValuesL(CDictionaryFileStore* aDStore,TInt aUID,TUint32& aValue);
	void SaveValuesL(CDictionaryFileStore* aDStore,TInt aUID, TUint32 aValue);
	void SaveValuesL(CDictionaryFileStore* aDStore,TInt aUID, const TDesC& aValue);
	void SetMenuL(void);
 private:
	CEikButtonGroupContainer* 	iCba;
	CAknSettingsListListbox*	iListBox;
#ifdef LAL_VERSION    
	MAdtronicApiClient* 			iApi;
#endif 	
	TBool							iAddIsValid;
	CMyHelpContainer*				iMyHelpContainer;
};



#endif // __EZANS_MAINCONTAINER_H__
