package com.fiuba.app.udrive;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.Editable;
import android.text.InputType;
import android.text.TextWatcher;
import android.text.method.DigitsKeyListener;
import android.view.Gravity;
import android.view.View;
import android.widget.AdapterView;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.Toast;

import com.fiuba.app.udrive.network.ConnectionConfig;

import java.util.ArrayList;
import java.util.regex.Pattern;

/**
 * This class is intended to handle the app settings by user
 * @access public
 */
public class SettingsActivity extends AppCompatActivity {

    private ListView mListView;

    private static final Pattern PARTIAL_IP_ADDRESS =
            Pattern.compile("^((25[0-5]|2[0-4][0-9]|[0-1][0-9]{2}|[1-9][0-9]|[0-9])\\.){0,3}" +
                    "((25[0-5]|2[0-4][0-9]|[0-1][0-9]{2}|[1-9][0-9]|[0-9])){0,1}$");

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
                    layout.setPaddingRelative(200, 50, 200, 50);
                    ip.setText(ConnectionConfig.getIP(SettingsActivity.this));
                    port.setText(ConnectionConfig.getPort(SettingsActivity.this));
                    ip.setGravity(Gravity.CENTER_HORIZONTAL);

                    ip.addTextChangedListener(new TextWatcher() {
                        @Override
                        public void onTextChanged(CharSequence s, int start, int before, int count) {
                        }

                        @Override
                        public void beforeTextChanged(CharSequence s, int start, int count, int after) {
                        }

                        private String mPreviousText = "";

                        @Override
                        public void afterTextChanged(Editable s) {
                            if (PARTIAL_IP_ADDRESS.matcher(s).matches()) {
                                mPreviousText = s.toString();
                            } else {
                                s.replace(0, s.length(), mPreviousText);
                            }
                        }
                    });

                    port.setInputType(InputType.TYPE_CLASS_NUMBER);
                    port.setKeyListener(DigitsKeyListener.getInstance("0123456789"));
                    port.setGravity(Gravity.CENTER_HORIZONTAL);
                    layout.addView(ip);
                    layout.addView(port);

                    AlertDialog.Builder builder = new AlertDialog.Builder(SettingsActivity.this);
                    builder.setCancelable(false)
                            .setTitle(getString(R.string.settings_item_connection))
                            .setView(layout)
                            .setPositiveButton(getString(R.string.settings_save), new DialogInterface.OnClickListener() {
                                public void onClick(DialogInterface dialog, int id) {
                                    if ((Integer.parseInt(port.getText().toString()) < 1024) || (Integer.parseInt(port.getText().toString()) > 49151)){
                                        Toast.makeText(getApplicationContext(), getString(R.string.error_port_number), Toast.LENGTH_LONG).show();
                                        return;
                                    }
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
