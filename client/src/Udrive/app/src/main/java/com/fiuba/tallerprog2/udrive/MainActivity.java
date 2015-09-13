package com.fiuba.tallerprog2.udrive;

import android.app.ActionBar;
import android.app.AlertDialog;

import android.content.DialogInterface;
import android.content.Intent;

import android.support.v7.app.ActionBarActivity;
import android.support.v7.app.AppCompatActivity;

import android.os.Bundle;

import android.view.Menu;
import android.view.MenuItem;
import android.view.View;

import android.widget.EditText;
import android.widget.TextView;

import org.w3c.dom.Text;

public class MainActivity extends AppCompatActivity {

    public static final String USER_CREDENTIALS = "com.fiuba.tallerprog2.udrive.CREDENTIALS";

    @Override
    protected void onCreate(final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        TextView signUpLink = (TextView)findViewById(R.id.sign_up_link);
        signUpLink.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                // Just for testing click on Sign Up button
                System.out.println(this.toString());
                AlertDialog alert= new AlertDialog.Builder(MainActivity.this).create();
                alert.setTitle("Sign up test");
                alert.setMessage("Testing...");
                alert.show();
            }
        });
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    /** Called when the user clicks the Sign in button */
    public void sendMessage(View view) {
        // Do something in response to button
        Intent intent = new Intent(this, DisplayMessageActivity.class);
        EditText emailText = (EditText) findViewById(R.id.email);
        EditText pwdText = (EditText) findViewById(R.id.password);
        String email = emailText.getText().toString();
        String password = pwdText.getText().toString();
        String[] credentials = {email,password};
        intent.putExtra(USER_CREDENTIALS, credentials);
        startActivity(intent);
    }



}
