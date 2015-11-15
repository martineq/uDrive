package com.fiuba.app.udrive;

import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.Toast;

import com.fiuba.app.udrive.model.Collaborator;
import com.fiuba.app.udrive.model.UserAccount;
import com.fiuba.app.udrive.network.StatusCode;
import com.fiuba.app.udrive.network.UserService;
import com.fiuba.app.udrive.utils.StringUtils;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class ShareActivity extends AppCompatActivity {

    private static final String TAG = "ShareActivity";
    public static final String EXTRA_USER_ACCOUNT = "userAccount";

    private ListView mUsersListView;
    private List<Collaborator> mCollaborators = new ArrayList<>();
    private UserAccount mUserAccount;
    private UserService mUserService;
    private CollaboratorsArrayAdapter mCollaboratorsAdapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_users_list);
        mUserAccount = (UserAccount) getIntent().getSerializableExtra(EXTRA_USER_ACCOUNT);
        mUsersListView = (ListView) findViewById(R.id.usersListView);
        mUsersListView.setEmptyView(findViewById(R.id.emptyCollaboratiosTextView));
        mUsersListView.setChoiceMode(ListView.CHOICE_MODE_MULTIPLE_MODAL);
        mUsersListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
               mCollaboratorsAdapter.toggleChecked(position);
                invalidateOptionsMenu();
            }
        });
        mCollaboratorsAdapter = new CollaboratorsArrayAdapter(this);
        mUsersListView.setAdapter(mCollaboratorsAdapter);
        mUserService = new UserService(mUserAccount.getToken(), this);
        loadCollaborators();
    }

    private void loadCollaborators() {
        final ProgressDialog progressDialog = ProgressDialog.show(this, null, getString(R.string.loading), true);
        progressDialog.setCancelable(false);

       /* mUserService.getTrashedFiles(mUserAccount.getUserId(), new ServiceCallback<List<File>>() {
            @Override
            public void onSuccess(List<File> files, int status) {
                setTrashedFiles(files);
                progressDialog.dismiss();
            }

            @Override
            public void onFailure(String message, int status) {
                showErrorMessage(message, status);
                progressDialog.dismiss();
            }
        });*/

        setCollaborators(Arrays.asList(new Collaborator("Romina", "Pamela", "romi@romi.com"),
                new Collaborator("Nahuel", "Cortazzo","nahu@nahu.com")));

    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
       /* getMenuInflater().inflate(R.menu.menu_trash, menu);
        if (mCollaborators.size() == 0) {
            menu.findItem(R.id.action_empty_trash).setVisible(false);
        }*/
        return true;
    }


    private String getCsvSelectedFileIds() {
        List<Integer> selectedFileIds = getSelectedUserIds();
        return StringUtils.join(selectedFileIds, ",");
    }



    private List<Integer> getSelectedUserIds() {
        List<Integer> checkedUserIds = new ArrayList<>();
        List<Collaborator> checkedItems =mCollaboratorsAdapter.getCheckedItems();
        for (Collaborator collaborator : checkedItems) {
            checkedUserIds.add(collaborator.getId());
        }
        return checkedUserIds;
    }

    private void showErrorMessage(String message, int status) {
        if (StatusCode.isHumanReadable(status)) {
            message = StatusCode.getMessage(ShareActivity.this, status);
            Toast.makeText(ShareActivity.this, message, Toast.LENGTH_LONG).show();
        }
        Log.e(TAG, message);
    }

    private void showAlertDialog(DialogInterface.OnClickListener onPositiveClickListener) {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setCancelable(false)
                .setMessage(getString(R.string.are_you_sure))
                .setPositiveButton(getString(R.string.yes), onPositiveClickListener)
                .setNegativeButton(getString(R.string.no), new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        dialog.cancel();
                    }
                });
        AlertDialog alert = builder.create();
        alert.show();
    }

    private void setCollaborators(List<Collaborator> collaborators) {
        this.mCollaborators = collaborators;
        mCollaboratorsAdapter.updateCollaborators(mCollaborators);
        invalidateOptionsMenu();
    }
}