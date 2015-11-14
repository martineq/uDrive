package com.fiuba.app.udrive.utils;

import android.support.annotation.NonNull;

import java.util.Collection;

public class StringUtils {

    @NonNull
    public static String join(@NonNull Object[] array, @NonNull String separator) {
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < array.length; i++) {
            Object o = array[i];
            sb.append(o.toString());
            if (i < array.length - 1) {
                sb.append(separator);
            }
        }
        return sb.toString();
    }

    @NonNull
    public static String join(@NonNull Collection collection, @NonNull String separator) {
        return join(collection.toArray(), separator);
    }

}