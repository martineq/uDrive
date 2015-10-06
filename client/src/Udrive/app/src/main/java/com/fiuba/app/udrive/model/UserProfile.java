package com.fiuba.app.udrive.model;

public class UserProfile extends UserData {
    private String firstname;
    private String lastname;
    private String photo = null; // TODO: check this.
    private String lastLocation = null;
    private int userId;

    public UserProfile(String email, String firstname, String lastname, String photo, String lastLocation, int userId){
        super(email, null);
        this.firstname = firstname;
        this.lastname = lastname;
        this.photo = photo;
        this.lastLocation = lastLocation;
        this.userId = userId;
    }

    public String getFirstname(){
        return firstname;
    }

    public void setFirstname(String firstname){
        this.firstname = firstname;
    }

    public String getLastname(){
        return lastname;
    }

    public void setLastname(String lastname){
        this.lastname = lastname;
    }

    public String getPhoto(){
        return photo;
    }

    public void setPhoto(String photo){
        this.photo = photo;
    }

    public String getLastLocation(){
        return lastLocation;
    }

    public void setLastLocation(String lastLocation){
        this.lastLocation = lastLocation;
    }

    public int getUserId(){
        return userId;
    }

    public void setUserId(int userId){
        this.userId = userId;
    }
}
