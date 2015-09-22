package com.fiuba.app.udrive;

import android.app.ProgressDialog;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.ListView;
import android.widget.Toast;

import com.fiuba.app.udrive.model.File;
import com.fiuba.app.udrive.model.UserAccount;
import com.fiuba.app.udrive.network.FilesService;
import com.fiuba.app.udrive.network.ServiceCallback;

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
        System.out.println("Desde FileListActivity >>> TOKEN: "+uAccount.getToken());

        this.mFilesAdapter = new FilesArrayAdapter(this, R.layout.file_list_item, this.mFiles);
        ListView list = (ListView)findViewById(R.id.fileListView);
        list.setAdapter(mFilesAdapter);
        this.mFilesService = new FilesService();
        loadFiles();
    }

    private void loadFiles(){
        final ProgressDialog progressDialog = ProgressDialog.show(this, null, getString(R.string.loading), true);
        progressDialog.setCancelable(false);

        mFilesService.getFiles(new ServiceCallback<List<File>>() {
            @Override
            public void onSuccess(List<File> files) {
                mFilesAdapter.updateFiles(files);
                progressDialog.dismiss();
            }

            @Override
            public void onFailure(String message) {
                Toast.makeText(FileListActivity.this, R.string.error_obtaining_files, Toast.LENGTH_LONG).show();
                progressDialog.dismiss();
            }
        });
    }
}
