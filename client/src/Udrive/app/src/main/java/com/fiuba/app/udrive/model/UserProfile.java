package com.fiuba.app.udrive.model;

/**
 * Created by eugenia on 20/09/15.
 */
public class UserProfile extends UserData {
    private String firstname;
    private String lastname;
    private String photo = null; // TODO: check this.
    private String lastLocation = null;
    private int id;

    public UserProfile(String email, String password, String firstname, String lastname, String photo, String lastLocation, int id){
        super(email, password);
        this.firstname = firstname;
        this.lastname = lastname;
        this.photo = photo;
        this.lastLocation = lastLocation;
        this.id = id;
    }

    public String getFirstname() {
        return firstname;
    }

    public void setFirstname(String firstname) {
        this.firstname = firstname;
    }

    public String getLastname() {
        return lastname;
    }

    public void setLastname(String lastname) {
        this.lastname = lastname;
    }

    public String getPhoto() {
        return photo;
    }

    public void setPhoto(String photo) {
        this.photo = photo;
    }

    public String getLastLocation() {
        return lastLocation;
    }

    public void setLastLocation(String lastLocation) {
        this.lastLocation = lastLocation;
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }
}
