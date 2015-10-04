package com.fiuba.app.udrive.network;

import android.content.Context;
import android.content.res.Resources;

import com.fiuba.app.udrive.R;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 * Handles the HTTP requests status code in relation to the user
 */
public class StatusCode {

    /**
     * Gets a message for a human readable error code
     * @param context to access to its resources
     * @param statusCode the integer HTTP request status code
     * @return a message
     */
    public static String getMessage(Context context, int statusCode){
        switch (statusCode){
            case 503:
                return context.getString(R.string.error_network);
            default:
                return context.getString(R.string.default_error);
        }
    }
    /**
     * Returns true if a status code is human readable to display a message on screen for the user,
     * returns false otherwise.
     * @param statusCode
     * @return true or false
     */
    public static boolean isHumanReadable(int statusCode){
        // Put others human readable codes if necessary
        int[] codes = {503};
        for (int i=0; i<codes.length; i++){
            if (codes[i] == statusCode)
                return true;
        }
        return false;
    }
}
