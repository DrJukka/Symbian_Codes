/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen
	All rights reserved 
*/
#ifndef __FILESEARCHER_VIEW_H__
#define __FILESEARCHER_VIEW_H__


#include <coecntrl.h>
#include <AknProgressDialog.h>
#include <aknlists.h> 
#include <aknsfld.h> 


#include "Public_Interfaces.h"

_LIT(KtxDrivesStuff			,":\\;");

_LIT(KtxAllDrives			,"c:\\;d:\\;e:\\;z:\\");
_LIT(KtxStarDotStar			,"*.*");

class CFileSearcher;
class CSearchValues;
class CSFileItem;

	class MMyFileSearchObserver
	{
	public:	//	New methods
		virtual void	FileSearchProcessL(TInt aReadCount, const TDesC& aFolderName) = 0;
		virtual void	FileSearchFinnishL(CFileSearcher* aObject) = 0;
	};
	

class CFileSearcher : public CActive
    {
public:
    CFileSearcher(MMyFileSearchObserver& aObServer,CSearchValues& aSearchValues,MYBrowserFileUtils& aFileUtils,MYBrowserIconUtils& aIconUtils);
    ~CFileSearcher();
	void ConstructL(void);
	TBool SearchIsActive(void);
public:
	void StartTheSearchL(const TDesC& aSearchPath);
protected:
	void DoCancel();
	void RunL();
private:
	void DoCheckOneFileL(void);
	void DoOneFolderSearchL(const TDesC& aSearchPath);
	TBool DoCheckSearchStringL(const TDesC& aFullName);
	TBool OkForSearchConditions(const TDesC& aFullName,TInt& aType);
	void AppendItemL(CSFileItem* aItem);
	void FinnishSearch(void);
private:
	MMyFileSearchObserver& 		iObServer;
	CSearchValues&				iSearchValues;
	MYBrowserFileUtils&			iYBrowserFileUtils;
	MYBrowserIconUtils&			iYBrowserIconUtils;
	RPointerArray<CSFileItem>	iTempArray;
	CDesCArrayFlat* 			iDirArray;
	TBool						iCancel,iReadingFile;
	TInt						iReadFileSoFar;
	TFileName					iStringSearchFile,iCurrFolder;
	TInt 						iSearchFileType,iCurrFile;
	CDir* 						iFile_list;
    };





#endif // __FILESEARCHER_VIEW_H__




