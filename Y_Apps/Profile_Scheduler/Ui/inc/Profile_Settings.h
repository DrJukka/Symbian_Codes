/* Copyright (c) 2001 - 2008, Dr. Jukka Silvennoinen. All rights reserved */

#ifndef __EZANS_PROFSETTINGS_H__
#define __EZANS_PROFSETTINGS_H__

#include <aknsettingitemlist.h> 
#include <ASSHDDEFS.H>



	class CAknSettingsListListbox : public CAknSettingItemList
		{
		public:
			CAknSettingItem* CreateSettingItemL(TInt identifier);
			void HandleListBoxEventL(CEikListBox* aListBox,TListBoxEvent aEventType);
		private:
			void SelectProfileL(TDes& aProfile,TInt& aProfId);
			void SelectSetOnL(TDes& aSetOn, TInt & aTypeId);
			virtual void SizeChanged();
		public:
			TBuf<100> 			iType,iSelProfile;
			TTime				iAlrmTime;
			TInt				iProfileId,iTypeId;
		};

	// Container class to draw text on screen
	//
	class CProfileSettings : public CCoeControl
	{
	public:
		CProfileSettings();
		~CProfileSettings();
		void ConstructL(void);
		TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
		TInt CountComponentControls() const;
		CCoeControl* ComponentControl( TInt aIndex) const;
	public:
		void GetValuesL(TInt& aProfile, TInt& aRepeat, TTime& aTime,TInt& aIndex,TBool& aEnabled);
		void SetDataL(TInt aProfile, TInt aRepeat, TTime aTime,TInt aIndex,const TBool& aEnabled);
	private:
		void Draw(const TRect& aRect) const;
		void MakeListboxL(void);
		virtual void SizeChanged();
	private:
		CAknSettingsListListbox* 	iListBox;
		TInt 						iIndex;
		TBool 						iEnabled;
	};


#endif // __EZANS_PROFSETTINGS_H__
