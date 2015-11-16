package com.fiuba.app.udrive;

import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.Toast;

import com.fiuba.app.udrive.model.File;
import com.fiuba.app.udrive.model.UserAccount;
import com.fiuba.app.udrive.network.FilesService;
import com.fiuba.app.udrive.network.ServiceCallback;
import com.fiuba.app.udrive.network.StatusCode;
import com.fiuba.app.udrive.utils.StringUtils;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class TrashActivity extends AppCompatActivity {

    private static final String TAG = "TrashActivity";
    public static final String EXTRA_USER_ACCOUNT = "userAccount";

    private ListView mFileListView;
    private List<File> mTrashedFiles = new ArrayList<>();
    private UserAccount mUserAccount;
    private FilesService mFilesService;
    private TrashedFilesAdapter mTrashedFilesAdapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_trash);
        mUserAccount = (UserAccount) getIntent().getSerializableExtra(EXTRA_USER_ACCOUNT);
        mFileListView = (ListView) findViewById(R.id.fileListView);
        mFileListView.setEmptyView(findViewById(R.id.emptyTextView));
        mFileListView.setChoiceMode(ListView.CHOICE_MODE_MULTIPLE_MODAL);
        mFileListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                mTrashedFilesAdapter.toggleChecked(position);
                invalidateOptionsMenu();
            }
        });
        mTrashedFilesAdapter = new TrashedFilesAdapter(this);
        mFileListView.setAdapter(mTrashedFilesAdapter);
        mFilesService = new FilesService(mUserAccount.getToken(), this);
        loadTrashedFiles();
    }

    private void loadTrashedFiles() {
       final ProgressDialog progressDialog = ProgressDialog.show(this, null, getString(R.string.loading), true);
        progressDialog.setCancelable(false);

        mFilesService.getTrashedFiles(mUserAccount.getUserId(), new ServiceCallback<List<File>>() {
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
        });

    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_trash, menu);
        if (mTrashedFiles.size() == 0) {
            menu.findItem(R.id.action_empty_trash).setVisible(false);
            menu.findItem(R.id.action_restore_all).setVisible(false);
            menu.findItem(R.id.action_delete_selected).setVisible(false);
            menu.findItem(R.id.action_restore_selected).setVisible(false);
        } else if (getSelectedFileIds().size() == 0) {
            menu.findItem(R.id.action_delete_selected).setVisible(false);
            menu.findItem(R.id.action_restore_selected).setVisible(false);
        }
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        if (id == R.id.action_empty_trash) {
            emptyTrash();
            return true;
        } else if (id == R.id.action_restore_all) {
            restoreAll();
            return true;
        } else if (id == R.id.action_delete_selected) {
            deleteSelectedFiles();
            return true;
        } else if (id == R.id.action_restore_selected) {
            restoreSelectedFiles();
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    private void restoreSelectedFiles() {
        showAlertDialog(new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                String csvFileIds = getCsvSelectedFileIds();

                final ProgressDialog progressDialog = ProgressDialog.show(TrashActivity.this, null, getString(R.string.loading), true);
                progressDialog.setCancelable(false);
                mFilesService.restoreTrashedFiles(mUserAccount.getUserId(), csvFileIds, new ServiceCallback<List<File>>() {
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
                });
            }
        });
    }

    private void deleteSelectedFiles() {
        showAlertDialog(new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                String csvFileIds = getCsvSelectedFileIds();

                final ProgressDialog progressDialog = ProgressDialog.show(TrashActivity.this, null, getString(R.string.loading), true);
                progressDialog.setCancelable(false);
                mFilesService.deleteTrashedFiles(mUserAccount.getUserId(), csvFileIds, new ServiceCallback<List<File>>() {
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
                });
            }
        });
    }

    private String getCsvSelectedFileIds() {
        List<Integer> selectedFileIds = getSelectedFileIds();
        return StringUtils.join(selectedFileIds, ",");
    }

    private void restoreAll() {
        showAlertDialog(new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                final ProgressDialog progressDialog = ProgressDialog.show(TrashActivity.this, null, getString(R.string.loading), true);
                progressDialog.setCancelable(false);
                mFilesService.restoreAllTrashedFiles(mUserAccount.getUserId(), new ServiceCallback<Void>() {
                    @Override
                    public void onSuccess(Void v, int status) {
                        setTrashedFiles(new ArrayList<File>());
                        progressDialog.dismiss();
                    }

                    @Override
                    public void onFailure(String message, int status) {
                        showErrorMessage(message, status);
                        progressDialog.dismiss();
                    }
                });
            }
        });
    }

    private void emptyTrash() {
        showAlertDialog(new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                final ProgressDialog progressDialog = ProgressDialog.show(TrashActivity.this, null, getString(R.string.loading), true);
                progressDialog.setCancelable(false);
                mFilesService.deleteAllTrashedFiles(mUserAccount.getUserId(), new ServiceCallback<Void>() {
                    @Override
                    public void onSuccess(Void object, int status) {
                        setTrashedFiles(new ArrayList<File>());
                        progressDialog.dismiss();
                    }

                    @Override
                    public void onFailure(String message, int status) {
                        showErrorMessage(message, status);
                        progressDialog.dismiss();
                    }
                });
            }
        });
    }

    private List<Integer> getSelectedFileIds() {
        List<Integer> checkedFileIds = new ArrayList<>();
        List<File> checkedItems = mTrashedFilesAdapter.getCheckedItems();
        for (File file : checkedItems) {
            checkedFileIds.add(file.getId());
        }
        return checkedFileIds;
    }

    private void showErrorMessage(String message, int status) {
        if (StatusCode.isHumanReadable(status)) {
            message = StatusCode.getMessage(TrashActivity.this, status);
            Toast.makeText(TrashActivity.this, message, Toast.LENGTH_LONG).show();
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

    private void setTrashedFiles(List<File> trashedFiles) {
        this.mTrashedFiles = trashedFiles;
        mTrashedFilesAdapter.updateFiles(mTrashedFiles);
        invalidateOptionsMenu();
    }
}