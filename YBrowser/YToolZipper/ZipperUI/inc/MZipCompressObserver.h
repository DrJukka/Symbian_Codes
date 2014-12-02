/* 
Copyright (c) 2007 Dr. Jukka Silvennoinen www.DrJukka.com

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. THE SOFTWARE IS ALSO SUBJECT TO CHANGE WITHOUT PRIOR NOTICE, 
NO COMPATIBILITY BETWEEN MODIFICATIONS IS GUARANTEED. 
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name(s) of the above copyright holders 
shall not be used in advertising or otherwise to promote the sale, use or other 
dealings in this Software without prior written authorization.

*/


#ifndef __YUCCATOOLS_COMPRESSOBSERVER_H__
#define __YUCCATOOLS_COMPRESSOBSERVER_H__

#include "Public_Interfaces.h"

	class MZipCompressObserver
	{
	public:	//	New methods
		virtual void CompressProcessL(TInt aFileCount,const TDesC& aCurrentFile) = 0;
		virtual void ProcessFinnishedL(CBase* aObject) = 0;
		virtual void AddFileToListL(TUint32 aCompressedSize,TUint32 aUncompressedSize,const TDesC& aCurrentFile,TInt aIconId) = 0;
		virtual MYBrowserFileHandlerUtils& GetFileHandler() = 0;
	};
	


#endif // __YUCCATOOLS_COMPRESSOBSERVER_H__
