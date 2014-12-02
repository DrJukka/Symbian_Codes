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
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <aknnotewrappers.h>
#include <akniconutils.h> 

#include <ecom.h>
#include <implementationproxy.h>

#include "YToolsZipper.h"
#include "Common.h"
#include "icons.mbg"

#include <eikstart.h>


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
(Not implemented in version 0.82)
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
		aFileType.Copy(KtxType);
		break;
	}
}


/*
-------------------------------------------------------------------------------
icons used for the file type in Y-Browser 
aIndex is same as with FileTypeL() function
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
			RetIcon = GetListIconL(aSize,AppFile.File(),EMbmIconsZip,EMbmIconsZip_m);
			break;
		};
	}
	
	return RetIcon;
}

/*
-----------------------------------------------------------------------------
You are reqiured to use AknIconUtils since icons can be used in different 
sizes, thus usually Y-Browser asks the icon with TSize(0,0) and re-sizes
it later as required, when using other methods than AknIconUtils, the
resizing will not work correctly
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
		// do not resize if the size is TSize(0,0)
		if(aIconsize.iHeight > 0 && aIconsize.iWidth > 0)
		{
			AknIconUtils::SetSize(FrameImg,aIconsize,EAspectRatioPreserved);  
					
			if(FrameMsk)
			{
				AknIconUtils::SetSize(FrameMsk,aIconsize,EAspectRatioPreserved);	
			}
		}
	}
	
	return CGulIcon::NewL(FrameImg,FrameMsk);
} 
/*
-------------------------------------------------------------------------------
Y-Browser will use this function to recognize file data

NOTE: This is non-leaving functions, thus do not use leaving code
or you have to use TRAP to trap the leaves..
-------------------------------------------------------------------------------
*/
void CYBRecognizer1::RecognizeFile(CYBRecognitionResult& aResult,const TDesC& aFileName,const TDesC8& aBuffer)
{		
	// better to be sure that the pointer is actually non-NULL
	// before using it to avoif KERN-EXEC 3 happening							
	if(aResult.iIdString)
	{
		aResult.iIdString->Des().Zero();
	}
	
	// first we say that its not recognized by us...
	aResult.iConfidence = CYBRecognitionResult::ERecNotRecognized;
	
	_LIT(KtxZipExtension	,".zip");
	
	// filenames can actually be shorter 
	// than normal extension lenght, thus it needs to be checked
	if(aFileName.Length() > 4)
	{
		TBuf<4> ZipExt;
		
		// the extension can be in any case
		// thus we copy it to be in lower case 
		// to make the identification easier
		ZipExt.CopyLC(aFileName.Right(4));
		
		//this recognizer only recognize extensions
		// but you could also use aBuffer data to recognize the file
		if(ZipExt == KtxZipExtension)
		{
			aResult.iConfidence = CYBRecognitionResult::ERecProbable;

			// incase it was constructed before
			// we better delete it to avoid memory leak..
			// I think this might change in future editions...
			delete aResult.iIdString;
			aResult.iIdString = NULL;
			aResult.iIdString = KtxType().Alloc();// non leaving !!!
							
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

