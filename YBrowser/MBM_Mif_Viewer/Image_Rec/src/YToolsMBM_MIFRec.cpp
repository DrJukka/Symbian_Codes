/* 	Copyright (c) 2006 - 2007, 
	Jukka Silvennoinen,
 	All rights reserved 
*/
// INCLUDE FILES
#include <avkon.hrh>
#include <APPARC.H>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <aknnotewrappers.h>
#include <akniconutils.h> 

#include <ecom.h>
#include <implementationproxy.h>

#include "YToolsMBM_MIFRec.h"
#include "CommonStuff.h"
#include "icons.mbg"

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
	return 2;
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
returns the filetype ID for the recognized files
-------------------------------------------------------------------------------
*/

void CYBRecognizer1::FileTypeL(TInt aIndex,TDes& aFileType)
{
	switch(aIndex)	
	{
	case 0:
		aFileType.Copy(KtxTypeIMageMIF);
		break;
	case 1:
		aFileType.Copy(KtxTypeIMageMBM);
		break;
	}
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
			RetIcon = GetListIconL(aSize,AppFile.File(),EMbmIconsMif,EMbmIconsMif_m);
			break;
		case 1:
			RetIcon = GetListIconL(aSize,AppFile.File(),EMbmIconsMbm,EMbmIconsMbm_m);
			break;
		};
	}
	
	return RetIcon;
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
CGulIcon* CYBRecognizer1::GetListIconL(const TSize& aIconsize,const TDesC& aFileName, TInt aImg, TInt aMsk)
{
	CFbsBitmap* FrameMsk(NULL);
	CFbsBitmap* FrameImg(NULL);
	
	if(aMsk >= 0)
	{
		AknIconUtils::CreateIconL(FrameImg,FrameMsk,aFileName,aImg,aMsk);
	}
	else
	{
		FrameImg = AknIconUtils::CreateIconL(aFileName, aImg);
	}
	
	if(FrameImg)
	{
		if(aIconsize.iHeight > 0 && aIconsize.iWidth > 0)
		{
			AknIconUtils::SetSize(FrameImg,aIconsize,EAspectRatioPreserved);  
		}
					
		if(FrameMsk && aIconsize.iHeight > 0 && aIconsize.iWidth > 0)
		{
			AknIconUtils::SetSize(FrameMsk,aIconsize,EAspectRatioPreserved);	
		}
	}
	
	return CGulIcon::NewL(FrameImg,FrameMsk);
} 

/*
-------------------------------------------------------------------------------
Y-Browser will use this function to recognize file data

NOTE: This is non-leaving functions, to do not leave...
-------------------------------------------------------------------------------
*/
void CYBRecognizer1::RecognizeFile(CYBRecognitionResult& aResult,const TDesC& aFileName,const TDesC8& aBuffer)
{									
	if(aResult.iIdString)
	{
		aResult.iIdString->Des().Zero();
	}
	
	aResult.iConfidence = CYBRecognitionResult::ERecNotRecognized;
	
	if(aBuffer.Length() >= 8)
	{
		if(aBuffer[0] == 66)
			if(aBuffer[1] == 35)
				if(aBuffer[2] == 35)
					if(aBuffer[3] == 52)
						if(aBuffer[4] == 2)
							if(aBuffer[5] == 0)
								if(aBuffer[6] == 0)
									if(aBuffer[7] == 0)
									{
										aResult.iConfidence = CYBRecognitionResult::ERecProbable;
										aResult.iIdString = KtxTypeIMageMIF().AllocL();
									}
	
		if (aResult.iConfidence >= CYBRecognitionResult::ERecPossible)
		{
			return ;
		}
		
/*		TBuf<5> Help;
		if(aFileName.Length() > 4)
		{
			Help.CopyLC(aFileName.Right(4));
			if(Help == KTXDotOLP)
			{
				if(aBuffer[3] == 0)
					if(aBuffer[4] == 72)
						if(aBuffer[5] == 14)
							if(aBuffer[6] == 1)
							{
								aResult.iConfidence = CYBRecognitionResult::ERecProbable;
								aResult.iIdString = KtxTypeImageOLP().AllocL();
							}
			}
		
			if (aResult.iConfidence >= CYBRecognitionResult::ERecPossible)
			{
				return ;
			}
		}
*/	}
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
	IMPLEMENTATION_PROXY_ENTRY(0x0F64,NewRecognizer)
};
#else

// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
    {
    {{0x0F64}, NewRecognizer}
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

