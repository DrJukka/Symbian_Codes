/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen
	All rights reserved 
*/
#ifndef __YB_SHOWSTRING_H__
#define __YB_SHOWSTRING_H__

#include <COECNTRL.H>
#include <W32STD.H>

	static const TUid KUidClipBoard = {0x0};


	class CShowString: public CCoeControl
	{
		public:
			void ConstructL(const TDesC& aBuffer);
			CShowString(void);
			~CShowString();
			TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
			void HandleViewCommandL(TInt aCommand);
			void UpDateSizeL(void);
		private:
			void Draw(const TRect& aRect) const;
		private:			
			TInt 			iCurrentCut;
			HBufC* 			iString;
			const CFont*	iMyFont;
	};


#endif	//	__YB_SHOWSTRING_H__
