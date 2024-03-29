package com.fiuba.app.udrive;

import android.app.Activity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

import java.util.ArrayList;

public class CollaboratorsListAdapter extends ArrayAdapter {

    private final Activity mContext;
    private final ArrayList<String> mItemname;
    private final ArrayList<String> mItemmail;
    private int mResourceItemId;

    public CollaboratorsListAdapter(Activity context, int resourceId, int resourceItemId,
                                    ArrayList<String> itemname, ArrayList<String> itemmail) {
        super(context, resourceId);
        // TODO Auto-generated constructor stub
        this.mContext = context;
        this.mItemname = itemname;
        this.mItemmail = itemmail;
        this.mResourceItemId = resourceItemId;
    }

    @Override
    public View getView(int position,View view,ViewGroup parent) {
        LayoutInflater inflater= mContext.getLayoutInflater();
        View rowView=inflater.inflate(mResourceItemId, parent, false);

        TextView txt = (TextView) rowView.findViewById(R.id.textAccessItem);
        TextView mail = (TextView) rowView.findViewById(R.id.mailAccessItem);

        txt.setText(mItemname.get(position));
        mail.setText("("+mItemmail.get(position)+")");
        return rowView;

    }

    @Override
    public View getDropDownView(int position,View view,ViewGroup parent) {
        LayoutInflater inflater= mContext.getLayoutInflater();
        View rowView=inflater.inflate(mResourceItemId, parent, false);

        TextView txt = (TextView) rowView.findViewById(R.id.textAccessItem);
        TextView mail = (TextView) rowView.findViewById(R.id.mailAccessItem);

        txt.setText(mItemname.get(position));
        mail.setText("("+mItemmail.get(position)+")");
        return rowView;
    }
}
