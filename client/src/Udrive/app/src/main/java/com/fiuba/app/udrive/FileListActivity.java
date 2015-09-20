package com.fiuba.app.udrive;

import android.app.ListActivity;
import android.app.ProgressDialog;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.ListView;

import com.fiuba.app.udrive.model.File;

import java.util.ArrayList;
import java.util.Date;
import java.util.List;

public class FileListActivity extends AppCompatActivity {

    public static final String TAG = "FileListActivity";

    private List<File> files = new ArrayList<File>();

    private FilesArrayAdapter filesAdapter;

    private ProgressDialog progressDialog;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_file_list);
        this.filesAdapter = new FilesArrayAdapter(this, R.layout.file_list_item, this.files);
        ListView list = (ListView)findViewById(R.id.fileListView);
        list.setAdapter(filesAdapter);
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

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }


    private void loadFiles(){
        String name = "ARCHIVO 1";
        Date today = new Date();
        File file = new File(name,100,'d',false,today,2);
        this.files.add(file);

        name = "ARCHIVO 2";
        today = new Date();
        file = new File(name,100,'d',true,today,2);
        this.files.add(file);

        name = "ARCHIVO 3";
        today = new Date();
        file = new File(name,100,'f',true,today,2);
        this.files.add(file);

        filesAdapter.notifyDataSetChanged();
    }
}
