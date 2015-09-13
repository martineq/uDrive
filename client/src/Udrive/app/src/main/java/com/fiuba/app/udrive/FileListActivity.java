package com.fiuba.app.udrive;

import android.app.ListActivity;
import android.app.ProgressDialog;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;

import com.fiuba.app.udrive.model.File;

import java.util.ArrayList;
import java.util.Date;
import java.util.List;

public class FileListActivity extends ListActivity{

    public static final String TAG = "FileListActivity";

    private List<File> files;

    private FilesArrayAdapter filesAdapter;

    private ProgressDialog progressDialog;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_file_list);
        loadFiles();
        this.filesAdapter = new FilesArrayAdapter(this, R.layout.file_list_item, this.files);
        this.setListAdapter(filesAdapter);

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

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }


    private void loadFiles(){
        this.files = new ArrayList<File>();
        for (int i = 0; i< 3 ; i++){
            String name = "ARCHIVO "+i;
            Date today = new Date();
            File file = new File(name,100,'F',false,today,2);
            this.files.add(file);
        }

    }
}
