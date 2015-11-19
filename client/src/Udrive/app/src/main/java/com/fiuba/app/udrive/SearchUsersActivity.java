package com.fiuba.app.udrive;

import android.content.Intent;
import android.os.Bundle;
import android.support.v4.view.MenuItemCompat;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.SearchView;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import com.fiuba.app.udrive.model.Collaborator;
import com.fiuba.app.udrive.model.UserAccount;
import com.fiuba.app.udrive.network.ServiceCallback;
import com.fiuba.app.udrive.network.StatusCode;
import com.fiuba.app.udrive.network.UserService;
import com.fiuba.app.udrive.utils.StringUtils;

import java.util.ArrayList;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

public class SearchUsersActivity extends AppCompatActivity implements SearchView.OnQueryTextListener {

    private static final String TAG = "SearchUsersActivity";
    public static final String EXTRA_USER_ACCOUNT = "userAccount";
    public static final String EXTRA_SELECTED_USER = "selectedUser";
    private final static long SEARCH_TIMER_DELAY = 1000;

    private ListView mUserListView;
    private List<Collaborator> mUsers = new ArrayList<>();
    private UserAccount mUserAccount;
    private UserService mUserService;
    private SearchUsersAdapter mSearchUsersAdapter;
    private SearchView mSearchView;
    private Timer mSearchTimer;
    private ProgressBar mProgressBar;
    private TextView mEmptyTextView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_search_users);
        mProgressBar = (ProgressBar) findViewById(R.id.progressBar);
        mEmptyTextView = (TextView) findViewById(R.id.emptyTextView);
        mUserAccount = (UserAccount) getIntent().getSerializableExtra(EXTRA_USER_ACCOUNT);
        mUserListView = (ListView) findViewById(R.id.userListView);
        mUserListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                Intent intent = new Intent();
                intent.putExtra(EXTRA_SELECTED_USER, mUsers.get(position));
                setResult(RESULT_OK, intent);
            }
        });
        mSearchUsersAdapter = new SearchUsersAdapter(this);
        mUserListView.setAdapter(mSearchUsersAdapter);
        mUserService = new UserService(mUserAccount.getToken(), this);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_search_users, menu);

        MenuItem searchItem = menu.findItem(R.id.action_search);
        mSearchView = (SearchView) MenuItemCompat.getActionView(searchItem);
        setupSearchView();

        return true;
    }

    private void setupSearchView() {
        mSearchView.setOnQueryTextListener(this);
        mSearchView.setFocusable(true);
        mSearchView.setIconifiedByDefault(false);
        mSearchView.requestFocusFromTouch();
    }

    @Override
    public boolean onQueryTextSubmit(String query) {
        return false;
    }

    @Override
    public boolean onQueryTextChange(String searchString) {
        cancelPreviousScheduledGetAutocompleteItems();

        if (StringUtils.isNotEmpty(searchString)) {
            scheduleGetAutocompleteItems(searchString);
        } else {
            refreshEmptyTextView();
        }
        return true;
    }

    private void cancelPreviousScheduledGetAutocompleteItems() {
        if (mSearchTimer != null) {
            mSearchTimer.cancel();
        }
        showProgressBar(false);
    }

    private void scheduleGetAutocompleteItems(final String searchString) {
        showProgressBar(true);
        mSearchTimer = new Timer();
        mSearchTimer.schedule(new TimerTask() {
            @Override
            public void run() {
                getAutocompleteItems(searchString);
            }
        }, SEARCH_TIMER_DELAY);
    }

    private void getAutocompleteItems(final String searchString) {
        mUserService.getUsers(searchString,mUserAccount.getUserId(), new ServiceCallback<List<Collaborator>>() {
            @Override
            public void onSuccess(List<Collaborator> users, int status) {
                mUsers = users;
                mSearchUsersAdapter.updateUsers(mUsers);
                showProgressBar(false);
                refreshEmptyTextView();
            }

            @Override
            public void onFailure(String message, int status) {
                if (StatusCode.isHumanReadable(status)) {
                    message = StatusCode.getMessage(SearchUsersActivity.this, status);
                    Toast.makeText(SearchUsersActivity.this, message, Toast.LENGTH_LONG).show();
                }
                Log.e(TAG, message);
                showProgressBar(false);
                refreshEmptyTextView();
            }
        });
    }

    private void showProgressBar(boolean show) {
        if (show) {
            mProgressBar.setVisibility(View.VISIBLE);
        } else {
            mProgressBar.setVisibility(View.GONE);
        }
    }

    private void refreshEmptyTextView() {
        showEmptyTextView(mUsers.isEmpty() && mSearchView.getQuery().length() > 0);
    }

    private void showEmptyTextView(boolean show) {
        if (show) {
            mEmptyTextView.setVisibility(View.VISIBLE);
        } else {
            mEmptyTextView.setVisibility(View.GONE);
        }
    }
}
