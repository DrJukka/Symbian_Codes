/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen
	All rights reserved 
*/
#ifndef __YBROWSER_PUBLIC_INTERFACES_H__
#define __YBROWSER_PUBLIC_INTERFACES_H__
   
#include <APMREC.H>
#include <GULICON.H>
#include <COECNTRL.H>
#include <EIKBTGPC.H>
#include <EIKMENUP.H>
#include <BAMDESCA.H>

#include <ecom\ecom.h>

/*
------------- general items  for recognition --------------------------
*/		
	class CFFileTypeItem :public CBase
		{
	public:
		~CFFileTypeItem(){delete iIdString;};
	public:
		HBufC* 	iIdString;
		TInt   	iTypeId;
		};

	class CYBRecognitionResult:public CBase
	{
	public:
		enum TYBRecConfidence
		{
			ERecCertain=KMaxTInt,
			ERecProbable=100,
			ERecPossible=0,
			ERecUnlikely=-100,
			ERecNotRecognized=KMinTInt
		};
		~CYBRecognitionResult(){delete iIdString;};
	public:
		HBufC* 	iIdString;
		TInt   	iConfidence;
		};
/*
------------- recognizer base class  --------------------------
*/					
	class CYBrowserFileRecognizer : public CBase
	{	
	public:
		~CYBrowserFileRecognizer(){REComSession::DestroyedImplementation(iDestructorIDKey);};
	public:	
		virtual void RecognizeFile(CYBRecognitionResult& aResult,const TDesC& aFileName,const TDesC8& aBuffer) = 0;
		virtual TInt MinimumFileSize(void) = 0;
		virtual TInt FileTypeCount(void) = 0;
		virtual void FileTypeL(TInt aIndex,TDes& aFileType) = 0;
		virtual CGulIcon* GetIconL(TInt aIndex, TSize aSize) = 0;
		virtual HBufC* GetCreditsL(void) = 0;
	public:
		TUid iDestructorIDKey;
	};

/*
------------- file handler base class  --------------------------
*/
class CYBrowserFileHandler;
class MYBrowserFileUtils;
class MYBrowserIconUtils;



	class CSupportTypeItem :public CBase
		{
	public:
		~CSupportTypeItem(){delete iIdString;};
	public:
		TUint32 iFlags;
		HBufC* 	iIdString;
		TBool	iPartialSting;
		};
		
	class MYBrowserFileHandlerUtils
	{
	public:	
		enum
		{
			ENoChangesMade,
			EChangesMade
		};
		virtual void HandleExitL(TAny* aFileHandler, TInt aChanges) = 0;
		virtual CEikButtonGroupContainer& GetCba() = 0;
		virtual MYBrowserFileUtils& GetFileUtils() = 0;
		virtual MYBrowserIconUtils& GetIconUtils() = 0;
		
	};

	class CYBrowserBaseHandler : public CCoeControl
	{	
	public: // default constructors	
		virtual void SetUtils(MYBrowserFileHandlerUtils* aExitHandler) = 0;
		virtual void ConstructL() = 0;
		// activate/deactivate handler
		virtual void SetFocusL(TBool aFocus) = 0;
		// commands handling
		virtual void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane) = 0;
    	virtual void HandleCommandL(TInt aCommand) = 0;
    public:
		TUid iDestructorIDKey;
	};
	
	class CYBrowserFileHandler : public CYBrowserBaseHandler
	{	
	public: // default constructors	
		// file opening function.
		virtual void OpenFileL(RFile& aOpenFile,const TDesC& aFileTypeID) = 0;
		virtual void OpenFileL(const TDesC& aFileName,const TDesC& aFileTypeID) = 0;
		virtual void NewFileL(const TDesC& aPath,const TDesC& aFileTypeID,MDesCArray& aFileArray) = 0;
		virtual void NewFileL(const TDesC& aPath,const TDesC& aFileTypeID) = 0;
		virtual void AddFilesL(const TDesC& aFileName,MDesCArray& aFileArray) = 0;
	};


	class CYBrowserFileSender : public CYBrowserBaseHandler
	{	
	public: // default constructors	
		virtual void SendFilesL(MDesCArray& aFileArray) = 0;
	};

/*
---- Selector callback to be implemented by selector caller -----------
*/
	class MYbSelectorCallBack
	{
	public:	//	New methods
		virtual void SelectedFilesL(const CDesCArray& aArray) = 0;
		virtual void SelectedFolderL(const TDesC& aFolderName) = 0;
		virtual void SelectorCancelledL(void) = 0;
	};	
		
/*
------------- File utilities implemented in y-browser ------------------
*/
	class MYBrowserFileUtils
	{
	public:	
		virtual void ShowFileErrorNoteL(const TDesC& aFileName, TInt aError) = 0;
		virtual TInt OpenFileWithHandler(const TDesC& aFileName) = 0;
		virtual TInt OpenFileWithHandler(RFile& aFile) = 0;
		virtual void RecognizeFile(CYBRecognitionResult& aResult,const TDesC& aFileName) = 0;
		virtual void RecognizeData(CYBRecognitionResult& aResult,const TDesC8& aFileData,const TDesC& aFileName) = 0;
		virtual const CFFileTypeItem& GetGeneralFileItem(void) = 0;
		virtual const RPointerArray<CFFileTypeItem>& GetItemTypeArray(void) = 0;
		virtual void GetCurrentPath(TDes& aFullPath) = 0;
		virtual void SetNaviTextL(const TDesC& aText) = 0;
		virtual void StartSelectionViewL(MYbSelectorCallBack* aCallback,TBool aFolderSelector,const TDesC& aFolder,const TDesC& aTitle,CDesCArrayFlat* aTypeFilter) = 0;
	};
/*
------------- Icon utilities implemented in y-browser ------------------
*/
	class MYBrowserIconUtils
	{
	public:
		virtual TInt GetIconIndex(const TDesC& aIdString) = 0;
		virtual CGulIcon* GetIconL(TInt aIndex, TSize aSize) = 0;
		virtual CGulIcon* GetIconL(const TDesC& aIdString, TSize aSize) = 0;
		virtual CArrayPtr<CGulIcon>* GetIconArrayL(TSize aSize) = 0;
		virtual CArrayPtr<CGulIcon>* GetSelectionIconArrayL(TSize aSize) = 0;
	};

	
	


#endif //__YBROWSER_PUBLIC_INTERFACES_H__
