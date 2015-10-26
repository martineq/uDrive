package com.fiuba.app.udrive.model;

import com.fiuba.app.udrive.MainActivity;

public class UserProfile extends UserData {
    private String firstname;
    private String lastname;
    private String photo = null; // TODO: check this.
    private String lastLocation = null; // TODO: Mobile GPS usage
    private int userId = 0;
    private String quota = null;

    public UserProfile(String _email, String _password, String _firstname, String _lastname,
                       String _photo, String _lastLocation, String _quota){
        super(_email, Util.encodePassword(_password));
        firstname = _firstname;
        lastname = _lastname;
        photo = _photo;
        lastLocation = _lastLocation;
        quota = _quota;
    }

    public String getFirstname(){
        return firstname;
    }

    public void setFirstname(String _firstname){
        firstname = _firstname;
    }

    public String getLastname(){
        return lastname;
    }

    public void setLastname(String _lastname){
        lastname = _lastname;
    }

    public String getPhoto(){
        return photo;
    }

    public void setPhoto(String _photo){
        photo = _photo;
    }

    public String getLastLocation(){
        return lastLocation;
    }

    public void setLastLocation(String _lastLocation){
        lastLocation = _lastLocation;
    }

    public int getUserId(){
        return userId;
    }

    public void setUserId(int _userId){
        userId = _userId;
    }

    public String getQuota(){
        return quota;
    }

    public void setQuota(String _quota){
        quota = _quota;
    }
}
