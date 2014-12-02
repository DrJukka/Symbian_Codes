/* Copyright (c) 2001 - 2008, Dr. Jukka Silvennoinen. All rights reserved */

#ifndef __FAKEMESS_PROFSETTINGS_H__
#define __FAKEMESS_PROFSETTINGS_H__

#include <aknsettingitemlist.h> 
#include <ASSHDDEFS.H>
#include <EIKBTGPC.H>
#include <CNTITEM.H>

#include "Contacts_Container.h"

class CMsgSched;
class CKeyCapcap;
class CMyHelpContainer;
class CProfileSettings;

	class CAknSettingsListListbox : public CAknSettingItemList
		{
		public:
			CAknSettingsListListbox(CProfileSettings* aSettings);
			CAknSettingItem* CreateSettingItemL(TInt identifier);
			void HandleListBoxEventL(CEikListBox* aListBox,TListBoxEvent aEventType);
		private:
			void SelectRepeatL(TDes& aSetOn, TInt & aTypeId);
			virtual void SizeChanged();
		public:
			CProfileSettings* 	iSettings;
			TTime		iAlrmTime,iAlrmDate;
			TBuf<100>	iSendNumber,iRepeat;
			TBuf<200>	iSenderName;
			TInt		iRepeatId,iDelay;
			TBool		iEnabled,iSchedule;
			HBufC* 		iDateFormatString;
			TBuf<50>	iDelayBuf;
		};

	// Container class to draw text on screen
	//

	
	class CProfileSettings : public CCoeControl,MContactClikked
	{
	public:
		CProfileSettings(CEikButtonGroupContainer* aCba,const TBool& aSchedule);
		~CProfileSettings();
		void ConstructL();
		TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
		TInt CountComponentControls() const;
		CCoeControl* ComponentControl( TInt aIndex) const;
		void HandleViewCommandL(TInt aCommand);
		void InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane);
		void SetMenuL(void);
	public:
		void ShowNoteL(const TDesC& aMessage);
		void GetValuesL(CMsgSched *aData);
		void SetDataL(CMsgSched *aData);
		void GetValuesL(CKeyCapcap *aData);
		void SetDataL(CKeyCapcap *aData);		
		void SetMessageL(const TDesC& aMessage);
	protected:
		void ContactClikkedL(void);
	private:
		TTime GetAlarmTimeL(TTime aTime, TTime aDate);
		void Draw(const TRect& aRect) const;
		void MakeListboxL(void);
		virtual void SizeChanged();
	private:
		CEikButtonGroupContainer* 	iCba;
		CAknSettingsListListbox* 	iListBox;
		TInt 						iIndex;
		CContacsContainer*			iContacsContainer;
		HBufC* 						iDateFormatString;
		TBool 						iSchedule;
		CMyHelpContainer*			iMyHelpContainer;
	};


#endif // __FAKEMESS_PROFSETTINGS_H__
