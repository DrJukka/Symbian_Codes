/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen
	All rights reserved 
*/
#ifndef __FILESEARCHING_VIEW_H__
#define __FILESEARCHING_VIEW_H__


#include <coecntrl.h>
#include <AknProgressDialog.h>
#include <aknlists.h> 
#include <aknsfld.h> 
#include <aknwaitdialog.h> 

#include "File_Searcher.h"
#include "Public_Interfaces.h"


class CShowString;
class CIconHandler;
class CIconHandler;
class CYuccaNavi;

	class CSFileItem :public CBase
		{
	public:
		~CSFileItem(){delete iPath;delete iName;};
	public:
		HBufC* 	iPath;
		TInt   	iType;
		HBufC* 	iName;
		TInt 	iSize,iFreeSize;
		TInt 	iFolders,iFiles;
		TTime 	iModified;
		};
		

class CSearchValues : public CBase
{
public:
	~CSearchValues(){
					 delete iPath;
					 delete iWildName;
					 delete iSearchString;
					 delete iSearchString8;
					 iIncludeArray.ResetAndDestroy();
					 iExcludeArray.ResetAndDestroy();
					 iItemArray.ResetAndDestroy();
					 };
public:
	HBufC*		iPath;
	HBufC*		iWildName;
	HBufC*		iSearchString;
	HBufC8*		iSearchString8;
	TBool		iCaseSensitive,iUnicode,iTypeCheckEnabled;
	TInt		iBigThan,iSmaThan;
	TBool		iUseAndAttrib,iHidden,iReadOnly,iSystem;
	TBool		iTimeBeforeSet,iTimeAfterSet;
	TTime							iTimeBefore,iTimeAfter;
	RPointerArray<CFFileTypeItem> 	iIncludeArray;
	RPointerArray<CFFileTypeItem> 	iExcludeArray;
	RPointerArray<CSFileItem> 		iItemArray;
   };

		
/*! 
  @class CSearchContainer
  
  @discussion An instance of the Application View object for the HelloWorld 
  example application
  */
class CSearchContainer : public CCoeControl,MProgressDialogCallback,MMyFileSearchObserver
    {
public:
    static CSearchContainer* NewL(const TRect& aRect,CEikButtonGroupContainer* aCba,CSearchValues& aValues,CIconHandler& aIconHandler,const TDesC& aPath,CYuccaNavi* aYuccaNavi);
    static CSearchContainer* NewLC(const TRect& aRect,CEikButtonGroupContainer* aCba,CSearchValues& aValues,CIconHandler& aIconHandler,const TDesC& aPath,CYuccaNavi* aYuccaNavi);
    ~CSearchContainer();
public: 
	TInt CountComponentControls() const;
	CCoeControl* ComponentControl(TInt aIndex) const; 
	void InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane);
	void SetMenuL(void);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	void HandleViewCommandL(TInt aCommand);
	TInt GetSelectedIndexL(void);
	// used from main container
	void GetCurrSelFileL(TDes& aFileName,TInt& aFileType);
	void AddSelectedFilesL(CDesCArray* aArray,TBool AlsoFolders);
	void PageUpDown(TBool aUp);
	TBool SearchOn(void){return iSearchOn;};
	void SelectionWithWildL(const TDesC& aWild);
protected:
	void HandlePointerEventL(const TPointerEvent& aPointerEvent);
	void FileSearchProcessL(TInt aReadCount, const TDesC& aFolderName);
	void FileSearchFinnishL(CFileSearcher* aObject);
	void DialogDismissedL (TInt aButtonId);
private:	
	void GetTimeBuffer(const TDateTime& aDateTime,TDes& aBuffer,TBool aDate);
	void ReFreshNaviL(void);
	void GetFileL(TDes& aFileName,TInt& aFileType,TInt aSeleted);
	void GetFileType(TInt& aTypeId, TInt aIndex);
	TBool AppendFileName(TDes& aFileName,TInt aIndex);
	TInt AddItemsToListL(void);
	void AddFoundItemL(CSFileItem& aItem,CDesCArray& aArray);
	void ChangeViewTypeL(TBool aSearchChange);
	CAknSearchField* CreateFindBoxL(CEikListBox* aListBox,CTextListBoxModel* aModel, CAknSearchField::TSearchFieldStyle aStyle );
	void Draw(const TRect& aRect) const;
	void DisplayListBoxL(void);
	void UpdateScrollBar(CEikListBox* aListBox);
	void SizeChangedForFindBox();
	virtual void SizeChanged();
    void ConstructL(const TRect& aRect,const TDesC& aPath);
    CSearchContainer(CEikButtonGroupContainer* aCba,CSearchValues& aValues,CIconHandler& aIconHandler,CYuccaNavi* aYuccaNavi);
private:
	CEikButtonGroupContainer*		iCba;	
	CEikTextListBox* 				iSelectionBox;//*
	CAknSearchField* 				iFindBox;
	CAknWaitDialog* 				iProgressDialog;
	CIconHandler&					iIconHandler;
	TBool							iExtendedInfoOn,iSearchOn;
	CFileSearcher*					iFileSearcher;
	CSearchValues&					iSearchValues;
	TTime							iSearchTime;	
	CYuccaNavi* 					iYuccaNavi;
    TPoint     iStartPoint;
	TBool      iDragEvent;
    };


#endif // __FILESEARCHING_VIEW_H__
