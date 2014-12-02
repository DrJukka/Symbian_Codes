/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen
	All rights reserved 
*/

#ifndef __SCHED_LIST_H__
#define __SCHED_LIST_H__

#include <e32base.h>
#include <F32FILE.H>
#include <d32dbms.h>

#include "IconHandler.h"
#include "Yucca_Settings.h"

const TInt KMinFileCountForProgress    = 25;

//_LIT(KtxFormatReadingFiles			,"Reading: %d/%d");

_LIT(KtxEmpty						,"");
_LIT(KtxDoubleSlash					,"\\");

_LIT(KtxFormatInfoDateList			,"%S.%S.%4d");
_LIT(KtxFormatInfoTimeList			,"%S:%S:%S");

_LIT(KtxShortListFormat				,"%d\t%S");
_LIT(KtxLongListFormatFolder		,"%d\t%S\t%S - %d,%d Entries");
_LIT(KtxLongListFormatDriveMB		,"%d\t%S\t%d / %d Mb");
_LIT(KtxLongListFormatDriveKb		,"%d\t%S\t%d / %d Kb");
_LIT(KtxLongListFormatDriveB		,"%d\t%S\t%d / %d Bytes");

_LIT(KtxLongListFormatFileMb		,"%d\t%S\t%S - %d Mb");
_LIT(KtxLongListFormatFileKb		,"%d\t%S\t%S - %d Kb");
_LIT(KtxLongListFormatFileB			,"%d\t%S\t%S - %d Bytes");
			
_LIT(KtxNoFile						,"<No file>");

class CFFolderReader;
	
	
	class CFFileItem :public CBase
		{
	public:
		enum TFFileType
		{
			EFExtraFolder = 1,
			EFDriveFile,
			EFFolderFile,
			EFJustFile
		};
		~CFFileItem(){delete iName;};
	public:
		TInt32 	iType;
		HBufC* 	iName;
		TInt32 	iSize,iExtra;
		TInt32 	iFolders,iFiles;
		TTime 	iModified;
		};
	

	class MMyFolderReaderObserver
	{
	public:	//	New methods
		virtual void	FolderReadOnProcessL(TInt aReadCount,TInt aFullCount, TInt aError) = 0;
		virtual void	FolderReadFinnishL(CFFolderReader* aObject, TInt aError) = 0;
	};
		
		
	class COldArrays :public CBase
		{
	public:
		~COldArrays(){delete iPath; iItemArray.ResetAndDestroy();};
		COldArrays():iPath(NULL){};
	public:
		HBufC* 						iPath;
		RPointerArray<CFFileItem> 	iItemArray;
		};
		
	
	struct TFReadSettings
	{
		TBool	iShowHiddenSystem,iOptimize,iDoRecocnize;
		TUint	iEntryMask,iEntrySort;
	};
	
	class CFFolderReader : public CActive
	    {
	    
	public:
		static CFFolderReader* NewL(MMyFolderReaderObserver& aObserver,CIconHandler& aIconUtils);
		static CFFolderReader* NewLC(MMyFolderReaderObserver& aObserver,CIconHandler& aIconUtils);
		~CFFolderReader();
		TInt StartReadingFoldersOnlyL(const TDesC& aFolder, TInt& aSave);
		TInt StartReadingL(const TDesC& aFolder, TInt& aSave);
		TBool HasFinishedReading(void){ return iDone;};
		TInt  ErrorCode(void){ return iError;};
		RPointerArray<CFFileItem>& ItemArray(void){ return iItemArray;};
		HBufC* CurrentPath(void){ return iSelPath;};
		void AppendOptimizedToListL(CFFileItem& aItem);
		TBool IsDriveReadOnly(void){ return iDriveReadOnly;};
		void AppendToListL(const TEntry& aEntry,TInt32 aType);
		void RemoveFromListL(const TDesC& aNameAndExt);
		TBool FileTypeOkL(const TDesC& aFileName,const TEntry& aFileEntry,TInt& aIcon);
		TInt RemovePathAndReFreshL(const TDesC& aFolder, TInt& aSave);
		TBool GetFolderFileCountL(const TDesC& aName,TInt32& aFolders,TInt32& aFiles,TBool aFullName);
		void RemoveTypeFilterArrayL(void);
		void SetTypeFilterArrayL(CDesCArrayFlat& aArray);
		void SetSettings(TSettingsItem& aSettings); 
	protected:	
		void DoCancel();
		void RunL();
	private:
		TInt DoStartReadingL(const TDesC& aFolder, TInt& aSave);
		void AppendDriveToListL(TBool AddMe,const TDesC& aName,TInt32 aSize,TInt32 aFree);
		CFFolderReader(MMyFolderReaderObserver& aObserver,CIconHandler& aIconUtils);
	    void AddDriveListL(void);
	    void ConstructL(void);
		void DoOneReadL(void);
		void FinnishReadL(void);
		void ShowNoteL(const TDesC&  aMessage, TBool aError);
	private:
		MMyFolderReaderObserver& 		iObserver;
		CIconHandler&					iIconHandler;
		TBool 							iCancel,iDone,iDriveReadOnly,iFoldersOnly;
		CDir* 							iFile_list;
		HBufC* 							iSelPath;
		TInt 							iReadNow,iError,iHadError;
		RPointerArray<COldArrays>		iOldArray;
		RPointerArray<CFFileItem> 		iOptimizeArray;
		RPointerArray<CFFileItem> 		iItemArray;
		RFs 							iFsSession;
		TFReadSettings					iSettings;
		TInt							iCount1,iCount2;
		CDesCArrayFlat*					iTypeFilterArray;	    
	    };


#endif // __SCHED_LIST_H__
