/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen
	All rights reserved 
*/
#include <eikenv.h>         //CEikEnv
#include <eikspane.h>       //CEikStatusPane
#include <avkon.hrh>
#include <aknnavi.h>        //CAknNavigationControlContainer
#include <aknnavide.h>      //CAknNavigationDecorator
#include <aknindicatorcontainer.h>  //CAknIndicatorContainer
#include <aknutils.h>
#include <aknappui.h>
#include <avkon.mbg>
#include <coeutils.h>
#include <eiklabel.h>
#include <aknconsts.h>

#include "Navipanel.h"

#include <aknsdrawutils.h>
#include <aknscontrolcontext.h>
#include <AknsSkinInstance.h>
#include <aknsutils.h>


const TInt 			KNaviReFreshTimeOut 	= 200000;
const TInt 			KNaviReFreshTimeOutLong	= 800000;
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CYuccaNavi* CYuccaNavi::NewL()
    {
    CYuccaNavi* self = new(ELeave)CYuccaNavi();
	self->ConstructL();
    return self;
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CYuccaNavi::CYuccaNavi()
    {
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CYuccaNavi::~CYuccaNavi()
{
	delete iFullText;
	iFullText = NULL;
	if(iTimeOutTimer)
	{
		iTimeOutTimer->Cancel();
	}
	delete iTimeOutTimer;
	iTimeOutTimer = NULL;


	if (!iNaviPane)
    {
    	CEikStatusPane *sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
        //Fetch pointer to the default navi pane control
        iNaviPane = (CAknNavigationControlContainer *)sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi));
    }
    
    iNaviPane->Pop(NULL); 
    iNaviPane = NULL;
    
    delete iNaviDecorator;
    iNaviDecorator = NULL;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CYuccaNavi::ConstructL(void)
{
    if (!iNaviPane)
    {
        CEikStatusPane *sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
        if(sp)
        {
	        iNaviPane = (CAknNavigationControlContainer *)sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi));
        }
    }
    
    if(iNaviPane)
    {
		delete iNaviDecorator;
		iNaviDecorator = NULL;
		iNaviDecorator = iNaviPane->CreateNavigationLabelL(_L("Hello world"));
		iNaviPane->PushL(*iNaviDecorator);
    }
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CYuccaNavi::GetTextL(TDes& aText)
{
	if(iNaviDecorator)
	{
		if(iNaviDecorator->DecoratedControl())
		{
			CAknNaviLabel* Tmp = STATIC_CAST(CAknNaviLabel*, iNaviDecorator->DecoratedControl());
			
			const TDesC* Texksti = Tmp->Text();
			if(Texksti)
			{
				aText.Copy(*Texksti);
			}
		}
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CYuccaNavi::SetTextL(const TDesC& aText)
{

	if(iTimeOutTimer)
	{
		iTimeOutTimer->Cancel();
	}
	delete iTimeOutTimer;
	iTimeOutTimer = NULL;
	
	delete iFullText;
	iFullText = NULL;
	
	if(aText.Length())
	{
		iFullText = aText.AllocL();
	}
	
	iGoingRight = ETrue;
	iCurrentCut = 0;
	
 	if(iNaviDecorator)
	{
		if(iNaviDecorator->DecoratedControl())
		{
			CAknNaviLabel* Tmp = STATIC_CAST(CAknNaviLabel*, iNaviDecorator->DecoratedControl());	
			
			if(iFullText)
			{
				TInt MyWidth = ((Tmp->Size().iWidth * 15) / 20);
				
				const CFont* UseFontMe = AknLayoutUtils::FontFromId(EAknLogicalFontSecondaryFont);
			
				TInt CharCnt= UseFontMe->TextCount(iFullText->Des(),MyWidth);
				
				if(CharCnt < iFullText->Des().Length())
				{
					iTimeOutTimer = CTimeOutTimer::NewL(EPriorityNormal, *this);
					iTimeOutTimer->After(KNaviReFreshTimeOut);
				}
			
				Tmp->SetTextL(iFullText->Des());
			}
			else
			{
				Tmp->SetTextL(KNullDesC);
			}
			
			CEikStatusPane *sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
        	if(sp)
        	{
        		sp->DrawNow();
        	}
		}
	}  
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CYuccaNavi::TimerExpired()
{
	if(iFullText && iNaviDecorator)
	{
		if(iNaviDecorator->DecoratedControl())
		{
			CAknNaviLabel* Tmp = STATIC_CAST(CAknNaviLabel*, iNaviDecorator->DecoratedControl());	
			
	
			TTimeIntervalMicroSeconds32 ReFreshInterval(KNaviReFreshTimeOut);
			
			TInt MyWidth = ((Tmp->Size().iWidth * 15) / 20);
			
			if(iGoingRight)
			{
				iCurrentCut++;
				
				if(iCurrentCut > iFullText->Des().Length())
				{
					iCurrentCut = 0; // never happens..
				}
				
				const CFont* UseFontMe = AknLayoutUtils::FontFromId(EAknLogicalFontSecondaryFont);
		
				TInt TextToDraw = iFullText->Des().Length() - iCurrentCut;
				TInt CharCnt = UseFontMe->TextCount(iFullText->Des().Right(TextToDraw),MyWidth);
			
				if(CharCnt >= TextToDraw)
				{
					if(iExtraCutCount < 3)
					{
						iExtraCutCount++;	
					}
					else
					{
						// lets change direction
						ReFreshInterval = KNaviReFreshTimeOutLong;
						iGoingRight = EFalse;
					}
				}
			}
			else // back to left
			{
				iCurrentCut--;
				if(iCurrentCut < 0)
				{
					ReFreshInterval = KNaviReFreshTimeOutLong;
					iExtraCutCount = 0;
					iCurrentCut = 0;
					iGoingRight = ETrue;
				}
			}
			
			if(iCurrentCut < iFullText->Des().Length())
			{
				TInt TextToDraw = iFullText->Des().Length() - iCurrentCut;
				
				if(iCurrentCut < 0)
				{
					TextToDraw = iFullText->Des().Length();
				}
				
				Tmp->SetTextL(iFullText->Des().Right(TextToDraw));
			}
			else
			{
				Tmp->SetTextL(*iFullText);
			}
			
			if(iTimeOutTimer)
			{
				iTimeOutTimer->After(ReFreshInterval);
			}
		
			CEikStatusPane *sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
        	if(sp)
        	{
        		sp->DrawNow();
        	}
		}
	}
}


// End of file
