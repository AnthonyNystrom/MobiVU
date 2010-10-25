/*
 * Copyright © 2008 Nokia Corporation.
 */

#ifndef __CHECKBOX_SETTING_H_
#define __CHECKBOX_SETTING_H_

#include <aknsettingitemlist.h>
#include <aknCheckBoxSettingPage.h>

/**
 * Class:		CSettingAppCheckBoxSettingItem
 *
 * Discussion:	Subclassed from CAknEnumeratedTextSettingItem. This class is
 *				used to provide a text indication of the state of multiple
 *				boolean flags.
 *
 *
 */
class CSettingAppCheckBoxSettingItem : public CAknEnumeratedTextSettingItem
	{
public:
	/**
	 * Function :	CSettingAppCheckBoxSettingItem
	 *
	 * Discussion :	First stage - non-leaving constructor. Initialises internal
	 *				memory and performs other non-leaving initialisation
	 *
	 * Params :		aIdentifier - Resource Id for the setting item
	 *				aExternalValueArray - array of external values, the state
	 *									  of which will be reflected in the
	 *									  display.
	 *
	 */
	CSettingAppCheckBoxSettingItem(TInt aIdentifier,
								   CArrayFix<TInt>& aExternalValueArray );

	/**
	 * Function :	~CSettingAppCheckBoxSettingItem	
	 *
	 * Discussion :	Virtual destructor - cleans up and deletes instance
	 *
	 */
	~CSettingAppCheckBoxSettingItem();

	/**
	 * Function :	LoadL
	 *
	 * Discussion :	Inherited from CAknSettingItem - loads values from external
	 *				data. Called at construction, but can also be called as 
	 *				required.
	 *
	 */
	void LoadL();
	
	/**
	 * Function :	StoreL
	 *
	 * Discussion :	Inherited from CAknSettingItem - transfers internal values 
	 *				back to external storage.
	 *
	 */
	void StoreL();

	/**
	 * Function :	EditItemL
	 *
	 * Discussion :	Inherited from CAknSettingItem - launches the setting page
	 *
	 * Params :		aCalledFromMenu - ignored in code.
	 */
	void EditItemL( TBool aCalledFromMenu );

	/**
	 * Function :	CompleteConstructionL
	 *
	 * Discussion :	Called by framework to complete construction. Second-phase
	 *				constructor.
	 *
	 */
	void CompleteConstructionL();

	/**
	 * Function :	SettingTextL	
	 *
	 * Discussion :	Obtains the current setting text for the item.
	 *
	 * Returns :	A descriptor containing the setting text
	 *
	 */
	const TDesC& SettingTextL();

private:
	/**
	 * Function :	SetTextRepresentationL
	 *
	 * Discussion :	Updates the internal text representation string according
	 *				to the current states of the items stored within 
	 *				iSelectionItemListData.
	 *
	 */
	void SetTextRepresentationL() ;
private:
	/** 
	 *  Selection item list. This acts as the internal
	 *	representation of the check box list
	 *	OWNED by this class.
	 */
	CSelectionItemList* iSelectionItemListData;

	
	/**
	 * Reference to the external array of integers
	 * NOT OWNED by this class
	 */
	CArrayFix<TInt>& iExternalValueArray; 

	/**
	 * Internal text represenation of data
	 * OWNED by this class
	 */
	HBufC* iTextRepresentation; // Owned
	};

#endif //__CHECKBOX_SETTING_H_
