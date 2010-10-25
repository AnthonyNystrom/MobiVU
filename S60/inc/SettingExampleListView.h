/*
 * Copyright © 2008 Nokia Corporation.
 */

#ifndef __SettingListView_H__
#define __SettingListView_H__


#include <aknview.h>

class CExampleSettingList;
class CSettingsData;
class CDemoVideoCallAppUi;

/**
 * Class: 		CSettingListView
 *
 * Discussion:	Main view of SettingExample app. Owns the controls used
 *				for editting the data contained within CSettingsData
 *
 *
 */
class CSettingListView : public CAknView
    {
public:

    /**
     * Function: 	NewL
     *
     * Discussion: 	Construct a CSettingListView two phase construction, and
     * 			   	return a pointer to the created object.
     *
     * Param:      	CSettingsData - the data which is being edited by the
     *								Settings list
     *
     * Returns:    	A pointer to the created instance of CSettingListView
     */
	static CSettingListView* NewL(CSettingsData &aData);

    /**
     * Function: 	NewLC
     *
     * Discussion: 	Construct a CSettingListView two phase construction, and
     * 			   	return a pointer to the created object. Pointer to object
	 *				is left on the cleanup stack.
     *
     * Param:      	CSettingsData - the data which is being edited by the
     *								Settings list
     *
     * Returns:    	A pointer to the created instance of CSettingListView
     */
	static CSettingListView* NewLC(CSettingsData &aData);

    /**
     * Function:  	DoActivateL
     *
     * Discussion: 	Called when the owning AppUi class activates the view.
     *			   	Causes view to create and register all client controls.
     *
     *
     * Param:      	aPrevViewId - 		ignored in code
     *			   	aCustomMessageId -	ignored in code
     *			   	aCustomMessage -		ignored in code
     *
     */
	void DoActivateL(const TVwsViewId& aPrevViewId,
					 	   TUid aCustomMessageId,
					 const TDesC8& aCustomMessage);


    /**
     * Function:  	DoDeactivateL
     *
     * Discussion: 	Called when the owning AppUi class deactivates the view.
     *			   	Causes view to deregister and destroy all client controls.
     *
     */
	void DoDeactivate();


    /**
     * Function:  	Id
     *
     * Discussion: 	Returns the UID of this view.
     *
     * Returns:		the UID of this view.
     */
	TUid Id() const;


    /**
     * Function:  	HandleCommandL
     *
     * Discussion: 	Handles any view-specific commands.
     *
     * Param:		aCommand - the command to be handled.
     */
	void HandleCommandL(TInt aCommand);
	
private:
    /**
     * Function:  	CSettingListView
     *
     * Discussion: 	Private first-stage non-leaving constructor.
     *
     * Param:		aData - the data to be editted within this view.
     */
	CSettingListView(CSettingsData &aData);


    /**
     * Function:  	ConstructL
     *
     * Discussion: 	Private second-stage constructor. Used for all
     *				construction that might result in a leave
     */
	void ConstructL();

	/**
	 * Often, the setting list would be the child control of some container.
	 * In this case, as only a single control is used, there is no container 
	 * class.
	 *
	 * This control is owned by the SettingListView and must be deleted in 
	 * the destructor.
	 */
	CExampleSettingList* iSettingList;

	/**
	 * the data which will be modified by the settings list.
	 */
	CSettingsData& iSettingsData;

	/**
	 * HandleStatusPaneSizeChange()
	 * Called by framework when resource is changed.
	 */
	void HandleStatusPaneSizeChange ();
	
	CDemoVideoCallAppUi* iAppUi;
	};

#endif // __SettingListView_H__

