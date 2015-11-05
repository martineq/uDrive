package com.fiuba.app.udrive;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.TextView;

import com.fiuba.app.udrive.model.File;

import java.util.List;

public class FilesArrayAdapter extends ArrayAdapter<File> {

    private List<File> mFiles;
    private int mResourceId;
    private OnContextMenuClickListener contextMenuClickListener;

    public FilesArrayAdapter(Context context, int resourceId, List<File> files, OnContextMenuClickListener contextMenuClickListener) {
        super(context, resourceId, files);
        this.mFiles = files;
        this.mResourceId = resourceId;
        this.contextMenuClickListener = contextMenuClickListener;
    }

    static class FilesViewHolder {
        TextView txName;
        TextView txLastMod;
        ImageView imagFile;
        ImageButton imageButton;
    }

    public View getView(final int position, View convertView, ViewGroup parent) {
        // Keeps reference to avoid future findViewById()
        final FilesViewHolder viewHolder;

        if (convertView == null) {
            LayoutInflater li = (LayoutInflater) getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            convertView = li.inflate(mResourceId, parent, false);

            viewHolder = new FilesViewHolder();
            viewHolder.txName = (TextView) convertView.findViewById(R.id.textViewFileListName);
            viewHolder.txLastMod = (TextView) convertView.findViewById(R.id.textViewFileListLastMod);
            viewHolder.imagFile = (ImageView) convertView.findViewById(R.id.imageViewFileListItem);

            ImageButton btnMenu = (ImageButton) convertView.findViewById(R.id.BtnFileContextMenu);
            btnMenu.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    if (contextMenuClickListener != null) {
                        contextMenuClickListener.onClick(v, position);
                    }
                }
            });

            viewHolder.imageButton = btnMenu;


            convertView.setTag(viewHolder);
        } else {
            viewHolder = (FilesViewHolder) convertView.getTag();
        }

        File file = mFiles.get(position);
        if (file != null) {
            String lastMod = convertView.getResources().getString(R.string.file_lastMod)+" "+file.getLastModDateFormated();
            viewHolder.txName.setText(file.getName());
            viewHolder.txLastMod.setText(lastMod);
            if(file.getType() == 'd'){
                if(file.getShared()){
                    viewHolder.imagFile.setImageResource(R.drawable.ic_folder_account);
                }else{
                    viewHolder.imagFile.setImageResource(R.drawable.ic_folder);
                }

            }else{
                viewHolder.imagFile.setImageResource(R.drawable.ic_file);
            }

        }
        return convertView;
    }

    public void updateFiles(List<File> files) {
        this.mFiles.clear();
        this.mFiles.addAll(files);
        notifyDataSetChanged();
    }

    public interface OnContextMenuClickListener {
        void onClick(View v, int position);
    }

}
