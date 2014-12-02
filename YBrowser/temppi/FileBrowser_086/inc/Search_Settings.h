/* Copyright (c) 2001 - 2005 , Jukka Silvennoinen, Solution One Telecom LTd. All rights reserved */

#ifndef __SAERCH_SETTINGS_H__
#define __SAERCH_SETTINGS_H__

#include <aknsettingitemlist.h> 
#include <coecntrl.h>
#include <S32FILE.H>
#include <EIKLBO.H>
#include <EIKLBX.H>


_LIT(KtxFormatTimeBuf		,"%2d/%2d/%2d-%2d:%2d");
_LIT(KtxOnePlusTab			,"1\t");



class CIconHandler;
class CSearchContainer;
class CSearchValues;
class CFFileTypeItem;
class CYuccaNavi;

	class CAknSearhcListListbox : public CAknSettingItemList
		{
		public:
			~CAknSearhcListListbox();
			CAknSearhcListListbox(CIconHandler& aIconHandler);
			void ConstructL(void);
			CAknSettingItem* CreateSettingItemL(TInt identifier);
			void ClearSelectedConditionL(void);
			void ShowSelectedFileTypesL(void);
			void HandleListBoxEventL(CEikListBox* aListBox,TListBoxEvent aEventType);
			void GetTimeBuffer(TDes& aBuffer,TTime aTime);
			void GetAttribBuffer(TDes& aBuffer,TBool aHidden, TBool aSystem,TBool aRo,TBool aAnd);
			void GetSizeBuffer(TDes& aBuffer,TInt aSize);
		private:
			void ShowTypesListL(RPointerArray<CFFileTypeItem>& aArray);
			TTime GetTimeL(TBool& Ok,TTime aInitialTime);
			void SelectFileTypesL(RPointerArray<CFFileTypeItem>& aArray);
			virtual void SizeChanged();
		public:
			CIconHandler& 			iIconHandler;
			TBuf<50>				iSearchFname;
			TBuf<30>				iSearchBigthantxt;
			TInt					iSearchBigthan;
			TBuf<30>				iSearchSmallThantxt;
			TInt					iSearchSmallThan;
			TBuf<30>				iSearchModBeforetxt;
			TTime					iSearchModBefore;
			TBuf<30>				iSearchModAftertxt;
			TTime					iSearchModAfter;
			TBool					iSearchCheckTypes,iSearchUnicode,iSearchCaseSensitive;
			TBuf<30>						iSearchExludeTyptxt;
			RPointerArray<CFFileTypeItem>	iSearchExludeTyp;
			TBuf<30>						iSearchIncludeTyptxt;
			RPointerArray<CFFileTypeItem>	iSearchIncludeTyp;
			RPointerArray<CFFileTypeItem>	iSearchIncludeTypOwn;
			TBuf<30>				iESearchAttrtxt;
			TBool					iHidden,iSystem,iReadOnly,iAndAttributes;
			TBuf<50>				iSearchString;
			TFileName				iSearchPath;
		};	
		

	// Container class to draw text on screen
	//
	class CSearchSettings : public CCoeControl
	{
	public:	
		CSearchSettings(CEikButtonGroupContainer* aCba,CIconHandler& aIconHandler,CYuccaNavi* aYuccaNavi);
		~CSearchSettings();
		void ConstructL(const TDesC& aPath,CSearchValues* aValues);
		TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
		TInt CountComponentControls() const;
		CCoeControl* ComponentControl( TInt aIndex) const;
		void Draw(const TRect& aRect) const;
		void SetMenuL(void);
		void InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane);
		void HandleViewCommandL(TInt aCommand);
		CSearchValues* GetSearchValues(void);
	// used from main container
		TBool AskToSave();
		void GetCurrSelFileL(TDes& aFileName,TInt& aFileType);
		void AddSelectedFilesL(CDesCArray* aArray,TBool AlsoFolders);
		void PageUpDown(TBool aUp);
	private:
		CSearchValues* GetSearchValuesL(void);
		void CopyTypeArrayL(RPointerArray<CFFileTypeItem>& aSource,RPointerArray<CFFileTypeItem>& aTarget);
		void SetValuesL(CSearchValues* aValues);
		void MakeListboxL(void);
		virtual void SizeChanged();
	private:
		CAknSearhcListListbox* 		iListBox;
		CEikButtonGroupContainer* 	iCba;
		CSearchContainer*			iSearchContainer;
		CIconHandler&				iIconHandler;
		CSearchValues*				iSearchValues;
		CYuccaNavi* 				iYuccaNavi;
		TBool						iAskToSave;
	};



#endif // __SAERCH_SETTINGS_H__
