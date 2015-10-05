package com.fiuba.app.udrive;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.ListView;
import android.widget.Toast;

import com.fiuba.app.udrive.model.File;
import com.fiuba.app.udrive.model.ObjectStream;
import com.fiuba.app.udrive.model.UserAccount;
import com.fiuba.app.udrive.network.FilesService;
import com.fiuba.app.udrive.network.ServiceCallback;
import com.fiuba.app.udrive.network.StatusCode;

import java.util.ArrayList;
import java.util.List;

public class FileListActivity extends AppCompatActivity {

    public static final String TAG = "FileListActivity";

    private List<File> mFiles = new ArrayList<File>();

    private FilesArrayAdapter mFilesAdapter;

    private ProgressDialog mProgressDialog;

    private FilesService mFilesService;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_file_list);
        UserAccount uAccount = (UserAccount) getIntent().getSerializableExtra("userAccount");
        Log.d(TAG, "TOKEN: " + uAccount.getToken());
        this.mFilesAdapter = new FilesArrayAdapter(this, R.layout.file_list_item, this.mFiles);
        ListView list = (ListView)findViewById(R.id.fileListView);
        list.setAdapter(mFilesAdapter);
        this.mFilesService = new FilesService(uAccount.getToken());
        loadFiles();
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
            Intent main = new Intent(FileListActivity.this, MainActivity.class);
            startActivity(main);
            finish();
        }

        return super.onOptionsItemSelected(item);
    }

    private void loadFiles(){
        final ProgressDialog progressDialog = ProgressDialog.show(this, null, getString(R.string.loading), true);
        progressDialog.setCancelable(false);

        mFilesService.getFiles(new ServiceCallback<List<File>>() {
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
}
