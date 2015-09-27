package com.fiuba.app.udrive;

import android.app.AlertDialog;

import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.support.v7.app.AppCompatActivity;

import android.os.Bundle;

import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;

import android.widget.EditText;
import android.widget.TextView;

import com.fiuba.app.udrive.model.File;
import com.fiuba.app.udrive.model.ObjectStream;
import com.fiuba.app.udrive.model.UserAccount;
import com.fiuba.app.udrive.model.UserData;
import com.fiuba.app.udrive.network.LoginService;
import com.fiuba.app.udrive.network.ServiceCallback;

import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectOutputStream;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

import retrofit.Callback;
import retrofit.RetrofitError;
import retrofit.client.Response;


public class MainActivity extends AppCompatActivity {

    private static final String PASS_SALT = "UDRIVE1234";
    private static final String ACCOUNT_FILENAME = "account_file";

    private LoginService mLoginService = null;


    @Override
    protected void onCreate(final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // Check if the user account already exists
        // In that case launch next activity
        ObjectStream<UserAccount> objectStream = new ObjectStream<>(ACCOUNT_FILENAME, MainActivity.this);
        if (objectStream.exists()){
            UserAccount uAccount = objectStream.get();
            Intent mNextIntent = new Intent(MainActivity.this, FileListActivity.class);
            mNextIntent.putExtra("userAccount", uAccount);
            startActivity(mNextIntent);
            finish();
        } else {
            // Continues showing login inputs
            setContentView(R.layout.activity_main);

            this.mLoginService = new LoginService();

            TextView signUpLink = (TextView) findViewById(R.id.sign_up_link);
            signUpLink.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    // Just for testing click on Sign Up button
                    System.out.println(this.toString());
                    AlertDialog alert = new AlertDialog.Builder(MainActivity.this).create();
                    alert.setTitle("Sign up test");
                    alert.setMessage("Testing...");
                    alert.show();
                }
            });
        }
    }

    /**
     * Called when the user clicks the Sign in button
     */
    public void sendMessage(View view) {

        String email = ((EditText) findViewById(R.id.email)).getText().toString();
        String password = ((EditText) findViewById(R.id.password)).getText().toString();

        UserData userData = new UserData(email, md5(PASS_SALT+password));
        System.out.println(md5(password));

        /* try and catch to handle a possible lack of internet connection */

        this.mLoginService.getToken(/*userData, */new ServiceCallback<UserAccount>() {
            @Override
            public void onSuccess(UserAccount uAccount) {
                System.out.println(uAccount.getToken());
                // Serialize UserAccount instance to get data when necessary
                // i.e. to get the token when opening app
                ObjectStream<UserAccount> objectFileUAccount =
                        new ObjectStream<>(ACCOUNT_FILENAME, MainActivity.this);
                objectFileUAccount.put(uAccount);
                Intent mNextIntent = new Intent(MainActivity.this, FileListActivity.class);
                mNextIntent.putExtra("userAccount", uAccount);
                startActivity(mNextIntent);
                finish();
            }

            @Override
            public void onFailure(String message) {
                Log.e("SERVER COMM ERROR", "Error attempting to get data from server");
                AlertDialog alert= new AlertDialog.Builder(MainActivity.this).create();
                alert.setTitle("Error");
                alert.setMessage(message);
                alert.show();
            }
        });
    }

    public static String md5(String s) {
        try {
            // Create MD5 Hash
            MessageDigest digest = java.security.MessageDigest.getInstance("MD5");
            digest.update(s.getBytes());
            byte messageDigest[] = digest.digest();

            // Create Hex String
            StringBuffer hexString = new StringBuffer();
            for (int i=0; i<messageDigest.length; i++)
                hexString.append(Integer.toHexString(0xFF & messageDigest[i]));
            return hexString.toString();

        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        }
        return "";
    }

    public final static String getAccountFilename(){
        return ACCOUNT_FILENAME;
    }

}
