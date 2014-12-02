/* 	
	Copyright (c) 2001 - 2007, 
	J.P. Silvennoinen.
	All rights reserved 
*/

#ifndef __YTASKS_ECOMHANDLER_H__
#define __YTASKS_ECOMHANDLER_H__

#include <E32BASE.H>
#include <YTasks_PublicInterface.h>

static const TUid KUidYTaskHandUID = {0x20009999};

/*
----------------------------------------------------------------
*/

_LIT8(KtxStrtYTools					,"<YTASKS>");
_LIT8(KtxEnddYTools					,"</YTASKS>");

_LIT8(KtxStrtVERSION				,"<VERSION>");
_LIT8(KtxEnddVERSION				,"</VERSION>");
_LIT8(KtxCurrVERSION				,"050");

_LIT8(KtxStrtSERVER					,"<SERVER>");
_LIT8(KtxEnddSERVER					,"</SERVER>");
_LIT8(KtxStrtICONFILE				,"<ICONFILE>");
_LIT8(KtxEnddICONFILE				,"</ICONFILE>");

/*
----------------------------------------------------------------
*/
	class CHandlerItem :public CBase
		{
	public:
		~CHandlerItem(){delete iName;delete iIconfile; delete iServerName;};
	public:
		TUid   	iUid;
		HBufC* 	iName;
		HBufC* 	iIconfile;
		HBufC* 	iServerName;
		};
/*
----------------------------------------------------------------
*/

	class CEcomHandler: public CBase
		{
	public:
		static CEcomHandler* NewL(MTasksHandlerExit* aTasksHandlerExit);
		~CEcomHandler();
		void ReFreshHandlersL();
		RPointerArray<CHandlerItem>& HandlerArray(){ return iHandlers;};
		CYTasksContainer* GetHandlerL(CHandlerItem& aHandler,CEikButtonGroupContainer* aCba);
	protected:
		CEcomHandler(MTasksHandlerExit* aTasksHandlerExit);
		void ConstructL();
		TBool CheckVersionL(const TDesC8& aData);
		HBufC* ParseInfoL(const TDesC8& aData,const TDesC8& aStartTag,const TDesC8& aEndTag);
	private:
		MTasksHandlerExit* 			iTasksHandlerExit;
		RPointerArray<CHandlerItem> iHandlers;
		};

#endif // __YTASKS_ECOMHANDLER_H__
