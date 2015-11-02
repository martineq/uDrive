package com.fiuba.app.udrive;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.Toast;

import com.fiuba.app.udrive.model.File;
import com.fiuba.app.udrive.model.ObjectStream;
import com.fiuba.app.udrive.model.UserAccount;
import com.fiuba.app.udrive.model.UserProfile;
import com.fiuba.app.udrive.network.FilesService;
import com.fiuba.app.udrive.network.ServiceCallback;
import com.fiuba.app.udrive.network.StatusCode;
import com.fiuba.app.udrive.network.UserService;
import com.nononsenseapps.filepicker.FilePickerActivity;

import java.util.ArrayList;
import java.util.List;

public class FileListActivity extends AppCompatActivity implements AdapterView.OnItemClickListener {

    public static final String TAG = "FileListActivity";

    private List<File> mFiles = new ArrayList<File>();

    private FilesArrayAdapter mFilesAdapter;

    private ProgressDialog mProgressDialog;

    private FilesService mFilesService;

    private UserService mUserService = null;

    private UserAccount mUserAccount;

    private Integer mDirId;


    public static final int FILE_CODE = 1;

    public static final String EXTRA_USER_ACCOUNT = "userAccount";

    public static final String EXTRA_DIR_ID = "dirId";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_file_list);
        mUserAccount = (UserAccount) getIntent().getSerializableExtra(EXTRA_USER_ACCOUNT);
        mDirId = (Integer) getIntent().getSerializableExtra(EXTRA_DIR_ID);
        Log.d(TAG, "TOKEN: " + mUserAccount.getToken());
        mFilesAdapter = new FilesArrayAdapter(this, R.layout.file_list_item, mFiles);
        ListView list = (ListView)findViewById(R.id.fileListView);
        list.setAdapter(mFilesAdapter);
        list.setOnItemClickListener(this);
        mFilesService = new FilesService(mUserAccount.getToken(), FileListActivity.this);
        //mUserService = new UserService(mUserAccount.getToken(), FileListActivity.this);
        System.out.println("idDir: "+mDirId);
        if (mDirId == null)
           mDirId = 0;
        loadFiles(mUserAccount.getUserId(), mDirId); // Change 0 to the corresponding dirId
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_file_list, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        // Action of signing out
        if (id == R.id.action_signout) {
            ObjectStream<UserAccount> os = new ObjectStream<>(MainActivity.getAccountFilename(), FileListActivity.this);
            os.delete();
            Intent main = new Intent(getApplicationContext(), MainActivity.class);
            main.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
            startActivity(main);
            finishAffinity();

        }  else if (id == R.id.action_profile) {
            UserProfile uProfile = new UserProfile(mUserAccount.getEmail(),
                            mUserAccount.getPassword(), "firstname",
                    "lastname", "photo", "lastLocation", mUserAccount.getUserId(),
                    "750 MB", "487.5", "35%");
            Intent profile = new Intent(FileListActivity.this, UserProfileActivity.class);
            profile.putExtra("userProfile", uProfile);
            startActivity(profile);
            mUserService.getProfile(mUserAccount.getUserId(), new ServiceCallback<UserProfile>() {
                @Override
                public void onSuccess(UserProfile uProfile, int status) {
                    Intent profile = new Intent(FileListActivity.this, UserProfileActivity.class);
                    /*UserProfile uProfile = new UserProfile(mUserAccount.getEmail(),
                            mUserAccount.getPassword(), "firstname",
                    "lastname", "photo", "lastLocation", mUserAccount.getUserId(),
                    "750 MB", "487.5", "35%");*/

                    profile.putExtra("userProfile", uProfile);
                    startActivity(profile);
                }

                @Override
                public void onFailure(String message, int status) {

                }
            });



        }  else if (id == R.id.action_upload_file) {
            Intent i = new Intent(this, FilePickerActivity.class);
            i.putExtra(FilePickerActivity.EXTRA_ALLOW_MULTIPLE, false);
            i.putExtra(FilePickerActivity.EXTRA_ALLOW_CREATE_DIR, false);
            i.putExtra(FilePickerActivity.EXTRA_MODE, FilePickerActivity.MODE_FILE);
            i.putExtra(FilePickerActivity.EXTRA_START_PATH, Environment.getExternalStorageDirectory().getPath());

            startActivityForResult(i, FILE_CODE);

        } else if (id == R.id.action_add_folder) {
            LayoutInflater li = LayoutInflater.from(this);
            View newFolderView = li.inflate(R.layout.new_folder, null);

            AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(this);
            alertDialogBuilder.setView(newFolderView);

            final EditText userInput = (EditText) newFolderView.findViewById(R.id.editTextDialogUserInput);

            // set dialog message
            alertDialogBuilder.setCancelable(false).setPositiveButton("OK", new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog,int id) {
                    // get user input and set it to result
                    // edit text
                    String nameFolder = userInput.getText().toString();
                    mFilesService.addFolder(mUserAccount.getUserId(), mDirId, nameFolder, new ServiceCallback<List<File>>() {
                        @Override
                        public void onSuccess(List<File> files, int status) {
                            mFilesAdapter.updateFiles(files);
                            Log.d(TAG, "Number of files received " + files.size());
                        }

                        @Override
                        public void onFailure(String message, int status) {
                            if (StatusCode.isHumanReadable(status)) {
                                message = StatusCode.getMessage(FileListActivity.this, status);
                                Toast.makeText(FileListActivity.this, message, Toast.LENGTH_LONG).show();
                            }
                            Log.e(TAG, message);
                        }
                    });

                }
            });
            alertDialogBuilder.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int id) {
                    dialog.cancel();
                }
            });

            // create alert dialog
            AlertDialog alertDialog = alertDialogBuilder.create();

            // show it
            alertDialog.show();

        }

        return super.onOptionsItemSelected(item);
    }

    private void loadFiles(int userId, int dirId){
        final ProgressDialog progressDialog = ProgressDialog.show(this, null, getString(R.string.loading), true);
        progressDialog.setCancelable(false);

        mFilesService.getFiles(userId, dirId, new ServiceCallback<List<File>>() {
            @Override
            public void onSuccess(List<File> files, int status) {
                mFilesAdapter.updateFiles(files);
                progressDialog.dismiss();
                Log.d(TAG, "Number of files received " + files.size());
            }

            @Override
            public void onFailure(String message, int status) {
                if (StatusCode.isHumanReadable(status)){
                   message = StatusCode.getMessage(FileListActivity.this, status);
                   Toast.makeText(FileListActivity.this, message, Toast.LENGTH_LONG).show();
                }
                progressDialog.dismiss();
                Log.e(TAG, message);
            }
        });
    }

    @Override
    public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
        Log.d(TAG, "Click--> position " + position);
        File actualFile = mFiles.get(position);
        if (actualFile.isDir()){
            Intent mNextIntent = new Intent(this, FileListActivity.class);
            mNextIntent.putExtra(this.EXTRA_USER_ACCOUNT, mUserAccount);
            mNextIntent.putExtra(this.EXTRA_DIR_ID, actualFile.getId());
            startActivity(mNextIntent);
        }

    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {

        if (requestCode == FILE_CODE && resultCode == Activity.RESULT_OK) {

            Uri uri = data.getData();
            //Toast.makeText(this, uri.getPath(), Toast.LENGTH_LONG).show();
            mFilesService.upload(mUserAccount.getUserId(), mDirId, uri.getPath(), new ServiceCallback<List<File>>() {
                @Override
                public void onSuccess(List<File> files, int status) {
                    mFilesAdapter.updateFiles(files);
                    Log.d(TAG, "Number of files received " + files.size());
                }

                @Override
                public void onFailure(String message, int status) {
                    if (StatusCode.isHumanReadable(status)) {
                        message = StatusCode.getMessage(FileListActivity.this, status);
                        Toast.makeText(FileListActivity.this, message, Toast.LENGTH_LONG).show();
                    }
                    Log.e(TAG, message);
                }
            });
        }
    }
}
