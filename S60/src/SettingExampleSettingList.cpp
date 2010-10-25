/*
 * Copyright © 2008 Nokia Corporation.
 */

#include <barsread.h>
#include <DemoVideoCall_0xE5EC06EB.RSG>
#include "DemoVideoCall.hrh"
#include "SettingExampleSettingList.h"
#include "SettingExampleSettingsData.h"

CExampleSettingList *CExampleSettingList::NewL(CSettingsData &aData)
	{
	CExampleSettingList* self = CExampleSettingList::NewLC(aData);		
	CleanupStack::Pop(self);
	return self;
	}

CExampleSettingList *CExampleSettingList::NewLC(CSettingsData &aData)
	{
	CExampleSettingList* self = new (ELeave) CExampleSettingList(aData);
	CleanupStack::PushL(self);
	return self;
	}

CExampleSettingList::CExampleSettingList(CSettingsData &aData) : 
	CAknSettingItemList(),
	iSettingsData(aData)	
	{
	}

CExampleSettingList::~CExampleSettingList()
	{
	// no specific destruction code required - no owned data
	}


void CExampleSettingList::SizeChanged()
    {
	// if size changes, make sure component takes whole available space
	CEikFormattedCellListBox *listbox = ListBox();

    if (listbox) 
        {
        listbox->SetRect(Rect());
        }
    }

void CExampleSettingList::EditCurrentItemL(TBool aFromMenu)
	{
	// Invoke EditItemL on the current item
    EditItemL(ListBox()->CurrentItemIndex(),	// the item index
                aFromMenu);							// invoked from menu
        
			  
	//Updating CAknPasswordSettingItem.
	if(ListBox()->CurrentItemIndex()==5)
		{
		(*(SettingItemArray()))[ListBox()->CurrentItemIndex()]->UpdateListBoxTextL();
		}
        
    StoreSettingsL();
	}
void CExampleSettingList::EditItemL ( TInt aIndex, TBool aCalledFromMenu )
	{
		CAknSettingItemList::EditItemL( aIndex, aCalledFromMenu );
		StoreSettingsL();
	}
CAknSettingItem * CExampleSettingList::CreateSettingItemL (TInt aIdentifier) 
	{
	// method is used to create specific setting item as required at run-time.
	// aIdentifier is used to determine what kind of setting item should be 
	// created
	CAknSettingItem* settingItem = NULL;

    switch (aIdentifier)
		{
		case ESettingText:
			settingItem = new (ELeave) CAknTextSettingItem (
											aIdentifier, 
							 				iSettingsData.UserName());
			break;
		case ESettingRadioMode:
			settingItem = new (ELeave) CAknEnumeratedTextPopupSettingItem (
											aIdentifier, 
											iSettingsData.iMode);
			break;
		case ESettingRadioBrand:
			settingItem = new (ELeave) CAknEnumeratedTextPopupSettingItem (
											aIdentifier, 
											iSettingsData.iBrand);
					break;			
			/*case ESettingCheckbox:
			settingItem = new (ELeave) CSettingAppCheckBoxSettingItem (
											aIdentifier, 
											(iSettingsData.CheckboxArray()));
			break;
		case ESettingVolume:
			settingItem = new (ELeave) CAknVolumeSettingItem (
											aIdentifier, 
											iSettingsData.Volume());
			break;
		case ESettingSlider:
			settingItem = new (ELeave) CAknSliderSettingItem (
											aIdentifier, 
											iSettingsData.Slider());
			break;
		case ESettingBinary:
			settingItem = new (ELeave) CAknBinaryPopupSettingItem (
											aIdentifier, 
											iSettingsData.Binary());
			break;
		case ESettingSecret:
			settingItem = new (ELeave) CAknPasswordSettingItem (
											aIdentifier, 
											CAknPasswordSettingItem::EAlpha,
											iSettingsData.SecretText());
			break;
		case ESettingNumeric:
			settingItem = new (ELeave) CAknTextSettingItem (
											aIdentifier, 
											iSettingsData.NumericText());
			break;
		case ESettingDate:
			settingItem = new (ELeave) CAknTimeOrDateSettingItem (
											aIdentifier, 
											CAknTimeOrDateSettingItem::EDate, 
											iSettingsData.Date());
			break;
		case ESettingTime:
			settingItem = new (ELeave) CAknTimeOrDateSettingItem (
											aIdentifier, 
											CAknTimeOrDateSettingItem::ETime, 
											iSettingsData.Time());
			break;*/
		case ESettingIp:
			settingItem = new (ELeave) CAknIpFieldSettingItem (
											aIdentifier, 
											iSettingsData.IpAddress());
			break;
		default:
			break;
        }
        
    return settingItem;
	}

