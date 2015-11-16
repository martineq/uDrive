package com.fiuba.app.udrive;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.webkit.WebView;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.ListAdapter;
import android.widget.ListView;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.Toast;

import com.fiuba.app.udrive.model.File;
import com.fiuba.app.udrive.model.FileInfo;
import com.fiuba.app.udrive.model.FolderData;
import com.fiuba.app.udrive.model.GenericResult;
import com.fiuba.app.udrive.model.Tag;
import com.fiuba.app.udrive.model.UserBasicData;
import com.fiuba.app.udrive.model.Util;
import com.fiuba.app.udrive.network.FileMetadataService;
import com.fiuba.app.udrive.network.ServiceCallback;

import java.util.ArrayList;


public class FileInfoActivity extends AppCompatActivity {
    public static final String FILE_INFO = "fileInfo";
    private FileInfo mFileInfo = null;
    private FileMetadataService mFileMetadataService = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mFileMetadataService = new FileMetadataService(((String)getIntent().getSerializableExtra("token"))
            ,FileInfoActivity.this);
        setContentView(R.layout.activity_file_info);
        ((ScrollView)findViewById(R.id.scroll_layout)).smoothScrollTo(0,0);
        //mFileInfo = (FileInfo) getIntent().getSerializableExtra(FILE_INFO);

        /** TODO: delete this fragment after loading file info from HTTP response **/
        UserBasicData owner = new UserBasicData("Owner", "User", "owner@owner.com");

        File file = new File("Image.jpg", 565421,'a', false, "21/08/2015 08:53", null,null);
        file.setId(1);
        UserBasicData updatedBy = owner;
        double updatedFromLatitude = -34.795713;
        double updatedFromLongitude = -58.348321;
        ArrayList<UserBasicData> collaborators = new ArrayList<>();
        collaborators.add(new UserBasicData("user1", "user1", "user1@lala.com"));
        collaborators.add(new UserBasicData("AnotherUser", "AnotherUser", "anotheruser@lolo.com"));
        collaborators.add(new UserBasicData("Bono", "Vox", "blabla@blabla.com"));
        collaborators.add(new UserBasicData("Sharon", "Den Adel", "xxxxx@yyyy.com"));
        ArrayList<Tag> tags = new ArrayList<>();
        tags.add(new Tag("tag1"));
        tags.add(new Tag("tag2"));
        tags.add(new Tag("tag3"));
        tags.add(new Tag("tag1"));
        tags.add(new Tag("tag2"));
        tags.add(new Tag("tag3"));
        tags.add(new Tag("tag1"));
        tags.add(new Tag("tag2"));
        tags.add(new Tag("tag3"));
        tags.add(new Tag("tag1"));
        tags.add(new Tag("tag2"));
        tags.add(new Tag("tag3"));
        tags.add(new Tag("tag2"));

        mFileInfo = new FileInfo(owner, file, updatedBy, updatedFromLatitude, updatedFromLongitude,
                collaborators, tags);
        /******/
        ListView access = (ListView)findViewById(R.id.list_access);
        // who has access
        ArrayList<String> names = new ArrayList<>();
        ArrayList<String> mails = new ArrayList<>();
        int i;
        ArrayList<UserBasicData> coll = mFileInfo.getCollaborators();
        for (i = 0; i < coll.size(); i++){
            names.add(coll.get(i).getFirstname()+" "+coll.get(i).getLastname());
            mails.add(coll.get(i).getEmail());
        }
        ArrayAdapter<String> adapter = new CollaboratorsListAdapter(this, names, mails);
        access.setAdapter(adapter);
        setListViewHeightBasedOnItems(access);

        // filename and icon
        ((TextView) findViewById(R.id.tv_filename)).setText(mFileInfo.getFile().getName());
        if (!mFileInfo.getFile().isDir()) {
            if (mFileInfo.getFile().getShared())
                ((ImageView) findViewById(R.id.filename_icon)).setImageResource(R.drawable.ic_file_cloud);
            else
                ((ImageView) findViewById(R.id.filename_icon)).setImageResource(R.drawable.ic_file);
        } else {
            if (mFileInfo.getFile().getShared())
                ((ImageView) findViewById(R.id.filename_icon)).setImageResource(R.drawable.ic_folder_account);
        }

        // tags
        WebView panel = ((WebView)findViewById(R.id.tags));
        panel.loadData(getPanelHTML(tags), "text/html", "utf-8");
        panel.setBackgroundColor(0);

        // labels
        ((TextView)findViewById(R.id.label_owner)).setText(mFileInfo.getOwner().getFirstname()+" "+
                mFileInfo.getOwner().getLastname());
        // Convert size to KB or MB
        int size = mFileInfo.getFile().getSize();
        String totalSize = "";
        if ( size >= Math.pow(2,20)){
            size = (int)(size/Math.pow(2,20));
            totalSize = size+" MB";
        } else {
            size = (int)(size/Math.pow(2,10));
            totalSize = size+" KB";
        }
        ((TextView)findViewById(R.id.label_size)).setText(totalSize);
        ((TextView)findViewById(R.id.label_update)).setText(mFileInfo.getFile().getLastModDateFormated());
        ((TextView)findViewById(R.id.label_by)).setText(mFileInfo.getUpdatedBy().getFirstname()+" "+mFileInfo.getUpdatedBy().getLastname());
        String location = Util.getGPSLocation(this, mFileInfo.getUpdatedFromLatitude(), mFileInfo.getUpdatedFromLongitude());
        ((TextView)findViewById(R.id.label_from)).setText(location);
        String numItems = " -- ";
        if (mFileInfo.getFile().isDir()){
            numItems = mFileInfo.getFile().getCantItems().toString();
        }
        ((TextView)findViewById(R.id.label_items)).setText(numItems);
    }


    private String getPanelHTML(ArrayList<Tag> tagList){
        String html = "<html>" +
                "   <body style=\"text-align:center;\">";
        int index;
        String tagCSS = "style=\"font-size:11pt; padding:4px; border:1px solid black; background-color:#f19c47" +
                "; border-radius:2px; font-family:monospace; margin-top:4px; margin-bottom:4px; display:inline-block; " +
                "text-align:center\"";
        for (index = 0; index < tagList.size(); index++) {
            html = html + "<span " + tagCSS + ">" +
                    tagList.get(index).getTagName() + "</span>  ";
        }
        html = html + "</body></html>";
        return html;
    }

    /**
     * Sets ListView height dynamically based on the height of the items.
     *
     * @param listView to be resized
     * @return true if the listView is successfully resized, false otherwise
     */
    public static boolean setListViewHeightBasedOnItems(ListView listView) {

        ListAdapter listAdapter = listView.getAdapter();
        if (listAdapter != null) {

            int numberOfItems = listAdapter.getCount();

            // Get total height of all items.
            int totalItemsHeight = 0;
            for (int itemPos = 0; itemPos < numberOfItems; itemPos++) {
                View item = listAdapter.getView(itemPos, null, listView);
                item.measure(0, 0);
                totalItemsHeight += item.getMeasuredHeight();
            }

            // Get total height of all item dividers.
            int totalDividersHeight = listView.getDividerHeight() *
                    (numberOfItems - 1);

            // Set list height.
            ViewGroup.LayoutParams params = listView.getLayoutParams();
            params.height = totalItemsHeight + totalDividersHeight;
            listView.setLayoutParams(params);
            listView.requestLayout();

            return true;

        } else {
            return false;
        }

    }

    public void editFileName(View view) {
        LayoutInflater inflater = getLayoutInflater();
        final View layout = inflater.inflate(R.layout.edit_file_name, null);
        android.app.AlertDialog.Builder builder = new android.app.AlertDialog.Builder(FileInfoActivity.this);
        builder.setView(layout);
        builder.setIcon(R.drawable.ic_pencil_24);
        final EditText filename = ((EditText)layout.findViewById(R.id.edit_filename));
        String name = Util.capitalize(mFileInfo.getFile().getName());
        filename.setText(name);

        builder.setCancelable(false)
                .setTitle(getString(R.string.edit_filename_title))
                .setView(layout)
                .setPositiveButton(getString(R.string.save_changes), new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        mFileMetadataService.updateFilename((mFileInfo.getFile().getId()), new FolderData(filename.getText().toString()),
                                new ServiceCallback<GenericResult>() {
                                    @Override
                                    public void onSuccess(GenericResult object, int status) {
                                        if (object.getResultCode() != 1) {
                                            Toast.makeText(FileInfoActivity.this, getString(R.string.error_filename), Toast.LENGTH_LONG).show();
                                        } else {
                                            // If Request OK:
                                            mFileInfo.getFile().setName(filename.getText().toString());
                                            ((TextView) findViewById(R.id.tv_filename)).setText(mFileInfo.getFile().getName());
                                        }
                                    }

                                    @Override
                                    public void onFailure(String message, int status) {
                                        Toast.makeText(FileInfoActivity.this, getString(R.string.error_filename), Toast.LENGTH_LONG).show();
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

}
