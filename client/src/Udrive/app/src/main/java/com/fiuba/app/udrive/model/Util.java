package com.fiuba.app.udrive.model;

import android.content.Context;
import android.location.Address;
import android.location.Geocoder;

import com.fiuba.app.udrive.R;

import java.io.IOException;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

/**
 * Provides some helpful general methods
 */
public class Util {
    private static final String PASS_SALT = "UDRIVE1234";

    /**
     * Encodes a password using a salt and a MD5 Hash
     * @param password
     * @return encoded password
     */
    public static String encodePassword(String password){
        return md5(PASS_SALT+password);
    }

    /**
     * Applies a md5 hash algorithm to the given string
     * @param s, is the string to be encoded
     * @return a hex representation of the hashed string or an empty string if
     * an exception occurs
     */
    private static String md5(String s) {
        try {
            // Create MD5 Hash
            MessageDigest digest = java.security.MessageDigest.getInstance("MD5");
            digest.update(s.getBytes());
            byte messageDigest[] = digest.digest();

            // Create Hex String
            StringBuffer hexString = new StringBuffer();
            for (int i=0; i<messageDigest.length; i++)
                hexString.append(Integer.toHexString(0xFF & messageDigest[i]));
            return hexString.toString();

        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        }
        return "";
    }

    /**
     * Checks if the given string matches to an occurrence within the given string array
     * @param target, is the string to search
     * @param items, is the string array
     * @return true if there is at least a match, false otherwise.
     */
    public static boolean matchString(String target, ArrayList<String> items){
        return items.contains(target);
    }

    public static String capitalize(String str) {
        StringBuilder b = new StringBuilder(str);
        int i = 0;
        do {
            b.replace(i, i + 1, b.substring(i, i + 1).toUpperCase());
            i = b.indexOf(" ", i) + 1;
        } while (i > 0 && i < b.length());
        return b.toString();
    }

    /**
     * Returns the digits contained in a string
     * @param src, string to be parsed
     * @return digits.
     */
    public static String extractDigits(String src) {
        StringBuilder builder = new StringBuilder();
        for (int i = 0; i < src.length(); i++) {
            char c = src.charAt(i);
            if (Character.isDigit(c)) {
                builder.append(c);
            }
        }
        return builder.toString();
    }

    public static String getGPSLocation(Context context, double latitude, double longitude) {
        Geocoder geocoder = new Geocoder(context, Locale.getDefault());
        List<Address> addresses = null;
        String location = null;
        if ((latitude != 0.00) && (longitude != 0.00)) {
            try {
                //addresses = geocoder.getFromLocation(-34.795713, -58.348321, 1);
                addresses = geocoder.getFromLocation(latitude,longitude, 1);
                location = addresses.get(0).getAddressLine(1);// + ", " + addresses.get(0).getAddressLine(2);
            } catch (IOException e) {
                // do something
            }
        } else {
            location = context.getString(R.string.unknown_location);
        }
        return location;
    }
}
