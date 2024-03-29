package com.fiuba.app.udrive;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.ListView;
import android.widget.Toast;
import com.fiuba.app.udrive.model.Collaborator;
import com.fiuba.app.udrive.model.UserAccount;
import com.fiuba.app.udrive.network.FilesService;
import com.fiuba.app.udrive.network.ServiceCallback;
import com.fiuba.app.udrive.network.StatusCode;
import java.util.ArrayList;
import java.util.List;

public class ShareActivity extends AppCompatActivity {

    private static final String TAG = "ShareActivity";
    public static final String EXTRA_USER_ACCOUNT = "userAccount";
    public static final String EXTRA_FILE_ID = "fileId";
    public static final String EXTRA_FILE_OWNER_ID = "ownerFileId";
    public static final String EXTRA_DIR_ID = "dirId";
    public static final int USER_CODE = 1;

    private ListView mUsersListView;
    private UserAccount mUserAccount;
    private Integer mFileId;
    private Integer mFileOwnerId;
    private Integer mDirId;
    private FilesService mFileService;
    private CollaboratorsArrayAdapter mCollaboratorsAdapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_users_list);
        mUserAccount = (UserAccount) getIntent().getSerializableExtra(EXTRA_USER_ACCOUNT);
        mFileId = (Integer) getIntent().getIntExtra(EXTRA_FILE_ID, -1);
        mFileOwnerId = (Integer) getIntent().getIntExtra(EXTRA_FILE_OWNER_ID, -1);
        mDirId = (Integer) getIntent().getIntExtra(EXTRA_DIR_ID, -1);
        mUsersListView = (ListView) findViewById(R.id.usersListView);
        mUsersListView.setEmptyView(findViewById(R.id.emptyCollaboratorsTextView));
        mUsersListView.setChoiceMode(ListView.CHOICE_MODE_MULTIPLE_MODAL);
        mUsersListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                if (mCollaboratorsAdapter.isChecked(position) && (!mFileOwnerId.equals(mUserAccount.getUserId()))){
                    String message = getString(R.string.message_validation_no_canShare);
                    Toast.makeText(ShareActivity.this,message, Toast.LENGTH_LONG).show();

                }
                else {
                    mCollaboratorsAdapter.toggleChecked(position);
                }
            }
        });
        mCollaboratorsAdapter = new CollaboratorsArrayAdapter(this);
        mUsersListView.setAdapter(mCollaboratorsAdapter);
        Button btnAddCollaborator = (Button)findViewById(R.id.userListBtnAdd);
        btnAddCollaborator.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent searchUserIntent = new Intent(ShareActivity.this, SearchUsersActivity.class);
                searchUserIntent.putExtra(SearchUsersActivity.EXTRA_USER_ACCOUNT, mUserAccount);
                startActivityForResult(searchUserIntent, USER_CODE);
            }
        });
        mFileService = new FilesService(mUserAccount.getToken(), this);
        loadCollaborators();
    }

    private void loadCollaborators() {
        Log.d(TAG, "Loading collaborators ");
        final ProgressDialog progressDialog = ProgressDialog.show(this, null, getString(R.string.loading), true);
        progressDialog.setCancelable(false);

       mFileService.getAllCollaborators(mUserAccount.getUserId(), mFileId, new ServiceCallback<List<Collaborator>>() {
           @Override
           public void onSuccess(List<Collaborator> collaborators, int status) {
               setCollaborators(collaborators);
               progressDialog.dismiss();
           }

           @Override
           public void onFailure(String message, int status) {
               showErrorMessage(message, status);
               progressDialog.dismiss();
           }
       });

    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
       getMenuInflater().inflate(R.menu.menu_share, menu);
       menu.findItem(R.id.action_add_collaborators).setVisible(true);

        return true;
    }

    private void showErrorMessage(String message, int status) {
        if (StatusCode.isHumanReadable(status)) {
            message = StatusCode.getMessage(ShareActivity.this, status);
            Toast.makeText(ShareActivity.this, message, Toast.LENGTH_LONG).show();
        }
        Log.e(TAG, message);
    }

    private void setCollaborators(List<Collaborator> collaborators) {
        mCollaboratorsAdapter.updateAndCheckCollaborators(collaborators);
    }

    private void addCollaborator(Collaborator collaborator){
        if(!mCollaboratorsAdapter.getCollaborators().contains(collaborator)){
            Log.i(TAG, "Add collaborator:  " + collaborator.getEmail());
            this.mCollaboratorsAdapter.addAndCheckCollaborator(collaborator);
        }
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();

        // Action of add collaborators
        if (id == R.id.action_add_collaborators) {
            Log.i(TAG, "Update collaborators ");
            Log.d(TAG, "Update collaborators fileId: " +mFileId);
            mFileService.updateCollaborators(mUserAccount.getUserId(), mFileId, mCollaboratorsAdapter.getCheckedItems(), new ServiceCallback<List<Collaborator>>() {
                @Override
                public void onSuccess(List<Collaborator> collaborators, int status) {
                    mCollaboratorsAdapter.updateAndCheckCollaborators(collaborators);
                    Log.d(TAG, "Number of collaborators received " + collaborators.size());
                    finish();
                }

                @Override
                public void onFailure(String message, int status) {
                    if (StatusCode.isHumanReadable(status)) {
                        message = StatusCode.getMessage(ShareActivity.this, status);
                        Toast.makeText(ShareActivity.this, message, Toast.LENGTH_LONG).show();
                    }
                    Log.e(TAG, message);
                }
            });
        }

        return super.onOptionsItemSelected(item);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {

        if (requestCode == USER_CODE && resultCode == Activity.RESULT_OK) {
           Collaborator collaboratorSelected = (Collaborator) data.getSerializableExtra(SearchUsersActivity.EXTRA_SELECTED_USER);
           addCollaborator(collaboratorSelected);
           //invalidateOptionsMenu();
        }
    }

}