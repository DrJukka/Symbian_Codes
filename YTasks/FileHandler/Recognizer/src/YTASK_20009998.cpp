// RecogEx.CPP
//
// Copyright (C) 2003 Nokia Corporation. All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////

#include "YTASK_20009998.h"

#include <BAUTILS.H>
#include <D32DBMS.H>


const TInt KMimeExRecognizerValue = 0x20009998;

const TUid KUidMimeExRecognizer={KMimeExRecognizerValue};

const TInt KMimeExRecognizerImplValue=0x9998;

_LIT(KtxtItemlist	,"RecFiles");

_LIT(KtxtDbFileName	,"\\system\\data\\20009998\\Rec.db");


_LIT8(KDataTypeCNT		,"YTools/cnt");
_LIT(KDataExtCNT		,".cnt");
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CApaExRecognizer::CApaExRecognizer()
:CApaDataRecognizerType(KUidMimeExRecognizer,CApaDataRecognizerType::ENormal)
// All these mime types have reasonable recognition
{
	iDbModified.HomeTime();
	iDbModified = iDbModified - TTimeIntervalDays(2);
	
	TRAPD(Err,iCountDataTypes=GetTypesArrayL(););
}

CApaExRecognizer::~CApaExRecognizer()
{
	iArray.ResetAndDestroy();
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
TUint CApaExRecognizer::PreferredBufSize()
{
	// no buffer recognition in this example
	return 0;
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
TDataType CApaExRecognizer::SupportedDataTypeL(TInt aIndex) const
{
	TDataType Typ;
	
	if(aIndex == 0)
	{
		Typ = TDataType(KDataTypeCNT);
	}
	else if(aIndex < iArray.Count())
	{
		if(iArray[aIndex])
		{
			if(iArray[aIndex]->iType)
			{
				Typ = TDataType(*iArray[aIndex]->iType);	
			}
		}
	}
	
	return Typ; 
}


/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CApaExRecognizer::DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer)
{	
	TBuf<100> ExtHelp;
	
	TBool Recognized(EFalse);
	
	if(aName.Length()
	&& BaflUtils::Parse(aName) == KErrNone)
	{
		TParsePtrC Hjelppp(aName);
		ExtHelp.CopyLC(Hjelppp.NameAndExt());
		
		if(ExtHelp.Length() > KDataExtCNT().Length())
		{
			if(ExtHelp.Right(KDataExtCNT().Length()) == KDataExtCNT)
			{
				iConfidence=ECertain;
				iDataType=TDataType(KDataTypeCNT);
				Recognized = ETrue;
			}
		}
	}
	
	if(!Recognized)
	{
		RFs FsSession;
		if(FsSession.Connect() == KErrNone)
		{
			TFindFile PrivFolder(FsSession);
			if(KErrNone == PrivFolder.FindByDir(KtxtDbFileName, KNullDesC))// finds the drive
			{
				TEntry DbEntry;
				if(KErrNone == FsSession.Entry(PrivFolder.File(),DbEntry))
				{	
					if(iDbModified != DbEntry.iModified)
					{
						iDbModified = DbEntry.iModified;
						TRAPD(Err,iCountDataTypes=GetTypesArrayL(););
					}
				}
			}
		
			FsSession.Close();
		}

		for(TInt i=0; i < iArray.Count(); i++)
		{
			if(iArray[i])
			{
				if(iArray[i]->iType)
				{
					if(iArray[i]->iExt)
					{
						if(aName.Length()
						&& BaflUtils::Parse(aName) == KErrNone)
						{
							TParsePtrC Hjelppp(aName);
							ExtHelp.CopyLC(Hjelppp.NameAndExt());
							
							if(ExtHelp.Length() > iArray[i]->iExt->Des().Length())
							{
								if(ExtHelp.Right(iArray[i]->iExt->Des().Length()) == iArray[i]->iExt->Des())
								{
									iConfidence=ECertain;
									iDataType=TDataType(*iArray[i]->iType);
								}
							}
						}
					}
					else if(iArray[i]->iData)
					{
						if(aBuffer.Length() >= iArray[i]->iData->Des().Length())
						{
							TBool MeOk(ETrue);
							
							for(TInt b=0; b < iArray[i]->iData->Des().Length(); b++)
							{
								if(aBuffer[b] != iArray[i]->iData->Des()[b])
								{
									MeOk = EFalse;
								}
							}
							
							if(MeOk)
							{
								iConfidence=ECertain;
								iDataType=TDataType(*iArray[i]->iType);	
							}
						}
					}
				}
			}
		}
	}
}



/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
TInt CApaExRecognizer::GetTypesArrayL(void)
{
	iArray.ResetAndDestroy();

	RFs FsSession;
	if(FsSession.Connect() == KErrNone)
	{
		TFindFile PrivFolder(FsSession);
		if(KErrNone == PrivFolder.FindByDir(KtxtDbFileName, KNullDesC))// finds the drive
		{
			TEntry DbEntry;
			if(KErrNone == FsSession.Entry(PrivFolder.File(),DbEntry))
			{	
				iDbModified = DbEntry.iModified;
			}
		
			RDbNamedDatabase Database;
			if(KErrNone == Database.Open(FsSession,PrivFolder.File()))
			{
				TFileName QueryBuffer;
				QueryBuffer.Copy(_L("SELECT * FROM "));// just get all columns & rows
				QueryBuffer.Append(KtxtItemlist);
				
				RDbView Myview;
				Myview.Prepare(Database,TDbQuery(QueryBuffer));
				CleanupClosePushL(Myview);
				Myview.EvaluateAll();
				Myview.FirstL();
				
				while(Myview.AtRow()) // Just delete one instance of the message           
				{	
					Myview.GetL();		
					
					if(Myview.ColDes8(2).Length())
					{
						if(Myview.ColDes8(3).Length()
						|| Myview.ColDes(4).Length())
						{
							CMyRecItem* NewIttem = new(ELeave)CMyRecItem();
							iArray.Append(NewIttem);
							
							NewIttem->iType= Myview.ColDes8(2).AllocL();
							
							if(Myview.ColDes8(3).Length())
							{
								NewIttem->iData= Myview.ColDes8(3).AllocL();
							}
							
							if(Myview.ColDes(4).Length())
							{
								NewIttem->iExt = Myview.ColDes(4).AllocL();
							}
						}
					}
						
					Myview.NextL();
				} 
				
				CleanupStack::PopAndDestroy(1); // Myview
				
				Database.Close();
			}
		}
	}
	
	FsSession.Close();
	
	return iArray.Count();
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
#ifdef EKA2

CApaDataRecognizerType* CApaExRecognizer::CreateRecognizerL()
    {
    return new (ELeave) CApaExRecognizer();
    }

const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KMimeExRecognizerImplValue,CApaExRecognizer::CreateRecognizerL )
    };//KUidMimeExRecognizer

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

#else

EXPORT_C CApaDataRecognizerType* CreateRecognizer()
	{
	CApaDataRecognizerType* thing=new CApaExRecognizer();
	return thing; // NULL if new failed
	}


GLDEF_C TInt E32Dll(TDllReason /*aReason*/)
	{
	return KErrNone;
	}

#endif
