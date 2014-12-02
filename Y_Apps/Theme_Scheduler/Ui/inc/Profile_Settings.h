/* Copyright (c) 2001 - 2008 , Dr Jukka Silvennoinen. All rights reserved */

#ifndef __EZANS_PROFSETTINGS_H__
#define __EZANS_PROFSETTINGS_H__

#include <aknsettingitemlist.h> 
#include <ASSHDDEFS.H>

class CProfileSettings;



	class CAknSettingsListListbox : public CAknSettingItemList
		{
		public:
			CAknSettingItem* CreateSettingItemL(TInt identifier);
			void HandleListBoxEventL(CEikListBox* aListBox,TListBoxEvent aEventType);
			void SetProfileSettings(CProfileSettings* aSettings);
		private:
			void SelectThemeL(TDes& aTheme,TUint32& aThemeId);
			void SelectProfileL(TDes& aProfile,TInt& aProfId);
			void SelectSetOnL(TDes& aSetOn, TInt & aTypeId);
			virtual void SizeChanged();
		public:
			CProfileSettings* 	iSettings;
			TBool				iProfile;
			TBuf<100> 			iType,iSelProfile,iSelTheme;
			TTime				iAlrmTime;
			TInt				iProfileId,iTypeId;
			TUint32				iThemeId;
			TFileName			iImageName;
		};

	// Container class to draw text on screen
	//
	class CProfileSettings : public CCoeControl
	{
	public:
		CProfileSettings(CEikButtonGroupContainer* aCba,TBool aProfile);
		~CProfileSettings();
		void ConstructL(void);
		TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
		TInt CountComponentControls() const;
		CCoeControl* ComponentControl( TInt aIndex) const;
		void Draw(const TRect& aRect) const;
	public:
		void SetDataL(const TInt& aProfile,const TUint32& aTheme,const TInt& aRepeat,const TTime& aTime,const TInt& aIndex,const TBool& aEnabled, const TDesC& aFilename);
		void GetValuesL(TBool& aType, TInt& aProfile, TUint32& aTheme,TInt& aRepeat, TTime& aTime,TInt& aIndex,TBool& aEnabled,TDes& aFilename);
		void SetMenuL(void);
	private:
		void MakeListboxL(void);
		virtual void SizeChanged();
	private:
		CAknSettingsListListbox* 	iListBox;
		TBool 						iProfile,iEnabled;
		CEikButtonGroupContainer* 	iCba;
		TInt 						iIndex;
	};


#endif // __EZANS_PROFSETTINGS_H__
