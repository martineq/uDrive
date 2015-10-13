package com.fiuba.app.udrive;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Gravity;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.Toast;

import com.fiuba.app.udrive.network.ConnectionConfig;

import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;
import java.util.ListIterator;

public class SettingsActivity extends AppCompatActivity {

    private ListView mListView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_settings);
        // Get ListView object from xml
        mListView = (ListView) findViewById(R.id.settingsView);

        ArrayList<String> items = new ArrayList<String>();
        ArrayList<Integer> imgs = new ArrayList<Integer>();
        // If settings was clicked not logged in, so we just show Connections settings
        // and anything else.
        if (getIntent().getBooleanExtra("mainCalling", false)) {
            items.add(getString(R.string.settings_item_connection));
            imgs.add(R.drawable.ic_wifi_grey600_36dp);
        } else { // Then we define other settings options
            items.add("Account");
            imgs.add(R.drawable.ic_account_plus);
        }
        String[] itemsArray = items.toArray(new String[items.size()]);
        Integer[] imgsArray = imgs.toArray(new Integer[imgs.size()]);

        SettingsListAdapter settingsAdapter = new SettingsListAdapter(this, itemsArray, imgsArray);
        // Assign adapter to ListView
        mListView.setAdapter(settingsAdapter);
        // ListView Item Click Listener
        mListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {

            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                // ListView Clicked item index
                int itemPosition = position;
                // ListView Clicked item value
                String itemValue = (String) mListView.getItemAtPosition(position);

                if (itemValue == getString(R.string.settings_item_connection)){

                    final EditText ip = new EditText(SettingsActivity.this);
                    final EditText port = new EditText(SettingsActivity.this);
                    LinearLayout layout = new LinearLayout(SettingsActivity.this);
                    layout.setOrientation(LinearLayout.VERTICAL);
                    layout.setGravity(Gravity.CENTER_VERTICAL);
                    // TODO: set padding
                    ip.setText(ConnectionConfig.getIP(SettingsActivity.this));
                    port.setText(ConnectionConfig.getPort(SettingsActivity.this));
                    layout.addView(ip);
                    layout.addView(port);

                    AlertDialog.Builder builder = new AlertDialog.Builder(SettingsActivity.this);
                    builder.setCancelable(false)
                            .setTitle(getString(R.string.settings_item_connection))
                            .setView(layout)
                            .setPositiveButton(getString(R.string.settings_save), new DialogInterface.OnClickListener() {
                                public void onClick(DialogInterface dialog, int id) {
                                    Toast.makeText(getApplicationContext(), "IP: " + ip.getText() + " Port: " + port.getText(), Toast.LENGTH_LONG).show();
                                    ConnectionConfig.setConnectionParams(ip.getText().toString(), port.getText().toString(), SettingsActivity.this);
                                    System.out.println("Connecting to >>> "+ConnectionConfig.getConnectionURL(SettingsActivity.this));
                                }
                            })
                            .setNegativeButton(getString(R.string.settings_cancel), new DialogInterface.OnClickListener() {
                                public void onClick(DialogInterface dialog, int id) {
                                    dialog.cancel();
                                }
                            });
                    AlertDialog alert = builder.create();
                    alert.show();
                }

            }

        });
    }
}
