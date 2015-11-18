package com.fiuba.app.udrive.view;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.LinearLayout;
import android.widget.ViewSwitcher;

import com.fiuba.app.udrive.R;


public class FileContextMenu extends LinearLayout {

    private int fileItem = -1;

    private OnFileContextMenuItemClickListener onItemClickListener;

    private OnFileContextMenuItemClickButtonVisibilityListener onItemClickBtnVisibilityListener;
    private int mFeedItem;

    public FileContextMenu(Context context) {
        super(context);
        init();
    }

    private void init() {
        LayoutInflater.from(getContext()).inflate(R.layout.view_context_menu, this, true);
        setBackgroundResource(R.drawable.bg_container_shadow);
        setOrientation(VERTICAL);
        setLayoutParams(new LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT));
    }

    public void bindToItem(int FileItem) {
        this.fileItem = FileItem;
    }

    @Override
    protected void onAttachedToWindow() {
        super.onAttachedToWindow();
        LinearLayout btnDownload = (LinearLayout) findViewById(R.id.lnBtnDownload);
        LinearLayout btnInfo = (LinearLayout) findViewById(R.id.lnBtnInformation);
        LinearLayout btnShare = (LinearLayout) findViewById(R.id.lnBtnShare);
        LinearLayout btnTag = (LinearLayout) findViewById(R.id.lnBtnTag);
        LinearLayout btnDelete = (LinearLayout) findViewById(R.id.lnBtnDelete);
        LinearLayout btnCancel = (LinearLayout) findViewById(R.id.lnBtnCancel);

        btnDownload.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                if (onItemClickListener != null) {
                    onItemClickListener.onDownloadClick(fileItem);
                }
            }
        });

        btnInfo.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                if (onItemClickListener != null) {
                    onItemClickListener.onInformationClick(fileItem);
                }
            }
        });

        btnShare.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                if (onItemClickListener != null) {
                    onItemClickListener.onShareClick(fileItem);
                }
            }
        });

        btnTag.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                if (onItemClickListener != null) {
                    onItemClickListener.onTagClick(fileItem);
                }
            }
        });

        btnDelete.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                if (onItemClickListener != null) {
                    onItemClickListener.onDeleteClick(fileItem);
                }
            }
        });

        if(!this.onItemClickBtnVisibilityListener.shouldShowDeleteButton(mFeedItem))
            btnDelete.setVisibility(GONE);

        btnCancel.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                if (onItemClickListener != null) {
                    onItemClickListener.onCancelClick(fileItem);
                }
            }
        });

    }

    public void dismiss() {
        ((ViewGroup) getParent()).removeView(FileContextMenu.this);
    }


    public void setOnFileMenuItemClickListener(OnFileContextMenuItemClickListener onItemClickListener, FileContextMenu.OnFileContextMenuItemClickButtonVisibilityListener btnVisibilityListener, int feedItem) {
        this.onItemClickListener = onItemClickListener;
        this.onItemClickBtnVisibilityListener = btnVisibilityListener;
        this.mFeedItem = feedItem;
    }

    public interface OnFileContextMenuItemClickListener {
        void onDownloadClick(int FileItem);
        void onInformationClick(int FileItem);
        void onShareClick(int FileItem);
        void onTagClick(int FileItem);
        void onDeleteClick(int FileItem);
        void onCancelClick(int FileItem);
    }

    public interface OnFileContextMenuItemClickButtonVisibilityListener {
        boolean shouldShowDeleteButton(int fileItem);
    }
}
