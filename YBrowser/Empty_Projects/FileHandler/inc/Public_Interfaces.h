/* 
Copyright (c) 2007 Dr. Jukka Silvennoinen www.DrJukka.com

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. THE SOFTWARE IS ALSO SUBJECT TO CHANGE WITHOUT PRIOR NOTICE, 
NO COMPATIBILITY BETWEEN MODIFICATIONS IS GUARANTEED. 
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name(s) of the above copyright holders 
shall not be used in advertising or otherwise to promote the sale, use or other 
dealings in this Software without prior written authorization.

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
------------- general File type item ----------------------------------

iIdString is human readable format name for the file type, and is the 
same as used by the OS, (for example for Gif images it is image\gif)

iTypeId is internal index number for the filetype, used for mapping file
types icons and handlers. This index is generated for eachtime Y-Browser
is run, thus filetypes will have different index values depending on 
the different conditions 
-----------------------------------------------------------------------
*/		
	class CFFileTypeItem :public CBase
		{
	public:
		~CFFileTypeItem(){delete iIdString;};
	public:
		HBufC* 	iIdString;
		TInt   	iTypeId;
		};

/*
------------- general items  for recognition --------------------------
Used by Y-Browser when recognizing files with plug-in recognizers

iIdString is the human readable format name for the file type, and should
be the  same as used by the OS, (for example for Gif images it is image\gif)

iConfidence is TYBRecConfidence value given by the recognizer for the
recognition result. Is used to determine the most likely filetype
-----------------------------------------------------------------------
*/
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
------------- recognizer base class  ----------------------------------
-----------------------------------------------------------------------
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
-----------------------------------------------------------------------
just some forward declarations..
-----------------------------------------------------------------------
*/
class CYBrowserFileHandler;
class MYBrowserFileUtils;
class MYBrowserIconUtils;

/*
------------- file handler base class  --------------------------------
Collection of Y-Browser handler utilities implemented in the Y-Browser

HandleExitL() has to be called when filehandler plug-in wants to exit
- aFileHandler, should be the this-pointer of the handler
- aChanges should identify if any changes on the file system was made
	so Y-Browser would know if it needs to read the current folder again

CEikButtonGroupContainer() gives current button group, so you can change your own on

GetFileUtils() gives MYBrowserFileUtils instance explained later

GetIconUtils() gives MYBrowserIconUtils instance explained later

-----------------------------------------------------------------------
*/
		
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
/*
------------- file handler base class  --------------------------------
All filehandlers are derived from this definition

SetUtils() should save the exithandler pointer as  a class member, 
	so it can be used later

ConstructL() is second leaving contructor, do all necessary leaving 
	initialization in this functio n

SetFocusL() called by Y-Browser when plug-in loses/gain focus, thus do
	any necessary functionality in this function
	
DynInitMenuPaneL() normal dynamic menu initialization function, 
	check SDK docs for more information
	
HandleCommandL() called by Y-Browser to offer a menu command to the plug-in

iDestructorIDKey is normal Ecom destructor key

-----------------------------------------------------------------------
*/
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

/*
------------- file handler class  -------------------------------------

This is the base calss from where all normal file handlers needs to derive from

OpenFileL() is called by Y-Browser to tell the plug-in to open the file (either name or opened RFile)

NewFileL() is called by Y-Browser to tell the plug-in to create a new file, 
	either empty or something that includes the files in the array
	
AddFilesL()	is called by Y-Browser to tell the plug-in to add files into a existing file

-----------------------------------------------------------------------
*/	
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

/*
------------- file sender class  --------------------------------------
This is the base class from where all file senders needs to derive from
basically file senders are the plug-ins that are shown in "Send via" menu.

SendFilesL() is called by Y-Browser to tell the plug-in the array of the 
	files to be send with this plug-in

-----------------------------------------------------------------------
*/
	class CYBrowserFileSender : public CYBrowserBaseHandler
	{	
	public: // default constructors	
		virtual void SendFilesL(MDesCArray& aFileArray) = 0;
	};

/*
---- Selector callback to be implemented by selector caller -----------
this is the callback interface which Y-Browser uses when the plug-in
wants to utilize the file/folder-selector implemented by Y-Browser

SelectedFilesL() is called by Y-Browser to tell the plug-in the array of the 
	files selected by the user with Y-Browser file selector

SelectedFolderL() is called by Y-Browser to tell the plug-in the folder 
	selected by the user with Y-Browser folder selector

SelectorCancelledL() is called by Y-Browser to tell the plug-in to tell 
	the plug-in that file/folder selection was cancelled.

-----------------------------------------------------------------------
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
File utility functions provided by the Y-browser for the plug-ins

ShowFileErrorNoteL() can be used to show normal Y-Browser file-error note

OpenFileWithHandler() can be used to open a file with Y-Browser

RecognizeFile() can be used to recognize data or file with Y-Browser file recognizer

GetGeneralFileItem() gives the general file type item, which should be used with all
	files that are not recognized by the Y-Browser file recognizer
	
GetItemTypeArray() gives the whole array of filetypes recognized by Y-Browser

GetCurrentPath() gives the current path Y-Browser is in.

SetNaviTextL() can be used to set the text to the Y-Browser navipanel control

StartSelectionViewL() starts the Y-Browser file/folder selector
	- aCallback is the MYbSelectorCallBack callback interface Y-Browser
		uses to tell the called when the file/folder selector finishes
	- aFolderSelector is ETrue a folder selector is opened, if EFalse file selector is opened
	- aFolder a folder to start with	
	- aTitle a title to show when showing the Y-Browser file/folder selector 
	- aTypeFilter, an array of filetype strings which will be shown, 
		if NULL all filetypes are shown (not used in folder selector of course).

------------------------------------------------------------------------
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
Icon utility functions provided by the Y-browser for the plug-ins

GetIconIndex() gives the index for the given filetype string

GetIconL() constructs an icon for selected index o file type string
			in selected size, no sizing is the aSize is TSize(0,0)
			
GetIconArrayL() construct a icon array for all icons, each icon in the array
				has same index as with the array given with GetItemTypeArray()
				for file types.
				

GetSelectionIconArrayL() selection icon array, which can be used to used
							for example in dialogs to show selections.

------------------------------------------------------------------------
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
