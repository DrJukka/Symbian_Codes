#include "CPUControl.h"

#include <coemain.h>    // CCoeEnv
#include <eikenv.h>     // CEikonEnv

/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
void CCPUControl::ConstructL(const TRect& aRect, const CCoeControl* aParent)
{
   SetContainerWindowL(*aParent);
   SetRect(aRect);
   ActivateL();
}
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
CCPUControl::~CCPUControl()
{

}

/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
void CCPUControl::Draw(const TRect& /*aRect*/) const
{
    CWindowGc& gc = SystemGc();
	gc.Clear();
	
	TRect DrawRect(Rect());
	DrawRect.Shrink(2,2);
	gc.DrawLine(DrawRect.iTl,TPoint(DrawRect.iTl.iX,DrawRect.iBr.iY));
	gc.DrawLine(DrawRect.iTl,TPoint(DrawRect.iBr.iX,DrawRect.iTl.iY));
	gc.DrawLine(DrawRect.iBr,TPoint(DrawRect.iTl.iX,DrawRect.iBr.iY));
	gc.DrawLine(DrawRect.iBr,TPoint(DrawRect.iBr.iX,DrawRect.iTl.iY));
}
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
void CCPUControl::SizeChanged()
{

}
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/


