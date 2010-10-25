/*
 * Copyright © 2008 Nokia Corporation.
 */

#include "SettingExampleSettingsData.h"
#include <S32STRM.H> // Streams

// granularity for growing CheckBox list item array
// within CSettingsData object only.
const int KCheckBoxDataListGranularity = 3;


CSettingsData *CSettingsData::NewL()
	{
	CSettingsData *self = CSettingsData::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CSettingsData *CSettingsData::NewLC()
	{
	CSettingsData *self = new (ELeave) CSettingsData();
	CleanupStack::PushL(self);

	self->ConstructL();

	return self;
	}

CSettingsData::~CSettingsData()
	{
	// clear all elements out of the array
	iCheckboxArray.Reset();
	}

CSettingsData::CSettingsData() :
	iVolume(3),
	iCheckboxArray(KCheckBoxDataListGranularity)
	{
	// initialise local data
	iTime.HomeTime();
	iDate.HomeTime();
	iIpAddress = TInetAddr( INET_ADDR( 69, 21, 114, 135 ), 0 );
	iMode = 0;
	iBrand= 0;
	}


void CSettingsData::ConstructL() 
    {
	iCheckboxArray.AppendL(0);
	iCheckboxArray.AppendL(1);
	iCheckboxArray.AppendL(0);
    }

void CSettingsData::LoadL(RReadStream& aStream)
    {
    aStream >> iUsrName;
    aStream >> iSecretText;
    aStream >> iNumericText;
    iMode = aStream.ReadInt32L();
    iBrand= aStream.ReadInt32L();
    iVolume = aStream.ReadInt32L();
    iSlider = aStream.ReadInt32L();
    iBinary = aStream.ReadInt16L();
    iIpAddress.SetAddress(aStream.ReadUint32L());
    TInt64 time;
    aStream >> time;
    iDate = TTime(time);
    aStream >> time;
    iTime = TTime(time);
    
    TInt checkboxArrayCount = aStream.ReadInt32L();
    for (TInt i = 0; i < checkboxArrayCount; i++)
        {
        iCheckboxArray[i] = aStream.ReadInt32L();
        }
    }
    
void CSettingsData::SaveL(RWriteStream& aStream) const
    {
    aStream << iUsrName;
    aStream << iSecretText;
    aStream << iNumericText;
    aStream.WriteInt32L(iMode);
    aStream.WriteInt32L(iBrand);
    aStream.WriteInt32L(iVolume);
    aStream.WriteInt32L(iSlider);
    aStream.WriteInt16L(iBinary);
    aStream.WriteUint32L(iIpAddress.Address());
    aStream << iDate.Int64();
    aStream << iTime.Int64();
    
    TInt checkboxArrayCount = iCheckboxArray.Count();
    aStream.WriteInt32L(checkboxArrayCount);
    for (TInt i = 0; i < checkboxArrayCount; i++)
        {
        aStream.WriteInt32L(iCheckboxArray.At(i));
        }
    }
//EOF
