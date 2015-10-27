package com.fiuba.app.udrive.model;

import java.lang.reflect.Array;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 * Created by eugenia on 26/10/15.
 */
public class Util {
    private static final String PASS_SALT = "UDRIVE1234";

    public static String encodePassword(String password){
        return md5(PASS_SALT+password);
    }

    /**
     * Applies a md5 hash algorithm to the given string
     * @param s, is the string to be encoded
     * @return a hex representation of the hashed string or an empty string if
     * an exception occurs
     */
    public static String md5(String s) {
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

    public static boolean validateString(String target, ArrayList<String> items){
        return items.contains(target);
    }
}
