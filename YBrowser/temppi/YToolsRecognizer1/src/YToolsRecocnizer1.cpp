/* 	Copyright (c) 2001 - 2006, 
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


#include "YToolsRecocnizer1.h"


#ifdef __SERIES60_3X__
	#include <eikstart.h>
#else
#endif

_LIT(KtxIconsFileName		,"\\system\\data\\A00007A6\\A0000F6B\\icons.mbm");

_LIT(KTXDotOLP					,".olp");
/*
-------------------------------------------------------------------------------
*************************** CYBRecognizer1 *************************************
-------------------------------------------------------------------------------
*/
_LIT(KtxTypeEpocResource		,"x-epoc/resource");
_LIT(KtxXE_Filestore			,"x-epoc/dictionary store");
_LIT(KtxXE_Database				,"x-epoc/named database");

_LIT(KtxTypeImageOLP			,"image/vnd.nokia.olp-bitmap");



/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
TInt CYBRecognizer1::MinimumFileSize(void)
{
	return 8;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
TInt CYBRecognizer1::FileTypeCount(void)
{
	return 4;
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
_LIT(KtxRec1Abbout		,"version 0.79(Beta test)\nJanuary 19th 2007\n\nCopyright Jukka Silvennoinen 2006, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.pushl.com/y_browser/");

HBufC* CYBRecognizer1::GetCreditsL(void)
{
	return KtxRec1Abbout().AllocL();
	
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CYBRecognizer1::FileTypeL(TInt aIndex,TDes& aFileType)
{
	switch(aIndex)	
	{
	case 0:
		aFileType.Copy(KtxTypeEpocResource);
		break;
	case 1:
		aFileType.Copy(KtxXE_Filestore);
		break;
	case 2:
		aFileType.Copy(KtxXE_Database);
		break;
	case 3:
		aFileType.Copy(KtxTypeImageOLP);
		break;
	}
	
	return Ret;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CGulIcon* CYBRecognizer1::GetIconL(TInt aIndex, TSize aSize)
{
	CGulIcon* RetIcon(NULL);
	
	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone == AppFile.FindByDir(KtxImagesFileName, KNullDesC))
	{
		switch(aIndex)
		{
		case 0:
			RetIcon = AddIconFromFileL(aSize,AppFile.File(),);//5
			break;
		case 1:
			RetIcon = AddIconFromFileL(aSize,AppFile.File(),);
			break;
		case 2:
			RetIcon = AddIconFromFileL(aSize,AppFile.File(),);		//6
			break;
		case 3:
			RetIcon = AddIconFromFileL(aSize,AppFile.File(),);			//
			break;
		};
	}
	
	return RetIcon;
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
CGulIcon* CYBRecognizer1::AddIconFromFileL(const TSize& aIconsize,const TDesC& aFileName, TInt aImg, TInt aMsk)
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
-------------------------------------------------------------------------------
*/
void CYBRecognizer1::RecognizeFile(CYBRecognitionResult& aResult,const TDesC& aFileName,const TDesC8& aBuffer)
{
	if(aResult.iIdString)
	{
		aResult.iIdString->Des().Zero();
	}
	
	aResult.iConfidence = CYBRecognitionResult::ERecNotRecognized;
	
	TBool StartOk = EFalse;

	if(aBuffer.Length() >= 8)
	{
		if(aBuffer[0] == 107)
			if(aBuffer[1] == 74)
				if(aBuffer[2] == 31)
					if(aBuffer[3] == 16)
						if(aBuffer[4] == 0)
							if(aBuffer[5] == 0)
								if(aBuffer[6] == 0)
									if(aBuffer[7] == 0)
									{
										aResult.iConfidence = CYBRecognitionResult::ERecPossible + 1;
										
										delete aResult.iIdString;
										aResult.iIdString = NULL;
										aResult.iIdString = KtxTypeEpocResource().AllocL();
									}
	
	
		if (aResult.iConfidence >= CYBRecognitionResult::ERecPossible)
		{
			return ;
		}

	}
	
	if(aBuffer.Length() >= 16)
	{
		if(aBuffer[0] == 80)
			if(aBuffer[1] == 0)
				if(aBuffer[2] == 0)
					if(aBuffer[3] == 16)
					{
						StartOk = ETrue;
						if(aBuffer[4] == 228)
							if(aBuffer[5] == 0)
								if(aBuffer[6] == 0)
									if(aBuffer[7] == 16)
									{
										aResult.iConfidence = CYBRecognitionResult::ERecPossible + 1;
										aResult.iIdString = KtxXE_Filestore().AllocL();
									}
					}
										
													
		if (aResult.iConfidence >= CYBRecognitionResult::ERecPossible)
		{
			return ;
		}
		
		if(StartOk)
		{
			if(aBuffer[4] == 190)
				if(aBuffer[5] == 14)
					if(aBuffer[6] == 0)
						if(aBuffer[7] == 16)
							if(aBuffer[8] == 0 && aBuffer[9] == 0 && aBuffer[10] == 0 && aBuffer[11] == 0 )
							{
								aResult.iConfidence = CYBRecognitionResult::ERecPossible + 1;
								aResult.iIdString = KtxXE_Database().AllocL();
							}
		}

		if (aResult.iConfidence >= CYBRecognitionResult::ERecPossible)
		{
			return ;
		}

		if(aBuffer[0] == 55)
			if(aBuffer[1] == 0)
				if(aBuffer[2] == 0)
					if(aBuffer[3] == 16)
						if(aBuffer[4] == 56)
							if(aBuffer[5] == 58)
								if(aBuffer[6] == 0)
									if(aBuffer[7] == 16)
									{
										aResult.iConfidence = CYBRecognitionResult::ERecPossible + 1;
										aResult.iIdString = KtxAIF_File().AllocL();
									}

		if (aResult.iConfidence >= CYBRecognitionResult::ERecPossible)
		{
			return ;
		}


		TBuf<5> Help;
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
								aResult.iConfidence = CYBRecognitionResult::ERecPossible + 1;
								aResult.iIdString = KtxTypeImageOLP().AllocL();
							}
			}
		
			if (aResult.iConfidence >= CYBRecognitionResult::ERecPossible)
			{
				return ;
			}
		}
	}
}



/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
CYBrowserFileRecognizer* NewRecognizer()
    {
    return (new CYBRecognizer1);
    }
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
#ifdef __SERIES60_3X__
LOCAL_D const TImplementationProxy ImplementationTable[] = 
{
	IMPLEMENTATION_PROXY_ENTRY(0x7145,NewRecognizer)
};
#else

// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
    {
    {{0x7145}, NewRecognizer}
    };
    
// DLL Entry point
GLDEF_C TInt E32Dll(TDllReason /*aReason*/)
    {
    return(KErrNone);
    }
#endif

/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
{
	aTableCount = sizeof(ImplementationTable)/sizeof(ImplementationTable[0]);
	return ImplementationTable;
}

