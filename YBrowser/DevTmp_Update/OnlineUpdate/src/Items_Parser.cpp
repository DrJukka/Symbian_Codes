/* Copyright (c) 2001 - 2007 , J.P. Silvennoinen. All rights reserved */

#include "Items_Parser.h"

#include <BAUTILS.H>
#include <CHARCONV.H>

const TInt	KOneSecondTimeOut = 1000000;

/*
-------------------------------------------------------------------------------
****************************  CRSSFeedParser   *************************************
-------------------------------------------------------------------------------
*/

CRSSFeedParser* CRSSFeedParser::NewL(RFs& aSession,MItemParserCallBack& aCallBack)
    {
    CRSSFeedParser* self = NewLC(aSession,aCallBack);
    CleanupStack::Pop(self);
    return self;
    }

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/    
CRSSFeedParser* CRSSFeedParser::NewLC(RFs& aSession,MItemParserCallBack& aCallBack)
    {
    CRSSFeedParser* self = new (ELeave) CRSSFeedParser(aSession,aCallBack);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
  
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CRSSFeedParser::CRSSFeedParser(RFs& aSession,MItemParserCallBack& aCallBack)
:iSession(aSession),iCallBack(aCallBack)
    {
    }

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CRSSFeedParser::~CRSSFeedParser()
{
	if(iTimeOutTimer)
	{
		iTimeOutTimer->Cancel();
	}
	
	delete iTimeOutTimer;
	iTimeOutTimer = NULL;
	
	iArray.ResetAndDestroy();
		
	delete iFileName;
	iFileName = NULL;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CRSSFeedParser::ConstructL()
{	
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CRSSFeedParser::TimerExpired()
{
	if(iFileName)
	{
		ReadMainL(*iFileName);
	}
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CRSSFeedParser::ReadMainL(const TDesC& aFileName)
{
	iDone = EFalse;
	
	delete iFileName;
	iFileName = NULL;
	
	iArray.ResetAndDestroy();
	
	if(BaflUtils::FileExists(iSession,aFileName))
	{
		iFileName = aFileName.AllocL();
		
		TBool FilOpen(EFalse);

		iSession.IsFileOpen(*iFileName,FilOpen);
		
		if(FilOpen)
		{
			if(iTimeOutTimer == NULL)
			{
				iTimeOutTimer = CTimeOutTimer::NewL(CActive::EPriorityStandard,*this);
			}
			
			iTimeOutTimer->After(KOneSecondTimeOut);
		}
		else 
		{
			GetHeaderL(*iFileName);
			GetMainArrayL(iArray,*iFileName);
			
			iDone = ETrue;
			iCallBack.NewsParsedL();
		}
	}
	else 
	{
		iDone = ETrue;
		iCallBack.NewsParsedL();
	}
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CRSSFeedParser::ReadCategoryL(CCategoryItems& aObject)
{	
	aObject.iItemsArray.ResetAndDestroy();
	
	if(aObject.iFileName && iFileName)
	{
		if(BaflUtils::Parse(*iFileName) == KErrNone)
		{
			TParsePtrC hjelppp(*iFileName);
			TFileName useFile;
			
			useFile.Copy(hjelppp.DriveAndPath());
			useFile.Append(*aObject.iFileName);
			if(BaflUtils::FileExists(iSession,useFile))
			{
				GetCategoryArrayL(aObject.iItemsArray,useFile);
			}
		}
	}
	
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CRSSFeedParser::GetCategoryArrayL(RPointerArray<CMyItems>& aArray,const TDesC& aFile)
{		
	RFile ReadFil;
	TInt pillutus = ReadFil.Open(iSession,aFile,EFileRead);
	if(KErrNone == pillutus)
	{	
		CleanupClosePushL(ReadFil);
		TInt FilSiz(0);
		if(ReadFil.Size(FilSiz) == KErrNone)
		{
			if(FilSiz > 0)
			{
				HBufC8* FilBuff = HBufC8::NewLC(FilSiz);
				TPtr8 ReadPoint(FilBuff->Des());
				if(ReadFil.Read(ReadPoint,FilSiz) == KErrNone)
				{
					TUint ThisCharset(KCharacterSetIdentifierUtf8);
					
					HBufC* UniBuf = HBufC::NewLC(FilBuff->Des().Length());
					TPtr UniPoint(UniBuf->Des());
					
					ConvertToUnicodeL(UniPoint,FilBuff->Des(),iSession,ThisCharset);
					
					TInt ItemStart(KErrNotFound);
					TInt ItemEnd(KErrNotFound);
					TInt ReadAlready(0);
					
					do
					{
						if(ReadAlready < UniBuf->Des().Length())
						{
							ItemStart = UniBuf->Des().Right((UniBuf->Des().Length() - ReadAlready)).Find(KtxStrtINFOPAGE);			
							ItemEnd = UniBuf->Des().Right((UniBuf->Des().Length() - ReadAlready)).Find(KtxEnddINFOPAGE);
						}
						else
						{
							ItemStart	= KErrNotFound;
							ItemEnd 	= KErrNotFound;
						}
						
						if(ItemStart != KErrNotFound && ItemEnd != KErrNotFound)
						{
							ItemStart = ItemStart + KtxStrtINFOPAGE().Length();
							
							if(ItemStart < ItemEnd)
							{
								AppendCategoryItemL(UniBuf->Des().Mid((ReadAlready + ItemStart),(ItemEnd - ItemStart)),aArray);						
							}

							ReadAlready = ReadAlready + ItemEnd + KtxEnddINFOPAGE().Length();
						}
						
					}while(ItemStart != KErrNotFound && ItemEnd != KErrNotFound);	
					
					
					CleanupStack::PopAndDestroy(UniBuf);
				}
				
				CleanupStack::PopAndDestroy(FilBuff);
			}
		}
		
		CleanupStack::PopAndDestroy(1);//ReadFil
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CRSSFeedParser::GetMainArrayL(RPointerArray<CCategoryItems>& aArray,const TDesC& aFile)
{		
	RFile ReadFil;
	if(KErrNone == ReadFil.Open(iSession,aFile,EFileRead))
	{	
		CleanupClosePushL(ReadFil);
		TInt FilSiz(0);
		if(ReadFil.Size(FilSiz) == KErrNone)
		{
			if(FilSiz > 0)
			{
				HBufC8* FilBuff = HBufC8::NewLC(FilSiz);
				TPtr8 ReadPoint(FilBuff->Des());
				if(ReadFil.Read(ReadPoint,FilSiz) == KErrNone)
				{
					TUint ThisCharset(KCharacterSetIdentifierUtf8);
					
					HBufC* UniBuf = HBufC::NewLC(FilBuff->Des().Length());
					TPtr UniPoint(UniBuf->Des());
					
					ConvertToUnicodeL(UniPoint,FilBuff->Des(),iSession,ThisCharset);
					
					TInt ItemStart(KErrNotFound);
					TInt ItemEnd(KErrNotFound);
					TInt ReadAlready(0);
					
					do
					{
						if(ReadAlready < UniBuf->Des().Length())
						{
							ItemStart = UniBuf->Des().Right((UniBuf->Des().Length() - ReadAlready)).Find(KtxStrtItem);			
							ItemEnd = UniBuf->Des().Right((UniBuf->Des().Length() - ReadAlready)).Find(KtxEnddItem);
						}
						else
						{
							ItemStart	= KErrNotFound;
							ItemEnd 	= KErrNotFound;
						}
						
						if(ItemStart != KErrNotFound && ItemEnd != KErrNotFound)
						{
							ItemStart = ItemStart + KtxStrtItem().Length();
							
							if(ItemStart < ItemEnd)
							{
								AppendMainItemL(UniBuf->Des().Mid((ReadAlready + ItemStart),(ItemEnd - ItemStart)),aArray);						
							}

							ReadAlready = ReadAlready + ItemEnd + KtxEnddItem().Length();
						}
						
					}while(ItemStart != KErrNotFound && ItemEnd != KErrNotFound);	
					
					
					CleanupStack::PopAndDestroy(UniBuf);
				}
				
				CleanupStack::PopAndDestroy(FilBuff);
			}
		}
		
		CleanupStack::PopAndDestroy(1);//ReadFil
	}
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CRSSFeedParser::GetHeaderL(const TDesC& aFile)
{
	RFile ReadFil;
	if(KErrNone == ReadFil.Open(iSession,aFile,EFileRead))
	{	
		CleanupClosePushL(ReadFil);
		
		iModified.HomeTime();
		
		TEntry FilEntry;
		if(KErrNone == iSession.Entry(aFile,FilEntry))
		{
			iModified = FilEntry.iModified;
		}
		
		TInt FilSiz(0);
		if(ReadFil.Size(FilSiz) == KErrNone)
		{
			if(FilSiz > 0)
			{
				if(FilSiz > 3000)
				{
					FilSiz = 3000;
				}
				
				HBufC8* FilBuff = HBufC8::NewLC(FilSiz);
				TPtr8 ReadPoint(FilBuff->Des());
				if(ReadFil.Read(ReadPoint,FilSiz) == KErrNone)
				{
					HBufC* UniBuf = HBufC::NewLC(FilBuff->Des().Length());
					TPtr UniPoint(UniBuf->Des());
					
					TUint characterSetIdentifier(KCharacterSetIdentifierUtf8);
	
					ConvertToUnicodeL(UniPoint,FilBuff->Des(),iSession,characterSetIdentifier);
							
					// use GetHeaderItemL to get expiry date, version & other header information.
				
					CleanupStack::PopAndDestroy(UniBuf);
				}
				
				CleanupStack::PopAndDestroy(FilBuff);
			}
		}
		
		CleanupStack::PopAndDestroy(1);//ReadFil
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
HBufC* CRSSFeedParser::GetHeaderItemL(const TDesC& aData, const TDesC& aStart, const TDesC& aEnd)
{
	HBufC* Ret(NULL);

	TInt DataStrt = aData.Find(aStart);
	TInt DataEndd = aData.Find(aEnd);
	if(DataStrt != KErrNotFound 
	&& DataEndd != KErrNotFound)
	{
		DataStrt = DataStrt + aStart.Length();
		if(DataStrt < DataEndd)
		{
			Ret = aData.Mid(DataStrt,(DataEndd - DataStrt)).AllocL();
		}
	}
	return Ret;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CRSSFeedParser::AppendMainItemL(const TDesC& aData, RPointerArray<CCategoryItems>& aArray)
{
	TInt TitleStrt = aData.Find(KtxStrtTitle);
	TInt TitleEndd = aData.Find(KtxEnddTitle);	
	
	TInt DescrStrt = aData.Find(KtxStrtFILE);
	TInt DescrEndd = aData.Find(KtxEnddFILE);
	
	TInt IconStrt = aData.Find(KtxStrtICON);
	TInt IconEndd = aData.Find(KtxEnddICON);
	
	if(TitleStrt != KErrNotFound 
	&& TitleEndd != KErrNotFound
	&& DescrStrt != KErrNotFound 
	&& DescrEndd != KErrNotFound)
	{
		CCategoryItems* NewItem = new(ELeave)CCategoryItems();
		CleanupStack::PushL(NewItem);
	
		TitleStrt = TitleStrt + KtxStrtTitle().Length();		
		if(TitleStrt < TitleEndd)
		{
			NewItem->iTitle = RemoveEscapesL(aData.Mid(TitleStrt,(TitleEndd - TitleStrt)));
		}
		
		DescrStrt = DescrStrt + KtxStrtFILE().Length();
		if(DescrStrt < DescrEndd)
		{
			NewItem->iFileName = RemoveEscapesL(aData.Mid(DescrStrt,(DescrEndd - DescrStrt)));
		}
			
		if(IconStrt != KErrNotFound 
		&& IconEndd != KErrNotFound)
		{
			IconStrt = IconStrt + KtxStrtICON().Length();
			if(IconStrt < IconEndd)
			{
				TUint32 tmpIccon(0);
				TLex MyLex(aData.Mid(IconStrt,(IconEndd - IconStrt)));
				if(KErrNone == MyLex.Val(tmpIccon,EDecimal))
				{
					NewItem->iIcon = tmpIccon;
				}
			}
		}
			
		CleanupStack::Pop();//NewItem
		
		aArray.Append(NewItem);	
		TRAPD(errNono,ReadCategoryL(*NewItem));
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CRSSFeedParser::AppendCategoryItemL(const TDesC& aData, RPointerArray<CMyItems>& aArray)
{
	TInt TitleStrt = aData.Find(KtxStrtTitle);
	TInt TitleEndd = aData.Find(KtxEnddTitle);	
	
	TInt DescrStrt = aData.Find(KtxStrtDescription);
	TInt DescrEndd = aData.Find(KtxEnddDescription);
	
	TInt PrewStrt = aData.Find(KtxStrtPreviews);
	TInt PrewEndd = aData.Find(KtxEnddPreviews);
	
	TInt LinkkStrt = aData.Find(KtxStrtLinks);
	TInt LinkkEndd = aData.Find(KtxEnddLinks);
	
	if(TitleStrt != KErrNotFound 
	&& TitleEndd != KErrNotFound)
	{
		CMyItems* NewItem = new(ELeave)CMyItems();
		CleanupStack::PushL(NewItem);
	
		TitleStrt = TitleStrt + KtxStrtTitle().Length();		
		if(TitleStrt < TitleEndd)
		{
			NewItem->iTitle = RemoveEscapesL(aData.Mid(TitleStrt,(TitleEndd - TitleStrt)));
		}
		
		if(DescrStrt != KErrNotFound
		&& DescrEndd != KErrNotFound)
		{
			DescrStrt = DescrStrt + KtxStrtDescription().Length();
			if(DescrStrt < DescrEndd)
			{
				NewItem->iDescr = RemoveEscapesL(aData.Mid(DescrStrt,(DescrEndd - DescrStrt)));
			}
		}
			
		if(PrewStrt != KErrNotFound 
		&& PrewEndd != KErrNotFound)
		{
			PrewStrt = PrewStrt + KtxStrtPreviews().Length();
			if(PrewStrt < PrewEndd)
			{
				GetPreviewArray(aData.Mid(PrewStrt,(PrewEndd - PrewStrt)),NewItem->iPrevsArray);
			}
		}
		
		if(LinkkStrt != KErrNotFound 
		&& LinkkEndd != KErrNotFound)
		{
			LinkkStrt = LinkkStrt + KtxStrtLinks().Length();
			if(LinkkStrt < LinkkEndd)
			{
				GetLinksArray(aData.Mid(LinkkStrt,(LinkkEndd - LinkkStrt)),NewItem->iLinksArray);
			}
		}
			
		CleanupStack::Pop();//NewItem
		aArray.Append(NewItem);	
	}
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CRSSFeedParser::GetPreviewArray(const TDesC& aData, RPointerArray<CMyPrevs>& aArray)
{
	TInt ItemStart(KErrNotFound);
	TInt ItemEnd(KErrNotFound);
	TInt ReadAlready(0);

	do
	{
		if(ReadAlready < aData.Length())
		{
			ItemStart = aData.Right((aData.Length() - ReadAlready)).Find(KtxStrtITEM);			
			ItemEnd = aData.Right((aData.Length() - ReadAlready)).Find(KtxEnddITEM);
		}
		else
		{
			ItemStart	= KErrNotFound;
			ItemEnd 	= KErrNotFound;
		}
		
		if(ItemStart != KErrNotFound && ItemEnd != KErrNotFound)
		{
			ItemStart = ItemStart + KtxStrtITEM().Length();
			
			if(ItemStart < ItemEnd)
			{
				AppendPreviewItemL(aData.Mid((ReadAlready + ItemStart),(ItemEnd - ItemStart)),aArray);						
			}

			ReadAlready = ReadAlready + ItemEnd + KtxEnddITEM().Length();
		}
		
	}while(ItemStart != KErrNotFound && ItemEnd != KErrNotFound);	
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CRSSFeedParser::AppendPreviewItemL(const TDesC& aData, RPointerArray<CMyPrevs>& aArray)
{
	TInt TitleStrt = aData.Find(KtxStrtNAME);
	TInt TitleEndd = aData.Find(KtxEnddNAME);	
	
	TInt DescrStrt = aData.Find(KtxStrtDescription);
	TInt DescrEndd = aData.Find(KtxEnddDescription);
	
	TInt PrewStrt = aData.Find(KtxStrtLINK);
	TInt PrewEndd = aData.Find(KtxEnddLINK);
	
	if(TitleStrt != KErrNotFound 
	&& TitleEndd != KErrNotFound)
	{
		CMyPrevs* NewItem = new(ELeave)CMyPrevs();
		CleanupStack::PushL(NewItem);
	
		TitleStrt = TitleStrt + KtxStrtNAME().Length();		
		if(TitleStrt < TitleEndd)
		{
			NewItem->iName = RemoveEscapesL(aData.Mid(TitleStrt,(TitleEndd - TitleStrt)));
		}
		
		if(DescrStrt != KErrNotFound
		&& DescrEndd != KErrNotFound)
		{
			DescrStrt = DescrStrt + KtxStrtDescription().Length();
			if(DescrStrt < DescrEndd)
			{
				NewItem->iInfo = RemoveEscapesL(aData.Mid(DescrStrt,(DescrEndd - DescrStrt)));
			}
		}
			
		if(PrewStrt != KErrNotFound 
		&& PrewEndd != KErrNotFound)
		{
			PrewStrt = PrewStrt + KtxStrtLINK().Length();
			if(PrewStrt < PrewEndd)
			{
				NewItem->iLink = RemoveEscapesL(aData.Mid(PrewStrt,(PrewEndd - PrewStrt)));
			}
		}
			
		CleanupStack::Pop();//NewItem
		aArray.Append(NewItem);	
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CRSSFeedParser::GetLinksArray(const TDesC& aData, RPointerArray<CMyLinks>& aArray)
{
	TInt ItemStart(KErrNotFound);
	TInt ItemEnd(KErrNotFound);
	TInt ReadAlready(0);

	do
	{
		if(ReadAlready < aData.Length())
		{
			ItemStart = aData.Right((aData.Length() - ReadAlready)).Find(KtxStrtITEM);			
			ItemEnd = aData.Right((aData.Length() - ReadAlready)).Find(KtxEnddITEM);
		}
		else
		{
			ItemStart	= KErrNotFound;
			ItemEnd 	= KErrNotFound;
		}
		
		if(ItemStart != KErrNotFound && ItemEnd != KErrNotFound)
		{
			ItemStart = ItemStart + KtxStrtITEM().Length();
			
			if(ItemStart < ItemEnd)
			{
				AppendLinksItemL(aData.Mid((ReadAlready + ItemStart),(ItemEnd - ItemStart)),aArray);						
			}

			ReadAlready = ReadAlready + ItemEnd + KtxEnddITEM().Length();
		}
		
	}while(ItemStart != KErrNotFound && ItemEnd != KErrNotFound);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CRSSFeedParser::AppendLinksItemL(const TDesC& aData, RPointerArray<CMyLinks>& aArray)
{
	TInt TitleStrt = aData.Find(KtxStrtNAME);
	TInt TitleEndd = aData.Find(KtxEnddNAME);	
	
	TInt DescrStrt = aData.Find(KtxStrtDescription);
	TInt DescrEndd = aData.Find(KtxEnddDescription);
	
	TInt PrewStrt = aData.Find(KtxStrtLINK);
	TInt PrewEndd = aData.Find(KtxEnddLINK);
	
	TInt LinkkStrt = aData.Find(KtxStrtTYPE);
	TInt LinkkEndd = aData.Find(KtxEnddTYPE);
	
	if(TitleStrt != KErrNotFound 
	&& TitleEndd != KErrNotFound)
	{
		CMyLinks* NewItem = new(ELeave)CMyLinks();
		CleanupStack::PushL(NewItem);
	
		TitleStrt = TitleStrt + KtxStrtNAME().Length();		
		if(TitleStrt < TitleEndd)
		{
			NewItem->iName = RemoveEscapesL(aData.Mid(TitleStrt,(TitleEndd - TitleStrt)));
		}
		
		if(DescrStrt != KErrNotFound
		&& DescrEndd != KErrNotFound)
		{
			DescrStrt = DescrStrt + KtxStrtDescription().Length();
			if(DescrStrt < DescrEndd)
			{
				NewItem->iInfo = RemoveEscapesL(aData.Mid(DescrStrt,(DescrEndd - DescrStrt)));
			}
		}
			
		if(PrewStrt != KErrNotFound 
		&& PrewEndd != KErrNotFound)
		{
			PrewStrt = PrewStrt + KtxStrtLINK().Length();
			if(PrewStrt < PrewEndd)
			{
				NewItem->iLink = RemoveEscapesL(aData.Mid(PrewStrt,(PrewEndd - PrewStrt)));
			}
		}
		
		if(LinkkStrt != KErrNotFound 
		&& LinkkEndd != KErrNotFound)
		{
			LinkkStrt = LinkkStrt + KtxStrtTYPE().Length();
			if(LinkkStrt < LinkkEndd)
			{
				TBuf<100> smallHjelp;
				smallHjelp.Copy(aData.Mid(LinkkStrt,(LinkkEndd - LinkkStrt)));
				smallHjelp.TrimAll();
			
				if(smallHjelp == KtxTypeDWNLOAD)
				{
					NewItem->iType = CMyLinks::EMyLinkDwldUrl;
				}
				else if(smallHjelp == KtxTypeSNDSMS)
				{
					NewItem->iType = CMyLinks::EMyLinkSMS;
				}
				else if(smallHjelp == KtxTypeLNKCLL)
				{
					NewItem->iType = CMyLinks::EMyLinkCall;
				}
				else // KtxTypeWEBOPN
				{
					NewItem->iType = CMyLinks::EMyLinkBrowseUrl;
				}
			}
		}
			
		CleanupStack::Pop();//NewItem
		aArray.Append(NewItem);	
	}
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CRSSFeedParser::ConvertToUnicodeL(TDes& aTo,const TDesC8& aFrom,RFs& aSession,TUint& aCharset) 
{
	if(aCharset != 0)
	{
		TInt Err(KErrNone);		
		TInt state = CCnvCharacterSetConverter::KStateDefault;
			
		TRAP(Err,
		CCnvCharacterSetConverter* CSConverter = CCnvCharacterSetConverter::NewLC();
		
		TPtrC8 Remainder(aFrom);

		if (CSConverter->PrepareToConvertToOrFromL(aCharset,aSession) != CCnvCharacterSetConverter::EAvailable)
		{
			CSConverter->PrepareToConvertToOrFromL(aCharset,aSession);
		}
		
		for(;;)
	    {
	      	const TInt returnValue = CSConverter->ConvertToUnicode(aTo,Remainder,state);
	        if (returnValue <= 0) // < error
	        {
	             break;
	        }
	        
			Remainder.Set(Remainder.Right(returnValue));
	     }

		CleanupStack::PopAndDestroy(CSConverter);
	 	);
	
		if(Err != KErrNone)
	 	{
	 		aTo.Copy(aFrom);
	 	}
 	
	}
	else
 	{
 		aTo.Copy(aFrom);
 	}
 }

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
HBufC* CRSSFeedParser::RemoveEscapesL(const TDesC& aData) 
{
	HBufC* RetBuffer = aData.AllocL();
	TPtr ThisBuf(RetBuffer->Des());
	
	ReplaceUniCodeNumbersL(ThisBuf);
	
	return RetBuffer;
}
 
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CRSSFeedParser::ReplaceUniCodeNumbersL(TDes& aData) 
{	
	TInt ItemStart(KErrNotFound);
	TInt ItemEnd(KErrNotFound);
	TInt ReadAlready(0);
	
	TBuf<200> DbgBufff;
	
	do
	{
		if(ReadAlready < aData.Length())
		{
			ItemStart= aData.Right((aData.Length() - ReadAlready)).Find(KtxRemoveUniStart);			
			ItemEnd  = aData.Right((aData.Length() - ReadAlready)).Find(KtxRemoveUniEnd);
		}
		else
		{
			ItemStart	= KErrNotFound;
			ItemEnd 	= KErrNotFound;
		}
		
		if(ItemStart != KErrNotFound && ItemEnd != KErrNotFound)
		{
			ItemEnd = ItemEnd + KtxRemoveUniEnd().Length();
			
			if((ItemStart + KtxRemoveUniStart().Length()) < ItemEnd)
			{
				TPtrC UniNum(aData.Mid((ReadAlready + ItemStart + KtxRemoveUniStart().Length()),(ItemEnd - (ItemStart + KtxRemoveUniStart().Length()))));
					
				if(UniNum.Length())
				{
					if(UniNum.Left(1) == KtxRemoveUniHex
					&& UniNum.Length() > 1)
					{
						TInt32 ValHexNum(0);
						TLex MyHexLex(UniNum.Right(UniNum.Length() - 1));
						if(KErrNone == MyHexLex.Val(ValHexNum,EHex))
						{
							TBuf<2> Char(_L(" ")); 
							Char[0] = ValHexNum;
							aData.Delete((ReadAlready + ItemStart),(ItemEnd - ItemStart));
							aData.Insert((ReadAlready + ItemStart),Char);
						}
					}
					else
					{
						TInt ValNum(0);
						TLex MyLex(UniNum);
						if(KErrNone == MyLex.Val(ValNum))
						{
							TBuf<2> Char(_L(" ")); 
							Char[0] = ValNum;
							aData.Delete((ReadAlready + ItemStart),(ItemEnd - ItemStart));
							aData.Insert((ReadAlready + ItemStart),Char);
						}
					}
				}		
			}

			ReadAlready = ReadAlready + ItemStart;
		}
		
	}while(ItemStart != KErrNotFound && ItemEnd != KErrNotFound);	
	
}

