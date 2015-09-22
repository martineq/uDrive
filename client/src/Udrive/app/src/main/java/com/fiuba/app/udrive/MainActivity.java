package com.fiuba.app.udrive;

import android.app.AlertDialog;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;

import android.os.Bundle;

import android.view.Menu;
import android.view.MenuItem;
import android.view.View;

import android.widget.EditText;
import android.widget.TextView;

import com.fiuba.app.udrive.model.UserAccount;
import com.fiuba.app.udrive.model.UserData;
import com.fiuba.app.udrive.network.LoginService;
import com.fiuba.app.udrive.network.ServiceCallback;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

import retrofit.Callback;
import retrofit.RetrofitError;
import retrofit.client.Response;


public class MainActivity extends AppCompatActivity {

    private static final String PASS_SALT = "UDRIVE1234";

    private LoginService mLoginService = null;

    @Override
    protected void onCreate(final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        this.mLoginService = new LoginService();

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
                Intent intent = new Intent(MainActivity.this, FileListActivity.class);
                intent.putExtra("userAccount", uAccount);
                startActivity(intent);
            }

            @Override
            public void onFailure(String message) {
                System.out.println(">>>>> Retrofit: An error has occurred.");
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

}
