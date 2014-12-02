/* Copyright (c) 2001 - 2007 , J.P. Silvennoinen. All rights reserved */

#ifndef __THEME_PROFSETTINGS_H__
#define __THEME_PROFSETTINGS_H__

#include <aknsettingitemlist.h> 
#include <ASSHDDEFS.H>

class CThemeSettings;



	class CThemeSettingsListListbox : public CAknSettingItemList
		{
		public:
			CAknSettingItem* CreateSettingItemL(TInt identifier);
			void HandleListBoxEventL(CEikListBox* aListBox,TListBoxEvent aEventType);
			void SetProfileSettings(CThemeSettings* aSettings);
		private:
			void SelectThemeL(TDes& aTheme,TUint32& aThemeId);
			virtual void SizeChanged();
		public:
			CThemeSettings* 	iSettings;
			TBuf<100> 			iSelTheme;
			TTime				iAlrmTime;
			TUint32				iThemeId;
		};

	// Container class to draw text on screen
	//
	class CThemeSettings : public CCoeControl
	{
	public:
		CThemeSettings(CEikButtonGroupContainer* aCba);
		~CThemeSettings();
		void ConstructL(void);
		TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
		TInt CountComponentControls() const;
		CCoeControl* ComponentControl( TInt aIndex) const;
		void Draw(const TRect& aRect) const;
	public:
		void SetDataL(TUint32 aTheme,TTime aTime,TInt aIndex);
		void GetValuesL(TUint32& aTheme,TTime& aTime,TInt& aIndex);
		void SetMenuL(void);
	private:
		void MakeListboxL(void);
		virtual void SizeChanged();
	private:
		CThemeSettingsListListbox* 	iListBox;
		CEikButtonGroupContainer* 	iCba;
		TInt 						iIndex;
	};


#endif // __THEME_PROFSETTINGS_H__
