package com.fiuba.app.udrive;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.PorterDuff;
import android.net.Uri;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.Editable;
import android.text.InputType;
import android.text.TextWatcher;
import android.text.method.DigitsKeyListener;
import android.view.Gravity;
import android.view.View;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import com.fiuba.app.udrive.model.UserProfile;
import com.fiuba.app.udrive.model.Util;
import com.fiuba.app.udrive.network.ConnectionConfig;
import com.squareup.picasso.Picasso;

import java.io.FileNotFoundException;
import java.io.InputStream;

public class UserProfileActivity extends AppCompatActivity {
    private UserProfile mUserProfile = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_user_profile);
        mUserProfile = (UserProfile)getIntent().getSerializableExtra("userProfile");
        ((TextView)findViewById(R.id.name)).setText(
                Util.capitalize(mUserProfile.getFirstname()) + " " + Util.capitalize(
                        mUserProfile.getLastname()));

        ProgressBar progressbar = (ProgressBar) findViewById(R.id.pbar1);
        progressbar.setProgress(Integer.parseInt(Util.extractDigits(mUserProfile.getQuotaUsagePercent())));
    }

    public void changePic(View view){
       // Picasso.with(this).load(R.drawable.user2).into((ImageView)findViewById(R.id.avatar));
        Intent photoPickerIntent = new Intent(Intent.ACTION_PICK);
        photoPickerIntent.setType("image/*");
        // SELECT_PHOTO = 100
        startActivityForResult(photoPickerIntent, 100);
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
        firstname.setText(fName); // get from Extra
        lastname.setText(lName); // get from Extra
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
