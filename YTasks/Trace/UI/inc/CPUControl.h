/*
	Copyright (C) 2006 Jukka Silvennoinen
	All right reserved
*/

#ifndef __YTASKS_CPUCONTAINER_H__
#define __YTASKS_CPUCONTAINER_H__

#include <coecntrl.h>   // CCoeControl

class CCPUControl : public CCoeControl
{
public:
    void ConstructL(const TRect& aRect, const CCoeControl* aParent);
    ~CCPUControl();

private:
    void Draw(const TRect& aRect) const;
    void SizeChanged();

private:

};

#endif // __YTASKS_CPUCONTAINER_H__
