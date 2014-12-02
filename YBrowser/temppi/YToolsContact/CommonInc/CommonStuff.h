/*
	Copyright (C) 2006 Jukka Silvennoinen
	
	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef COMMONSTUFF_HEADER_H
#define COMMONSTUFF_HEADER_H

#include <e32base.h>


#include <BldVariant.hrh>



_LIT(KtxApplicationName		,"YTools Contacts");
_LIT(KtxAbbout				,"version 0.76(Beta-test)\nNovember 14th 2006\nfor S60, Symbian OS 9,\n\nCopyright:\nJukka Silvennoinen 2006\nAll right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.pushl.com/y_browser/");

_LIT(KtxRec1Abbout			,"version 0.76(Beta-test)\nNovember 14th 2006\nfor S60, Symbian OS 9,\n\nCopyright\nJukka Silvennoinen 2006\nAll right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.pushl.com/y_browser/");


_LIT(KtxTypeName			,"YTools Contacts");
_LIT(KtxType				,"YTools/CVcrd");
#ifdef __SERIES60_3RD
	_LIT(KtxIconsFileName		,"\\system\\data\\A00007A6\\A0000F67\\icons.mbm");
	_LIT(KMyResourceFileName	,"\\resource\\apps\\YToolsCVcrdUI_res.RSC");
#else
	_LIT(KtxIconsFileName		,"\\system\\data\\A00007A6\\A0000F67\\icons.mbm");
	_LIT(KMyResourceFileName	,"\\system\\libs\\plugins\\YToolsCVcrdUI_res.RSC");
#endif


#endif	// COMMONSTUFF_HEADER_H
