package com.fiuba.app.udrive;

import android.app.Activity;

import android.content.Intent;

import android.os.Bundle;

import android.support.v7.app.AppCompatActivity;

import android.view.MenuItem;

import android.widget.TextView;

public class DisplayMessageActivity extends AppCompatActivity{

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //setContentView(R.layout.activity_display_message);

        Intent intent = getIntent();
        String[] credentials = intent.getStringArrayExtra(MainActivity.USER_CREDENTIALS);
        TextView textView = new TextView(this);
        textView.setTextSize(20);
        String userCredentials = "Your e-mail is: "+credentials[0]+" and your password: "+credentials[1];
        textView.setText(userCredentials);

        setContentView(textView);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        if (id == R.id.action_settings) {
            return true;
        }
        return super.onOptionsItemSelected(item);
    }
}
