package com.fiuba.app.udrive;

import android.app.Activity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.TextView;

public class SettingsListAdapter extends ArrayAdapter<String> {

    private final Activity mContext;
    private final String[] mItemname;
    private final Integer[] mImgid;

    public SettingsListAdapter(Activity context, String[] itemname, Integer[] imgid) {
        super(context, R.layout.settings_item, itemname);
        // TODO Auto-generated constructor stub

        this.mContext =context;
        this.mItemname =itemname;
        this.mImgid =imgid;
    }

    public View getView(int position,View view,ViewGroup parent) {
        LayoutInflater inflater= mContext.getLayoutInflater();
        View rowView=inflater.inflate(R.layout.settings_item, null,true);

        TextView txt = (TextView) rowView.findViewById(R.id.textViewSettingItem);
        ImageView image = (ImageView) rowView.findViewById(R.id.imageViewSettingItem);

        txt.setText(mItemname[position]);
        image.setImageResource(mImgid[position]);
        return rowView;

    };
}
