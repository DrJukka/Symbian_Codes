/* 	Copyright (c) 2001 - 2006, 
	Jukka Silvennoinen,
 	All rights reserved 
*/

#include <BAUTILS.H>
#include <APGCLI.H>
#include <AknsSkinInstance.h>
#include <AknsUtils.h>

#include "Owr_Recognizer.h"
#include "FileTypesDataBase.h"


/*
-------------------------------------------------------------------------------
*************************** CYBOWRRecognizer *************************************
-------------------------------------------------------------------------------
*/

CYBOWRRecognizer::~CYBOWRRecognizer()
{
	iTypesArray.ResetAndDestroy();
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CYBOWRRecognizer::ConstructL()
{
	TRAPD(error,ReFreshFTOverWritesL());
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/	
void CYBOWRRecognizer::ReFreshFTOverWritesL(void)
{
	iTypesArray.ResetAndDestroy();
	
	CMyDBClass* dataBase = new(ELeave)CMyDBClass();
	CleanupStack::PushL(dataBase);
	dataBase->ConstructL();
	
	dataBase->ReadTypeItemsL(iTypesArray);
	
	CleanupStack::PopAndDestroy(dataBase);
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CYBOWRRecognizer::RecognizeFile(CYBRecognitionResult& aResult,const TDesC& aFileName,const TDesC8& /*aBuffer*/)
{
	if(aResult.iIdString)
	{
		aResult.iIdString->Des().Zero();
	}
	aResult.iConfidence = CYBRecognitionResult::ERecNotRecognized;
	
	TBuf<50> ExtHelp,Another;

	TRAPD(Err,
	if(aFileName.Length()
	&& BaflUtils::Parse(aFileName) == KErrNone)
	{
		TParsePtrC Hjelppp(aFileName);
		ExtHelp.CopyLC(Hjelppp.Ext());
		
		for(TInt i=0; i < iTypesArray.Count(); i++)
		{
			if(iTypesArray[i])
			{
				if(iTypesArray[i]->iTypeName && iTypesArray[i]->iExtensions)
				{
					for(TInt w=0; w < iTypesArray[i]->iExtensions->MdcaCount(); w++)
					{				
						if(ExtHelp.Length() == iTypesArray[i]->iExtensions->MdcaPoint(w).Length())
						{
							Another.CopyLC(iTypesArray[i]->iExtensions->MdcaPoint(w));
							
							if(ExtHelp == Another)
							{
								aResult.iConfidence = CYBRecognitionResult::ERecCertain;
								delete aResult.iIdString;
								aResult.iIdString = NULL;
								aResult.iIdString = iTypesArray[i]->iTypeName->Des().AllocL();
							}
						}
					}
				}
			}
		}
	});
}

        
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
TInt CYBOWRRecognizer::MinimumFileSize(void)
{
	return 0x0;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
TInt CYBOWRRecognizer::FileTypeCount(void)
{
	TInt cUnt(0);

	for(TInt i=0; i < iTypesArray.Count(); i++)
	{
		if(iTypesArray[i])
		{
			if(iTypesArray[i]->iExtensions)
			{
				cUnt = cUnt + iTypesArray[i]->iExtensions->MdcaCount();
			}
		}
	}
	
	return cUnt;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CYBOWRRecognizer::FileTypeL(TInt aIndex,TDes& aFileType)
{
	aFileType.Zero();
	
	if(aIndex >= 0 && aIndex < iTypesArray.Count())
	{
		if(iTypesArray[aIndex])
		{
			if(iTypesArray[aIndex]->iTypeName)
			{
				aFileType.Copy(iTypesArray[aIndex]->iTypeName->Des());
			}
		}
	}
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CGulIcon* CYBOWRRecognizer::GetIconL(TInt aIndex, TSize aSize)
{
	CGulIcon* RetIcon(NULL);
	
	if(aIndex >= 0 && aIndex < iTypesArray.Count())
	{
		if(iTypesArray[aIndex])
		{
			if(iTypesArray[aIndex]->iHandler)
			{
				RetIcon = GetAppIccon(*iTypesArray[aIndex]->iHandler,aSize);
			}
		}
	}
		
	return RetIcon;	
}


/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CGulIcon* CYBOWRRecognizer::GetAppIccon(const TDesC& aFileName, TSize aSize) 
{
	CGulIcon* RetIcon(NULL);
	
	RApaLsSession ls;
	if(ls.Connect() == KErrNone)
	{
		if(ls.GetAllApps() == KErrNone)
		{
			TInt Errnono(KErrNone);
			TFileName HelpFn;
			TApaAppInfo AppInfo;
	
			MAknsSkinInstance* skin = AknsUtils::SkinInstance();
	
			do
			{
				Errnono = ls.GetNextApp(AppInfo);
				if(KErrNone == Errnono)
				{
					if(AppInfo.iFullName == aFileName)
					{
						TRAPD(Err,
						CFbsBitmap*	AppIcon(NULL);
						CFbsBitmap*	AppIconMsk(NULL);
				    	AknsUtils::CreateAppIconLC(skin,AppInfo.iUid,  EAknsAppIconTypeContext,AppIcon,AppIconMsk);
				
						if(AppIcon && AppIconMsk)
						{
							if(aSize.iHeight > 0 && aSize.iWidth > 0)
							{
								AknIconUtils::SetSize(AppIcon,aSize,EAspectRatioPreserved);  
								AknIconUtils::SetSize(AppIconMsk,aSize,EAspectRatioPreserved);  
							}
							
							RetIcon = CGulIcon::NewL(AppIcon,AppIconMsk);
							CleanupStack::Pop(2);					
						});
					}
				}
				
			}while(KErrNone == Errnono && !RetIcon);

		}
		
		ls.Close();
	}

	return RetIcon;				
}


/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/

HBufC* CYBOWRRecognizer::GetCreditsL(void)
{
	return KNullDesC().AllocL();
	
}
