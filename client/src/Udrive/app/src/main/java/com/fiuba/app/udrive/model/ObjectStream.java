package com.fiuba.app.udrive.model;


import android.content.Context;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectOutputStream;
import java.io.ObjectInputStream;


public class ObjectStream<T> {

    private String mFileName = null;
    private Context mContext = null;

    public ObjectStream(String mfileName, Context mContext){
        this.mFileName = mfileName;
        this.mContext = mContext;
    }

    public void put(T object){
        try {
            FileOutputStream fileOutputStream = mContext.openFileOutput(this.mFileName, Context.MODE_PRIVATE);
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

    public T get(){
        T myObject = null;
        try {
            FileInputStream fileInputStream = mContext.openFileInput(this.mFileName);
            ObjectInputStream objectInputStream = new ObjectInputStream(fileInputStream);
            myObject = (T) objectInputStream.readObject();
            objectInputStream.close();
            fileInputStream.close();
        } catch (IOException e) {

        } catch (ClassNotFoundException e){

        }
        return myObject;
    }

    public void delete(){
        mContext.deleteFile(mFileName);
    }

    public boolean exists(){
        try {
            FileInputStream file = mContext.openFileInput(mFileName);
        } catch (FileNotFoundException e) {
            return false;
        }
        return true;
    }
}
