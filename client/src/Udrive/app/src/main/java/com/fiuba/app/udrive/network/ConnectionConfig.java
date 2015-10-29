package com.fiuba.app.udrive.network;

import android.content.Context;
import android.content.SharedPreferences;

/**
 * Saves the connection server parameters into a device's private context
 */
public class ConnectionConfig {

    /**
     * Returns the server URL
     * @param context
     * @return
     */
    public static String getConnectionURL(Context context){
        SharedPreferences sharedPref = context.getSharedPreferences("connection", Context.MODE_PRIVATE);
        String serverIP = sharedPref.getString("server_ip", "190.7.56.249");
        String serverPort = sharedPref.getString("server_port", "8055");
        return "http://"+serverIP+":"+serverPort;
    }

    /**
     * Sets the required fields to establish a connection with the server
     * @param serverIP
     * @param serverPort
     * @param context
     */
    public static void setConnectionParams(String serverIP, String serverPort, Context context){
        SharedPreferences sharedPref = context.getSharedPreferences("connection", Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPref.edit();
        editor.putString("server_ip", serverIP);
        editor.putString("server_port", serverPort);
        editor.commit();
    }

    private static String getURLParts(Context context, String part){
        String url = getConnectionURL(context);
        url = url.substring(7);
        String[] parts = url.split(":");
        if (part == "ip")
            return parts[0];
        return parts[1];
    }

    /**
     * Gets IP from the configuration settings
     * @param context
     * @return IP
     */
    public static String getIP(Context context){
        return getURLParts(context, "ip");
    }

    /**
     * Gets the port number from the configuration settings
     * @param context
     * @return Port
     */
    public static String getPort(Context context){
        return getURLParts(context, "port");
    }

    /**
     * Clears all connection settings.
     * @param context
     */
    public static void clear(Context context){
        context.getSharedPreferences("connection", 0).edit().clear().commit();
    }
}
