
#ifndef MMSEXAMPLEAPP_H
#define MMSEXAMPLEAPP_H


// INCLUDES
#include <eikapp.h>                         // for CEikApplication
#include <eikdoc.h>                         // for CEikDocument
#include <aknappui.h>
#include "MMSExampleContainer.h" 
#include "ContactReader.h"

const TUid KUidMMSExample = { 0x2002B0B0 }; // MMSExample application UID 

/*
#ifndef __SERIES60_28__
#define __SERIES60_28__
#endif
*/

//
// Application Ui class, this contains all application functionality
//
class CMMSExampleAppUi : public CAknAppUi,MContactReadNotify
    {
public:
    void ConstructL();
    void HandleCommandL(TInt aCommand);   
    ~CMMSExampleAppUi();
    void ShowNoteL(const TDesC& aMessage);
    void GetContactsRTArrayL(RPointerArray<CMyCItem>& aArray);
    RPointerArray<CMyCItem>& GetContactsArrayL(void);
protected:
	void ContactReadDone();
	void ContactReadProgress(TInt aProgress, TInt aTotal);
private:
	static TInt OpenMobileWEBSiteL(TAny* aAny);
	void ReReadContactL(TContactItemId aItem);
    void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);
    void CmdExit();
    void HandleStatusPaneSizeChange();
private:
	CMMSExampleContainer* 	iAppContainer;
	CContactsReader*		iContactsReader;
	TBool 					iContactDone;
    };


//
// Application document class
//
class CMMSExampleDocument : public CEikDocument
    {
public:
    static CMMSExampleDocument* NewL(CEikApplication& aApp); 
    CEikAppUi* CreateAppUiL();
private:
    CMMSExampleDocument(CEikApplication& aApp);
    void ConstructL();
    };

//
// Application class
//
class CMMSExampleApplication : public CEikApplication
    {
public:
    TUid AppDllUid() const;
private:
    CApaDocument* CreateDocumentL();
    };

#endif
