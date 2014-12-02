/* Copyright (c) 2001 - 2008 , Dr Jukka Silvennoinen. All rights reserved */

#include "TrialHanler.h"                     // own definitions

#include <BAUTILS.H>
#include <S32FILE.H>

#include "Common.h"

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 

void CTrialHandler::SetDateNowL()
{
    RFs fsSession;
    User::LeaveIfError(fsSession.Connect());
    
    TFindFile AufFolder5(fsSession);
    if(KErrNone == AufFolder5.FindByDir(KtxTrialHandFileFileName, KNullDesC))
    {
        TTime homme;
        homme.HomeTime();
        
        CDictionaryFileStore* MyDStore = CDictionaryFileStore::OpenLC(fsSession,AufFolder5.File(), TUid::Uid(0x102013AD));
                    
        TUid FileUid = {0x10};
        FileUid.iUid = 0x9999;
        if(MyDStore->IsPresentL(FileUid))
        {
            MyDStore->Remove(FileUid);
            MyDStore->CommitL();
        }
        
        RDictionaryWriteStream out1;
        out1.AssignLC(*MyDStore,FileUid);
        out1.WriteInt32L(homme.DateTime().Year());
        out1.CommitL();     
        CleanupStack::PopAndDestroy(1);// out1       

        FileUid.iUid = 0x1688;
        if(MyDStore->IsPresentL(FileUid))
        {
            MyDStore->Remove(FileUid);
            MyDStore->CommitL();
        } 
        
        RDictionaryWriteStream out2;
        out2.AssignLC(*MyDStore,FileUid);
        out2.WriteInt32L(homme.DateTime().Month());
        out2.CommitL();     
        CleanupStack::PopAndDestroy(1);// out2          
                        
        FileUid.iUid = 0x4889;
        if(MyDStore->IsPresentL(FileUid))
        {
            MyDStore->Remove(FileUid);
            MyDStore->CommitL();
        }
        
        RDictionaryWriteStream out3;
        out3.AssignLC(*MyDStore,FileUid);
        out3.WriteInt32L(homme.DateTime().Day());
        out3.CommitL();     
        CleanupStack::PopAndDestroy(1);// out3          

        FileUid.iUid = 0x4821;    
        if(MyDStore->IsPresentL(FileUid))
        {
            MyDStore->Remove(FileUid);
            MyDStore->CommitL();
        }
        
        RDictionaryWriteStream out31;
        out31.AssignLC(*MyDStore,FileUid);
        out31.WriteInt32L(homme.DateTime().Hour());
        out31.CommitL();     
        CleanupStack::PopAndDestroy(1);// out31 
        
        FileUid.iUid = 0x4822;        
        if(MyDStore->IsPresentL(FileUid))
        {
            MyDStore->Remove(FileUid);
            MyDStore->CommitL();
        }
        
        RDictionaryWriteStream out32;
        out32.AssignLC(*MyDStore,FileUid);
        out32.WriteInt32L(homme.DateTime().Minute());
        out32.CommitL();     
        CleanupStack::PopAndDestroy(1);// out32        
        
        MyDStore->CommitL();
        CleanupStack::PopAndDestroy(1);// Store
    }   
    
    fsSession.Close();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 

TBool CTrialHandler::IsNowOkL(TBool& aFirstTime,TInt& aDaysLeft)
{
    TBool ret(EFalse);
    aFirstTime = EFalse;
    
    RFs fsSession;
    User::LeaveIfError(fsSession.Connect());
    
    TFindFile AufFolder2(fsSession);
    if(KErrNone == AufFolder2.FindByDir(KtxTrialHandFileFileName, KNullDesC)){
        TFindFile AufFolder3(fsSession);
        if(KErrNone == AufFolder3.FindByDir(KtxTrialHandFileFileName2, KNullDesC))
        {        
            BaflUtils::DeleteFile(fsSession,AufFolder3.File());
        }    
    }else{
        TFindFile AufFolder4(fsSession);
        if(KErrNone == AufFolder4.FindByDir(KtxTrialHandFileFileName2, KNullDesC))
        {
            aFirstTime = ETrue;        
            TParsePtrC parsse(AufFolder4.File());
            TFileName hjelpper;
            hjelpper.Copy(parsse.Drive());
            hjelpper.Append(KtxTrialHandFileFileName);
            BaflUtils::RenameFile(fsSession,AufFolder4.File(),hjelpper);
        }
    }
    
    TFindFile AufFolder5(fsSession);
    if(KErrNone == AufFolder5.FindByDir(KtxTrialHandFileFileName, KNullDesC)){
        TTime homme;
        homme.HomeTime();
    
        CDictionaryFileStore* MyDStore = CDictionaryFileStore::OpenLC(fsSession,AufFolder5.File(), TUid::Uid(0x102013AD));
        
        TInt sYear(-1),sMonth(-1),sDay(-1),sHour(-1),sMinute(-1);
        TInt mYear(-1),mMonth(-1),mDay(-1),mHour(-1),mMinute(-1);
        TInt eYear(-1),eMonth(-1),eDay(-1),eHour(-1),eMinute(-1);
        
        TUid FileUid = {0x8867675};
        
        if(MyDStore->IsPresentL(FileUid))
        {
            FileUid.iUid = 0x1299;
            if(MyDStore->IsPresentL(FileUid))
            {
                RDictionaryReadStream in;
                in.OpenLC(*MyDStore,FileUid);
                sYear = in.ReadInt32L();
                CleanupStack::PopAndDestroy(1);// in
            }
            
            FileUid.iUid = 0x1669;
            if(MyDStore->IsPresentL(FileUid))
            {
                RDictionaryReadStream in;
                in.OpenLC(*MyDStore,FileUid);
                sMonth = in.ReadInt32L();
                CleanupStack::PopAndDestroy(1);// in 
            }  
            
            FileUid.iUid = 0x4469;
            if(MyDStore->IsPresentL(FileUid))
            {
                RDictionaryReadStream in;
                in.OpenLC(*MyDStore,FileUid);
                sDay = in.ReadInt32L();
                CleanupStack::PopAndDestroy(1);// in 
            } 
            
            FileUid.iUid = 0x4421;
            if(MyDStore->IsPresentL(FileUid))
            {
                RDictionaryReadStream in;
                in.OpenLC(*MyDStore,FileUid);
                sHour = in.ReadInt32L();
                CleanupStack::PopAndDestroy(1);// in 
            } 
            
            FileUid.iUid = 0x4422;
            if(MyDStore->IsPresentL(FileUid))
            {
                RDictionaryReadStream in;
                in.OpenLC(*MyDStore,FileUid);
                sMinute = in.ReadInt32L();
                CleanupStack::PopAndDestroy(1);// in 
            }             
            
            FileUid.iUid = 0x9999;
            if(MyDStore->IsPresentL(FileUid))
            {
                RDictionaryReadStream in;
                in.OpenLC(*MyDStore,FileUid);
                mYear = in.ReadInt32L();
                CleanupStack::PopAndDestroy(1);// in
            }
            
            FileUid.iUid = 0x1688;
            if(MyDStore->IsPresentL(FileUid))
            {
                RDictionaryReadStream in;
                in.OpenLC(*MyDStore,FileUid);
                mMonth = in.ReadInt32L();
                CleanupStack::PopAndDestroy(1);// in 
            }  
            
            FileUid.iUid = 0x4889;
            if(MyDStore->IsPresentL(FileUid))
            {
                RDictionaryReadStream in;
                in.OpenLC(*MyDStore,FileUid);
                mDay = in.ReadInt32L();
                CleanupStack::PopAndDestroy(1);// in 
            } 
            
            FileUid.iUid = 0x4821;
            if(MyDStore->IsPresentL(FileUid))
            {
                RDictionaryReadStream in;
                in.OpenLC(*MyDStore,FileUid);
                mHour = in.ReadInt32L();
                CleanupStack::PopAndDestroy(1);// in 
            } 
            
            FileUid.iUid = 0x4822;
            if(MyDStore->IsPresentL(FileUid))
            {
                RDictionaryReadStream in;
                in.OpenLC(*MyDStore,FileUid);
                mMinute = in.ReadInt32L();
                CleanupStack::PopAndDestroy(1);// in 
            }             
                 
            FileUid.iUid = 0x6789;
            if(MyDStore->IsPresentL(FileUid))
            {
                RDictionaryReadStream in;
                in.OpenLC(*MyDStore,FileUid);
                eYear = in.ReadInt32L();
                CleanupStack::PopAndDestroy(1);// in
            }
            
            FileUid.iUid = 0x1111;
            if(MyDStore->IsPresentL(FileUid))
            {
                RDictionaryReadStream in;
                in.OpenLC(*MyDStore,FileUid);
                eMonth = in.ReadInt32L();
                CleanupStack::PopAndDestroy(1);// in 
            }  
            
            FileUid.iUid = 0x9786;
            if(MyDStore->IsPresentL(FileUid))
            {
                RDictionaryReadStream in;
                in.OpenLC(*MyDStore,FileUid);
                eDay = in.ReadInt32L();
                CleanupStack::PopAndDestroy(1);// in 
            }  
            
            FileUid.iUid = 0x9721;
            if(MyDStore->IsPresentL(FileUid))
            {
                RDictionaryReadStream in;
                in.OpenLC(*MyDStore,FileUid);
                eHour = in.ReadInt32L();
                CleanupStack::PopAndDestroy(1);// in 
            } 
            
            FileUid.iUid = 0x9722;
            if(MyDStore->IsPresentL(FileUid))
            {
                RDictionaryReadStream in;
                in.OpenLC(*MyDStore,FileUid);
                eMinute = in.ReadInt32L();
                CleanupStack::PopAndDestroy(1);// in 
            }              
            
            if(sYear == -1 && sMonth == -1 && sDay == -1
            && mYear == -1 && mMonth == -1 && mDay == -1
            && eYear == -1 && eMonth == -1 && eDay == -1){
            
                FileUid.iUid = 0x1299;
                if(MyDStore->IsPresentL(FileUid))
                {
                    MyDStore->Remove(FileUid);
                    MyDStore->CommitL();
                }
                    
                RDictionaryWriteStream out1;
                out1.AssignLC(*MyDStore,FileUid);
                out1.WriteInt32L(homme.DateTime().Year());
                out1.CommitL();     
                CleanupStack::PopAndDestroy(1);// out1       

                FileUid.iUid = 0x1669;
                if(MyDStore->IsPresentL(FileUid))
                {
                    MyDStore->Remove(FileUid);
                    MyDStore->CommitL();
                } 
                    
                RDictionaryWriteStream out2;
                out2.AssignLC(*MyDStore,FileUid);
                out2.WriteInt32L((TInt)homme.DateTime().Month());
                out2.CommitL();     
                CleanupStack::PopAndDestroy(1);// out2          
                                    
                FileUid.iUid = 0x4469;
                if(MyDStore->IsPresentL(FileUid))
                {
                    MyDStore->Remove(FileUid);
                    MyDStore->CommitL();
                }
                    
                RDictionaryWriteStream out3;
                out3.AssignLC(*MyDStore,FileUid);
                out3.WriteInt32L(homme.DateTime().Day());
                out3.CommitL();     
                CleanupStack::PopAndDestroy(1);// out3     
                
                FileUid.iUid = 0x4421;
                if(MyDStore->IsPresentL(FileUid))
                {
                    MyDStore->Remove(FileUid);
                    MyDStore->CommitL();
                }
                    
                RDictionaryWriteStream out31;
                out31.AssignLC(*MyDStore,FileUid);
                out31.WriteInt32L(homme.DateTime().Hour());
                out31.CommitL();     
                CleanupStack::PopAndDestroy(1);// out31                 
                
                FileUid.iUid = 0x4422;                
                if(MyDStore->IsPresentL(FileUid))
                {
                    MyDStore->Remove(FileUid);
                    MyDStore->CommitL();
                }
                    
                RDictionaryWriteStream out32;
                out32.AssignLC(*MyDStore,FileUid);
                out32.WriteInt32L(homme.DateTime().Minute());
                out32.CommitL();     
                CleanupStack::PopAndDestroy(1);// out32                 
                
                FileUid.iUid = 0x6789;
                if(MyDStore->IsPresentL(FileUid))
                {
                    MyDStore->Remove(FileUid);
                    MyDStore->CommitL();
                }
                
                homme = homme + TTimeIntervalDays(2);
                                    
                RDictionaryWriteStream out4;
                out4.AssignLC(*MyDStore,FileUid);
                out4.WriteInt32L(homme.DateTime().Year());
                out4.CommitL();     
                CleanupStack::PopAndDestroy(1);// out4       

                FileUid.iUid = 0x1111;
                if(MyDStore->IsPresentL(FileUid))
                {
                    MyDStore->Remove(FileUid);
                    MyDStore->CommitL();
                } 
                                    
                RDictionaryWriteStream out5;
                out5.AssignLC(*MyDStore,FileUid);
                out5.WriteInt32L((TInt)homme.DateTime().Month());
                out5.CommitL();     
                CleanupStack::PopAndDestroy(1);// out5          
                                                    
                FileUid.iUid = 0x9786;
                if(MyDStore->IsPresentL(FileUid))
                {
                    MyDStore->Remove(FileUid);
                    MyDStore->CommitL();
                }
                                    
                RDictionaryWriteStream out6;
                out6.AssignLC(*MyDStore,FileUid);
                out6.WriteInt32L(homme.DateTime().Day());
                out6.CommitL();     
                CleanupStack::PopAndDestroy(1);// out6  

                FileUid.iUid = 0x9721;  
                if(MyDStore->IsPresentL(FileUid))
                {
                    MyDStore->Remove(FileUid);
                    MyDStore->CommitL();
                }
                                    
                RDictionaryWriteStream out61;
                out61.AssignLC(*MyDStore,FileUid);
                out61.WriteInt32L(homme.DateTime().Hour());
                out61.CommitL();     
                CleanupStack::PopAndDestroy(1);// out61
                
                FileUid.iUid = 0x9722;                
                if(MyDStore->IsPresentL(FileUid))
                {
                    MyDStore->Remove(FileUid);
                    MyDStore->CommitL();
                }
                                    
                RDictionaryWriteStream out62;
                out62.AssignLC(*MyDStore,FileUid);
                out62.WriteInt32L(homme.DateTime().Minute());
                out62.CommitL();     
                CleanupStack::PopAndDestroy(1);// out62                 
                
                
                MyDStore->CommitL();
                
                TTime nowwTimm;
                nowwTimm.HomeTime();
                TTimeIntervalHours leftTimm;
                homme.HoursFrom(nowwTimm,leftTimm);                  
                aDaysLeft = leftTimm.Int();
                                    
                ret = ETrue;
            }else{

                TTime startTime(TDateTime(sYear,(TMonth)sMonth,sDay,sHour,sMinute,0,0));
                TTime middleTime(TDateTime(mYear,(TMonth)mMonth,mDay,mHour,mMinute,0,0));
                TTime edningTime(TDateTime(eYear,(TMonth)eMonth,eDay,eHour,eMinute,0,0));
                
                TTimeIntervalMinutes sInterval(0);
                TTimeIntervalMinutes mInterval(0);
                TTimeIntervalMinutes eInterval(0);
                homme.MinutesFrom(startTime,sInterval);
                homme.MinutesFrom(middleTime,mInterval);
                edningTime.MinutesFrom(homme,eInterval);
                
                if(sInterval.Int() >= 0
                && mInterval.Int() >= 0
                && eInterval.Int() >= 0){
                
                    TTimeIntervalHours leftTimm;
                    edningTime.HoursFrom(homme,leftTimm);                  
                    aDaysLeft = leftTimm.Int();
                    ret = ETrue;
                }
            }        
        }else{
        /*    RDictionaryWriteStream out3;
            out3.AssignLC(*MyDStore,FileUid);
            out3.WriteInt32L(12985674);
            out3.CommitL();     
            CleanupStack::PopAndDestroy(1);// out3  
            
            MyDStore->CommitL();*/
        }
        
        CleanupStack::PopAndDestroy(1);// Store
    }
    
    fsSession.Close();
    return ret;
}

