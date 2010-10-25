/*
 * Copyright © 2008 Nokia Corporation.
 */

#ifndef __SettingsData_H__
#define __SettingsData_H__

#include <e32base.h>
#include <in_sock.h>

// size for data descriptors
const TInt KBufSize = 100;

// Forward declarations
class RReadStream;
class RWriteStream;

/**
 * Class: 		CSettingsData
 *
 * Discussion:	An aggregate class containing data for all editing controls
 *				This class is used as a central respository for all data
 *				to be displayed and editted within this app.
 */
class CSettingsData : public CBase
{
public:
	/**
 	* Function: 	NewL
	*
 	* Discussion:	Factory function used to create a new instance of
 	* 				CSettingsData
	*
 	* Returns:		A newly created instance of CSettingsData with all data
 	*				items valid
	*/
	static CSettingsData* NewL();

	/**
 	* Function: 	NewLC
	*
 	* Discussion:	Factory function used to create a new instance of
 	* 				CSettingsData and leave a pointer to it on the cleanup
 	*				stack
	*
 	* Returns:		A newly created instance of CSettingsData with all data
 	*				items valid
	*/
	static CSettingsData* NewLC();

	/**
	* Function:		~CSettingsData
	*
	* Discussion: 	Virtual destructor for CSettingsData. Ensures that all
	*				allocated memory is returned to the heap before deletion
	* 				of object.
	*
	*/
	virtual ~CSettingsData();

    /**
    * Function:    LoadL
    * 
    * Discussion:  Loads itself from stream.
    * 
    */
    void LoadL(RReadStream& aStream);
    
    /**
    * Function:     SaveL
    * 
    * Discussion:   Saves itself to stream.
    * 
    */
	void SaveL(RWriteStream& aStream) const;
    
	/**
 	* Function: 	Text
	*
 	* Discussion:	Trivial accessor function
	*
 	* Returns:		A reference to the iText member
	*/
	inline TBuf<KBufSize>& UserName() { return iUsrName; }

	/**
 	* Function: 	Enumeration
	*
 	* Discussion:	Trivial accessor function
	*
 	* Returns:		A reference to the iEnumeration member
	*/
	inline TInt& Mode() { return iMode; }
	
	inline TInt& Brand() { return iBrand; }

	/**
 	* Function: 	Volume
	*
 	* Discussion:	Trivial accessor function
	*
 	* Returns:		A reference to the iVolume member
	*/
	inline TInt& Volume() { return iVolume; }

	/**
 	* Function: 	Slider
	*
 	* Discussion:	Trivial accessor function
	*
 	* Returns:		A reference to the iSlider member
	*/
	inline TInt& Slider() { return iSlider; }

	/**
 	* Function: 	Binary
	*
 	* Discussion:	Trivial accessor function
	*
 	* Returns:		A reference to the iBinary member
	*/
	inline TBool& Binary() {return iBinary; }

	/**
 	* Function: 	SecretText
	*
 	* Discussion:	Trivial accessor function
	*
 	* Returns:		A reference to the iSecretText member
	*/
	inline TBuf<KBufSize>& SecretText() { return iSecretText; }

		/**
 	* Function: 	NumericText
	*
 	* Discussion:	Trivial accessor function
	*
 	* Returns:		A reference to the iNumericText member
	*/
	inline TBuf<KBufSize>& NumericText() { return iNumericText; }

	/**
 	* Function: 	Date
	*
 	* Discussion:	Trivial accessor function
	*
 	* Returns:		A reference to the iDate member
	*/
	inline TTime& Date() { return iDate; }

	/**
 	* Function: 	Time
	*
 	* Discussion:	Trivial accessor function
	*
 	* Returns:		A reference to the iTime member
	*/
	inline TTime& Time() { return iTime; }

	/**
 	* Function: 	IpAddress
	*
 	* Discussion:	Trivial accessor function
	*
 	* Returns:		A reference to the iIpAddress member
	*/
	inline TInetAddr& IpAddress() { return iIpAddress; }

	/**
 	* Function: 	CheckboxArray
	*
 	* Discussion:	Trivial accessor function
	*
 	* Returns:		A reference to the iCheckboxArray member
	*/
	inline CArrayFix<TInt>& CheckboxArray() { return iCheckboxArray; }

private:
	/**
	* Function:		CSettingsData
	*
	* Discussion: 	Hidden, first-stage non-leaving constructor.
	*
	*/
	CSettingsData();

	/**
	* Function:		ConstructL
	*
	* Discussion: 	Hidden, second-stage possibly-leaving constructor.
	*
	*/
	void ConstructL();


public:
	TBuf<KBufSize> iUsrName;
	TInt iMode;
	TInt iBrand;
	TInt iVolume;							/* Data for volume field         */
	TInt iSlider;							/* Data for slider field         */
	TBool iBinary;							/* Data for binary field         */
	TBuf<KBufSize> iSecretText;				/* Data for secret text field    */
	TBuf<KBufSize> iNumericText;			/* Data for numeric text field   */
	TTime iDate;							/* Data for date field           */
	TTime iTime;							/* Data for time field           */
	TInetAddr iIpAddress;					/* Data for IP Address field     */
	CArrayFixFlat<TInt> iCheckboxArray;		/* Data for checkboxes           */
};

#endif // __SettingsData_H__

