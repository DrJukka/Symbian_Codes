/*
	Copyright (C) 2006 Jukka Silvennoinen
*/

#ifndef COMMONSTUFF_HEADER_H
#define COMMONSTUFF_HEADER_H

#include <e32base.h>


_LIT(KtxApplicationName			,"YTools MBM/MIF Viewer");
_LIT(KtxAbbout					,"version 0.79(Beta test)\nFebruary 19th 2007\nfor S60, Symbian OS 9,\n\nCopyright:\nJukka Silvennoinen 2006\nAll right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.pushl.com/y_browser/");

_LIT(KtxRec1Abbout				,"version 0.79(Beta test)\nFebruary 19th 2007\nfor S60, Symbian OS 9,\n\nCopyright\nJukka Silvennoinen 2006\nAll right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.pushl.com/y_browser/");

_LIT(KtxTypeIMageMIF			,"image/epoc-mif");
_LIT(KtxTypeIMageMIFName		,"Mif image");

_LIT(KtxTypeIMageMBM			,"image/x-epoc-mbm");
_LIT(KtxTypeIMageMBMName		,"Mbm image");



#ifdef __SERIES60_3X__
	_LIT(KtxIconsFileName		,"\\system\\data\\A00007A6\\A0000F65\\icons.mbm");
	_LIT(KMyResourceFileName	,"\\resource\\apps\\YToolsMBM_MIFUI_res.RSC");
#else
	_LIT(KtxIconsFileName		,"\\system\\data\\A00007A6\\A0000F65\\icons.mbm");
	_LIT(KMyResourceFileName	,"\\system\\libs\\plugins\\YToolsMBM_MIFUI_res.RSC");
#endif


#endif	// COMMONSTUFF_HEADER_H
