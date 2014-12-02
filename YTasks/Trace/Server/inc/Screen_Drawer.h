
#ifndef __SCREEN_DRAWER_H__
#define __SCREEN_DRAWER_H__

#include <e32std.h>
#include <w32std.h>
#include <coedef.h>
#include <apgwgnam.h>

struct TSettingsItem
	{
		TBool iEnabledOn,iDrawStyle;
		TInt iVerSlider,iHorSlider,iFontSize;
	};	

_LIT(KTxMyWGName			,"Trace");


class CScreenDrawer : public CActive

{
public:
	static CScreenDrawer* NewL(const TSettingsItem& aSettings);
	static CScreenDrawer* NewLC(const TSettingsItem& aSettings);
	~CScreenDrawer();
	void SetValues(const TInt& aMemory,const TInt& aTotalRamInBytes,const TInt& aCpuLoad,const TInt& aGetMaxCPU);
protected:
	void DoCancel();
	TInt RunError(TInt aError);
	void RunL();
private:
	CScreenDrawer(const TSettingsItem& aSettings);
	void ConstructL(void);
	void Listen();
	void Draw(void);
	void PieDrawer(const TPoint& aLeftTop,const TInt& aSize, const TInt& aProsentages, TBool aInvertColors);
private:
	TSettingsItem		iSettings;
	RWsSession 			iWsSession; 
	CWindowGc* 			iGc;
	CWsScreenDevice* 	iScreen;
	RWindowGroup 		iWg; 
	RWindow 			iMyWindow; 
	CFont*				iMyFont;
	TRect				iScreenRect;
	TInt				iCpuVal,iMemVal;
};




#endif //__SCREEN_DRAWER_H__


