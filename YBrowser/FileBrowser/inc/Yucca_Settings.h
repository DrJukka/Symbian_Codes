/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen
	All rights reserved 
*/
#ifndef __YUCCA_SETTINGS_H__
#define __YUCCA_SETTINGS_H__

#include <aknsettingitemlist.h> 

struct TSettingsItem
	{
		TBool iSort,iFolders,iShow,iSavePath,iOptimize,iDoRecocnize,iConfirmExit,iFullScreen,iPathScroll;
		TInt iOrder;
	};


class CAknYuccaSettingsListListbox : public CAknSettingItemList 
    {
    public:
        CAknSettingItem* CreateSettingItemL(TInt identifier);
	private:
		virtual void SizeChanged();
	public:
		TSettingsItem 	iSettings;
		TBool 			iReadOnly,iHidden,iSystem;
		TTime			iFileTime;
		TTime			iFileDate;
    };

// Container class to draw text on screen
//
	class CYuccaSettings : public CCoeControl
	{
	public:	
		CYuccaSettings(void);
		~CYuccaSettings();
		void ConstructL(TSettingsItem aSettings);
		void ConstructL(TBool aReadOnly,TBool aHidden,TBool aSystem,TTime aTime);
		TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
		TInt CountComponentControls() const;
		CCoeControl* ComponentControl( TInt aIndex) const;
		void Draw(const TRect& aRect) const;
		void GetValuesL(TSettingsItem& aSettings);
		void GetValuesL(TBool& aReadOnly,TBool& aHidden,TBool& aSystem,TTime& aTime,TInt& aHours,TInt& aMinutes,TInt& aSeconds);
		TBool AttribSelector(void){ return iAttr;};
	private:
		virtual void SizeChanged();
		void CreateListBoxL(TSettingsItem aSettings);
	private:
		CAknYuccaSettingsListListbox* 	iListBox;
		TSettingsItem 					iSettings;
		TBool 							iReadOnly,iHidden,iSystem,iAttr;
		TTime							iFileDate;
	};


  
#endif // __YUCCA_SETTINGS_H__
