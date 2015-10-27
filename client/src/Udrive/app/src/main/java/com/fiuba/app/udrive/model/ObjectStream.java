package com.fiuba.app.udrive.model;


import android.content.Context;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectOutputStream;
import java.io.ObjectInputStream;

/**
 * This class associates a device's private file to an object
 * For manipulating the object persistence in a context restricted only to the app.
 * @param <T> is the type of object to be persisted.
 */
public class ObjectStream<T> {

    private String mFileName = null;
    private Context mContext = null;

    /**
     * Constructor needs the filename to represent the object into the device
     * and the context from where is being called from.
     * @param _mfileName
     * @param _mContext
     */
    public ObjectStream(String _mfileName, Context _mContext){
        mFileName = _mfileName;
        mContext = _mContext;
    }

    /**
     * Saves an object into the file
     * @param object
     */
    public void put(T object){
        try {
            FileOutputStream fileOutputStream = mContext.openFileOutput(mFileName, Context.MODE_PRIVATE);
            ObjectOutputStream objectOutputStream = new ObjectOutputStream(fileOutputStream);
            objectOutputStream.writeObject(object);
            objectOutputStream.close();
            fileOutputStream.close();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * Gets the object instance persisted into the device
     * @return object
     */
    public T get(){
        T myObject = null;
        try {
            FileInputStream fileInputStream = mContext.openFileInput(mFileName);
            ObjectInputStream objectInputStream = new ObjectInputStream(fileInputStream);
            myObject = (T) objectInputStream.readObject();
            objectInputStream.close();
            fileInputStream.close();
        } catch (IOException e) {

        } catch (ClassNotFoundException e){

        }
        return myObject;
    }

    /**
     * Removes the file from the device.
     */
    public void delete(){
        mContext.deleteFile(mFileName);
    }

    /**
     * Checks whether the file already exists into the device or not.
     * @return true if exists, false otherwise.
     */
    public boolean exists(){
        try {
            FileInputStream file = mContext.openFileInput(mFileName);
        } catch (FileNotFoundException e) {
            return false;
        }
        return true;
    }
}
