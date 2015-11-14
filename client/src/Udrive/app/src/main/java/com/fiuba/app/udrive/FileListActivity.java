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
import com.fiuba.app.udrive.view.FileContextMenu;
import com.fiuba.app.udrive.view.FileContextMenuManager;
import com.nononsenseapps.filepicker.FilePickerActivity;

import java.io.FileOutputStream;
import java.util.ArrayList;
import java.util.List;

public class FileListActivity extends AppCompatActivity implements FilesArrayAdapter.OnContextMenuClickListener, AdapterView.OnItemClickListener, FileContextMenu.OnFileContextMenuItemClickListener {

    public static final String TAG = "FileListActivity";

    private List<File> mFiles = new ArrayList<File>();

    private FilesArrayAdapter mFilesAdapter;

    private ProgressDialog mProgressDialog;

    private FilesService mFilesService;

    private UserService mUserService = null;

    private UserAccount mUserAccount;

    private Integer mDirId;

    private Integer mFileId;

    private File selectedFileForDownload;

    public static final int FILE_CODE = 1;

    public static final int DIR_CODE = 2;

    public static final String EXTRA_USER_ACCOUNT = "userAccount";

    public static final String EXTRA_DIR_ID = "dirId";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_file_list);
        mUserAccount = (UserAccount) getIntent().getSerializableExtra(EXTRA_USER_ACCOUNT);
        mDirId = (Integer) getIntent().getSerializableExtra(EXTRA_DIR_ID);
        Log.d(TAG, "TOKEN: " + mUserAccount.getToken());
        mFilesAdapter = new FilesArrayAdapter(this, R.layout.file_list_item, mFiles, this);
        ListView list = (ListView)findViewById(R.id.fileListView);
        list.setAdapter(mFilesAdapter);
        list.setOnItemClickListener(this);
        mFilesService = new FilesService(mUserAccount.getToken(), FileListActivity.this);
        mUserService = new UserService(mUserAccount.getToken(), FileListActivity.this);
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
            /*UserProfile uProfile = new UserProfile(mUserAccount.getEmail(),mUserAccount.getPassword(), "firstname",
                    "lastname", "", -34.795713, -58.348321, mUserAccount.getUserId(),
                    "750 MB", "487.5", "35%");
            Intent profile = new Intent(FileListActivity.this, UserProfileActivity.class);
            profile.putExtra("userProfile", uProfile);
            profile.putExtra("userAccount", mUserAccount);
            startActivity(profile);*/

            mUserService.getProfile(mUserAccount.getUserId(), new ServiceCallback<UserProfile>() {
                @Override
                public void onSuccess(UserProfile uProfile, int status) {
                    Intent profile = new Intent(FileListActivity.this, UserProfileActivity.class);
                    /*UserProfile uProfile = new UserProfile(mUserAccount.getEmail(),
                            mUserAccount.getPassword(), "firstname",
                    "lastname", "photo", "lastLocation", mUserAccount.getUserId(),
                    "750 MB", "487.5", "35%");*/

                    profile.putExtra("userProfile", uProfile);
                    profile.putExtra("userAccount", mUserAccount);
                    startActivity(profile);
                }

                @Override
                public void onFailure(String message, int status) {
                    Toast.makeText(FileListActivity.this, getString(R.string.error_profile), Toast.LENGTH_LONG).show();
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
            String ok_option = getString(R.string.alert_ok);
            String cancel_option = getString(R.string.alert_cancel);

            // set dialog message
            alertDialogBuilder.setCancelable(false).setPositiveButton(ok_option, new DialogInterface.OnClickListener() {
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
            alertDialogBuilder.setNegativeButton(cancel_option, new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int id) {
                    dialog.cancel();
                }
            });

            // create alert dialog
            AlertDialog alertDialog = alertDialogBuilder.create();

            // show it
            alertDialog.show();

        } else if (id == R.id.action_show_trash) {
            Intent i = new Intent(this, TrashActivity.class);
            i.putExtra(TrashActivity.EXTRA_USER_ACCOUNT, mUserAccount);
            startActivity(i);
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
            uploadSelectedFile(data);
        } else if (requestCode == DIR_CODE && resultCode == Activity.RESULT_OK) {
            downloadFileIntoSelectedDir(data);
        }
    }

    private void downloadFileIntoSelectedDir(Intent data) {
        final File selectedFileForDownload = getSelectedFileForDownload();
        if (selectedFileForDownload != null) {
            final String fullPath = data.getData().getPath() + "/" + selectedFileForDownload.getName();

            ServiceCallback<FileOutputStream> callback = new ServiceCallback<FileOutputStream>() {
                @Override
                public void onSuccess(FileOutputStream outputStream, int status) {
                    Log.i(TAG, "File downloaded to " + fullPath);
                    setSelectedFileForDownload(null);
                }

                @Override
                public void onFailure(String message, int status) {
                    Log.e(TAG, message);
                    setSelectedFileForDownload(null);
                }
            };

            if (selectedFileForDownload.isFile()) {
                mFilesService.downloadFile(mUserAccount.getUserId(), selectedFileForDownload.getId(), fullPath, callback);
            } else {
                mFilesService.downloadDir(mUserAccount.getUserId(), selectedFileForDownload.getId(), fullPath, callback);
            }
        }
    }

    private void uploadSelectedFile(Intent data) {
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

    @Override
    public void onDownloadClick(int FileItem) {
        FileContextMenuManager.getInstance().hideContextMenu();
        setSelectedFileForDownload(mFiles.get(FileItem));
        Intent i = new Intent(this, FilePickerActivity.class);
        i.putExtra(FilePickerActivity.EXTRA_ALLOW_MULTIPLE, false);
        i.putExtra(FilePickerActivity.EXTRA_ALLOW_CREATE_DIR, false);
        i.putExtra(FilePickerActivity.EXTRA_MODE, FilePickerActivity.MODE_DIR);
        i.putExtra(FilePickerActivity.EXTRA_START_PATH, Environment.getExternalStorageDirectory().getPath());

        startActivityForResult(i, DIR_CODE);
    }

    @Override
    public void onInformationClick(int FileItem) {
        Log.i(TAG, "Information File position " + FileItem);}

    @Override
    public void onShareClick(int FileItem) {
        Log.i(TAG,"Share File position "+FileItem);
    }

    @Override
    public void onTagClick(int FileItem) {
        Log.i(TAG,"Tag File position "+FileItem);
    }

    @Override
    public void onDeleteClick(int FileItem) {
        FileContextMenuManager.getInstance().hideContextMenu();
        AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(this);
        String title = getString(R.string.delete_option_title);
        String message = getString(R.string.delete_option_message);
        String ok_option = getString(R.string.alert_ok);
        String cancel_option = getString(R.string.alert_cancel);
        alertDialogBuilder.setTitle(title).setMessage(message);
        mFileId = mFiles.get(FileItem).getId();

        alertDialogBuilder.setCancelable(false).setPositiveButton(ok_option, new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog,int id) {
                mFilesService.deleteFile(mUserAccount.getUserId(), mFileId, new ServiceCallback<List<File>>() {
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
        alertDialogBuilder.setNegativeButton(cancel_option, new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int id) {
                dialog.cancel();
            }
        });

        // create alert dialog
        AlertDialog alertDialog = alertDialogBuilder.create();

        // show it
        alertDialog.show();
    }

    @Override
    public void onCancelClick(int FileItem) {

    }

    @Override
    public void onClick(View v, int position) {
        FileContextMenuManager.getInstance().toggleContextMenuFromView(v,position,this);
    }


    public File getSelectedFileForDownload() {
        return selectedFileForDownload;
    }

    public void setSelectedFileForDownload(File selectedFileForDownload) {
        this.selectedFileForDownload = selectedFileForDownload;
    }

}