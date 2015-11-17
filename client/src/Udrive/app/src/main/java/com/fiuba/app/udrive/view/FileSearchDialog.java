package com.fiuba.app.udrive.view;

import android.app.AlertDialog;
import android.content.Context;
import android.support.v4.content.ContextCompat;
import android.widget.TabHost;

import com.fiuba.app.udrive.R;

public class FileSearchDialog extends AlertDialog {

    public FileSearchDialog(final Context context){
        super(context);
        setTitle("My First Custom Tabbed Dialog");
        setContentView(R.layout.filesearch_custom_dialog);

        // get our tabHost from the xml
        TabHost tabHost = (TabHost)findViewById(R.id.TabHost01);
        tabHost.setup();

        // create tab 1
        TabHost.TabSpec spec1 = tabHost.newTabSpec("tab1");
        spec1.setIndicator("Profile", ContextCompat.getDrawable(context, R.drawable.ic_tag));
        spec1.setContent(R.id.TextView01);
        tabHost.addTab(spec1);
        //create tab2
        TabHost.TabSpec spec2 = tabHost.newTabSpec("tab2");
        spec2.setIndicator("Profile", ContextCompat.getDrawable(context, R.drawable.ic_tag));
        spec2.setContent(R.id.TextView02);
        tabHost.addTab(spec2);
    }
}
