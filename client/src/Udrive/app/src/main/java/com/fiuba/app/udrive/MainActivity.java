package com.fiuba.app.udrive;

import android.app.AlertDialog;

import android.support.v7.app.AppCompatActivity;

import android.os.Bundle;

import android.view.Menu;
import android.view.MenuItem;
import android.view.View;

import android.widget.EditText;
import android.widget.TextView;

import com.fiuba.app.udrive.model.UserAccount;
import com.fiuba.app.udrive.model.UserData;
import com.fiuba.app.udrive.rest.service.RestClient;
import com.fiuba.app.udrive.rest.service.RestService;

import retrofit.Callback;
import retrofit.RetrofitError;
import retrofit.client.Response;

public class MainActivity extends AppCompatActivity {

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

    /**
     * Called when the user clicks the Sign in button
     */
    public void sendMessage(View view) {

        String email = ((EditText) findViewById(R.id.email)).getText().toString();
        String password = ((EditText) findViewById(R.id.password)).getText().toString();

        UserData userData = new UserData(email, password);

        /* try and catch to handle a possible lack of internet connection */

        /////////////////////////////////
        // Testing Retrofit requests
        /////////////////////////////////

        final String API_URL = "http://jsonplaceholder.typicode.com";

        RestService service = RestClient.createService(RestService.class, API_URL);
        service.getToken(userData, new Callback<UserAccount>() {
            @Override
            public void success(UserAccount uAccount, Response response) {
              //  System.out.println(testRest.getOrigin());
                System.out.println(uAccount.getToken());
                response.getStatus();
            }

            @Override
            public void failure(RetrofitError error) {
                // Implement getStatusCode(RetrofitError error) which returns an int code
                if (error.getKind() == RetrofitError.Kind.NETWORK) {
                    System.out.println(">>>> No Internet connection");
                } else
                    System.out.println(">>>> An error occurred");
            }
        });

        ///////////////////
        // Do something in response to button
        // send user, pass and get token. Then instantiate
        // Launch FileListActivity after getting token
        // Send user object

    }

}
