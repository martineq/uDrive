package com.fiuba.app.udrive;

import android.app.ProgressDialog;
import android.content.Intent;
import android.support.v7.app.AppCompatActivity;

import android.os.Bundle;

import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;

import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import com.fiuba.app.udrive.model.ObjectStream;
import com.fiuba.app.udrive.model.UserAccount;
import com.fiuba.app.udrive.model.UserData;
import com.fiuba.app.udrive.network.LoginService;
import com.fiuba.app.udrive.network.ServiceCallback;
import com.fiuba.app.udrive.network.StatusCode;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

/**
 * This class represents the main activity for the app.
 * Displays the input fields for the user credentials
 */
public class MainActivity extends AppCompatActivity {

    private static final String PASS_SALT = "UDRIVE1234";
    private static final String ACCOUNT_FILENAME = "account_file";
    public static final String TAG = "MainActivity";

    private LoginService mLoginService = null;


    /**
     * Shows the structure for the main activity screen and creates
     * a LoginService instance to make requests
     * @param savedInstanceState
     */
    @Override
    protected void onCreate(final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // Check if the user account is already saved locally
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

            //this.mLoginService = new LoginService(MainActivity.this);

            TextView signUpLink = (TextView) findViewById(R.id.sign_up_link);
            signUpLink.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    // Just for testing click on Sign Up button
                    Toast.makeText(MainActivity.this, "Sign up: Just for testing.", Toast.LENGTH_LONG).show();
                }
            });
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main_settings, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        if (id == R.id.action_settings) {
            Intent settings = new Intent(MainActivity.this, SettingsActivity.class);
            settings.putExtra("mainCalling", true);
            startActivity(settings);
        }
        return super.onOptionsItemSelected(item);
    }


    /**
     * Makes a request to server when the user clicks the Sign in button
     * @param view, is the button to perform the signing in
     */
    public void sendMessage(View view) {
        this.mLoginService = new LoginService(MainActivity.this);

        final ProgressDialog progressDialog = ProgressDialog.show(this, null, getString(R.string.loading), true);
        progressDialog.setCancelable(false);

        String email = ((EditText) findViewById(R.id.email)).getText().toString();
        String password = ((EditText) findViewById(R.id.password)).getText().toString();

        UserData userData = new UserData(email, md5(PASS_SALT+password));
        System.out.println(userData.getPassword());

        this.mLoginService.getToken(userData, new ServiceCallback<UserAccount>() {
            @Override
            public void onSuccess(UserAccount uAccount, int status) {
                if (uAccount.getUserId() != 0 ) {

                    Log.d(TAG, "Getting token: Done.");
                    // Serialize UserAccount instance to get data when necessary
                    // i.e. to get the token when opening app
                    ObjectStream<UserAccount> objectFileUAccount =
                            new ObjectStream<>(ACCOUNT_FILENAME, MainActivity.this);
                    objectFileUAccount.put(uAccount);
                    Intent mNextIntent = new Intent(MainActivity.this, FileListActivity.class);
                    mNextIntent.putExtra(FileListActivity.EXTRA_USER_ACCOUNT, uAccount);
                    mNextIntent.putExtra(FileListActivity.EXTRA_DIR_ID, new Integer(0));
                    progressDialog.dismiss();
                    startActivity(mNextIntent);
                    finish();
                } else {
                    progressDialog.dismiss();
                    Toast.makeText(MainActivity.this, R.string.error_mail_pass, Toast.LENGTH_LONG).show();
                }

            }

            @Override
            public void onFailure(String message, int status) {
                Log.e(TAG, "Error attempting to get data from server");
                progressDialog.dismiss();
               if (StatusCode.isHumanReadable(status)){
                   message = StatusCode.getMessage(MainActivity.this, status);
                   Toast.makeText(MainActivity.this, message, Toast.LENGTH_LONG).show();
                }
            }
        });
    }

    /**
     * Applies a md5 hash algorithm to the given string
     * @param s, is the string to be encoded
     * @return a hex representation of the hashed string or an empty string if
     * an exception occurs
     */
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
