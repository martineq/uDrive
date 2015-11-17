package com.fiuba.app.udrive;

import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.location.Address;
import android.location.Geocoder;
import android.net.Uri;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.Html;
import android.util.Base64;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import com.fiuba.app.udrive.model.GenericResult;
import com.fiuba.app.udrive.model.MyPhoto;
import com.fiuba.app.udrive.model.UserAccount;
import com.fiuba.app.udrive.model.UserFullName;
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
    private UserService mUserService = null;
    private UserAccount mUserAccount = null;
    private static final int SELECT_PHOTO = 100;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        System.out.println(">>>>> I am in UserProfileActivity!");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_user_profile);
        mUserAccount = ((UserAccount)getIntent().getSerializableExtra("userAccount"));
        String token = mUserAccount.getToken();
        mUserService = new UserService(token, UserProfileActivity.this);

        // Getting profile
        final ProgressDialog progressDialog = ProgressDialog.show(this, null, getString(R.string.loading), true);
        progressDialog.setCancelable(false);
        mUserService.getProfile(mUserAccount.getUserId(), new ServiceCallback<UserProfile>() {
            @Override
            public void onSuccess(UserProfile uProfile, int status) {
                mUserProfile = uProfile;
                progressDialog.dismiss();
                // If there is not an image from server, sets one by default.
                if (mUserProfile.getPhoto().compareTo("") == 0)
                    ((ImageView)findViewById(R.id.avatar)).setImageResource(R.drawable.user1);
                else {
                    System.out.println(">>>> Just goint to decode base64 picture!");
                    // Gets Base64 encoded bitmap and converts it to be set on ImageView
                    byte[] decodedString = Base64.decode(mUserProfile.getPhoto(), Base64.DEFAULT);
                    Bitmap decodedByte = BitmapFactory.decodeByteArray(decodedString, 0, decodedString.length);
                    ((ImageView)findViewById(R.id.avatar)).setImageBitmap(decodedByte);
                    System.out.println(">>>> Avatar decoded!");
                }

                // Sets firstname and lastname
                ((TextView)findViewById(R.id.name)).setText(
                        Util.capitalize(mUserProfile.getFirstname()) + " " + Util.capitalize(
                              mUserProfile.getLastname()));
                // Sets email
                ((TextView)findViewById(R.id.email)).setText(Html.fromHtml(getString(R.string.email_profile)));
                ((TextView)findViewById(R.id.email)).append(": "+mUserProfile.getEmail());
                // Last location city
                mUserProfile.setGPSLatitude(-34.795713);
                mUserProfile.setGPSLongitude(-58.348321);
                String location = Util.getGPSLocation(getApplicationContext(), mUserProfile.getGPSLatitude(), mUserProfile.getGPSLongitude());
                //String cityName = addresses.get(0).getAddressLine(0);
                ((TextView)findViewById(R.id.lastLocation)).setText(Html.fromHtml(getString(R.string.last_location)));
                ((TextView)findViewById(R.id.lastLocation)).append(": "+location);
                // Builds bar for quota information
                // Convert quota to KB or MB.
                String used = null;
                String total = null;
                if (Double.parseDouble(mUserProfile.getQuotaUsed())>=Math.pow(2,20))
                    used = Math.round(Double.parseDouble(mUserProfile.getQuotaUsed())/Math.pow(2,20))+" MB";
                else
                    used = Math.round(Double.parseDouble(mUserProfile.getQuotaUsed())/Math.pow(2,10))+" KB";
                if (Double.parseDouble(mUserProfile.getQuotaTotal())>=Math.pow(2,20))
                    total = Math.round(Double.parseDouble(mUserProfile.getQuotaTotal())/Math.pow(2,20))+" MB";
                else
                    total = Math.round(Double.parseDouble(mUserProfile.getQuotaTotal())/Math.pow(2,10))+" KB";
                ((TextView) findViewById(R.id.textProgressBar)).setText(Html.fromHtml(getString(R.string.quota_usage)));
                ((TextView) findViewById(R.id.textProgressBar)).append(": "+used+" ("+mUserProfile.getQuotaUsagePercent()+") "+
                        getString(R.string.quota_of)+" "+ total);
                ProgressBar progressbar = (ProgressBar) findViewById(R.id.pbar1);
                progressbar.setProgress(Integer.parseInt(Util.extractDigits(mUserProfile.getQuotaUsagePercent())));
            }

            @Override
            public void onFailure(String message, int status) {
                Toast.makeText(UserProfileActivity.this, getString(R.string.error_profile), Toast.LENGTH_LONG).show();
            }
        });

    }

    public void changePic(View view){
        Intent photoPickerIntent = new Intent(Intent.ACTION_PICK);
        photoPickerIntent.setType("image/*");
        startActivityForResult(photoPickerIntent, SELECT_PHOTO);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent imageReturnedIntent) {
        super.onActivityResult(requestCode, resultCode, imageReturnedIntent);

        switch(requestCode) {
            case SELECT_PHOTO:
                if(resultCode == RESULT_OK){
                    Uri selectedImage = imageReturnedIntent.getData();
                    InputStream imageStream = null;
                    try {
                        imageStream = getContentResolver().openInputStream(selectedImage);
                    } catch (FileNotFoundException e){
                        // Do something
                    }
                    Bitmap yourSelectedImage = BitmapFactory.decodeStream(imageStream);
                    // Put picture if size is less than 1 MB
                    int size = getPictureSize(yourSelectedImage);
                    System.out.println("Avatar size >>>> "+size);
                    if (size <= 1048576) {
                        ((ImageView) findViewById(R.id.avatar)).setImageBitmap(yourSelectedImage);

                        // Encode to Base64 and Send to the server
                        ByteArrayOutputStream stream = new ByteArrayOutputStream();
                        yourSelectedImage.compress(Bitmap.CompressFormat.JPEG, 100, stream);
                        byte[] byteArray = stream.toByteArray();
                        String strBase64 = Base64.encodeToString(byteArray, Base64.DEFAULT);
                        //System.out.println(strBase64);

                        mUserService.updatePhoto(mUserProfile.getUserId(), new MyPhoto(strBase64), new ServiceCallback<GenericResult>() {
                            @Override
                            public void onSuccess(GenericResult object, int status) {
                                Toast.makeText(UserProfileActivity.this, getString(R.string.picture_ok), Toast.LENGTH_LONG).show();
                            }

                            @Override
                            public void onFailure(String message, int status) {
                                Toast.makeText(UserProfileActivity.this, getString(R.string.picture_error), Toast.LENGTH_LONG).show();
                            }
                        });
                    } else {
                        Toast.makeText(UserProfileActivity.this, getString(R.string.picture_size_error), Toast.LENGTH_LONG).show();
                    }
                }
        }
    }

    public void editName(View view) {
        LayoutInflater inflater = getLayoutInflater();
        final View layout = inflater.inflate(R.layout.edit_fullname_layout, null);
        AlertDialog.Builder builder = new AlertDialog.Builder(this, R.style.AppCompatAlertDialogStyle);
        builder.setView(layout);
        builder.setIcon(R.drawable.ic_pencil_24);
        final EditText firstname = ((EditText)layout.findViewById(R.id.edittext_firstname));
        final EditText lastname = ((EditText)layout.findViewById(R.id.edittext_lastname));
        String fName = Util.capitalize(mUserProfile.getFirstname());
        String lName = Util.capitalize(mUserProfile.getLastname());
        firstname.setText(fName);
        lastname.setText(lName);


        builder.setCancelable(false)
                .setTitle(getString(R.string.screen_name))
                .setView(layout)
                .setPositiveButton(getString(R.string.save_changes), new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        mUserService.updateFullName(mUserProfile.getUserId(), new UserFullName(firstname.getText().toString(),
                                lastname.getText().toString()), new ServiceCallback<GenericResult>() {
                            @Override
                            public void onSuccess(GenericResult object, int status) {
                                if (object.getResultCode() != 1) {
                                    Toast.makeText(UserProfileActivity.this, getString(R.string.fullname_error), Toast.LENGTH_LONG).show();
                                } else {
                                    // If Request OK:
                                    mUserProfile.setFirstname(firstname.getText().toString());
                                    mUserProfile.setLastname(lastname.getText().toString());
                                    ((TextView) findViewById(R.id.name)).setText(
                                            Util.capitalize(mUserProfile.getFirstname()) + " " + Util.capitalize(
                                                    mUserProfile.getLastname()));
                                    Toast.makeText(UserProfileActivity.this, getString(R.string.fullname_ok), Toast.LENGTH_LONG).show();
                                }
                            }

                            @Override
                            public void onFailure(String message, int status) {
                                Toast.makeText(UserProfileActivity.this, getString(R.string.fullname_error), Toast.LENGTH_LONG).show();
                            }
                        });

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

    private int getPictureSize(Bitmap bitmap){
        int pixels = bitmap.getHeight() * bitmap.getWidth();
        int bytesPerPixel = 0;
        switch(bitmap.getConfig()) {
            case ARGB_8888:
                bytesPerPixel = 4;
                break;
            case RGB_565:
                bytesPerPixel = 2;
                break;
            case ARGB_4444:
                bytesPerPixel = 2;
                break;
            case ALPHA_8 :
                bytesPerPixel = 1;
            break;
        }
        int byteCount = pixels / bytesPerPixel;
        return byteCount;
    }


}
