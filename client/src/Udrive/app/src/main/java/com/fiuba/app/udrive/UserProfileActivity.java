package com.fiuba.app.udrive;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.location.Address;
import android.location.Geocoder;
import android.net.Uri;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Base64;
import android.view.Gravity;
import android.view.View;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.fiuba.app.udrive.model.GenericResult;
import com.fiuba.app.udrive.model.MyPhoto;
import com.fiuba.app.udrive.model.UserAccount;
import com.fiuba.app.udrive.model.UserProfile;
import com.fiuba.app.udrive.model.Util;
import com.fiuba.app.udrive.network.ServiceCallback;
import com.fiuba.app.udrive.network.UserService;

import java.io.ByteArrayOutputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.util.List;
import java.util.Locale;

public class UserProfileActivity extends AppCompatActivity {
    private UserProfile mUserProfile = null;
    private static final int SELECT_PHOTO = 100;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_user_profile);
        mUserProfile = (UserProfile)getIntent().getSerializableExtra("userProfile");

        // If there is not an image from server, sets one by default.
        if (mUserProfile.getPhoto().compareTo("") == 0)
            ((ImageView)findViewById(R.id.avatar)).setImageResource(R.drawable.user1);
        else {
            // Gets Base64 encoded bitmap and converts it to be set on ImageView
            byte[] decodedString = Base64.decode(mUserProfile.getPhoto(), Base64.DEFAULT);
            Bitmap decodedByte = BitmapFactory.decodeByteArray(decodedString, 0, decodedString.length);
            ((ImageView)findViewById(R.id.avatar)).setImageBitmap(decodedByte);
        }

        // Sets firstname and lastname
        ((TextView)findViewById(R.id.name)).setText(
                Util.capitalize(mUserProfile.getFirstname()) + " " + Util.capitalize(
                        mUserProfile.getLastname()));
        // Sets email
        ((TextView)findViewById(R.id.email)).setText(mUserProfile.getEmail());

        // Last location city
        Geocoder geocoder = new Geocoder(this, Locale.getDefault());
        List<Address> addresses = null;
        String stateName = null;
        String countryName = null;
        if ((mUserProfile.getGPSLatitude()!=0.00)&&(mUserProfile.getGPSLongitude()!=0.00)) {
            try {
                //addresses = geocoder.getFromLocation(-34.795713, -58.348321, 1);
                addresses = geocoder.getFromLocation(mUserProfile.getGPSLatitude(),
                        mUserProfile.getGPSLongitude(), 1);
                stateName = addresses.get(0).getAddressLine(1);
                countryName = addresses.get(0).getAddressLine(2);
            } catch (IOException e) {
                // do something
            }
        } else {
            stateName = "Undefined state";
            countryName = "Undefined country";
        }

        //String cityName = addresses.get(0).getAddressLine(0);

        ((TextView)findViewById(R.id.lastLocation)).setText("Last location: " + stateName+", "+countryName);

        // Builds bar for quota information
        // Convert quota to MB.
        double usedMB = Double.parseDouble(mUserProfile.getQuotaUsed())/Math.pow(2,20);
        double totalMB = Double.parseDouble(mUserProfile.getQuotaTotal())/Math.pow(2, 20);
        ((TextView) findViewById(R.id.textProgressBar)).setText("Usage: "+usedMB
            +" MB ("+mUserProfile.getQuotaUsagePercent()+") of "+totalMB+" MB");
        ProgressBar progressbar = (ProgressBar) findViewById(R.id.pbar1);
        progressbar.setProgress(Integer.parseInt(Util.extractDigits(mUserProfile.getQuotaUsagePercent())));
    }

    public void changePic(View view){
       // Picasso.with(this).load(R.drawable.user2).into((ImageView)findViewById(R.id.avatar));
        Intent photoPickerIntent = new Intent(Intent.ACTION_PICK);
        photoPickerIntent.setType("image/*");
        startActivityForResult(photoPickerIntent, SELECT_PHOTO);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent imageReturnedIntent) {
        super.onActivityResult(requestCode, resultCode, imageReturnedIntent);

        switch(requestCode) {
            case 100: // TODO: replace with corresponding var name
                if(resultCode == RESULT_OK){
                    Uri selectedImage = imageReturnedIntent.getData();
                    InputStream imageStream = null;
                    try {
                        imageStream = getContentResolver().openInputStream(selectedImage);
                    } catch (FileNotFoundException e){
                        // Do something
                    }
                    Bitmap yourSelectedImage = BitmapFactory.decodeStream(imageStream);
                   ((ImageView)findViewById(R.id.avatar)).setImageBitmap(yourSelectedImage);

                    // Encode to Base64 and Send to the server
                    ByteArrayOutputStream stream = new ByteArrayOutputStream();
                    yourSelectedImage.compress(Bitmap.CompressFormat.JPEG, 100, stream);
                    byte[] byteArray = stream.toByteArray();
                    String strBase64= Base64.encodeToString(byteArray, 0);
                    //System.out.println(strBase64);

                    String token = ((UserAccount)getIntent().getSerializableExtra("userAccount")).getToken();
                    UserService userService = new UserService(token, UserProfileActivity.this);
                    userService.updatePhoto(mUserProfile.getUserId(), new MyPhoto(strBase64), new ServiceCallback<GenericResult>() {
                        @Override
                        public void onSuccess(GenericResult object, int status) {

                        }

                        @Override
                        public void onFailure(String message, int status) {

                        }
                    });


                }
        }
    }

    public void editName(View view) {
        final EditText firstname = new EditText(UserProfileActivity.this);
        final EditText lastname = new EditText(UserProfileActivity.this);
        LinearLayout layout = new LinearLayout(UserProfileActivity.this);
        layout.setOrientation(LinearLayout.VERTICAL);
        layout.setGravity(Gravity.CENTER_VERTICAL);
        layout.setPaddingRelative(200, 50, 200, 50);
        String fName = Util.capitalize(mUserProfile.getFirstname());
        String lName = Util.capitalize(mUserProfile.getLastname());
        firstname.setText(fName);
        lastname.setText(lName);
        firstname.setGravity(Gravity.CENTER_HORIZONTAL);
        lastname.setGravity(Gravity.CENTER_HORIZONTAL);
        layout.addView(firstname);
        layout.addView(lastname);

        AlertDialog.Builder builder = new AlertDialog.Builder(UserProfileActivity.this);
        builder.setCancelable(false)
                .setTitle(getString(R.string.screen_name))
                .setView(layout)
                .setPositiveButton(getString(R.string.save_changes), new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        mUserProfile.setFirstname(firstname.getText().toString());
                        mUserProfile.setLastname(lastname.getText().toString());
                        ((TextView)findViewById(R.id.name)).setText(
                                Util.capitalize(mUserProfile.getFirstname()) + " " + Util.capitalize(
                                        mUserProfile.getLastname()));
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
