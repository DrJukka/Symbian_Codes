/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen
	All rights reserved 
*/
#ifndef __FILEMANHNADLER_H__
#define __FILEMANHNADLER_H__
   
#include <eikenv.h>
#include <basched.h>
#include <badesca.h>
#include <coecntrl.h>
#include <coeutils.h>
#include <eikfutil.h>
#include <eikappui.h>
#include <e32keys.h>
#include <eikdef.h>
#include <eikmenup.h>
#include <eikdialg.h>
#include <eikon.hrh>
#include <AknProgressDialog.h>

#include <eikon.rsg>
#include <eikapp.h>
#include <eikdoc.h>


class CEikFileHandler;
class MYBrowserFileUtils;

	class MMyFileManReaderObserver
	{
	public:	//	New methods
		virtual void FileManOperatioDoneL(CEikFileHandler* aObject,TInt aOperation, TInt aError,TFileManError aErrorInfo) = 0;
	};
	
	
	
///////////////////////////////////////////////////////////////////////////////////////////////////
enum TEikFileManagementOperation 
	{
	EEikFCopy, EEikFDelete, EEikFMove, EEikFRename,
	EEikNewFolder, EEikFormatDisk
	};


class CEikFileHandler : public CActive,MProgressDialogCallback 
	{
public:
	CEikFileHandler(MMyFileManReaderObserver& aObserver,MYBrowserFileUtils& aFileHandler);
	~CEikFileHandler();
	void ConstructL(void);
	CFileMan* FileMan() {return iFileMan;};
	virtual void SetOperationL(TInt aOperation, const CDesCArray* aSourceFullNameArray, const TFileName* aTargetFullName);
	virtual void Start();
protected:	
	void DialogDismissedL (TInt aButtonId);				
	virtual TInt PerformOperationL();
	virtual void HandleFileServerErrorL(TInt aAdditionalError);
protected: // from CActive
	virtual void DoCancel();
	virtual void RunL();
private:
	void CheckAndChnageReadOnlyL(const TDesC& aSourceFile);
	void FileManStartedL(void);
	void FileManEnded(void);
	TBool DoSpecialCopyL(const TDesC& aSourceFile,const TDesC& aDestFile, TBool aCopy);
	TBool DoSpecialDeleteL(const TDesC& aSourceFile);
	TBool DoForceCopyL(const TDesC& aSourceFile,const TDesC& aDestFile);
	void HandleOperationEndedL(void);
	TInt IsFolder(const TDesC& aFullName, TBool& aIsFolder);
private:	
	MMyFileManReaderObserver& 	iObserver;
	MYBrowserFileUtils& 		iYBrowserFileUtils;
	CFileMan* 					iFileMan;
	RFs& 						iFsSession;
	CAknProgressDialog* 		iProgressDialog;
	CEikProgressInfo* 			iProgressInfo;
	TInt 						iOperation,iIndex,iLastError,iHadError;
	const CDesCArray* 			iSourceFullNameArray;
	const TFileName* 			iTargetFullName;
	TBool 						iCancel;
	TFileManError 				iErrorInfo;
	TFileName 					iCurrentSource;
	TFileName 					iCurrentTarget;
	};
 

#endif //__FILEMANHNADLER_H__
