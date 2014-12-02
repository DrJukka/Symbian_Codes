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
		~CFFileTypeItem(){delete iIdString;delete iName;};
	public:
		HBufC* 	iIdString;
		HBufC* 	iName;
		TInt   	iTypeId;
		TBool	iPartialSting;
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
		virtual HBufC* FileTypeL(TInt aIndex,TBool& aPartial) = 0;
		virtual HBufC* FileTypeNameL(TInt aIndex) = 0;
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

// flag defines
const TInt EYBFilehandlerSupportsRFile		 		= 0x0001; // can open file with opened RFile

const TInt EYBFilehandlerSupportsNewFileNoFiles		= 0x0010; // New file function can be called without files, "File->New File"
const TInt EYBFilehandlerSupportsNewFileMultiple	= 0x0020; // New file can have array of files "Edit->Paste to new file"
const TInt EYBFilehandlerSupportsAddFiles	 		= 0x0040; // Files can be added to existing file "Edit->Paste to file"

const TInt EYBFilehandlerSupportsOpenWith	 		= 0x0100; // will be added to 'Open with' menu "File->Open with"
const TInt EYBFilehandlerAddtoFoldersList	 		= 0x0200; // will be added to folders list. i.e. C,D,E,Z + folders list


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
		virtual void HandleExitL(CYBrowserFileHandler* aFileHandler) = 0;
		virtual CEikButtonGroupContainer& GetCba() = 0;
		virtual MYBrowserFileUtils& GetFileUtils() = 0;
		virtual MYBrowserIconUtils& GetIconUtils() = 0;
		
	};

	class CYBrowserFileHandler : public CCoeControl
	{	
	public: // default constructors	
		virtual void SetUtils(MYBrowserFileHandlerUtils* aExitHandler) = 0;
		virtual void ConstructL() = 0;
		// handler settings and support
		virtual HBufC*GetSupportedItemTypes(TBool& aPartialString,TUint32& aFlags, TInt aIndex) = 0;
		virtual TInt GetSupportedItemTypesCount(void) = 0;
		// file opening function.
		virtual void OpenFileL(RFile& aOpenFile,const TDesC& aFileTypeID) = 0;
		virtual void OpenFileL(const TDesC& aFileName,const TDesC& aFileTypeID) = 0;
		virtual void NewFileL(const TDesC& aPath,const TDesC& aFileTypeID,MDesCArray& aFileArray) = 0;
		virtual void AddFilesL(const TDesC& aFileName,MDesCArray& aFileArray) = 0;
		// activate/deactivate handler
		virtual void SetFocusL(TBool aFocus) = 0;
		// commands handling
		virtual void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane) = 0;
    	virtual void HandleCommandL(TInt aCommand) = 0;
    public:
		TUid iDestructorIDKey;
	};


/*
------------- File utilities implemented in y-browser ------------------
*/
	class MYBrowserFileUtils
	{
	public:	
		virtual void ShowFileErrorNoteL(const TDesC& aFileName, TInt aError) = 0;
		virtual TInt OpenFileWithHandler(const TDesC& aFileName) = 0;
		virtual void RecognizeFile(CYBRecognitionResult& aResult,const TDesC& aFileName) = 0;
		virtual void RecognizeData(CYBRecognitionResult& aResult,const TDesC8& aFileData,const TDesC& aFileName) = 0;
		virtual const CFFileTypeItem& GetGeneralFileItem(void) = 0;
		virtual const RPointerArray<CFFileTypeItem>& GetItemTypeArray(void) = 0;
		virtual void GetCurrentPath(TDes& aFullPath) = 0;
		virtual void SetNaviTextL(const TDesC& aText) = 0;
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
