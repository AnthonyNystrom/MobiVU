package com.mobivu;

import android.content.SharedPreferences;
import android.content.SharedPreferences.OnSharedPreferenceChangeListener;
import android.content.res.Resources;
import android.os.Bundle;
import android.preference.CheckBoxPreference;
import android.preference.EditTextPreference;
import android.preference.ListPreference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceManager;
import android.preference.RingtonePreference;

public class Settings extends PreferenceActivity implements OnSharedPreferenceChangeListener {
	 @Override
	    protected void onCreate(Bundle savedInstanceState) {
	        super.onCreate(savedInstanceState);
	        
	        // Load the preferences from an XML resource
	        addPreferencesFromResource(R.xml.settings);
	        
	    }	
	 @Override
	    protected void onResume() {
	        super.onResume();
	        getPreferenceScreen().getSharedPreferences().registerOnSharedPreferenceChangeListener(this);
	        UpdateSummries();
	 	}    
	 	private void UpdateSummries(){
	        Resources res = getResources();
			SharedPreferences sharedPref = PreferenceManager.getDefaultSharedPreferences(this);
			
			// set summary like current value of mode
			String sKey = res.getString(R.string.prefMode);
			String []sModeValues = res.getStringArray(R.array.entries_list_preference_mode);
	        ListPreference listMode = (ListPreference)getPreferenceScreen().findPreference(sKey);
	        String sValMode = sharedPref.getString(sKey, "");
	        int iIdx = Integer.parseInt(sValMode);
	        listMode.setSummary(sModeValues[iIdx]);
	        
	        // set summary of nickname value
	        sKey = res.getString(R.string.prefNikName);
	        EditTextPreference edNickName = (EditTextPreference)getPreferenceScreen().findPreference(sKey);
	        edNickName.setSummary(sharedPref.getString(sKey, ""));
	        
	        // set summary of server value
	        sKey = res.getString(R.string.prefIPServer);
	        EditTextPreference edIP = (EditTextPreference)getPreferenceScreen().findPreference(sKey);
	        edIP.setSummary(sharedPref.getString(sKey, ""));
	 }
	public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String key) {
		UpdateSummries();		
	}
	 
}
