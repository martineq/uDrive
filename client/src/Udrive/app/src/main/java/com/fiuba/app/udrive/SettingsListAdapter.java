package com.fiuba.app.udrive;

import android.app.Activity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.TextView;

public class SettingsListAdapter extends ArrayAdapter<String> {

    private final Activity context;
    private final String[] itemname;
    private final Integer[] imgid;

    public SettingsListAdapter(Activity context, String[] itemname, Integer[] imgid) {
        super(context, R.layout.settings_item, itemname);
        // TODO Auto-generated constructor stub

        this.context=context;
        this.itemname=itemname;
        this.imgid=imgid;
    }

    public View getView(int position,View view,ViewGroup parent) {
        LayoutInflater inflater=context.getLayoutInflater();
        View rowView=inflater.inflate(R.layout.settings_item, null,true);

        TextView txt = (TextView) rowView.findViewById(R.id.textViewSettingItem);
        ImageView image = (ImageView) rowView.findViewById(R.id.imageViewSettingItem);

        txt.setText(itemname[position]);
        image.setImageResource(imgid[position]);
        return rowView;

    };
}
