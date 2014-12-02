/*
	Copyright (C) 2006 Jukka Silvennoinen
	
*/

// INCLUDE FILES
#include <avkon.hrh>
#include <APPARC.H>
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
#include <eikstart.h>

#include "YTools_GIFUI.h"
#include "YTools_GIFUI.hrh"
#include "YTools_GIFUI_res.rsg"


/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
CYBrowserFileHandler1::~CYBrowserFileHandler1()
{	
	CEikonEnv::Static()->DeleteResourceFile(iResId);
	delete iGif_Reader;
	
	REComSession::DestroyedImplementation(iDestructorIDKey);
}
/*
-------------------------------------------------------------------------
this will be called right after first constructor has finished
there shouldn't be any need to change this one.
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::SetUtils(MYBrowserFileHandlerUtils* aFileHandlerUtils)
{
	iFileHandlerUtils = aFileHandlerUtils;
}


/*
-------------------------------------------------------------------------
This will be called when this file handler is preraped to be used for, 
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
 		iResId = CEikonEnv::Static()->AddResourceFileL(AppFile.File());
	}
	
	SetFocusL(ETrue);
 	ActivateL();
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
		SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
		
		iFileHandlerUtils->GetCba().SetCommandSetL(R_MAIN_CBA);				
		iFileHandlerUtils->GetCba().DrawDeferred();
		DrawNow();	
	}
}

/*
-------------------------------------------------------------------------
we only support one file type, so we don't care what aFileTypeID is
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::OpenFileL(const TDesC& aFileName,const TDesC& /*aFileTypeID*/)	
{	
	iFileHandlerUtils->GetFileUtils().SetNaviTextL(aFileName);	
	
	iGif_Reader = new(ELeave)CGif_Reader(*this);
	iGif_Reader->ConstructL(aFileName);
}
/*
-------------------------------------------------------------------------
Handle command called by Y-Browser,make sure you set your menu & CBA's 
when focus chnages, and that your commands are started at least with
first command as 0x7000.

-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::HandleCommandL(TInt /*aCommand*/)
{
	iFileHandlerUtils->HandleExitL(this,MYBrowserFileHandlerUtils::ENoChangesMade);	
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
 	gc.Clear(aRect);

 	if(iGif_Reader)
 	{
 		if(iGif_Reader->Bitmap())
 		{
 			if(iGif_Reader->Bitmap()->Handle())
 			{
 				gc.DrawBitmap(aRect,iGif_Reader->Bitmap());		
 			}
 		}	
 	}
 	
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
	IMPLEMENTATION_PROXY_ENTRY(0x0F63,NewFileHandler)
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

