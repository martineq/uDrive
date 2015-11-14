package com.fiuba.app.udrive;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import com.fiuba.app.udrive.model.File;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class TrashedFilesAdapter extends BaseAdapter {

    private final Context mContext;
    private List<File> mTrashedFiles = Collections.emptyList();
    private Map<Integer, Boolean> mCheckedPositions = new HashMap<>();

    public TrashedFilesAdapter(Context context) {
        this.mContext = context;
    }

    public void updateFiles(List<File> files) {
        this.mTrashedFiles = files;
        notifyDataSetChanged();
    }

    @Override
    public int getCount() {
        return mTrashedFiles.size();
    }

    @Override
    public File getItem(int position) {
        return mTrashedFiles.get(position);
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    private static class ViewHolder {
        ImageView ivItem;
        TextView txName;
        ImageView ivCheck;
    }

    public View getView(int position, View convertView, ViewGroup parent) {
        final ViewHolder viewHolder;

        if (convertView == null) {
            convertView = LayoutInflater.from(mContext)
                    .inflate(R.layout.trashed_file_list_item, parent, false);

            viewHolder = new ViewHolder();
            viewHolder.ivItem = (ImageView) convertView.findViewById(R.id.imageViewFileListItem);
            viewHolder.txName = (TextView) convertView.findViewById(R.id.textViewFileListName);
            viewHolder.ivCheck = (ImageView) convertView.findViewById(R.id.imageViewCheck);

            convertView.setTag(viewHolder);
        } else {
            viewHolder = (ViewHolder) convertView.getTag();
        }

        File file = mTrashedFiles.get(position);
        if (file != null) {
            if (file.isDir()) {
                viewHolder.ivItem.setImageResource(R.drawable.ic_folder);
            } else {
                viewHolder.ivItem.setImageResource(R.drawable.ic_file);
            }
            viewHolder.txName.setText(file.getName());
        }
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

    private boolean isChecked(int position) {
        Boolean aBoolean = mCheckedPositions.get(position);
        return aBoolean != null && aBoolean == true;
    }

    public List<File> getCheckedItems(){
        List<File> checkedItems = new ArrayList<>();

        for(int i = 0; i < mCheckedPositions.size(); i++) {
            if (mCheckedPositions.get(i)) {
                checkedItems.add(mTrashedFiles.get(i));
            }
        }

        return checkedItems;
    }
}