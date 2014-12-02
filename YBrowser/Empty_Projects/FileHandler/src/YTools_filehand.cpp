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

// INCLUDE FILES
#include <avkon.hrh>
#include <APPARC.H>
#include <GDI.H>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <aknnotewrappers.h>
#include <BAUTILS.H> 
#include <ecom.h>
#include <implementationproxy.h>
#include <aknmessagequerydialog.h> 
#include <akniconarray.h> 
#include <akntitle.h> 
#include <akncontext.h> 
#include <eikmenub.h> 

#include "YTools_filehand.h"
#include "YTools_filehand.hrh"
#include "YTools_filehand_res.rsg"


#include <eikstart.h>
_LIT(KMyResourceFileName	,"\\resource\\apps\\YTools_filehand_res.RSC");

_LIT(KtxApplicationName		,"File Handler");
_LIT(KtxAbbout				,"version 0.80\nAugust 15th 2007\nfor S60, Symbian OS 9,\n\nCopyright:\nJukka Silvennoinen 2006-07\nAll right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com");

/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
CYBrowserFileHandler1::~CYBrowserFileHandler1()
{	
	CEikonEnv::Static()->DeleteResourceFile(iResId);

	REComSession::DestroyedImplementation(iDestructorIDKey);
}
/*
-------------------------------------------------------------------------
this will be called right after first constructor has finished
there shouldn't be any need to chnage this one.
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::SetUtils(MYBrowserFileHandlerUtils* aFileHandlerUtils)
{
	iFileHandlerUtils = aFileHandlerUtils;
}

/*
-------------------------------------------------------------------------
This will be called when this file shandler is preraped to be used for, 
opening or constructing a file. The open/new etc. functions are called
strait after calling this function.
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::ConstructL()
{
 	CreateWindowL();
 	
 	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone == AppFile.FindByDir(KMyResourceFileName, KNullDesC))
	{
 		TFileName resourceName(AppFile.File());
		BaflUtils::NearestLanguageFile(CEikonEnv::Static()->FsSession(), resourceName);
		iResId = CEikonEnv::Static()->AddResourceFileL(resourceName);
	}
	
	SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect()); 	
	
 	ActivateL();
	SetMenuL();
}

/*
-----------------------------------------------------------------------------
Y-Browser will call setsize for client rect when lay-out or size changes,
This will be then called by the framework
-----------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::SizeChanged()
{

}

/*
-------------------------------------------------------------------------
Y-Browser will call this when focus changes.

i.e. When you ask y-browser to open a file, and it will be opened by
other y-browser handler, then you will be called with focus false, 
right before new handler takes focus.

And when it returns back to y-browser, this function will be called with
ETrue,
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::SetFocusL(TBool aFocus)
{
	
	if(aFocus)
	{			
		CEikStatusPane*		sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
		if(sp)
		{	
			CAknTitlePane* TitlePane = STATIC_CAST(CAknTitlePane*,sp->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
			TitlePane->SetTextL(KtxMyTitle);		
		}
		
		SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
		SetMenuL();
		DrawNow();	
	}
}
/*
-------------------------------------------------------------------------
internal function for updating menu and CBA's for the application
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::SetMenuL(void)
{
	CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(R_MAIN_MENUBAR);	
		
	iFileHandlerUtils->GetCba().SetCommandSetL(R_MAIN_CBA);				
	iFileHandlerUtils->GetCba().DrawDeferred();
}
/*
-------------------------------------------------------------------------
Normal DynInitMenuPaneL, which Y-Browser will call.

See SDK documentation for more information
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::DynInitMenuPaneL(TInt /*aResourceId*/, CEikMenuPane* /*aMenuPane*/)
{

}

/*
-------------------------------------------------------------------------
called by Y-Browser to tell the plug-in to open a file
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::OpenFileL(const TDesC& aFileName,const TDesC& aFileTypeID)	
{	
	if(BaflUtils::FileExists(CCoeEnv::Static()->FsSession(),aFileName))
	{
		// do open the file in here
	}
	else
	{	
		HandleCommandL(EBacktoYBrowser);
	}
}
/*
-------------------------------------------------------------------------
called by Y-Browser to tell the plug-in to open a file

-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::OpenFileL(RFile& /*aOpenFile*/,const TDesC& /*aFileTypeID*/)
{
	HandleCommandL(EBacktoYBrowser);
}
/*
-------------------------------------------------------------------------
called by Y-Browser to tell the plug-in to make a new file
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::NewFileL(const TDesC& /*aPath*/,const TDesC& /*aFileTypeID*/)
{
	HandleCommandL(EBacktoYBrowser);
}
/*
-------------------------------------------------------------------------
called by Y-Browser to tell the plug-in to make a new file
-------------------------------------------------------------------------
*/

void CYBrowserFileHandler1::NewFileL(const TDesC& /*aPath*/,const TDesC& /*aFileTypeID*/,MDesCArray& /*aFileArray*/)
{
	HandleCommandL(EBacktoYBrowser);
}
/*
-------------------------------------------------------------------------
called by Y-Browser to tell the plug-in to add file to a existing file
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::AddFilesL(const TDesC& /*aFileName*/,MDesCArray& /*aFileArray*/)
{	
	HandleCommandL(EBacktoYBrowser);
}
/*
-------------------------------------------------------------------------
Handle command called by Y-Browser,make sure you set your menu & CBA's 
when focus chnages, and that your commands are started at least with
first command as 0x7000.

-------------------------------------------------------------------------
*/

void CYBrowserFileHandler1::HandleCommandL(TInt aCommand)
{
	switch (aCommand)
    { 
    case EAbout:
    	{
    		HBufC* Abbout = KtxAbbout().AllocLC();
			TPtr Pointter(Abbout->Des());
			CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(Pointter);
			dlg->PrepareLC(R_ABOUT_HEADING_PANE);
			dlg->SetHeaderTextL(KtxApplicationName);  
			dlg->RunLD();
			
			CleanupStack::PopAndDestroy(Abbout);
    	}
    	break;
    case EBacktoYBrowser:
    	if(iFileHandlerUtils)
    	{
    		iFileHandlerUtils->HandleExitL(this,MYBrowserFileHandlerUtils::ENoChangesMade);	
    	}
    	break; 
	default:
		break;
    };
}


/*
-----------------------------------------------------------------------
normal implementation of the OfferKeyEventL, called  by Y-Browser when in focus

See SDK documentations for more information
-----------------------------------------------------------------------
*/
TKeyResponse CYBrowserFileHandler1::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	TKeyResponse Ret = EKeyWasNotConsumed;
	
	switch (aKeyEvent.iCode)
    {
	case EKeyDevice3:
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->TryDisplayMenuBarL();
		break;
	case EKeyRightArrow:
	default:
		break;
    };
	
	return Ret;
}
/*
-----------------------------------------------------------------------
just normal Draw

See SDK documentations for more information
-----------------------------------------------------------------------
*/
void CYBrowserFileHandler1::Draw(const TRect& aRect) const
{
 	CWindowGc& gc = SystemGc();
 	gc.SetBrushColor(KRgbGreen);
 	gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
 	gc.Clear(aRect);
}
/*
-------------------------------------------------------------------------
just normal CountComponentControls

See SDK documentations for more information
-------------------------------------------------------------------------
*/
TInt CYBrowserFileHandler1::CountComponentControls() const
{
	return 0;
}
/*
-------------------------------------------------------------------------
just normal ComponentControl

See SDK documentations for more information
-------------------------------------------------------------------------
*/
CCoeControl* CYBrowserFileHandler1::ComponentControl(TInt aIndex) const
{
	return NULL;
}
	
/*
-------------------------------------------------------------------------
interface function for getting the implementation instance
-------------------------------------------------------------------------
*/
CYBrowserFileHandler* NewFileHandler()
    {
    return (new CYBrowserFileHandler1);
    }
/*
-------------------------------------------------------------------------
ECom ImplementationTable function

See SDK documentations for more information
-------------------------------------------------------------------------
*/

LOCAL_D const TImplementationProxy ImplementationTable[] = 
{
	IMPLEMENTATION_PROXY_ENTRY(0x3135,NewFileHandler)
};


/*
-------------------------------------------------------------------------
ECom ImplementationGroupProxy function

See SDK documentations for more information
-------------------------------------------------------------------------
*/
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
{
	aTableCount = sizeof(ImplementationTable)/sizeof(ImplementationTable[0]);
	return ImplementationTable;
}

