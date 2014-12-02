/* Copyright (c) 2001 - 2005 , J.P. Silvennoinen. All rights reserved */

#ifndef __FILTYPESETTINGS_H__
#define __FILTYPESETTINGS_H__

#include <aknsettingitemlist.h> 


class CImeiSettings;

	class CImeiSettingsListListbox : public CAknSettingItemList
		{
		public:
			~CImeiSettingsListListbox();
			CImeiSettingsListListbox();
			CAknSettingItem* CreateSettingItemL(TInt identifier);
		private:
			virtual void SizeChanged();
		public:
			TBuf<150>		 iType,iTypeId,iExtension;
			
		};

	// Container class to draw text on screen
	//
	class CImeiSettings : public CCoeControl
	{
	public:
		CImeiSettings();
		~CImeiSettings();
		void ConstructL(const TDesC8& aType,const TDesC8& aData,const TDesC& aExtension,TInt aId);
		TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
		TInt CountComponentControls() const;
		CCoeControl* ComponentControl( TInt aIndex) const;
		void Draw(const TRect& aRect) const;
		void GetDataL(TDes8& aType,TDes8& aData,TDes& aExtension,TInt& aId);

	private:
		void MakeListboxL(const TDesC8& aType,const TDesC8& aData,const TDesC& aExtension);
		virtual void SizeChanged();
	private:
		CImeiSettingsListListbox* 	iListBox;
		TInt 						iId;
	};


#endif // __FILTYPESETTINGS_H__
