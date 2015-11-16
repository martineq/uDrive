package com.fiuba.app.udrive;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import com.fiuba.app.udrive.model.Collaborator;

import java.util.Collections;
import java.util.List;

public class SearchUsersAdapter extends BaseAdapter {

    private final Context mContext;
    private List<Collaborator> mUsers = Collections.emptyList();

    public SearchUsersAdapter(Context context) {
        this.mContext = context;
    }

    public void updateUsers(List<Collaborator> users) {
        this.mUsers = users;
        notifyDataSetChanged();
    }

    @Override
    public int getCount() {
        return mUsers.size();
    }

    @Override
    public Collaborator getItem(int position) {
        return mUsers.get(position);
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    private static class ViewHolder {
        TextView txEmail;
    }

    public View getView(int position, View convertView, ViewGroup parent) {
        final ViewHolder viewHolder;

        if (convertView == null) {
            convertView = LayoutInflater.from(mContext)
                    .inflate(R.layout.search_user_list_item, parent, false);

            viewHolder = new ViewHolder();
            viewHolder.txEmail = (TextView) convertView.findViewById(R.id.textViewEmail);

            convertView.setTag(viewHolder);
        } else {
            viewHolder = (ViewHolder) convertView.getTag();
        }

        Collaborator user = mUsers.get(position);
        if (user != null) {
            viewHolder.txEmail.setText(user.getEmail());
        }

        return convertView;
    }
}
