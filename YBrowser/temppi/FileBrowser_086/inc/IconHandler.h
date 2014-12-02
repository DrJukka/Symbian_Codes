/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen
	All rights reserved 
*/
#ifndef __YB_ICONHANDLER_H__
#define __YB_ICONHANDLER_H__

#include <COECNTRL.H>
#include <W32STD.H>
#include <aknserverapp.h>

#include "Public_Interfaces.h"

// flag defines

const TInt EYBFilehandlerSupportsRFile		 		= 0x0001; // can open file with opened RFile

const TInt EYBFilehandlerSupportsNewFileNoFiles		= 0x0010; // New file function can be called without files, "File->New File"
const TInt EYBFilehandlerSupportsNewFileMultiple	= 0x0020; // New file can have array of files "Edit->Paste to new file"
const TInt EYBFilehandlerSupportsAddFiles	 		= 0x0040; // Files can be added to existing file "Edit->Paste to file"

const TInt EYBFilehandlerSupportsOpenWith	 		= 0x0100; // will be added to 'Open with' menu "File->Open with"
const TInt EYBFilehandlerAddtoFoldersList	 		= 0x0200; // will be added to folders list. i.e. C,D,E,Z + folders list

const TInt EYBFilehandlerSupportsSendFiles	 		= 0x0400; // will be added to 'Open with' menu "File->Open with"
const TInt EYBFilehandlerSupportsRecvFiles	 		= 0x0800; // will be added to folders list. i.e. C,D,E,Z + folders list


//_LIT(KtxGeneralFile					,"General file");

/*
------------ Handler opaque data strings ---------
*/

_LIT8(KtxStrtYTools					,"<YTOOLS>");
_LIT8(KtxEnddYTools					,"</YTOOLS>");

_LIT8(KtxStrtVERSION				,"<VERSION>");
_LIT8(KtxEnddVERSION				,"</VERSION>");
_LIT8(KtxCurrVERSION				,"080");

_LIT8(KtxStrtImpementation			,"<IMPLEMENTATION>");
_LIT8(KtxEnddImpementation			,"</IMPLEMENTATION>");

_LIT8(KtxStrtTYPE					,"<TYPE>");
_LIT8(KtxEnddTYPE					,"</TYPE>");

_LIT8(KtxStrtSUPPORT				,"<SUPPORT>");
_LIT8(KtxEnddSUPPORT				,"</SUPPORT>");

_LIT8(KtxStrtFILETYPE				,"<FILETYPE>");
_LIT8(KtxEnddFILETYPE				,"</FILETYPE>");

_LIT8(KtxStrtFTPartial				,"<TYPEPARTIAL>");
_LIT8(KtxEnddFTPartial				,"</TYPEPARTIAL>");

_LIT8(KtxStrtRFILE					,"<RFILE>");
_LIT8(KtxEnddRFILE					,"</RFILE>");

_LIT8(KtxStrtNEWEMPY				,"<NEWEMPY>");
_LIT8(KtxEnddNEWEMPY				,"</NEWEMPY>");

_LIT8(KtxStrtNEWWITHFILES			,"<NEWWITHFILES>");
_LIT8(KtxEnddNEWWITHFILES			,"</NEWWITHFILES>");

_LIT8(KtxImpFILESENDER				,"FILESENDER");
_LIT8(KtxImpFILERECEIVER			,"FILERECEIVER");

_LIT8(KtxImpFILEHANDLER				,"FILEHANDLER");
_LIT8(KtxImpFILEFOLDER				,"FILEFOLDER");
_LIT8(KtxImpOPENWITH				,"OPENWITH");



class CGulIcon;
class CFFileItem;
class CDocumentHandler;	
	
static const TUid KUidYHandlerUID = {0x20009991};
static const TUid KUidYRecogzrUID = {0x20009992};
	
	class CIconMapping :public CBase
		{
	public:
		TBool	iSystemApp;
		TUid 	iIconUid;
		TInt   	iIndex;
		};
		
	class CFHandlerItem :public CBase
		{
	public:
		~CFHandlerItem(){delete iName;iSupArray.ResetAndDestroy();};
	public:
		TUint32 iFlags;
		TUid   	iUid;
		HBufC* 	iName;
		RPointerArray<CSupportTypeItem> iSupArray;
		};
		

	class MYBrowserFileUtilsHelp
	{
	public:	
		virtual void GetCurrentPath(TDes& aFullPath) = 0;
		virtual void StartSelectionViewL(MYbSelectorCallBack* aCallback,TBool aFolderSelector,const TDesC& aFolder,const TDesC& aTitle,CDesCArrayFlat* aTypeFilter) = 0;
		virtual void AddFileHandler(CYBrowserBaseHandler* aHandler) = 0;
		virtual void SetNaviTextL(const TDesC& aText) = 0;
	};
	
	class CIconHandler: public CBase,public MYBrowserIconUtils
	,public MYBrowserFileUtils
	{
	public:
		void ConstructL(void);
		CIconHandler(MYBrowserFileUtilsHelp& aFileUtilsHelp,MAknServerAppExitObserver* aParent);
		~CIconHandler();
	public:	
		TBool MakeNewFileL(void);
		TBool PasteFilesToHandlerL(MDesCArray& aFileArray);
	public:
		void ReReadIcosL(void);
		void SetIconFile(const TDesC& aFileName){iConFile.Copy(aFileName);};
		void AddExtraFoldersL(RPointerArray<CFFileItem>& aItemArray);
		TBool OpenExtraFoldersL(const TDesC& aName,TInt32& aUID);
		TBool OpenWithHandlerL(CFHandlerItem& aHandler,const TDesC& aFileName,const TDesC& aTypeID);
		TBool OpenWithHandlerL(CFHandlerItem& aHandler,RFile& aFile,const TDesC& aTypeID);
		
		void OpenFileWithL(const TDesC& aFileName);
		void OpenFileWithL(RFile& aFile);
		
		void SendFilesL(MDesCArray& aFileArray,TBool& aSendWithSendUI);
		TBool HasOpenWith(){ return iHasOpenWith;};
		TBool HasPasteMany(){return iHasPasteMany;};
		TBool HasNewFile(){ return iHasNewFile;};
		TBool HasSenders(){ return iHasSenders;};
		TBool HasReceivers(){ return iHasReceivers;};
		void GetSenderReceiverL(RPointerArray<CFHandlerItem>& aArray,TBool aSender);
		void SetUtils(MYBrowserFileHandlerUtils* aExitHandler);
		CGulIcon* GetIconL(TInt aIndex);
		CGulIcon* GetIconL(const TDesC& aIdString);
		CArrayPtr<CGulIcon>* GetIconArrayL(void);
		CArrayPtr<CGulIcon>* GetSelectionIconArrayL(void);
		void ShowNoteL(const TDesC&  aMessage, TBool aError);
	public: //MYBrowserIconUtils
		TInt GetIconIndex(const TDesC& aIdString);
		CGulIcon* GetIconL(TInt aIndex, TSize aSize);
		CGulIcon* GetIconL(const TDesC& aIdString,TSize aSize);
		CArrayPtr<CGulIcon>* GetIconArrayL(TSize aSize);
		CArrayPtr<CGulIcon>* GetSelectionIconArrayL(TSize aSize);
		void StartSelectionViewL(MYbSelectorCallBack* aCallback,TBool aFolderSelector,const TDesC& aFolder,const TDesC& aTitle,CDesCArrayFlat* aTypeFilter);
		void GetCurrentPath(TDes& aFullPath);
		void SetNaviTextL(const TDesC& aText);
	public://MYBrowserFileUtils
		void ShowFileErrorNoteL(const TDesC& aFileName, TInt aError);
		TInt OpenFileWithHandler(const TDesC& aFileName);
		TInt OpenFileWithHandler(RFile& aFile);
		const CFFileTypeItem& GetGeneralFileItem(void){return *iGeneralFile;};
		const RPointerArray<CFFileTypeItem>& GetItemTypeArray(void){return iItemTypeArray;};
		void RecognizeFile(CYBRecognitionResult& aResult,const TDesC& aFileName);
		void RecognizeData(CYBRecognitionResult& aResult,const TDesC8& aFileData,const TDesC& aFileName);
	
	private:
		CGulIcon* GetAppIconL(const TUid& aAppUid,TSize aSize);
		CGulIcon* AddIconFromFileL(const TSize& aIconsize,const TDesC& aFileName, TInt aImg, TInt aMsk);

		void OpenWithApplicationL(const TDesC& aFileName);
		TInt AskOpenFileWithL(RPointerArray<CFHandlerItem>& aArray,const TBool& aAppSelect);
		TBool CheckImplTypeL(const TDesC8& aSupportData,CFHandlerItem& aSupItem);
		void ReadSupportTypesL(const TDesC8& aSupportData,RPointerArray<CSupportTypeItem>& aArray);
		TBool CheckVersionL(const TDesC8& aData);
		TBool CheckSupportedTypesL(const TDesC8& aData,CFHandlerItem& NewItem);
		void GetFileHandlers();
		void GetRecognizers();
		void GetSupportedFilesArrayL();
		void CheckRecognizersL(RImplInfoPtrArray& aArray);
	private:	
		MAknServerAppExitObserver* 				iParent;
		RPointerArray<CFFileTypeItem> 			iItemTypeArray;	
		RPointerArray<CIconMapping> 			iItemIconArray;		
		CFFileTypeItem* 						iGeneralFile;
		RPointerArray<CYBrowserFileRecognizer>	iExtraFileRecognizer;
		MYBrowserFileUtilsHelp&					iYBrowserFileUtilsHelp;
		RPointerArray<CFHandlerItem> 			iFHandlerArray;
		MYBrowserFileHandlerUtils* 				iExitHandler;
		TBool									iHasOpenWith,iHasPasteMany,iHasNewFile,iHasSenders,iHasReceivers;
		CDocumentHandler*						iDocumentHandler;
		TFileName								iConFile;
	};


#endif	//	__YB_ICONHANDLER_H__
