/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen
	All rights reserved 
*/
#ifndef __SETTINGS_CONTAINER_H__
#define __SETTINGS_CONTAINER_H__


#include <coecntrl.h>
#include <AknProgressDialog.h>
#include <aknlists.h> 
#include <aknsfld.h> 
#include <msvapi.h> 
#include <AknsBasicBackgroundControlContext.h> 
#include <aknsdrawutils.h>
#include <aknscontrolcontext.h>
#include <AknsSkinInstance.h>
#include <aknsutils.h>

#include "Yucca_Settings.h"
#include "General_Settings.h"

class CAknTabGroup;
class CAknNavigationControlContainer;
class CAknNavigationDecorator;
class CTypesContainer;
class CPluInContainer;
class CCommandSCSettings;
class CIconHandler;
/*! 
  */
class CSettingsContainer : public CCoeControl
    {
public:
    static CSettingsContainer* NewL(CEikButtonGroupContainer* aCba,TSettingsItem aSettings,CIconHandler* aIconHandler,CGeneralSettings& aGeneralSettings);
    static CSettingsContainer* NewLC(CEikButtonGroupContainer* aCba,TSettingsItem aSettings,CIconHandler* aIconHandler,CGeneralSettings& aGeneralSettings);
    ~CSettingsContainer();
public:  
	void InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane);
	void SetMenuL(void);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	void HandleViewCommandL(TInt aCommand);
	TInt GetSelectedIndexL(void);
	TInt CountComponentControls() const;
	CCoeControl* ComponentControl(TInt aIndex) const;
	TBool GetGeneralSettings(TSettingsItem& Settings);
protected:
	TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
private:
	void SwitchToSettigL(TInt aSelected);
	void Draw(const TRect& aRect) const;
	void DisplayListBoxL(void);
	void UpdateScrollBar(CEikListBox* aListBox);
	virtual void SizeChanged();
	void ConstructL();
    CSettingsContainer(CEikButtonGroupContainer* aCba,TSettingsItem aSettings,CIconHandler* aIconHandler,CGeneralSettings& aGeneralSettings);
	CArrayPtr<CGulIcon>* GetIconArrayL(void);
private:
	CEikButtonGroupContainer*		iCba;	
	TSettingsItem 					iSettings;
	CIconHandler* 					iIconHandler;
	TBool							iGenChanged;
	CGeneralSettings&               iGeneralSettings;
	CAknSingleLargeStyleListBox* 		iSelectionBox;
	CDesCArrayFlat*						iListArray;
        
	CAknsBasicBackgroundControlContext*	iBgContext;
	CTypesContainer*					iTypesContainer;
	CPluInContainer*					iPluInContainer;
	CYuccaSettings*						iYuccaSettings;
	CCommandSCSettings*					iCommandSCSettings;	
    };


#endif // __SETTINGS_CONTAINER_H__
