package com.fiuba.app.udrive.model;


// Represents coordinates for GPS location
public class MyPhoto{
    private String photoStream;

    public MyPhoto(String _photoStream){
        photoStream = _photoStream;
    }

    public String getPhotoStream(){
        return photoStream;
    }

}