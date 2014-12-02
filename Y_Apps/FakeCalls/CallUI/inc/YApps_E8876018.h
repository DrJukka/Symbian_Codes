/*
* ==============================================================================
* ==============================================================================
*/

#ifndef __CallEffectsAPPUI_H__
#define __CallEffectsAPPUI_H__

// INCLUDES
#include <aknappui.h>
#include <akndoc.h>
#include <aknapp.h>

#include "Caller_Active.h" 
#include "Ps_Observer.h"
#include "CalAlarm_Observer.h"

// FORWARD DECLARATIONS
class CPlayContainer;



// CLASS DECLARATION
/**
* CCallEffectsAppUi application UI class.
* Interacts with the user through the UI and request message processing
* from the handler class
*/
class CCallEffectsAppUi : public CAknAppUi,MTelMonCallback,MMyPsObsCallBack,MMyLogCallBack
    {
    public: // Constructors and destructor
        void ConstructL();
        CCallEffectsAppUi();
        ~CCallEffectsAppUi();
    public:
    	void ShowNoteL(const TDesC& aMessage);
    	void OpenFileL(const TDesC& aFileName);
    	TBool IsCallOn(void){return iCallIsOn;};
    	static TInt OpenMobileWEBSiteL(TAny* aAny);
    protected:
    	void DoShowAlarmNoteL(const TDes& aMessage);
    	void DoHideAlarmNoteL(void);    	
    	void PsValueChangedL(TInt aValue,TInt aError);
    	void PsValueChangedL(const TDesC8& aValue,TInt aError);
    	void ProcessMessageL(TUid aUid,const TDesC8& aParams);
    	TBool ProcessCommandParametersL(TApaCommand aCommand,TFileName& aDocumentName,const TDesC8& /*aTail*/);
    	void NotifyChangeInStatus(CTelephony::TCallStatus& aStatus, TInt aError);
	    void HandleForegroundEventL(TBool aForeground);
	    void HandleStatusPaneSizeChange();
    private:  // Functions from base classes
		void HandleCommandL( TInt aCommand );	
		void GetTelephonyStateAndType();
    private: // Data
        CPlayContainer* 	iPlayContainer;
        CTelephonyMonitor*	iTelephonyMonitor;
        CmyPsObserver*		imyPsObserver;
        TBool				iCallIsOn,iAlarmIsOn;
        CCalAlarmObserver*	iCalAlarmObserver;
	};





// CLASS DECLARATION

/**
* CCallEffectsDocument application class.
* An instance of class CCallEffectsDocument is the Document part of the
* AVKON application framework for the CallEffects example application.
*/
class CCallEffectsDocument : public CAknDocument
    {
    public: // Constructors and destructor
        static CCallEffectsDocument* NewL( CEikApplication& aApp );
        static CCallEffectsDocument* NewLC( CEikApplication& aApp );
        virtual ~CCallEffectsDocument();
    public: // Functions from base classes
        CEikAppUi* CreateAppUiL();
        CFileStore* OpenFileL(TBool aDoOpen, const TDesC& aFilename,RFs& aFs);
        
    private: // Constructors
        void ConstructL();
        CCallEffectsDocument( CEikApplication& aApp );
    private:
    	CCallEffectsAppUi* iMyAppUI;
    };
    
    


// CLASS DECLARATION

/**
* CCallEffectsApplication application class.
* Provides factory to create concrete document object.
* An instance of CCallEffectsApplication is the application part of the
* AVKON application framework for the CallEffects example application.
*/
class CCallEffectsApplication : public CAknApplication
    {
    public: // Functions from base classes
        TUid AppDllUid() const;

    protected: // Functions from base classes
        CApaDocument* CreateDocumentL();
    };
    
    
#endif // __CallEffectsAPPUI_H__
