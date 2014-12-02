/*
	Copyright (C) 2006 Jukka Silvennoinen
	All right reserved
*/

#ifndef __SETTINGS_PLUGINS_H__
#define __SETTINGS_PLUGINS_H__

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
#include <EIKRTED.H>
#include <TXTRICH.H>


#include "FileTypesDataBase.h"

class CShowString;
class CIconHandler;
class CAknTabGroup;
class CAknNavigationControlContainer;
class CAknNavigationDecorator;
class CMyDBClass;
class CSupportTypeItem;


class CPluInListbox : public CAknSettingItemList
		{
		public:
			~CPluInListbox();
			CPluInListbox(void);
			CAknSettingItem* CreateSettingItemL(TInt identifier);
			void HandleListBoxEventL(CEikListBox* aListBox,TListBoxEvent aEventType);
		private:
			void SelectApplicationL(void);
			virtual void SizeChanged();	
		public:
			TBuf<200>	iTypes,iName;
			TBool		iUnRecognized;
			TUint32 	iUid;
		};
		
//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
class CPluInContainer : public CCoeControl
    {
public:
	~CPluInContainer();
	CPluInContainer(CEikButtonGroupContainer* aCba,CIconHandler* aIconHandler);
    void ConstructL(void);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	CCoeControl* ComponentControl( TInt aIndex) const;
	TInt CountComponentControls() const;
	void HandleViewCommandL(TInt aCommand);
	void SetMenuL(void);
	void InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane);
	TBool CheckReFresh(void);
protected:
	TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
private:
	void ConstructEditorL(void);
	void AddPlugInInfoL(CRichText& aText,CImplementationInformation& aInfo,const TBool& aHandler);
	CImplementationInformation* CurrPlugIn(TBool& aHandler);
	void MakeNavipanelL(void);
	void RemoveNavipanel(void);
	CAknSingleGraphicStyleListBox* CurrentListBox(void) const;
	void ReReadHandlersL(RPointerArray<CDisabledItem>& aArray);
	void ReReadRecognizersL(RPointerArray<CDisabledItem>& aArray);
	void ShowNoteL(const TDesC&  aMessage, TBool aError);
	virtual void SizeChanged();			
	TInt GetSelectedIndex(void);	
	void Draw(const TRect& aRect) const;
	void UpdateScrollBar(CEikListBox* aListBox);
	CAknSingleGraphicStyleListBox* ConstructListBoxL(TInt aIndex,CDesCArray* aArray);
	void AddFileTypesL(RPointerArray<CSupportTypeItem>& aSupArray);
	void ShowFileTypesL(RPointerArray<CSupportTypeItem>& aSupArray);
private:
	CEikButtonGroupContainer* 		iCba;
	CIconHandler* 					iIconHandler;
	CAknsBasicBackgroundControlContext*	iBgContext; 
	CAknSingleGraphicStyleListBox*	iSysListBox;
	CDesCArrayFlat* 				iSysFilTypArray;
	CAknSingleGraphicStyleListBox*	iOwnListBox;
	CDesCArrayFlat* 				iOwnFilTypArray;
	
	CEikGlobalTextEditor* 			iEditor;	
	CParaFormatLayer* 				iParaformMe;
	CCharFormatLayer* 				iCharformMe;
	CRichText* 						iRichMessageMe;
	
	TBool							iFtypesChanged;
	RPointerArray<CSupportTypeItem> iTmpSupArray;
	CImplementationInformation* 	iTmpPlugIn;
	CPluInListbox*					iTypesListbox;
	
	CAknTabGroup* 						iTabGroup;
	CAknNavigationControlContainer* 	iNaviPane;
	CAknNavigationDecorator* 			iNaviDecorator;
	CAknNavigationDecorator* 			iNaviDecoratorForTabs;

	RImplInfoPtrArray 				iRecognizerArray;
	RImplInfoPtrArray 				iHandlerArray;
	TBool							iChanged,iCanHandModify,iIsOpenWith,iWasDefHand,iRecOpen;
	CMyDBClass*						iMyDBClass;
};


#endif // __SETTINGS_PLUGINS_H__

