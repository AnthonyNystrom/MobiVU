/*
 * Copyright © 2008 Nokia Corporation.
 */

#ifndef __ExampleSettingList_H__
#define __ExampleSettingList_H__


#include <coecntrl.h>
#include <aknsettingitemlist.h> 

#include "SettingExampleListView.h"
#include "SettingExampleCheckboxSetting.h"

class CSettingsData;

/**
 * Class: CExampleSettingList
 *
 * Discussion: 	A specialised subclass of CAknSettingItemList. This class is
 *				the control used to both display the current values of the
 *				settings and to launch the relevant editor for those items.
 *
 */
class CExampleSettingList : public CAknSettingItemList
	{
public:
	/**
 	* Function: 	NewL
	*
 	* Discussion:	Factory function used to create a new instance of
 	* 				CExampleSettingList
	*
	* Params:		aData - reference to the data to be displayed and edited.
	*
 	* Returns:		A newly created instance of CExampleSettingList
	*/
	static CExampleSettingList* NewL(CSettingsData &aData);

	/**
 	* Function: 	NewLC
	*
 	* Discussion:	Factory function used to create a new instance of
 	* 				CExampleSettingList and leave a pointer to it on the
 	*				cleanup	stack.
	*
	* Params:		aData - reference to the data to be displayed and edited.
	*
 	* Returns:		A newly created instance of CExampleSettingList
	*/
	static CExampleSettingList* NewLC(CSettingsData &aData);

	/**
	* Function:		~CExampleSettingList
	*
	* Discussion: 	Virtual destructor for CExampleSettingList. Ensures that
	*				all	allocated memory is returned to the heap before
	*				deletion of object.
	*
	*/
	virtual ~CExampleSettingList();


	/**
	* Function:		SizeChanged
	*
	* Discussion: 	Inherited from CCoeControl. Calculates size based on
	*				contained control required sizes.
	*
	*/
	void SizeChanged();

	/**
	 * Function:	EditCurrentItemL
	 *
	 * Discussion:	Starts the setting page for the currently selected item 
	 *				in the list.
	 */
	void EditCurrentItemL(TBool aFromMenu);
	void EditItemL( TInt aIndex, TBool aCalledFromMenu );

	
	
private:
	/**
	* Function:		CExampleSettingList
	*
	* Discussion: 	Hidden, first-stage non-leaving constructor.
	*
	* Params:		aData - reference to the data to be displayed and edited.
	*
	*/
	CExampleSettingList(CSettingsData &aData);

	/**
	* Function:		CreateSettingItemL
	*
	* Discussion: 	Factory function for creating CAknSettingItem derived
	*				SettingItem classes used t display current values
	*
	* Params:		aSettingId - the id of the control which should be created
	*
	*/
	CAknSettingItem* CreateSettingItemL (TInt aSettingId);

private:
	CSettingsData &iSettingsData;		/* Reference to data owned by AppUi */
	};

#endif // __SettingListContainer_H__

