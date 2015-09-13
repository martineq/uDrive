package com.fiuba.app.udrive;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.TextView;
import com.fiuba.app.udrive.R;
import com.fiuba.app.udrive.model.File;
import java.util.List;

public class FilesArrayAdapter extends ArrayAdapter<File> {

    private List<File> files;
    private int resourceId;

    public FilesArrayAdapter(Context context, int resourceId,
                                List<File> files) {
        super(context, resourceId, files);
        this.files = files;
        this.resourceId = resourceId;
    }

    static class ContactsViewHolder {
        TextView txName;
        TextView txLastMod;
        ImageView imagFile;
    }

    public View getView(int position, View convertView, ViewGroup parent) {
        // Keeps reference to avoid future findViewById()
        final ContactsViewHolder viewHolder;

        if (convertView == null) {
            LayoutInflater li = (LayoutInflater) getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            convertView = li.inflate(resourceId, parent, false);

            viewHolder = new ContactsViewHolder();
            viewHolder.txName = (TextView) convertView.findViewById(R.id.textViewFileListName);
            viewHolder.txLastMod = (TextView) convertView.findViewById(R.id.textViewFileListLastMod);
            viewHolder.imagFile = (ImageView) convertView.findViewById(R.id.imageViewFileListItem);

            convertView.setTag(viewHolder);
        } else {
            viewHolder = (ContactsViewHolder) convertView.getTag();
        }

        File file = files.get(position);
        if (file != null) {
            viewHolder.txName.setText(file.getName());
            viewHolder.txLastMod.setText(file.getLastModDate().toString());
            viewHolder.imagFile.setImageResource(R.drawable.file);

        }
        return convertView;
    }



}
