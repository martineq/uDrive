package com.fiuba.app.udrive;

import android.app.Activity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.concurrent.atomic.AtomicReferenceArray;

/**
 * Created by eugenia on 20/11/15.
 */
public class CollabListViewAdapter extends ArrayAdapter<String> {

    private final Activity mContext;
    private final ArrayList<String> mItemname;
    private final ArrayList<String> mItemmail;
    private int mResourceItemId;

    public CollabListViewAdapter(Activity context, int resourceId,
                                    ArrayList<String> itemname, ArrayList<String> itemmail) {
        super(context, -1, itemname);
        this.mContext = context;
        this.mItemname = itemname;
        this.mItemmail = itemmail;
        this.mResourceItemId = resourceId;
    }

    @Override
    public View getView(int position,View view,ViewGroup parent) {
        LayoutInflater inflater= mContext.getLayoutInflater();
        View rowView=inflater.inflate(mResourceItemId, parent, false);

        TextView txt = (TextView) rowView.findViewById(R.id.textAccessItem);
        TextView mail = (TextView) rowView.findViewById(R.id.mailAccessItem);

        txt.setText(mItemname.get(position));
        mail.setText("("+mItemmail.get(position)+")");
        System.out.println("name "+ mItemname.get(position)+ " mail "+ mItemmail.get(position));
        return rowView;
    }
}
