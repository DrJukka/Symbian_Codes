/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen
	All rights reserved 
*/

#include <aknmessagequerydialog.h>
#include <BAUTILS.H>
#include <AknLists.h>
#include <StringLoader.h>

#include "General_Settings.h"
#include "IconHandler.h"

#include "YuccaBrowser.h"
#include "YuccaBrowser.hrh"


#include "Common.h"
#ifdef SONE_VERSION
	#include <YFB_2002B0AA.rsg>
#else
	#ifdef LAL_VERSION

	#else
		#ifdef __YTOOLS_SIGNED
			#include <YuccaBrowser_2000713D.rsg>
		#else
			#include <YuccaBrowser.rsg>
		#endif
	#endif
#endif




/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CGeneralSettings* CGeneralSettings::NewL(void)
    {
    CGeneralSettings* self = CGeneralSettings::NewLC();
    CleanupStack::Pop(self);
    return self;
    }
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CGeneralSettings* CGeneralSettings::NewLC(void)
    {
    CGeneralSettings* self = new (ELeave) CGeneralSettings();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CGeneralSettings::~CGeneralSettings()
{
	delete iCommandSCArray;
	iCommandSCArray = NULL;
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CGeneralSettings::CGeneralSettings()
    {
	
    }

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CGeneralSettings::ConstructL()
{
    
}


/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
TBool CGeneralSettings::ShowDisclaimerL(void)
{
	TBool OkToContinue(EFalse);

	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone != AppFile.FindByDir(KtxDisclaimerFileName, KNullDesC))
	{
		HBufC* Abbout = KtxDisclaimer().AllocLC();
		TPtr Pointter(Abbout->Des());
		CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(Pointter);
		dlg->PrepareLC(R_DDD_HEADING_PANE);
		dlg->SetHeaderTextL(KtxDisclaimerTitle);  
		if(dlg->RunLD())
		{
			TFileName ShortFil;
			if(KErrNone ==CCoeEnv::Static()->FsSession().PrivatePath(ShortFil))
			{
				TFindFile privFile(CCoeEnv::Static()->FsSession());
				if(KErrNone == privFile.FindByDir(ShortFil, KNullDesC))
				{
					TParsePtrC hjelp(privFile.File());
					ShortFil.Copy(hjelp.Drive());
					ShortFil.Append(KtxDisclaimerFileName);
				}
			}

		
			BaflUtils::EnsurePathExistsL(CCoeEnv::Static()->FsSession(),ShortFil);
		
			RFile MyFile;
			if(KErrNone == MyFile.Create(CCoeEnv::Static()->FsSession(),ShortFil,EFileWrite))
			{
				TTime NowTime;
				NowTime.HomeTime();
				
				TBuf8<255> InfoLine;
				InfoLine.Copy(_L8("Accepted on Date\t"));
					
				InfoLine.AppendNum(NowTime.DateTime().Day() + 1);
				InfoLine.Append(_L8("."));
				InfoLine.AppendNum((NowTime.DateTime().Month() + 1));
				InfoLine.Append(_L8("."));
				InfoLine.AppendNum(NowTime.DateTime().Year());
				InfoLine.Append(_L8(" "));
				InfoLine.Append(_L8("--"));
				InfoLine.AppendNum(NowTime.DateTime().Hour());
				InfoLine.Append(_L8(":"));		
				TInt HelperInt = NowTime.DateTime().Minute();
				if(HelperInt < 10)
					InfoLine.AppendNum(0);
				InfoLine.AppendNum(HelperInt);
				InfoLine.Append(_L8(":"));
				HelperInt = NowTime.DateTime().Second();
				if(HelperInt < 10)
					InfoLine.AppendNum(0);
				InfoLine.AppendNum(HelperInt);
				InfoLine.Append(_L8(" "));
				
				MyFile.Write(InfoLine);
				MyFile.Close();
			}
			
			OkToContinue = ETrue;
		}
		
		CleanupStack::PopAndDestroy(Abbout);
	}
	else
	{
		OkToContinue = ETrue;
	}
	
	return OkToContinue;
}
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
void CGeneralSettings::AddSCNumbertoMenuL(CEikMenuPane* aMenuPane)
{	
	if(aMenuPane && iCommandSCArray)
	{
		for(TInt i=0; i < iCommandSCArray->Count(); i++)
		{
			TInt ittemPosition(-1);
		
			if(aMenuPane->MenuItemExists(iCommandSCArray->At(i),ittemPosition))
			{
				if(ittemPosition >= 0)
				{
					TBuf<100> addText;
					addText.Copy(aMenuPane->ItemData(iCommandSCArray->At(i)).iText);
					
					addText.Append(_L(" ["));
					addText.AppendNum(i);
					addText.Append(_L("]"));
				
					aMenuPane->SetItemTextL(iCommandSCArray->At(i),addText);
				}
			}
		}
	}
}
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
TBool CGeneralSettings::SelectAndChangeIconL(void)
{	
	TBool Ret(EFalse);
	
	TFileName IconName,SelFile;

	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone == AppFile.FindByDir(KtxImagesFileName, KNullDesC))
	{
		IconName.Copy(AppFile.File());
	}
	
	if(IconName.Length())
	{	    
		CAknSinglePopupMenuStyleListBox* list = new(ELeave) CAknSinglePopupMenuStyleListBox;
		CleanupStack::PushL(list);

	    CAknPopupList* popupList = CAknPopupList::NewL(list, R_AVKON_SOFTKEYS_SELECT_BACK);  
	    CleanupStack::PushL(popupList);

	    list->ConstructL(popupList, 0);
	    list->CreateScrollBarFrameL(ETrue);
	    list->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);
	    
		CDesCArrayFlat* AnimList = new(ELeave)CDesCArrayFlat(10);
		CleanupStack::PushL(AnimList);	
	
		CDir* File_list(NULL);
		
		TParsePtrC Hjelpper(IconName);
		
		SelFile.Copy(Hjelpper.Drive());
		SelFile.Append(KTxtIconsFolder);
		
		if(KErrNone == CCoeEnv::Static()->FsSession().GetDir(SelFile,KEntryAttMaskSupported,EDirsFirst,File_list))	
		{
			if(File_list)
			{
				TBuf<5> HelpBuffer;
				
				File_list->Sort(ESortByName);
				for(TInt i=0; i < File_list->Count(); i++)
				{
					if((*File_list)[i].IsSystem() 
					|| (*File_list)[i].IsHidden()
					|| (*File_list)[i].IsDir())
					{
						// ignore System & Hidden Files
					}
					else if((*File_list)[i].iName.Length() > 4)
					{
						HelpBuffer.CopyLC((*File_list)[i].iName.Right(4));
						
						if(HelpBuffer == KTxtMifExtension)
						{
							AnimList->AppendL((*File_list)[i].iName);
						}
					}
				}
			}
		}
		
		delete File_list;
		File_list = NULL;
	
		CleanupStack::Pop(AnimList);
	    list->Model()->SetItemTextArray(AnimList);
	    list->Model()->SetOwnershipType(ELbmOwnsItemArray);

		StringLoader::Load(SelFile,R_SH_STR_SELICONFF);
		popupList->SetTitleL(SelFile);
		
		if (popupList->ExecuteLD())
	    {
			TInt Selected = list->CurrentItemIndex();
			if(Selected >= 0 && AnimList->MdcaCount() > Selected)
			{
				SelFile.Copy(AnimList->MdcaPoint(Selected));
				
		
				TParsePtrC AnotherHjelp1(IconName);
				TParsePtrC AnotherHjelp2(SelFile);
					
				TFileName Target;
				Target.Copy(AnotherHjelp1.DriveAndPath());
				Target.Append(AnotherHjelp2.NameAndExt());

				SaveIconNameL(Target);
	
				if(iIconHandler)
				{
					iIconHandler->SetIconFile(Target);
					iIconHandler->ReReadIcosL();
				}
				
				Ret = ETrue;	
			}
		}
	  	CleanupStack::Pop();             // popuplist
		CleanupStack::PopAndDestroy(1);  // list
	}
	
	return Ret;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CGeneralSettings::ShowPasteFilesL(CDesCArray* aArray,const TDesC& aTitle)
{
	if(aArray && iFFolderReader)
	{
		if(aArray->MdcaCount())
		{
			CAknSingleGraphicPopupMenuStyleListBox* list = new(ELeave) CAknSingleGraphicPopupMenuStyleListBox;
		    CleanupStack::PushL(list);

			CAknPopupList* popupList = CAknPopupList::NewL(list,R_AVKON_SOFTKEYS_BACK);
		    CleanupStack::PushL(popupList);
			
		    list->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
			list->CreateScrollBarFrameL(ETrue);
		    list->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);
		    
		    CDesCArrayFlat* Array = new (ELeave) CDesCArrayFlat(1);
			CleanupStack::PushL(Array);
			
			TInt FileType(CFFileItem::EFJustFile);
			TFileName AddFil,Hjrlp;
			TEntry MyEntry;
			
			for(TInt i=0; i < aArray->MdcaCount(); i++)
			{
				if(KErrNone == CCoeEnv::Static()->FsSession().Entry(aArray->MdcaPoint(i),MyEntry))
				{
					FileType = (CFFileItem::EFJustFile);
					if(iFFolderReader)
					{
						// do we need to care on the return value, jukka 16.12.2007
						iFFolderReader->FileTypeOkL(aArray->MdcaPoint(i),MyEntry,FileType);
					}

					TParsePtrC HjelpP(aArray->MdcaPoint(i));
					Hjrlp.Copy(HjelpP.NameAndExt());
				
					AddFil.Format(KtxShortListFormat,FileType,&Hjrlp);
					Array->AppendL(AddFil);				
				}
			}
			
		    list->Model()->SetItemTextArray(Array);
		    CleanupStack::Pop();//Array
			list->Model()->SetOwnershipType(ELbmOwnsItemArray);
			
			list->ItemDrawer()->ColumnData()->SetIconArray(iIconHandler->GetIconArrayL());
			
			popupList->SetTitleL(aTitle);
			popupList->ExecuteLD();
			
		    CleanupStack::Pop();            // popuplist
		    CleanupStack::PopAndDestroy();  // list
		}
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TKeyResponse CGeneralSettings::MoveToShortCutL(TInt aShortCut, TDes& aNewPath)
{
	TKeyResponse Ret = EKeyWasNotConsumed;
	
	TFileName ShortFil;
	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone != AppFile.FindByDir(KtxShortCutFileName, KNullDesC))
	{
		if(KErrNone ==CCoeEnv::Static()->FsSession().PrivatePath(ShortFil))
		{
			TFindFile privFile(CCoeEnv::Static()->FsSession());
			if(KErrNone == privFile.FindByDir(ShortFil, KNullDesC))
			{
				TParsePtrC hjelp(privFile.File());
				ShortFil.Copy(hjelp.Drive());
				ShortFil.Append(KtxShortCutFileName);
			}
		}
	}
	else
	{
		ShortFil.Copy(AppFile.File());
	}

	if(ShortFil.Length() && iFFolderReader)
	{
		CDictionaryFileStore* iDStore = CDictionaryFileStore::OpenLC(CCoeEnv::Static()->FsSession(),ShortFil, TUid::Uid(0x101F7BBD));

		TUid FileUid = {0};
		FileUid.iUid = 0x10 + aShortCut;

		if(iDStore->IsPresentL(FileUid))
		{
			RDictionaryReadStream in;
			in.OpenLC(*iDStore,FileUid);
			in >> aNewPath;
			CleanupStack::PopAndDestroy(1);// in
			
			Ret = EKeyWasConsumed;
		}

		CleanupStack::PopAndDestroy(1);// Store
	}
 
	 return Ret;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CGeneralSettings::SetShortCutL(TInt aShortCut, const TDesC& aPath)
{
	TFileName ShortFil;
	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone != AppFile.FindByDir(KtxShortCutFileName, KNullDesC))
	{
		if(KErrNone ==CCoeEnv::Static()->FsSession().PrivatePath(ShortFil))
		{
			TFindFile privFile(CCoeEnv::Static()->FsSession());
			if(KErrNone == privFile.FindByDir(ShortFil, KNullDesC))
			{
				TParsePtrC hjelp(privFile.File());
				ShortFil.Copy(hjelp.Drive());
				ShortFil.Append(KtxShortCutFileName);
			}
		}
	}
	else
	{
		ShortFil.Copy(AppFile.File());
	}

	if(ShortFil.Length())
	{
		
		CDictionaryFileStore* iDStore = CDictionaryFileStore::OpenLC(CCoeEnv::Static()->FsSession(),ShortFil, TUid::Uid(0x101F7BBD));

		TUid FileUid = {0};
		FileUid.iUid = 0x10 + aShortCut;

		if(iDStore->IsPresentL(FileUid))
		{
			iDStore->Remove(FileUid);
			iDStore->CommitL();
		}

		RDictionaryWriteStream out1;
		out1.AssignLC(*iDStore,FileUid);

		out1 << aPath;

		out1.CommitL(); 
		CleanupStack::PopAndDestroy(1);// out1

		iDStore->CommitL();
		CleanupStack::PopAndDestroy(1);// Store,
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CGeneralSettings::ShowShortCutListL(void)
{
	TInt Ret = -1;

	TFileName ShortFil;
	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone != AppFile.FindByDir(KtxShortCutFileName, KNullDesC))
	{
		if(KErrNone ==CCoeEnv::Static()->FsSession().PrivatePath(ShortFil))
		{
			TFindFile privFile(CCoeEnv::Static()->FsSession());
			if(KErrNone == privFile.FindByDir(ShortFil, KNullDesC))
			{
				TParsePtrC hjelp(privFile.File());
				ShortFil.Copy(hjelp.Drive());
				ShortFil.Append(KtxShortCutFileName);
			}
		}
	}
	else
	{
		ShortFil.Copy(AppFile.File());
	}

	if(ShortFil.Length())
	{
		TFileName Hjelpper;
		CDesCArray* Array = new (ELeave) CDesCArrayFlat(1);
		CleanupStack::PushL(Array);

		if(BaflUtils::FileExists(CCoeEnv::Static()->FsSession(),ShortFil))
		{
			CDictionaryFileStore* iDStore = CDictionaryFileStore::OpenLC(CCoeEnv::Static()->FsSession(),ShortFil, TUid::Uid(0x101F7BBD));

			TUid FileUid = {0};
			
			for(TInt i = 0; i < 10; i++)
			{
				FileUid.iUid = 0x10 + 48 + i;

				if(iDStore->IsPresentL(FileUid))
				{
					RDictionaryReadStream in;
					in.OpenLC(*iDStore,FileUid);

					in >> Hjelpper;

					if(!Hjelpper.Length())
					{
						StringLoader::Load(Hjelpper,R_SH_STR_ROOT);
					}

					CleanupStack::PopAndDestroy(1);// in
				}
				else
				{	
					StringLoader::Load(Hjelpper,R_SH_STR_FREE);
				}
				
				ShortFil.Format(KtxFormatShortCutlList,i,&Hjelpper);
				Array->AppendL(ShortFil);
			}

			CleanupStack::PopAndDestroy();// Store
		}
		else
		{	
			StringLoader::Load(Hjelpper,R_SH_STR_FREE);
		
			for(TInt i = 0; i < 10; i++)
			{
				ShortFil.Format(KtxFormatShortCutlList,i,&Hjelpper);
				Array->AppendL(ShortFil);
			}
		}

		CAknSinglePopupMenuStyleListBox* list = new(ELeave)CAknSinglePopupMenuStyleListBox;
	    CleanupStack::PushL(list);
	    CAknPopupList* popupList = CAknPopupList::NewL(list,R_AVKON_SOFTKEYS_SELECT_CANCEL);
	    CleanupStack::PushL(popupList);
		
	    list->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
		list->CreateScrollBarFrameL(ETrue);
	    list->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);
	    
	    list->Model()->SetItemTextArray(Array);
		list->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);
		
		StringLoader::Load(ShortFil,R_SH_STR_SCTITLE);
		popupList->SetTitleL(ShortFil);
		if (popupList->ExecuteLD())
	    {
			Ret = list->CurrentItemIndex();
		};
		
	    CleanupStack::Pop();            // popuplist
	    CleanupStack::PopAndDestroy(2);  // list,Array
	}
	return Ret;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CGeneralSettings::ShowInfoL(const TDesC& aFileName,TInt aFileType)
{
	if(aFileType >= CFFileItem::EFDriveFile)
	{
		CDesCArrayFlat* Array = new (ELeave) CDesCArrayFlat(1);
		CleanupStack::PushL(Array);
		
		TEntry MyEntry;
		TFileName InfoLine,Helper;
		TBuf<100> Helper2;
		
		if(aFileType == CFFileItem::EFDriveFile)
		{
			if(aFileName.Length())
			{
				TInt CurrentDrive;
				TVolumeInfo volumeInfo; 
				TChar DriveChar = aFileName[0];
				
				CCoeEnv::Static()->FsSession().CharToDrive(DriveChar,CurrentDrive); 

				if(CCoeEnv::Static()->FsSession().IsValidDrive(CurrentDrive))
				{
					CCoeEnv::Static()->FsSession().Volume(volumeInfo,CurrentDrive);
					
					StringLoader::Load(Helper,R_SH_STR_FORMETNAME);
					InfoLine.Format(Helper,&volumeInfo.iName);
					Array->AppendL(InfoLine); 
					
					Helper2.Zero();
					Helper.Zero();
					CCoeEnv::Static()->FsSession().FileSystemName(Helper,CurrentDrive);
					StringLoader::Load(Helper2,R_SH_STR_FORMETFSYS);
					InfoLine.Format(Helper2,&Helper);
					Array->AppendL(InfoLine); 

			/**/		Helper.Num(volumeInfo.iUniqueID,EHex);
					
					StringLoader::Load(Helper2,R_SH_STR_FORMETID);
					InfoLine.Format(Helper2,&Helper);
					Array->AppendL(InfoLine);
					
					
			
					TUint32 HelperInt = (volumeInfo.iFree / 1024);
					
					StringLoader::Load(Helper,R_SH_STR_FORMETFREE);
					InfoLine.Format(Helper,HelperInt,EDecimal);	
					Array->AppendL(InfoLine);
					
					HelperInt = ((volumeInfo.iSize - volumeInfo.iFree) / 1024);

					StringLoader::Load(Helper,R_SH_STR_FORMETUSED);
					InfoLine.Format(Helper,HelperInt,EDecimal);
					Array->AppendL(InfoLine);
					
					HelperInt = (volumeInfo.iSize / 1024);	
					
					StringLoader::Load(Helper,R_SH_STR_FORMETSIZEKB);
					InfoLine.Format(Helper,HelperInt,EDecimal);
					Array->AppendL(InfoLine);
					
					switch(volumeInfo.iDrive.iType)
					{
					case EMediaNotPresent:
						StringLoader::Load(Helper,R_SH_STR_NOTPRESENT);
						break;
					case EMediaFloppy:
						StringLoader::Load(Helper,R_SH_STR_FLOPPY);
						break;
					case EMediaHardDisk:
						StringLoader::Load(Helper,R_SH_STR_HARDDISK);
						break;
					case EMediaCdRom:
						StringLoader::Load(Helper,R_SH_STR_CDROM);
						break;
					case EMediaRam:
						StringLoader::Load(Helper,R_SH_STR_RAM);
						break;
					case EMediaFlash:
						StringLoader::Load(Helper,R_SH_STR_FLASH);
						break;
					case EMediaRom:
						StringLoader::Load(Helper,R_SH_STR_ROM);
						break;	
					case EMediaRemote:
						StringLoader::Load(Helper,R_SH_STR_REMOTE);
						break;
					default: //case TMediaType::EMediaUnknown:
						StringLoader::Load(Helper,R_SH_STR_UNKNOWN);
						break;
					}
					
					StringLoader::Load(Helper2,R_SH_STR_FORMETTYPE);
					InfoLine.Format(Helper2,&Helper);
					Array->AppendL(InfoLine);

					TUint AnotherHelper = volumeInfo.iDrive.iMediaAtt;

					if(AnotherHelper & KMediaAttWriteProtected)
					{
						StringLoader::Load(Helper,R_SH_STR_ATRWPROT);
						InfoLine.Copy(Helper);
						Array->AppendL(InfoLine);
					}

					if(AnotherHelper & KMediaAttLocked)
					{
						StringLoader::Load(Helper,R_SH_STR_ATRLOCKED);
						InfoLine.Copy(Helper);
						Array->AppendL(InfoLine);
					}

					if(AnotherHelper & KMediaAttFormattable)
					{
						StringLoader::Load(Helper,R_SH_STR_ATRFRMATTABLE);
						InfoLine.Copy(Helper);
						Array->AppendL(InfoLine);
					}

					if(AnotherHelper & KMediaAttVariableSize)
					{
						StringLoader::Load(Helper,R_SH_STR_ATRVARSIZE);
						InfoLine.Copy(Helper);
						Array->AppendL(InfoLine);
					}

					AnotherHelper = volumeInfo.iDrive.iDriveAtt;

					if(AnotherHelper & KDriveAttRom)
					{
						StringLoader::Load(Helper,R_SH_STR_ATRROM);
						InfoLine.Copy(Helper);
						Array->AppendL(InfoLine);
					}

					if(AnotherHelper & KDriveAttRemovable)
					{
						StringLoader::Load(Helper,R_SH_STR_ATRREMABLE);
						InfoLine.Copy(Helper);
						Array->AppendL(InfoLine);
					}
			/*	*/
				}
			}
		}
		else if(KErrNone == CCoeEnv::Static()->FsSession().Entry(aFileName,MyEntry))
		{
			if(aFileType == CFFileItem::EFFolderFile)
			{			
				if(MyEntry.IsDir())
				{
					TInt32 HelperInt(0);
					
					TInt32 Ret = 0;
					
					if(iFFolderReader)
					{
						iFFolderReader->GetFolderFileCountL(aFileName,HelperInt,Ret,ETrue);
					}
					
					StringLoader::Load(Helper,R_SH_STR_FORMETFILES);
					InfoLine.Format(Helper,Ret);
					Array->AppendL(InfoLine);
					
					StringLoader::Load(Helper,R_SH_STR_FORMETFLDR);
					InfoLine.Format(Helper,HelperInt);
					Array->AppendL(InfoLine);
					
					TLocale MyLocale;
					TTime modTime = (MyEntry.iModified + MyLocale.UniversalTimeOffset());
					
					GetTimeBuffer(modTime.DateTime(),InfoLine,ETrue,EFalse);
					Array->AppendL(InfoLine);
					
					GetTimeBuffer(modTime.DateTime(),InfoLine,EFalse,EFalse);
					Array->AppendL(InfoLine);
					
					if(MyEntry.IsTypeValid())
					{
						TUint Num = MyEntry.MostDerivedUid().iUid;
						Helper.AppendNum(Num,EHex);
						
						
						StringLoader::Load(Helper,R_SH_STR_FORMETTYPEID);
						InfoLine.Format(Helper,&Helper);
						Array->AppendL(InfoLine);
					}

					if(MyEntry.IsHidden())
					{	
						StringLoader::Load(InfoLine,R_SH_STR_ATRHIDDEN);
						Array->AppendL(InfoLine);
					}

					if(MyEntry.IsReadOnly())
					{
						StringLoader::Load(InfoLine,R_SH_STR_ATRREADONLY);
						Array->AppendL(InfoLine);
					}
					 
					if(MyEntry.IsSystem())
					{
						StringLoader::Load(InfoLine,R_SH_STR_ATRSYSTEM);
						Array->AppendL(InfoLine);
					}

					if(MyEntry.iAtt & KEntryAttVolume)
					{
						StringLoader::Load(InfoLine,R_SH_STR_ATRVOLUME);
						Array->AppendL(InfoLine);
					}
				}
			}
			else // it is a file
			{
				TBuf<100> Hjelpper,Hjelpper2;
				
				TInt HelperInt = MyEntry.iSize;
			
				if(HelperInt < 1024)
				{
					StringLoader::Load(Helper,R_SH_STR_FORMETSIZEB);
					InfoLine.Format(Helper,HelperInt);
				}
				else
				{
					StringLoader::Load(Helper,R_SH_STR_FORMETSIZEKB);
					
					HelperInt = HelperInt /1024;
					InfoLine.Format(Helper,HelperInt);
				}

				Array->AppendL(InfoLine);
				
				Hjelpper.Zero();
				if(iIconHandler)
				{
					CYBRecognitionResult* Res = new(ELeave)CYBRecognitionResult(); 
			
					iIconHandler->RecognizeFile(*Res,aFileName);
					
					if(Res->iIdString)
					{
						if(Res->iIdString->Des().Length() > 100)
							Hjelpper.Copy(Res->iIdString->Des().Left(100));
						else
							Hjelpper.Copy(Res->iIdString->Des());
					}
					
					delete Res;
					Res = NULL;
				}
					
				TInt i =0;
							
				for(i = 0; i < Hjelpper.Length(); i++)
				{
					if(Hjelpper[i] == 47)
						break;
				}

				if(i < Hjelpper.Length())
				{
					TBuf<100> Hjelpper3;
					
					StringLoader::Load(Hjelpper3,R_SH_STR_FORMETTYPE);
					Hjelpper2.Copy(Hjelpper.Left(i));
					InfoLine.Format(Hjelpper3,&Hjelpper2);
					Array->AppendL(InfoLine);
					
					
					StringLoader::Load(Hjelpper3,R_SH_STR_FORMETTYPEID);
					Hjelpper2.Copy(Hjelpper.Right(Hjelpper.Length() - (i+1)));
					InfoLine.Format(Hjelpper3,&Hjelpper2);
					Array->AppendL(InfoLine);
				}
				else
				{
					StringLoader::Load(Hjelpper2,R_SH_STR_FORMETTYPE);
					InfoLine.Format(Hjelpper2,&Hjelpper);
					Array->AppendL(InfoLine);
				}
				
				TLocale MyLocale;
				TTime modTime = (MyEntry.iModified + MyLocale.UniversalTimeOffset());
					
				GetTimeBuffer(modTime.DateTime(),InfoLine,ETrue,EFalse);
				Array->AppendL(InfoLine);
				
				GetTimeBuffer(modTime.DateTime(),InfoLine,EFalse,EFalse);
				Array->AppendL(InfoLine);

				if(MyEntry.IsTypeValid())
				{
					TUint Num = MyEntry.MostDerivedUid().iUid;
					Helper.Num(Num,EHex);
					
					StringLoader::Load(Hjelpper2,R_SH_STR_FORMETID);
					InfoLine.Format(Hjelpper2,&Helper);
					
					InfoLine.TrimAll();
					if(InfoLine.Length())
					{
						Array->AppendL(InfoLine);
					}
				}

				if(MyEntry.IsArchive())
				{
					StringLoader::Load(InfoLine,R_SH_STR_ATRARCHIV);
					Array->AppendL(InfoLine);
				}

				if(MyEntry.IsHidden())
				{
					StringLoader::Load(InfoLine,R_SH_STR_ATRHIDDEN);
					Array->AppendL(InfoLine);
				}

				if(MyEntry.IsReadOnly())
				{
					StringLoader::Load(InfoLine,R_SH_STR_ATRREADONLY);
					Array->AppendL(InfoLine);
				}
				 
				if(MyEntry.IsSystem())
				{
					StringLoader::Load(InfoLine,R_SH_STR_ATRSYSTEM);
					Array->AppendL(InfoLine);
				}
			}
		}
	
		if(Array->MdcaCount())
		{			
			CAknSingleHeadingPopupMenuStyleListBox* list = new(ELeave) CAknSingleHeadingPopupMenuStyleListBox;
		    CleanupStack::PushL(list);

			CAknPopupList* popupList = CAknPopupList::NewL(list,R_AVKON_SOFTKEYS_BACK);
		    CleanupStack::PushL(popupList);
			
		    list->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
			list->CreateScrollBarFrameL(ETrue);
		    list->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);
		    list->Model()->SetItemTextArray(Array);
			list->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);
			
			TParsePtrC NameHjelp(aFileName);
			
			popupList->SetTitleL(NameHjelp.NameAndExt());
			popupList->ExecuteLD();
			
		    CleanupStack::Pop();            // popuplist
		    CleanupStack::PopAndDestroy();  // list
		}
	
		CleanupStack::PopAndDestroy(Array);
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CGeneralSettings::GetTimeBuffer(const TDateTime& aDateTime,TDes& aBuffer,TBool aDate,TBool aList)
{
	TBuf<2> HourHjlep,MinHjelp,SecHjelp;

	if(aDate)
	{
		if((aDateTime.Day() + 1) < 10)
		{
			HourHjlep.Num(0);
			HourHjlep.AppendNum((aDateTime.Day() + 1));
		}
		else
		{
			HourHjlep.Num((aDateTime.Day() + 1));
		}
		
		if((aDateTime.Month() + 1) < 10)
		{
			MinHjelp.Num(0);
			MinHjelp.AppendNum((aDateTime.Month() + 1));
		}
		else
		{
			MinHjelp.Num((aDateTime.Month() + 1));
		}


		if(aList)
		{
			aBuffer.Format(KtxFormatInfoDateList,&HourHjlep,&MinHjelp,aDateTime.Year());
		}
		else
		{
			TBuf<60> Hjelpper;
			Hjelpper.Format(KtxFormatInfoDateList,&HourHjlep,&MinHjelp,aDateTime.Year());		
			
			StringLoader::Load(aBuffer,R_SH_STR_DATE);
			aBuffer.Append(_L("\t"));
			aBuffer.Append(Hjelpper);
		}
	}
	else
	{
		if(aDateTime.Hour() < 10)
		{
			HourHjlep.Num(0);
			HourHjlep.AppendNum(aDateTime.Hour());
		}
		else
		{
			HourHjlep.Num(aDateTime.Hour());
		}
		
		if(aDateTime.Minute() < 10)
		{
			MinHjelp.Num(0);
			MinHjelp.AppendNum(aDateTime.Minute());
		}
		else
		{
			MinHjelp.Num(aDateTime.Minute());
		}
		
		if(aDateTime.Second() < 10)
		{
			SecHjelp.Num(0);
			SecHjelp.AppendNum(aDateTime.Second());
		}
		else
		{
			SecHjelp.Num(aDateTime.Second());
		}
	
		if(aList)
		{
			aBuffer.Format(KtxFormatInfoTimeList,&HourHjlep,&MinHjelp,&SecHjelp);
		}
		else
		{
			TBuf<60> Hjelpper;
			Hjelpper.Format(KtxFormatInfoTimeList,&HourHjlep,&MinHjelp,&SecHjelp);		
			
			StringLoader::Load(aBuffer,R_SH_STR_TIME);
			aBuffer.Append(_L("\t"));
			aBuffer.Append(Hjelpper);
		}
	}
}
	

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
TKeyResponse CGeneralSettings::SelectAndExecuteSCCommandL(TInt aKey)
{
	TKeyResponse Ret = EKeyWasNotConsumed;

	if(iCommandSCArray)
	{
		TInt CommandToExecute(-1);			;
	
		switch (aKey)
    	{
		  	case 48:
		  		if(iCommandSCArray->Count() > 0)
		  		{
		  			CommandToExecute  = iCommandSCArray->At(0);
		  		}
		  		break;
			case 49:
				if(iCommandSCArray->Count() > 1)
				{
		  			CommandToExecute  = iCommandSCArray->At(1);
		  		}
		  		break;
			case 50:
				if(iCommandSCArray->Count() > 2)
				{
		  			CommandToExecute  = iCommandSCArray->At(2);
		  		}
		  		break;
			case 51:
				if(iCommandSCArray->Count() > 3)
				{
		  			CommandToExecute  = iCommandSCArray->At(3);
		  		}
		  		break;
			case 52:
				if(iCommandSCArray->Count() > 4)	
				{
		  			CommandToExecute  = iCommandSCArray->At(4);
		  		}
		  		break;
			case 53:
				if(iCommandSCArray->Count() > 5)
				{
		  			CommandToExecute  = iCommandSCArray->At(5);
		  		}
		  		break;
			case 54:
				if(iCommandSCArray->Count() > 6)
				{
		  			CommandToExecute  = iCommandSCArray->At(6);
		  		}
		  		break;
			case 55:
				if(iCommandSCArray->Count() > 7)
				{
		  			CommandToExecute  = iCommandSCArray->At(7);
		  		}
		  		break;
			case 56:
				if(iCommandSCArray->Count() > 8)
				{
		  			CommandToExecute  = iCommandSCArray->At(8);
		  		}
		  		break;
			case 57:
				if(iCommandSCArray->Count() > 9)
				{
		  			CommandToExecute  = iCommandSCArray->At(9);
		  		}
		  		break;
			default:
				break;
    	}
    	
    	if(CommandToExecute > 0)
    	{
    		CEikonEnv::Static()->EikAppUi()-> HandleCommandL(CommandToExecute);
    		Ret = EKeyWasConsumed;
    	}
	}

	return Ret;
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CGeneralSettings::GetIconNameL(TDes& aFileName)
{
	aFileName.Zero();
	
	TFileName ShortFil;
	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone != AppFile.FindByDir(KtxIconFileName, KNullDesC))
	{
		if(KErrNone ==CCoeEnv::Static()->FsSession().PrivatePath(ShortFil))
		{
			TFindFile privFile(CCoeEnv::Static()->FsSession());
			if(KErrNone == privFile.FindByDir(ShortFil, KNullDesC))
			{
				TParsePtrC hjelp(privFile.File());
				ShortFil.Copy(hjelp.Drive());
				ShortFil.Append(KtxIconFileName);
			}
		}	
	}
	else
	{
		ShortFil.Copy(AppFile.File());
	}

	if(ShortFil.Length())
	{
		CDictionaryFileStore* MyDStore = CDictionaryFileStore::OpenLC(CCoeEnv::Static()->FsSession(),ShortFil, TUid::Uid(0x102013AD));
			
		TUid FileUid = {0x10};

		if(MyDStore->IsPresentL(FileUid))
		{
			RDictionaryReadStream in;
			in.OpenLC(*MyDStore,FileUid);
			in >> aFileName;
			CleanupStack::PopAndDestroy(1);// in
		}
			
		CleanupStack::PopAndDestroy(1);// Store
	}
	
	if(!BaflUtils::FileExists(CCoeEnv::Static()->FsSession(),aFileName) || aFileName.Length() < 5)
	{	
		TFindFile AppFile(CCoeEnv::Static()->FsSession());
		if(KErrNone == AppFile.FindByDir(KtxImagesFileName, KNullDesC))
		{
			aFileName.Copy(AppFile.File());
		}
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CGeneralSettings::SaveIconNameL(const TDesC& aFileName)
{
	TFileName ShortFil;
	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone != AppFile.FindByDir(KtxIconFileName, KNullDesC))
	{
		if(KErrNone ==CCoeEnv::Static()->FsSession().PrivatePath(ShortFil))
		{
			TFindFile privFile(CCoeEnv::Static()->FsSession());
			if(KErrNone == privFile.FindByDir(ShortFil, KNullDesC))
			{
				TParsePtrC hjelp(privFile.File());
				ShortFil.Copy(hjelp.Drive());
				ShortFil.Append(KtxIconFileName);
			}
		}
	}
	else
	{
		ShortFil.Copy(AppFile.File());
	}

	if(ShortFil.Length())
	{
		CCoeEnv::Static()->FsSession().Delete(ShortFil);
		CDictionaryFileStore* MyDStore = CDictionaryFileStore::OpenLC(CCoeEnv::Static()->FsSession(),ShortFil, TUid::Uid(0x102013AD));
			
		TUid FileUid = {0x10};	
				
		RDictionaryWriteStream out1;
		out1.AssignLC(*MyDStore,FileUid);
		
		out1 << aFileName; 
		out1.CommitL(); 	
		CleanupStack::PopAndDestroy(1);// out1
		
		MyDStore->CommitL();
		CleanupStack::PopAndDestroy(1);// Store
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CGeneralSettings::GetPathL(TDes& aFolder)
{
	TFileName ShortFil;
	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone != AppFile.FindByDir(KtxPathSaveFileName, KNullDesC))
	{
		if(KErrNone ==CCoeEnv::Static()->FsSession().PrivatePath(ShortFil))
		{
			TFindFile privFile(CCoeEnv::Static()->FsSession());
			if(KErrNone == privFile.FindByDir(ShortFil, KNullDesC))
			{
				TParsePtrC hjelp(privFile.File());
				ShortFil.Copy(hjelp.Drive());
				ShortFil.Append(KtxPathSaveFileName);
			}
		}	
	}
	else
	{
		ShortFil.Copy(AppFile.File());
	}

	if(ShortFil.Length())
	{
		CDictionaryFileStore* MyDStore = CDictionaryFileStore::OpenLC(CCoeEnv::Static()->FsSession(),ShortFil, TUid::Uid(0x102013AD));
			
		TUid FileUid = {0x10};

		if(MyDStore->IsPresentL(FileUid))
		{
			RDictionaryReadStream in;
			in.OpenLC(*MyDStore,FileUid);
			in >> aFolder;
			CleanupStack::PopAndDestroy(1);// in
		}
			
		CleanupStack::PopAndDestroy(1);// Store
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CGeneralSettings::SavePathL(const TDesC& aPath)
{
	TFileName ShortFil;
	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone != AppFile.FindByDir(KtxPathSaveFileName, KNullDesC))
	{
		if(KErrNone ==CCoeEnv::Static()->FsSession().PrivatePath(ShortFil))
		{
			TFindFile privFile(CCoeEnv::Static()->FsSession());
			if(KErrNone == privFile.FindByDir(ShortFil, KNullDesC))
			{
				TParsePtrC hjelp(privFile.File());
				ShortFil.Copy(hjelp.Drive());
				ShortFil.Append(KtxPathSaveFileName);
			}
		}
	}
	else
	{
		ShortFil.Copy(AppFile.File());
	}

	if(ShortFil.Length())
	{
		CCoeEnv::Static()->FsSession().Delete(ShortFil);
		CDictionaryFileStore* MyDStore = CDictionaryFileStore::OpenLC(CCoeEnv::Static()->FsSession(),ShortFil, TUid::Uid(0x102013AD));
			
		TUid FileUid = {0x10};	
				
		RDictionaryWriteStream out1;
		out1.AssignLC(*MyDStore,FileUid);
		out1 << aPath; 
		
		out1.CommitL(); 	
		CleanupStack::PopAndDestroy(1);// out1
		
		MyDStore->CommitL();
		CleanupStack::PopAndDestroy(1);// Store
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CGeneralSettings::SetSettingsValuesL(TSettingsItem& aSettings)
{
	TFileName ShortFil;
	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone != AppFile.FindByDir(KtxSettingsFileName, KNullDesC))
	{
		if(KErrNone ==CCoeEnv::Static()->FsSession().PrivatePath(ShortFil))
		{
			TFindFile privFile(CCoeEnv::Static()->FsSession());
			if(KErrNone == privFile.FindByDir(ShortFil, KNullDesC))
			{
				TParsePtrC hjelp(privFile.File());
				ShortFil.Copy(hjelp.Drive());
				ShortFil.Append(KtxSettingsFileName);
			}
		}		
	}
	else
	{
		ShortFil.Copy(AppFile.File());
	}

	if(ShortFil.Length())
	{
		CCoeEnv::Static()->FsSession().Delete(ShortFil);
		CDictionaryFileStore* MyDStore = CDictionaryFileStore::OpenLC(CCoeEnv::Static()->FsSession(),ShortFil, TUid::Uid(0x102013AD));
			
		TUid FileUid = {0x10};	
				
		RDictionaryWriteStream out1;
		out1.AssignLC(*MyDStore,FileUid);
		if(aSettings.iFolders)
			out1.WriteInt32L(0x100);
		else
			out1.WriteInt32L(0);
		out1.CommitL(); 	
		CleanupStack::PopAndDestroy(1);// out1
			
		//aSettings.iSort
		FileUid.iUid = 0x11;	
		RDictionaryWriteStream out2;
		out2.AssignLC(*MyDStore,FileUid);
		if(aSettings.iSort)
			out2.WriteInt32L(0x100);
		else
			out2.WriteInt32L(0);
		out2.CommitL(); 	
		CleanupStack::PopAndDestroy(1);// out2
		
		//aSettings.iShow
		FileUid.iUid = 0x12;
		RDictionaryWriteStream out3;
		out3.AssignLC(*MyDStore,FileUid);
		if(aSettings.iShow)
			out3.WriteInt32L(0x100);
		else
			out3.WriteInt32L(0);
		out3.CommitL(); 	
		CleanupStack::PopAndDestroy(1);// out3
		
		//aSettings.iOrder
		FileUid.iUid = 0x13;
		RDictionaryWriteStream out4;
		out4.AssignLC(*MyDStore,FileUid);
		out4.WriteInt32L(aSettings.iOrder);
		out4.CommitL(); 	
		CleanupStack::PopAndDestroy(1);// out4
		
		//aSettings.iOptimize
		FileUid.iUid = 0x14;
		RDictionaryWriteStream out63;
		out63.AssignLC(*MyDStore,FileUid);
		if(aSettings.iOptimize)
			out63.WriteInt32L(0x100);
		else
			out63.WriteInt32L(0);
		out63.CommitL(); 	
		CleanupStack::PopAndDestroy(1);// out63
		
		//aSettings.iSavePath
		FileUid.iUid = 0x15;
		RDictionaryWriteStream out35;
		out35.AssignLC(*MyDStore,FileUid);
		if(aSettings.iSavePath)
			out35.WriteInt32L(0x100);
		else
			out35.WriteInt32L(0);
		out35.CommitL(); 	
		CleanupStack::PopAndDestroy(1);// out35
		
		FileUid.iUid = 0x20;
		RDictionaryWriteStream out37;
		out37.AssignLC(*MyDStore,FileUid);
		if(aSettings.iDoRecocnize)
			out37.WriteInt32L(0x100);
		else
			out37.WriteInt32L(0);
		out37.CommitL(); 	
		CleanupStack::PopAndDestroy(1);// out37	
		
		FileUid.iUid = 0x22;
		RDictionaryWriteStream out39;
		out39.AssignLC(*MyDStore,FileUid);
		if(aSettings.iConfirmExit)
			out39.WriteInt32L(0x100);
		else
			out39.WriteInt32L(0);
		out39.CommitL(); 	
		CleanupStack::PopAndDestroy(1);// out39
		
		// iFullScreen
		FileUid.iUid = 0x30;
		RDictionaryWriteStream out40;
		out40.AssignLC(*MyDStore,FileUid);
		if(aSettings.iFullScreen)
			out40.WriteInt32L(0x100);
		else
			out40.WriteInt32L(0);
		out40.CommitL(); 	
		CleanupStack::PopAndDestroy(1);// out40
		
		
      // iPathScroll
        FileUid.iUid = 0x95;
        RDictionaryWriteStream out41;
        out41.AssignLC(*MyDStore,FileUid);
        if(aSettings.iPathScroll)
            out41.WriteInt32L(0x100);
        else
            out41.WriteInt32L(0);
        out41.CommitL();    
        CleanupStack::PopAndDestroy(1);// out10	

		
		MyDStore->CommitL();
		CleanupStack::PopAndDestroy(1);// Store
	}
}

/*
-----------------------------------------------------------------------------
	
-----------------------------------------------------------------------------
*/

void CGeneralSettings::GetCommandSCFromStoreL(void)
{	
	delete iCommandSCArray;
	iCommandSCArray = NULL;
	
	TFileName ShortFil;
	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone != AppFile.FindByDir(KtxCommandSCFileName, KNullDesC))
	{
		if(KErrNone ==CCoeEnv::Static()->FsSession().PrivatePath(ShortFil))
		{
			TFindFile privFile(CCoeEnv::Static()->FsSession());
			if(KErrNone == privFile.FindByDir(ShortFil, KNullDesC))
			{
				TParsePtrC hjelp(privFile.File());
				ShortFil.Copy(hjelp.Drive());
				ShortFil.Append(KtxCommandSCFileName);
			}
		}
	}
	else
	{
		ShortFil.Copy(AppFile.File());
	}
	
	if(ShortFil.Length())
	{
		CDictionaryFileStore* MyDStore = CDictionaryFileStore::OpenLC(CCoeEnv::Static()->FsSession(),ShortFil, TUid::Uid(0x102013AD));
		
		iCommandSCArray = new(ELeave)CArrayFixFlat<TInt>(10);	
	
		TInt CommandSc(0);
		GetValuesL(MyDStore,0x5000,CommandSc);
		iCommandSCArray->AppendL(CommandSc);
		
		CommandSc =0;
		GetValuesL(MyDStore,0x5001,CommandSc);
		iCommandSCArray->AppendL(CommandSc);
		
		CommandSc =0;
		GetValuesL(MyDStore,0x5002,CommandSc);
		iCommandSCArray->AppendL(CommandSc);
		
		CommandSc =0;
		GetValuesL(MyDStore,0x5003,CommandSc);
		iCommandSCArray->AppendL(CommandSc);
		
		CommandSc =0;
		GetValuesL(MyDStore,0x5004,CommandSc);
		iCommandSCArray->AppendL(CommandSc);

		CommandSc =0;
		GetValuesL(MyDStore,0x5005,CommandSc);
		iCommandSCArray->AppendL(CommandSc);
		
		CommandSc =0;
		GetValuesL(MyDStore,0x5006,CommandSc);
		iCommandSCArray->AppendL(CommandSc);
		
		CommandSc =0;
		GetValuesL(MyDStore,0x5007,CommandSc);
		iCommandSCArray->AppendL(CommandSc);
		
		CommandSc =0;
		GetValuesL(MyDStore,0x5008,CommandSc);
		iCommandSCArray->AppendL(CommandSc);
		
		CommandSc =0;
		GetValuesL(MyDStore,0x5009,CommandSc);
		iCommandSCArray->AppendL(CommandSc);
		
		iSwipeLeft = iSwipeRight = iLongPress;
		
		GetValuesL(MyDStore,0x500A,iSwipeLeft);
		if(iSwipeLeft < 0){
            iSwipeLeft = EBackOneLevel;
		}
		
		GetValuesL(MyDStore,0x500B,iSwipeRight);
        if(iSwipeRight < 0){
        iSwipeRight = EFolderViewMarkTogle;
        }
        
		GetValuesL(MyDStore,0x500C,iLongPress);
        if(iLongPress < 0){
            iLongPress = EChangeExtraInfo;
        }

		CleanupStack::PopAndDestroy(1);// Store
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CGeneralSettings::GetValuesL(CDictionaryFileStore* aDStore,TInt aUID,TInt& aValue)
{
	if(aDStore == NULL)
		return;
		
	TUid FileUid = {0x10};
	FileUid.iUid = aUID;

	if(aDStore->IsPresentL(FileUid))
	{
		RDictionaryReadStream in;
		in.OpenLC(*aDStore,FileUid);
		aValue = in.ReadInt32L();
		CleanupStack::PopAndDestroy(1);// in
	}
	else
		aValue = -1;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

void CGeneralSettings::GetSettingsValuesL(TSettingsItem& aSettings)
{
	TFileName ShortFil;
	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone != AppFile.FindByDir(KtxSettingsFileName, KNullDesC))
	{
		if(KErrNone ==CCoeEnv::Static()->FsSession().PrivatePath(ShortFil))
		{
			TFindFile privFile(CCoeEnv::Static()->FsSession());
			if(KErrNone == privFile.FindByDir(ShortFil, KNullDesC))
			{
				TParsePtrC hjelp(privFile.File());
				ShortFil.Copy(hjelp.Drive());
				ShortFil.Append(KtxSettingsFileName);
			}
		}
	}
	else
	{
		ShortFil.Copy(AppFile.File());
	}

	aSettings.iFolders 	= ETrue;
	aSettings.iSort 	= ETrue;
	aSettings.iShow 	= ETrue;
	aSettings.iOrder 	= 0; // file name;
	aSettings.iOptimize = ETrue;
	aSettings.iSavePath = EFalse;
	aSettings.iDoRecocnize= ETrue;
	aSettings.iConfirmExit= EFalse;	
	aSettings.iPathScroll = ETrue;
	
	if(ShortFil.Length())
	{
		CDictionaryFileStore* MyDStore = CDictionaryFileStore::OpenLC(CCoeEnv::Static()->FsSession(),ShortFil, TUid::Uid(0x102013AD));
			
		TUid FileUid = {0x10};

		if(MyDStore->IsPresentL(FileUid))
		{
			RDictionaryReadStream in;
			in.OpenLC(*MyDStore,FileUid);
			TInt MyValue = in.ReadInt32L();
			CleanupStack::PopAndDestroy(1);// in
		
			if(MyValue > 50)
			{
				aSettings.iFolders 	= ETrue;
			}
			else
			{
				aSettings.iFolders 	= EFalse;
			}
		}
		
		FileUid.iUid = 0x11;

		if(MyDStore->IsPresentL(FileUid))
		{
			RDictionaryReadStream in;
			in.OpenLC(*MyDStore,FileUid);
			TInt MyValue = in.ReadInt32L();
			CleanupStack::PopAndDestroy(1);// in
		
			if(MyValue > 50)
			{
				aSettings.iSort 	= ETrue;
			}
			else
			{
				aSettings.iSort 	= EFalse;
			}
		}
		
		FileUid.iUid = 0x12;

		if(MyDStore->IsPresentL(FileUid))
		{
			RDictionaryReadStream in;
			in.OpenLC(*MyDStore,FileUid);
			TInt MyValue = in.ReadInt32L();
			CleanupStack::PopAndDestroy(1);// in
		
			if(MyValue > 50)
			{
				aSettings.iShow 	= ETrue;
			}
			else
			{
				aSettings.iShow 	= EFalse;
			}
		}
		
		FileUid.iUid = 0x13;

		if(MyDStore->IsPresentL(FileUid))
		{
			RDictionaryReadStream in;
			in.OpenLC(*MyDStore,FileUid);
			TInt MyValue = in.ReadInt32L();
			CleanupStack::PopAndDestroy(1);// in
		
			if(MyValue >=0 && MyValue <= 4 )
			{
				aSettings.iOrder 	= MyValue;;
			}
			else
			{
				aSettings.iOrder 	= 2; // file type;
			}
		}
				
		FileUid.iUid = 0x14;

		if(MyDStore->IsPresentL(FileUid))
		{
			RDictionaryReadStream in;
			in.OpenLC(*MyDStore,FileUid);
			TInt MyValue = in.ReadInt32L();
			CleanupStack::PopAndDestroy(1);// in
		
			if(MyValue > 50)
			{
				aSettings.iOptimize 	= ETrue;
			}
			else
			{
				aSettings.iOptimize 	= EFalse;
			}
		}	
		
		FileUid.iUid = 0x15;

		if(MyDStore->IsPresentL(FileUid))
		{
			RDictionaryReadStream in;
			in.OpenLC(*MyDStore,FileUid);
			TInt MyValue = in.ReadInt32L();
			CleanupStack::PopAndDestroy(1);// in
		
			if(MyValue > 50)
			{
				aSettings.iSavePath 	= ETrue;
			}
			else
			{
				aSettings.iSavePath 	= EFalse;
			}
		}
		
		FileUid.iUid = 0x20;

		if(MyDStore->IsPresentL(FileUid))
		{
			RDictionaryReadStream in;
			in.OpenLC(*MyDStore,FileUid);
			TInt MyValue = in.ReadInt32L();
			CleanupStack::PopAndDestroy(1);// in
		
			if(MyValue > 50)
			{
				aSettings.iDoRecocnize 	= ETrue;
			}
			else
			{
				aSettings.iDoRecocnize 	= EFalse;
			}
		}
		
		FileUid.iUid = 0x22;

		if(MyDStore->IsPresentL(FileUid))
		{
			RDictionaryReadStream in;
			in.OpenLC(*MyDStore,FileUid);
			TInt MyValue = in.ReadInt32L();
			CleanupStack::PopAndDestroy(1);// in
		
			if(MyValue > 50)
			{
				aSettings.iConfirmExit 	= ETrue;
			}
			else
			{
				aSettings.iConfirmExit 	= EFalse;
			}
		}
		
		FileUid.iUid = 0x30;

		if(MyDStore->IsPresentL(FileUid))
		{
			RDictionaryReadStream in;
			in.OpenLC(*MyDStore,FileUid);
			TInt MyValue = in.ReadInt32L();
			CleanupStack::PopAndDestroy(1);// in
		
			if(MyValue > 50)
			{
				aSettings.iFullScreen 	= ETrue;
			}
			else
			{
				aSettings.iFullScreen 	= EFalse;
			}
		}
	
        FileUid.iUid = 0x95;

        if(MyDStore->IsPresentL(FileUid))
        {
            RDictionaryReadStream in;
            in.OpenLC(*MyDStore,FileUid);
            TInt MyValue = in.ReadInt32L();
            CleanupStack::PopAndDestroy(1);// in
        
            if(MyValue > 50)
            {
                aSettings.iPathScroll   = ETrue;
            }
            else
            {
                aSettings.iPathScroll   = EFalse;
            }
        }		
				
		CleanupStack::PopAndDestroy(1);// Store
	}

}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CGeneralSettings::GetCommandText(TDes& aTxtx,TInt aSC)
{
    switch(aSC)
    {
    case EEditCopy:
        StringLoader::Load(aTxtx,R_MNU_COPY);                   
        break;
    case EEditCut:              
        StringLoader::Load(aTxtx,R_MNU_CUT);    
        break;
    case EEditPaste:            
        StringLoader::Load(aTxtx,R_MNU_PASTE);      
        break;
    case EFileDelete:           
        StringLoader::Load(aTxtx,R_MNU_DELETE); 
        break;
    case EFileRename:           
        StringLoader::Load(aTxtx,R_MNU_RENAME); 
        break;
    case EFileDetails:          
        StringLoader::Load(aTxtx,R_MNU_PROPERTIES); 
        break;
    case EFileAttributes:       
        StringLoader::Load(aTxtx,R_MNU_ATTRIBUTES); 
        break;
    case EFileShowName:         
        StringLoader::Load(aTxtx,R_MNU_SHOWNAME);   
        break;
    case ESendFiles:            
        StringLoader::Load(aTxtx,R_MNU_SEND);
        break;
    case EFileNewFile:          
        StringLoader::Load(aTxtx,R_MNU_NEWFILE);    
        break;
    case EFileNewFolder:        
        StringLoader::Load(aTxtx,R_MNU_NEWFOLDER);
        break;
    case ESettings:             
        StringLoader::Load(aTxtx,R_MNU_SETTINGS);   
        break;
    case EExtrasShowShortCut:   
        StringLoader::Load(aTxtx,R_MNU_SHOWSC); 
        break;
    case EExtrasSetShortCut:    
        StringLoader::Load(aTxtx,R_MNU_SETSC);  
        break;
    case EExtrasSearchOn:       
        StringLoader::Load(aTxtx,R_MNU_SETSEARCHON);
        break;
    case EExtrasRefresh:        
        StringLoader::Load(aTxtx,R_MNU_REFRESH);        
        break;      
    case EFolderViewMarkTogle:    
        StringLoader::Load(aTxtx,R_MNU_MARKTOGLE);
        break;        
    case EFolderViewMarkAll:    
        StringLoader::Load(aTxtx,R_MNU_MARKALL);
        break;
    case EFolderViewUnMarkAll:  
        StringLoader::Load(aTxtx,R_MNU_UNMARKALL);  
        break;
    case EVBOpenFile:
        StringLoader::Load(aTxtx,R_MNU_OPEN);
        break;
    case EOpenWithViewer:
        StringLoader::Load(aTxtx,R_MNU_OPENWITH);
        break;
    case EEditShowPaste:        
        StringLoader::Load(aTxtx,R_MNU_PASTESHOW);      
        break;
    case EEditClearPaste:
        StringLoader::Load(aTxtx,R_MNU_CLEARPASTE);
        break;
    case ESearchFiles:
        StringLoader::Load(aTxtx,R_MNU_SEARCHFLES);
        break;
    case EMovePageUp:
        StringLoader::Load(aTxtx,R_MNU_PAGEUP);
        break;
    case EMovePageDown:
        StringLoader::Load(aTxtx,R_MNU_PAGEDOWN);
        break; 
    case EBackOneLevel:
        StringLoader::Load(aTxtx,R_CMD_BACK);
        break;    
    case EChangeExtraInfo:
        StringLoader::Load(aTxtx,R_STR_EXTRASINFO);
        break;            
    default:
        break;
    }
}




