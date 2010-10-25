/*
 * Copyright © 2008 Nokia Corporation.
 */

#include "SettingExampleCheckboxSetting.h"
#include <avkon.rsg>
#include <DemoVideoCall_0xE5EC06EB.RSG>

// granularity for growing CheckBox list item array
// within CSettingAppCheckBoxSettingItem object only.
// This value may be different to that used in the 
// CSettingsData object.
const TInt KCheckBoxItemListGranularity = 3;

// size allocated to internal representation of checkbox states
const TInt KInternalRepSize = 128;	

CSettingAppCheckBoxSettingItem::CSettingAppCheckBoxSettingItem(TInt aIdentifier, 
												CArrayFix<TInt>& aValueList )
	: CAknEnumeratedTextSettingItem(aIdentifier), iExternalValueArray(aValueList)
	{
	// no specific construction code required
	}

CSettingAppCheckBoxSettingItem::~CSettingAppCheckBoxSettingItem()
	{
	if ( iSelectionItemListData	) // if list data has been created
		{
		// delete all items in the array
		iSelectionItemListData->ResetAndDestroy();
		// delete the array itself
		delete iSelectionItemListData;
		}

	// delete the text representation descriptor
	delete iTextRepresentation;
	}

void CSettingAppCheckBoxSettingItem::CompleteConstructionL()
	{
	// finish constructing base class
	CAknEnumeratedTextSettingItem::CompleteConstructionL();

	// construct the item list array
	iSelectionItemListData = new (ELeave) CSelectionItemList( 
											KCheckBoxItemListGranularity );

	// the number of variables we have names for - 
	// EnumeratedTextArray is from CAknEnumeratedTextSettingItem
	TInt count = EnumeratedTextArray()->Count();			

	// the number of variables in the external array (from data object)
	TInt numExternals = iExternalValueArray.Count();		


	for (TInt index = 0; index < count; index++ )
		{
		// create a new CSelectableItem based on the string from the enumerated
		// text array
		CSelectableItem* selectionItem = new(ELeave)CSelectableItem( 
								*(EnumeratedTextArray()->At(index)->Value()),
								EFalse );

		CleanupStack::PushL(selectionItem);
		// second-stage construction
		selectionItem->ConstructL();

		// if a corresponding external data item exists
		if (index < numExternals)
			{
			// set the selection status according to the external data value
			selectionItem->SetSelectionStatus(iExternalValueArray[index]);
			}

		// add the item to the SelectionItemListData array
		// selectionItem is now "owned" by CheckboxSettingItem and must be 
		// ResetAndDestroyed at destruction
		iSelectionItemListData->AppendL(selectionItem); 

		CleanupStack::Pop(selectionItem);
		}
	}

void CSettingAppCheckBoxSettingItem::LoadL()
	{

	// number of items in external data array
	TInt numExternals = iExternalValueArray.Count();

	// copy all items from the external data representation into the internal
	for (TInt i=0;i<numExternals;i++)
		{
		(*iSelectionItemListData)[i]->SetSelectionStatus(iExternalValueArray[i]);
		}

	// make sure the text reflects what's actually stored
	SetTextRepresentationL();
	}

void CSettingAppCheckBoxSettingItem::StoreL()
	{
	// number of items in external data array
	TInt numExternals = iExternalValueArray.Count();

	// copy all items from the internal data representation to the external
	for (TInt i=0;i<numExternals;i++)
		{
		iExternalValueArray[i] = 
					(*iSelectionItemListData)[i]->SelectionStatus();
		}
	}

void CSettingAppCheckBoxSettingItem::EditItemL( TBool  /*aCalledFromMenu */)
	{
	
	// construct the settings page based on R_CHECKBOX_PAGE resource
	CAknCheckBoxSettingPage* dlg = 
			new ( ELeave )CAknCheckBoxSettingPage(R_CHECKBOX_PAGE, iSelectionItemListData);

	// execute the dialog - will display the settings page
	if ( dlg->ExecuteLD(CAknSettingPage::EUpdateWhenChanged) )
		{ 
		// something changed - so update internal representation 
		// of display text
		SetTextRepresentationL();
		// and update the text being displayed
		UpdateListBoxTextL();
		}
	}

const TDesC& CSettingAppCheckBoxSettingItem::SettingTextL()
	{

	// if external data is not available or the text representation has not
	// been created
	if ( (iExternalValueArray.Count() == 0) || !iTextRepresentation ) 
		{
		return EmptyItemText();	// no data
		}
	else
		{
		return *(iTextRepresentation); // internal representation
		}
	}


void CSettingAppCheckBoxSettingItem::SetTextRepresentationL()
	{

	// if internal representation has not been created
	if (!iTextRepresentation)
		{
		iTextRepresentation = HBufC::NewL(KInternalRepSize); // create it
		}

	// get modifiable pointer to descriptor
	TPtr text = iTextRepresentation->Des();

	// clear descriptor
	text.Zero();
	
	TBool addComma=EFalse;
	_LIT(KComma,", ");

	// number of items in selection list
	TInt nItems = iSelectionItemListData->Count();
	
	for (TInt i=0; i<nItems; i++)		// run through each item
		{
		// if item is selected
		if ( (*iSelectionItemListData)[i]->SelectionStatus() )	
			{
			// add a comma if required
			if (addComma)text.Append(KComma);

			// add item's select string to display string
			text.Append( *(EnumeratedTextArray()->At(i)->Value()));	
		
			// any item other than the first should be preceded by a comma
			addComma=ETrue;
			}
		}
	}
//
// End of File
// 
//
