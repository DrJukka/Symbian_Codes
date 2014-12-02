/* Copyright (c) 2001 - 2007 , J.P. Silvennoinen. All rights reserved */

#ifndef __RSSFEED_READER_H__
#define __RSSFEED_READER_H__

#include <e32base.h>
#include <w32std.h>
#include <e32std.h>
#include "CTimeOutTimer.h"

_LIT(KtxStrtItem			,"<INFOGROUP");
_LIT(KtxEnddItem			,"</INFOGROUP>");

_LIT(KtxStrtTitle			,"<TITLE>");
_LIT(KtxEnddTitle			,"</TITLE>");
_LIT(KtxStrtFILE			,"<FILE>");
_LIT(KtxEnddFILE			,"</FILE>");
_LIT(KtxStrtICON			,"<ICON>");
_LIT(KtxEnddICON			,"</ICON>");


_LIT(KtxStrtINFOPAGE		,"<INFOPAGE>");
_LIT(KtxEnddINFOPAGE		,"</INFOPAGE>");

_LIT(KtxStrtNAME			,"<NAME>");
_LIT(KtxEnddNAME			,"</NAME>");
_LIT(KtxStrtLINK			,"<LINK>");
_LIT(KtxEnddLINK			,"</LINK>");
_LIT(KtxStrtTYPE			,"<TYPE>");
_LIT(KtxEnddTYPE			,"</TYPE>");
_LIT(KtxStrtDescription		,"<DESCRIPTION>");
_LIT(KtxEnddDescription		,"</DESCRIPTION>");
_LIT(KtxStrtLinks			,"<LINKS>");
_LIT(KtxEnddLinks			,"</LINKS>");
_LIT(KtxStrtPreviews		,"<PREVIEWS>");
_LIT(KtxEnddPreviews		,"</PREVIEWS>");

_LIT(KtxStrtITEM			,"<ITEM>");
_LIT(KtxEnddITEM			,"</ITEM>");


_LIT(KtxStrtLink			,"<link>");
_LIT(KtxEnddLink			,"</link>");
_LIT(KtxStrtType			,"<type>");
_LIT(KtxEnddType			,"</type>");
_LIT(KtxStrtImage			,"<image");
_LIT(KtxEnddImage			,"</image>");


_LIT(KtxRemoveUniStart		,"&#");
_LIT(KtxRemoveUniHex		,"x");
_LIT(KtxRemoveUniEnd		,";"); 

_LIT(KtxTypeWEBOPN			,"WEBOPN"); 
_LIT(KtxTypeDWNLOAD			,"DWNLOAD"); 
_LIT(KtxTypeSNDSMS			,"SNDSMS"); 
_LIT(KtxTypeLNKCLL			,"LNKCLL"); 


	class MItemParserCallBack
	{
	public:
		virtual void NewsParsedL() = 0;
		};
		
		
	class CMyLinks :public CBase
		{
	public:
		enum TLinkTypes
		{
			EMyLinkBrowseUrl,
			EMyLinkDwldUrl,
			EMyLinkSMS,
			EMyLinkCall
		};
		~CMyLinks(){delete iLink;delete iName;delete iInfo;};
		CMyLinks():iLink(NULL),iName(NULL),iInfo(NULL),iType(EMyLinkBrowseUrl){};
	public:
		HBufC* 		iLink;
		HBufC* 		iName;
		HBufC* 		iInfo;
		TLinkTypes	iType;
		};	
		
	class CMyPrevs :public CBase
		{
	public:
		~CMyPrevs(){delete iName; delete iInfo;delete iLink;};
		CMyPrevs():iName(NULL),iInfo(NULL),iLink(NULL){};
	public:
		HBufC* 		iName;
		HBufC* 		iInfo;
		HBufC* 		iLink;
		};	
		
	class CMyItems :public CBase
		{
	public:
		~CMyItems()
		{
			delete iTitle;
			delete iDescr;
			iLinksArray.ResetAndDestroy();
			iPrevsArray.ResetAndDestroy();
		};

	public:
		HBufC* 		iTitle;
		HBufC* 		iDescr;
		RPointerArray<CMyLinks>	iLinksArray;
		RPointerArray<CMyPrevs>	iPrevsArray;
		};
	
	
	class CCategoryItems :public CBase
		{
	public:
		~CCategoryItems(){delete iTitle;delete iFileName;iItemsArray.ResetAndDestroy();};
		CCategoryItems():iTitle(NULL),iFileName(NULL),iIcon(0){};
	public:
		HBufC* 					iTitle;
		HBufC* 					iFileName;
		TUint32					iIcon;
		RPointerArray<CMyItems>	iItemsArray;
		};


	class CRSSFeedParser : public CBase,MTimeOutNotify
	{
	public:
	    static CRSSFeedParser* NewL(RFs& aSession,MItemParserCallBack& aCallBack);
	    static CRSSFeedParser* NewLC(RFs& aSession,MItemParserCallBack& aCallBack);
	    ~CRSSFeedParser();
	public:	
		void ReadMainL(const TDesC& aFileName);
		void ReadCategoryL(CCategoryItems& aObject);
		RPointerArray<CCategoryItems> GetArray(void){return iArray;};
		TBool IsDone(void){ return iDone;};
	public:
		void GetHeaderL(const TDesC& aFile);
		void GetMainArrayL(RPointerArray<CCategoryItems>& aArray,const TDesC& aFile);
		void GetCategoryArrayL(RPointerArray<CMyItems>& aArray,const TDesC& aFile);
		void ConvertToUnicodeL(TDes& aTo,const TDesC8& aFrom,RFs& aSession,TUint& aCharset);
	protected:
		void TimerExpired();
	private:
	
		void GetPreviewArray(const TDesC& aData, RPointerArray<CMyPrevs>& aArray);
		void GetLinksArray(const TDesC& aData, RPointerArray<CMyLinks>& aArray);

		HBufC* RemoveEscapesL(const TDesC& aData);
		void ReplaceUniCodeNumbersL(TDes& aData);
		HBufC* GetHeaderItemL(const TDesC& aData, const TDesC& aStart, const TDesC& aEnd);
		void AppendCategoryItemL(const TDesC& aData, RPointerArray<CMyItems>& aArray);
		void AppendMainItemL(const TDesC& aData, RPointerArray<CCategoryItems>& aArray);
		void AppendLinksItemL(const TDesC& aData, RPointerArray<CMyLinks>& aArray);
		void AppendPreviewItemL(const TDesC& aData, RPointerArray<CMyPrevs>& aArray);
		CRSSFeedParser(RFs& aSession,MItemParserCallBack& aCallBack);
	    void ConstructL(void);
	private:
		RFs 							iSession;
		MItemParserCallBack& 			iCallBack;
		CTimeOutTimer*					iTimeOutTimer;
		RPointerArray<CCategoryItems> 	iArray;
		HBufC*							iFileName;
		TBool							iDone;
		TTime iModified,iExpires;
	};


#endif // __RSSFEED_READER_H__

