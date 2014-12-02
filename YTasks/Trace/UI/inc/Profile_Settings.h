

#ifndef __LAF_SETTINGS_H__
#define __LAF_SETTINGS_H__

#include <aknsettingitemlist.h> 
#include <coecntrl.h>
#include <S32FILE.H>
#include <EIKLBO.H>
#include <EIKLBX.H>
#include <AknsBasicBackgroundControlContext.h> 
#include <aknsdrawutils.h>
#include <aknscontrolcontext.h>
#include <AknsSkinInstance.h>
#include <aknsutils.h>


	class CAknSettingsListListbox : public CAknSettingItemList
		{
		public:
			CAknSettingItem* CreateSettingItemL(TInt identifier);
		public:
			TBool	iEnabledOn;
			TInt	iHorSlider,iVerSlider,iFontSize,iDrawStyle;
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
		void Draw(const TRect& aRect) const;
		void SaveValuesL(void);
	private:
		TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
		void MakeListboxL(TInt aSelected);
		virtual void SizeChanged();
		virtual void HandleResourceChange(TInt aType);
		void GetValuesFromStoreL();
		void GetValuesL(CDictionaryFileStore* aDStore,TInt aUID,TInt32& aValue);
		void SaveValuesL(CDictionaryFileStore* aDStore,TInt aUID,TInt32 aValue);
	private:
		CAknSettingsListListbox* 			iListBox;
		CAknsBasicBackgroundControlContext*	iBgContext;
	};



#endif // __LAF_SETTINGS_H__
