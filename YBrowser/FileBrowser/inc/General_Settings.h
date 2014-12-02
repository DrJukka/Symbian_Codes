/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen
	All rights reserved 
*/
#ifndef __GENERAL_SETTINGS_H__
#define __GENERAL_SETTINGS_H__


#include <coecntrl.h>
#include <S32FILE.H>

#include "Yucca_Settings.h"
#include "Folder_reader.h"

class CIconHandler;



_LIT(KtxFormatShortCutlList			,"<%d>: %S");

/*! 
  @class CGeneralSettings
  
  @discussion An instance of the Application View object for the HelloWorld 
  example application
  */
   
  
class CGeneralSettings : public CBase 
    {
public:
    static CGeneralSettings* NewL(void);
    static CGeneralSettings* NewLC(void);
    ~CGeneralSettings();
public:  
	void SetFolderReader(CFFolderReader* aFFolderReader){iFFolderReader = aFFolderReader;};
	void SetIconHandler(CIconHandler* aIconHandler){iIconHandler = aIconHandler;};
	void AddSCNumbertoMenuL(CEikMenuPane* aMenuPane);
	void ShowPasteFilesL(CDesCArray* aArray,const TDesC& aTitle);
    void GetValuesL(CDictionaryFileStore* aDStore,TInt aUID,TInt& aValue);
    void GetCommandSCFromStoreL(void);
    void GetSettingsValuesL(TSettingsItem& aSettings);
    void SetSettingsValuesL(TSettingsItem& aSettings);
    void GetPathL(TDes& aFolder);
    void GetIconNameL(TDes& aFileName);
    void SaveIconNameL(const TDesC& aFileName);
    TKeyResponse SelectAndExecuteSCCommandL(TInt aKey);
    void ShowInfoL(const TDesC& aFileName,TInt aFileType);
    void GetTimeBuffer(const TDateTime& aDateTime,TDes& aBuffer,TBool aDate,TBool aList);
    TBool ShowDisclaimerL(void);
    TBool SelectAndChangeIconL(void); // need to pass icon handler to the class..
    void SavePathL(const TDesC& aPath);
    TKeyResponse MoveToShortCutL(TInt aShortCut, TDes& aNewPath);
    TInt ShowShortCutListL(void);
    void SetShortCutL(TInt aShortCut, const TDesC& aPath);
    void GetCommandText(TDes& aTxtx,TInt aSC);
    TInt SwipeLeft(){return iSwipeLeft;};
    TInt SwipeRight(){return iSwipeRight;};
    TInt LongPress(){return iLongPress;};
    TTimeIntervalMicroSeconds32 LongPressTimeOut(){return TTimeIntervalMicroSeconds32(1500000);};
private:	

    void ConstructL();
    CGeneralSettings();       
private:
	CIconHandler* 			iIconHandler;
	CFFolderReader*			iFFolderReader;	
	CArrayFixFlat<TInt>*	iCommandSCArray;
	TInt                    iSwipeLeft,iSwipeRight,iLongPress;
    };


#endif // __GENERAL_SETTINGS_H__
