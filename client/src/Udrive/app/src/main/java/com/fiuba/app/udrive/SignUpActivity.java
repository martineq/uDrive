package com.fiuba.app.udrive;

import android.app.ProgressDialog;
import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

import com.fiuba.app.udrive.model.GenericResult;
import com.fiuba.app.udrive.model.UserProfile;
import com.fiuba.app.udrive.model.Util;
import com.fiuba.app.udrive.network.ServiceCallback;
import com.fiuba.app.udrive.network.SignUpService;
import com.fiuba.app.udrive.network.StatusCode;

import java.util.ArrayList;

public class SignUpActivity extends AppCompatActivity {

    private SignUpService mSignUpService = null;

    @Override
    protected void onCreate(final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_sign_up);
    }

    //Toast.makeText(MainActivity.this, "Sign up: Just for testing.", Toast.LENGTH_LONG).show();

    /**
     * Makes a request to server when the user clicks the Sign up button
     * @param view, is the button to perform the signing up
     */
    public void signUp(View view) {
        mSignUpService = new SignUpService(SignUpActivity.this);

        final ProgressDialog progressDialog = ProgressDialog.show(this, null, getString(R.string.loading), true);
        progressDialog.setCancelable(false);

        String email = ((EditText) findViewById(R.id.signup_email)).getText().toString();
        String password = ((EditText) findViewById(R.id.signup_password)).getText().toString();
        String firstname = ((EditText) findViewById(R.id.signup_firstname)).getText().toString();
        String lastname = ((EditText) findViewById(R.id.signup_lastname)).getText().toString();

        ArrayList<String> words = new ArrayList<String>();
        words.add("");
        words.add(" ");
        words.add(null);

        String error = "";

        if (Util.validateString(firstname, words))
            error += getString(R.string.error_firstname);

        if (Util.validateString(lastname, words))
            error += "\n"+getString(R.string.error_lastname);

        if (Util.validateString(email, words))
            error += "\n"+getString(R.string.error_email);

        if (Util.validateString(password, words))
            error += "\n"+getString(R.string.error_password);

        System.out.println("Error >>>> "+error);

        if (error.compareTo("") != 0){
            progressDialog.dismiss();
            Toast.makeText(SignUpActivity.this, error, Toast.LENGTH_LONG).show();
            return;
        }

        UserProfile userProfile = new UserProfile(email, password, firstname, lastname,
                null, null, null, null, null);
        // photo, lastLocation, quotaTotal, quotaAvailable, quotaUsagePercent

        mSignUpService.signUp(userProfile, new ServiceCallback<GenericResult>() {
            @Override
            public void onSuccess(GenericResult res, int status) {
                // TODO: delete. Just for testing.
                //res.setResultCode(1);
                if (res.getResultCode() == 1) {
                    progressDialog.dismiss();
                    Toast.makeText(SignUpActivity.this, R.string.signup_ok, Toast.LENGTH_LONG).show();
                    finish();
                } else {
                    progressDialog.dismiss();
                    Toast.makeText(SignUpActivity.this, R.string.error_mail_exists, Toast.LENGTH_LONG).show();
                }
            }

            @Override
            public void onFailure(String message, int status) {
               progressDialog.dismiss();
                if (StatusCode.isHumanReadable(status)) {
                    message = StatusCode.getMessage(SignUpActivity.this, status);
                    Toast.makeText(SignUpActivity.this, message, Toast.LENGTH_LONG).show();
                }
            }
        });
    }

}
