package com.fiuba.app.udrive;

import android.content.Context;
import android.content.Intent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import com.fiuba.app.udrive.model.Collaborator;
import com.fiuba.app.udrive.model.File;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class CollaboratorsArrayAdapter extends BaseAdapter {

    private final Context mContext;
    private List<Collaborator> mCollaborators = Collections.emptyList();
    private Map<Integer, Boolean> mCheckedPositions = new HashMap<>();

    public CollaboratorsArrayAdapter(Context context) {
        this.mContext = context;
    }

    public List<Collaborator> getCollaborators() {
        return mCollaborators;
    }

    public void updateAndCheckCollaborators(List<Collaborator> collaborators) {
        this.mCollaborators = collaborators;
        for (Collaborator collaborator : this.mCollaborators) {
            this.mCheckedPositions.put(this.mCollaborators.indexOf(collaborator), true);
        }
        notifyDataSetChanged();
    }

    public void addAndCheckCollaborator(Collaborator collaborator) {
        this.mCollaborators.add(collaborator);
        this.mCheckedPositions.put(this.mCollaborators.indexOf(collaborator), true);
        notifyDataSetChanged();
    }

    @Override
    public int getCount() {
        return mCollaborators.size();
    }

    @Override
    public Collaborator getItem(int position) {
        return mCollaborators.get(position);
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    private static class ViewHolder {
        TextView txEmail;
        ImageView ivCheck;
        ImageView ivUser;
    }

    public View getView(int position, View convertView, ViewGroup parent) {
        final ViewHolder viewHolder;

        if (convertView == null) {
            convertView = LayoutInflater.from(mContext)
                    .inflate(R.layout.users_list_item, parent, false);

            viewHolder = new ViewHolder();
            viewHolder.ivUser = (ImageView) convertView.findViewById(R.id.imageViewUserListItem);
            viewHolder.txEmail = (TextView) convertView.findViewById(R.id.textViewUserListName);
            viewHolder.ivCheck = (ImageView) convertView.findViewById(R.id.imageViewCheckUser);


            convertView.setTag(viewHolder);
        } else {
            viewHolder = (ViewHolder) convertView.getTag();
        }

        Collaborator colaborator = mCollaborators.get(position);

        viewHolder.txEmail.setText(colaborator.getEmail());

        if (isChecked(position)) {
            viewHolder.ivCheck.setImageResource(R.drawable.ic_checkbox_marked_circle);
        } else {
            viewHolder.ivCheck.setImageResource(R.drawable.ic_checkbox_blank_circle_outline);
        }

        return convertView;
    }

    public void toggleChecked(int position) {
        mCheckedPositions.put(position, !isChecked(position));
        notifyDataSetChanged();
    }

    public boolean isChecked(int position) {
        Boolean aBoolean = mCheckedPositions.get(position);
        return aBoolean != null && aBoolean == true;
    }

    public List<Collaborator> getCheckedItems(){
        List<Collaborator> checkedItems = new ArrayList<>();

        for(int i = 0; i < mCheckedPositions.size(); i++) {
            if (mCheckedPositions.get(i)) {
                checkedItems.add(mCollaborators.get(i));
            }
        }

        return checkedItems;
    }
}