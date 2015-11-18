package com.fiuba.app.udrive.view;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.content.Context;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewTreeObserver;
import android.view.animation.AccelerateInterpolator;
import android.view.animation.OvershootInterpolator;
import com.fiuba.app.udrive.utils.Utils;

public class FileContextMenuManager implements View.OnAttachStateChangeListener {

    private static FileContextMenuManager instance;

    private FileContextMenu contextMenuView;

    private boolean isContextMenuDismissing;
    private boolean isContextMenuShowing;

    public static FileContextMenuManager getInstance() {
        if (instance == null) {
            instance = new FileContextMenuManager();
        }
        return instance;
    }

    private FileContextMenuManager() {

    }

    public void toggleContextMenuFromView(View openingView, int feedItem, FileContextMenu.OnFileContextMenuItemClickListener listener, FileContextMenu.OnFileContextMenuItemClickButtonVisibilityListener btnVisibilityListener ) {
        if (contextMenuView == null) {
            showContextMenuFromView(openingView, feedItem, listener, btnVisibilityListener);
        } else {
            hideContextMenu();
        }
    }

    private void showContextMenuFromView(final View openingView, int feedItem, FileContextMenu.OnFileContextMenuItemClickListener listener, FileContextMenu.OnFileContextMenuItemClickButtonVisibilityListener btnVisibilityListener) {
        if (!isContextMenuShowing) {
            isContextMenuShowing = true;
            contextMenuView = new FileContextMenu(openingView.getContext());
            contextMenuView.bindToItem(feedItem);
            contextMenuView.addOnAttachStateChangeListener(this);
            contextMenuView.setOnFileMenuItemClickListener(listener,btnVisibilityListener,feedItem);

            ((ViewGroup) openingView.getRootView().findViewById(android.R.id.content)).addView(contextMenuView);

            contextMenuView.getViewTreeObserver().addOnPreDrawListener(new ViewTreeObserver.OnPreDrawListener() {
                @Override
                public boolean onPreDraw() {
                    contextMenuView.getViewTreeObserver().removeOnPreDrawListener(this);
                    setupContextMenuInitialPosition(openingView);
                    performShowAnimation();
                    return false;
                }
            });
        }
    }

    private void setupContextMenuInitialPosition(View openingView) {
        final int[] openingViewLocation = new int[2];
        openingView.getLocationOnScreen(openingViewLocation);
        int additionalBottomMargin = 16;
        Context myContext = openingView.getContext();
        int screenHeight = Utils.getScreenHeight(myContext);
        contextMenuView.setTranslationX(openingViewLocation[0] - contextMenuView.getWidth());

        if( openingViewLocation[1] <= (screenHeight/2) ){
            contextMenuView.setTranslationY(openingViewLocation[1] - 10*additionalBottomMargin);
        }else{
            contextMenuView.setTranslationY(openingViewLocation[1] - contextMenuView.getHeight() - additionalBottomMargin);
        }
    }

    private void performShowAnimation() {
        contextMenuView.setPivotX(contextMenuView.getWidth() / 2);
        contextMenuView.setPivotY(contextMenuView.getHeight());
        contextMenuView.setScaleX(0.1f);
        contextMenuView.setScaleY(0.1f);
        contextMenuView.animate()
                .scaleX(1f).scaleY(1f)
                .setDuration(150)
                .setInterpolator(new OvershootInterpolator())
                .setListener(new AnimatorListenerAdapter() {
                    @Override
                    public void onAnimationEnd(Animator animation) {
                        isContextMenuShowing = false;
                    }
                });
    }

    public void hideContextMenu() {
        if (!isContextMenuDismissing) {
            isContextMenuDismissing = true;
            performDismissAnimation();
        }
    }

    private void performDismissAnimation() {
        contextMenuView.setPivotX(contextMenuView.getWidth() / 2);
        contextMenuView.setPivotY(contextMenuView.getHeight());
        contextMenuView.animate()
                .scaleX(0.1f).scaleY(0.1f)
                .setDuration(150)
                .setInterpolator(new AccelerateInterpolator())
                .setStartDelay(100)
                .setListener(new AnimatorListenerAdapter() {
                    @Override
                    public void onAnimationEnd(Animator animation) {
                        if (contextMenuView != null) {
                            contextMenuView.dismiss();
                        }
                        isContextMenuDismissing = false;
                    }
                });
    }

    @Override
    public void onViewAttachedToWindow(View v) {

    }

    @Override
    public void onViewDetachedFromWindow(View v) {
        contextMenuView = null;
    }
}