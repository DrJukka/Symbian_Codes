/* 	Copyright (c) 2006, 
	Jukka Silvennoinen,
 	All rights reserved 
*/
// INCLUDE FILES
#include <avkon.hrh>
#include <APPARC.H>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <aknnotewrappers.h>

#include <ecom.h>
#include <implementationproxy.h>

#include "YtoolCVcrdRec.h"
#include "CommonStuff.h"
#include "icons.mbg"


#include <BldVariant.hrh>

#ifdef __SERIES60_3X__
	#include <eikstart.h>
#else
#endif

/*
-------------------------------------------------------------------------------
*************************** CYBRecognizer1 *************************************
-------------------------------------------------------------------------------
*/
/*
-------------------------------------------------------------------------------
tells Y-Browser the minumium size of the buffer that can be recognized
if file to be recognized is smaller than returned value, it will not be 
aske to be recognized
-------------------------------------------------------------------------------
*/
TInt CYBRecognizer1::MinimumFileSize(void)
{
	return 8;
}
/*
-------------------------------------------------------------------------------
tells Y-Browser how many file types this recognizer can recognize
-------------------------------------------------------------------------------
*/
TInt CYBRecognizer1::FileTypeCount(void)
{
	return 1;
}
/*
-------------------------------------------------------------------------------
Credits, will be shown in addOns->recognizer->about
-------------------------------------------------------------------------------
*/

HBufC* CYBRecognizer1::GetCreditsL(void)
{
	return KtxRec1Abbout().AllocL();
	
}
/*
-------------------------------------------------------------------------------
tells Y-Browser the human readable name of the filetype
shown in file type selection dialogs (like the include/exclude types in file search)
-------------------------------------------------------------------------------
*/
HBufC* CYBRecognizer1::FileTypeNameL(TInt aIndex)
{
	TBuf<100> aName;
	switch(aIndex)	
	{
	case 0:
		aName.Copy(KtxTypeName);
		break;
	}
	
	return aName.AllocL();
}
/*
-------------------------------------------------------------------------------
returns the filetype ID for the recognized files
-------------------------------------------------------------------------------
*/
HBufC* CYBRecognizer1::FileTypeL(TInt aIndex,TBool& aPartial)
{
	HBufC* Ret(NULL);
	aPartial = EFalse;
	
	switch(aIndex)	
	{
	case 0:
		Ret = KtxType().AllocL();
		break;
	}
	
	return Ret;
}


/*
-------------------------------------------------------------------------------
icons used for the file type in Y-Browser 
-------------------------------------------------------------------------------
*/
CGulIcon* CYBRecognizer1::GetIconL(TInt aIndex, TSize aSize)
{
	CGulIcon* RetIcon(NULL);
	
	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone == AppFile.FindByDir(KtxIconsFileName, KNullDesC))
	{
		switch(aIndex)
		{
		case 0:
			RetIcon = GetListIconL(AppFile.File(),EMbmIconsZip,EMbmIconsZip_m,aSize);
			break;
		};
	}
	
	return RetIcon;
}

/*
-------------------------------------------------------------------------------
internal icon re-sizer function
-------------------------------------------------------------------------------
*/   
CGulIcon* CYBRecognizer1::GetListIconL(const TDesC& aFileName,TInt aImage,TInt aMask, TSize aSize)
{
	CGulIcon* RetIcon(NULL);
	TBool OkToAdd(EFalse);
	
	CFbsBitmap* MyBitmap = new(ELeave)CFbsBitmap();
	CleanupStack::PushL(MyBitmap);
	CFbsBitmap* MyMask = new(ELeave)CFbsBitmap();
	CleanupStack::PushL(MyMask);
	
	if(KErrNone == MyBitmap->Load(aFileName,aImage))
	{
		if(KErrNone == MyMask->Load(aFileName,aMask))
		{
			OkToAdd = ETrue;
		}
	}
	
	if(OkToAdd)
	{
		TSize ImgSiz = MyBitmap->SizeInPixels();
		if(aSize.iWidth != ImgSiz.iWidth
		|| aSize.iHeight!= ImgSiz.iHeight)
		{
			CFbsBitmap* TmpBackBitmap = new(ELeave)CFbsBitmap();
			CleanupStack::PushL(TmpBackBitmap);
			
			if(KErrNone == TmpBackBitmap->Create(aSize,MyBitmap->DisplayMode()))
			{	
				CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(TmpBackBitmap);
				CleanupStack::PushL(bitmapDevice);

				CFbsBitGc* graphicsContext = NULL;
				User::LeaveIfError(bitmapDevice->CreateContext(graphicsContext));
				CleanupStack::PushL(graphicsContext);

				graphicsContext->DrawBitmap(TRect(0,0,aSize.iWidth,aSize.iHeight),MyBitmap);
				
				CleanupStack::PopAndDestroy(2);//graphicsContext,bitmapDevice,
			}
			
			CFbsBitmap* TmpBackMask = new(ELeave)CFbsBitmap();
			CleanupStack::PushL(TmpBackMask);
			if(KErrNone == TmpBackMask->Create(aSize,MyMask->DisplayMode()))
			{	
				CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(TmpBackMask);
				CleanupStack::PushL(bitmapDevice);

				CFbsBitGc* graphicsContext = NULL;
				User::LeaveIfError(bitmapDevice->CreateContext(graphicsContext));
				CleanupStack::PushL(graphicsContext);

				graphicsContext->DrawBitmap(TRect(0,0,aSize.iWidth,aSize.iHeight),MyMask);
				
				CleanupStack::PopAndDestroy(2);//graphicsContext,bitmapDevice,
			}
		
			CleanupStack::Pop(2);//TmpBackBitmap, TmpBackMask
			RetIcon = CGulIcon::NewL(TmpBackBitmap, TmpBackMask);
		}
	}
	
	if(!RetIcon && OkToAdd)
	{
		CleanupStack::Pop(2);//MyBitmap, MyMask
		RetIcon = CGulIcon::NewL(MyBitmap, MyMask);
	}
	else
	{
		CleanupStack::PopAndDestroy(2);//MyBitmap, MyMask
	}
		
	return RetIcon;
}
/*
-------------------------------------------------------------------------------
Y-Browser will use this function to recognize file data

NOTE: This is non-leaving functions, to do not leave...
-------------------------------------------------------------------------------
*/
void CYBRecognizer1::RecognizeFile(CYBRecognitionResult& aResult,const TDesC& /*aFileName*/,const TDesC8& aBuffer)
{									
	if(aResult.iIdString)
	{
		aResult.iIdString->Des().Zero();
	}
	
	aResult.iConfidence = CYBRecognitionResult::ERecNotRecognized;
	
	
	if(aBuffer.Length() >= 12)
	{
		TBool StartOk(EFalse);
	
		if(aBuffer[0] == 80)
			if(aBuffer[1] == 0)
				if(aBuffer[2] == 0)
					if(aBuffer[3] == 16)
						if(aBuffer[4] == 228)
							if(aBuffer[5] == 0)
								if(aBuffer[6] == 0)
									if(aBuffer[7] == 16)
									{
										StartOk = ETrue;	
									}
				
		if(StartOk)
		{// Check this UID
			if(aBuffer[8] == 184)
				if(aBuffer[9] == 147)
					if(aBuffer[10] == 32)
						if(aBuffer[11] == 16)
						{		
							aResult.iConfidence = CYBRecognitionResult::ERecProbable;
										
							delete aResult.iIdString;
							aResult.iIdString = NULL;
							aResult.iIdString = KtxType().Alloc();
						}
					
		}

	}
}
/*
-------------------------------------------------------------------------
interface function for getting the implementation instance
-------------------------------------------------------------------------
*/
CYBrowserFileRecognizer* NewRecognizer()
    {
    return (new CYBRecognizer1);
    }
/*
-------------------------------------------------------------------------
ECom ImplementationTable function

See SDK documentations for more information
-------------------------------------------------------------------------
*/
#ifdef __SERIES60_3X__
LOCAL_D const TImplementationProxy ImplementationTable[] = 
{
	IMPLEMENTATION_PROXY_ENTRY(0x0F6D,NewRecognizer)
};
#else

// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
    {
    {{0x0F6D}, NewRecognizer}
    };
    
// DLL Entry point
GLDEF_C TInt E32Dll(TDllReason /*aReason*/)
    {
    return(KErrNone);
    }
#endif


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

