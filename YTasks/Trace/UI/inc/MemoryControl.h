/*
	Copyright (C) 2006 Jukka Silvennoinen
	All right reserved
*/

#ifndef __YTASKS_MEMORYCONTAINER_H__
#define __YTASKS_MEMORYCONTAINER_H__

#include <coecntrl.h>   // CCoeControl

class CMemControl : public CCoeControl
{
public:
    void ConstructL(const TRect& aRect, const CCoeControl* aParent);
    ~CMemControl();

private:
    void Draw(const TRect& aRect) const;
    void SizeChanged();

private:

};

#endif // __YTASKS_MEMORYCONTAINER_H__
