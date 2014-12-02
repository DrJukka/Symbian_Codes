/* Copyright (c) 2001 - 2005 , Jukka Silvennoinen, Solution One Telecom LTd. All rights reserved */

#ifndef __LAF_SETTINGS_H__
#define __LAF_SETTINGS_H__

#include <aknsettingitemlist.h> 
#include <coecntrl.h>
#include <S32FILE.H>
#include <EIKLBO.H>
#include <EIKLBX.H>


	class CAknSettingsListListbox : public CAknSettingItemList
		{
		public:
			~CAknSettingsListListbox();
			CAknSettingsListListbox(void);
			CAknSettingItem* CreateSettingItemL(TInt identifier);
			void HandleListBoxEventL(CEikListBox* aListBox,TListBoxEvent aEventType);
		private:
			void SelectCommandSCL(TInt aSC);
			virtual void SizeChanged();
		public:
			TBuf<30>				iCommandTxt0;
			TBuf<30>				iCommandTxt1;
			TBuf<30>				iCommandTxt2;
			TBuf<30>				iCommandTxt3;
			TBuf<30>				iCommandTxt4;
			TBuf<30>				iCommandTxt5;
			TBuf<30>				iCommandTxt6;
			TBuf<30>				iCommandTxt7;
			TBuf<30>				iCommandTxt8;
			TBuf<30>				iCommandTxt9;
			TInt					iCommand0;
			TInt					iCommand1;
			TInt					iCommand2;
			TInt					iCommand3;
			TInt					iCommand4;
			TInt					iCommand5;
			TInt					iCommand6;
			TInt					iCommand7;
			TInt					iCommand8;
			TInt					iCommand9;
		};

	// Container class to draw text on screen
	//
	class CCommandSCSettings : public CCoeControl
	{
	public:	
		CCommandSCSettings(void);
		~CCommandSCSettings();
		void ConstructL(void);
		TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
		TInt CountComponentControls() const;
		CCoeControl* ComponentControl( TInt aIndex) const;
		void Draw(const TRect& aRect) const;
		void SaveValuesL(void);
	private:
		void GetCommandText(TDes& aTxtx,TInt aSC);
		void MakeListboxL(void);
		virtual void SizeChanged();
		void GetValuesFromStoreL();
		void GetValuesL(CDictionaryFileStore* aDStore,TInt aUID,TInt& aValue);
		void SaveValuesL(CDictionaryFileStore* aDStore,TInt aUID,TInt aValue);
	private:
		CAknSettingsListListbox* 	iListBox;
	};



#endif // __LAF_SETTINGS_H__
