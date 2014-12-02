/*
	Copyright (C) 2006 Jukka Silvennoinen
	All right reserved
*/

#ifndef __YUCCATOOLS_BASICAPPSTUFF_H__
#define __YUCCATOOLS_BASICAPPSTUFF_H__

#include <COECNTRL.H>
#include <AknsBasicBackgroundControlContext.h> 
#include <aknsdrawutils.h>
#include <aknscontrolcontext.h>
#include <AknsSkinInstance.h>
#include <aknsutils.h>
#include <AknLists.h>
#include <EIKBTGPC.H>
#include <EIKMENUP.H>
#include <AknSettingItemList.h>

class CShowString;
class CMyDBClass;
class CAknTabGroup;
class CAknNavigationControlContainer;
class CAknNavigationDecorator;
class CIconHandler;

class CTypesListbox : public CAknSettingItemList
		{
		public:
			~CTypesListbox();
			CTypesListbox(void);
			CAknSettingItem* CreateSettingItemL(TInt identifier);
			void HandleListBoxEventL(CEikListBox* aListBox,TListBoxEvent aEventType);
		private:
			virtual void SizeChanged();	
		public:
			TFileName		iExecutable;
			TBuf<150>		iHandlerApp,iFileType;
			TBuf<30>		iExtensionCount;
			CDesCArrayFlat*	iExtensions;
			TInt			iIndex;	
			TBool			iOwn,iHasChanges;
		};
		
//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
class CTypesContainer : public CCoeControl
    {
public:
	~CTypesContainer();
    void ConstructL(CEikButtonGroupContainer* aCba,CIconHandler* aIconHandler);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	CCoeControl* ComponentControl( TInt aIndex) const;
	TInt CountComponentControls() const;
	void HandleViewCommandL(TInt aCommand);
	void SetMenuL(void);
	void InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane);
	TBool CheckReFresh(){return iHasChanges;};
protected:
	TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
private:
	void SelectApplicationL(void);
	TBool RemoveExtensionsL(CDesCArray& aArray);

	void MakeNavipanelL(void);
	void RemoveNavipanel(void);
	CAknSingleStyleListBox* CurrentListBox(void) const;
	void ReReadMyTypesL(void);
	void ShowNoteL(const TDesC&  aMessage, TBool aError);
	virtual void SizeChanged();			
	TBool GetSelFileType(TDes& aFileType);	
	void Draw(const TRect& aRect) const;
	void UpdateScrollBar(CEikListBox* aListBox);
	CAknSingleStyleListBox* ConstructListBoxL(TInt aIndex,CDesCArray* aArray);
private:
	CEikButtonGroupContainer* 		iCba;
	CIconHandler* 					iIconHandler;
	CAknsBasicBackgroundControlContext*	iBgContext; 
	CAknSingleStyleListBox*			iSysListBox;
	CDesCArrayFlat* 				iSysFilTypArray;
	CAknSingleStyleListBox*			iOwnListBox;
	CDesCArrayFlat* 				iOwnFilTypArray;
	TBool							iHasChanges;
	CShowString*					iShowString;
	CTypesListbox*					iTypesListbox;
	CMyDBClass*						iMyDBClass;
	TBool							iShowOwn;
	CAknTabGroup* 						iTabGroup;
	CAknNavigationControlContainer* 	iNaviPane;
	CAknNavigationDecorator* 			iNaviDecorator;
	CAknNavigationDecorator* 			iNaviDecoratorForTabs;
};


#endif // __YUCCATOOLS_BASICAPPSTUFF_H__

