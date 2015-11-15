package com.fiuba.app.udrive.model;

import java.io.Serializable;

public class UserProfile extends UserData implements Serializable {
    private String firstname;
    private String lastname;
    private String photo = null; // TODO: check this.
    private double GPSLatitude;
    private double GPSLongitude;
    private int userId = 0;
    private String quotaUsed = null;
    private String quotaTotal = null;
    private String quotaUsagePercent = null;

    public UserProfile(String _email, String _password, String _firstname, String _lastname,
                       String _photo, double _GPSLatitude, double _GPSLongitude,
                       int _userId, String _quotaTotal, String _quotaAvailable,
                       String _quotaUsagePercent){
        super(_email.toLowerCase(), Util.encodePassword(_password));
        firstname = _firstname.toLowerCase();
        lastname = _lastname.toLowerCase();
        photo = _photo;
        GPSLatitude = _GPSLatitude;
        GPSLongitude = _GPSLongitude;
        userId = _userId;
        quotaTotal = _quotaTotal;
        quotaUsed = _quotaAvailable;
        quotaUsagePercent = _quotaUsagePercent;
    }

    public String getFirstname(){
        return firstname;
    }

    public void setFirstname(String _firstname){
        firstname = _firstname.toLowerCase();
    }

    public String getLastname(){
        return lastname;
    }

    public void setLastname(String _lastname){
        lastname = _lastname.toLowerCase();
    }

    public String getPhoto(){
        return photo;
    }

    public void setPhoto(String _photo){
        photo = _photo;
    }

    public double getGPSLatitude(){
        return GPSLatitude;
    }

    public double getGPSLongitude(){
        return GPSLongitude;
    }

    public void setGPSLatitude(double GPSLatitude) {
        this.GPSLatitude = GPSLatitude;
    }

    public void setGPSLongitude(double GPSLongitude) {
        this.GPSLongitude = GPSLongitude;
    }

    public int getUserId(){
        return userId;
    }

    public void setUserId(int _userId){
        userId = _userId;
    }

    public String getQuotaTotal(){
        return quotaTotal;
    }

    public void setQuotaTotal(String _quotaTotal){
        quotaTotal = _quotaTotal;
    }

    public String getQuotaUsed(){
        return quotaUsed;
    }

    public void setQuotaUsed(String _quotaUsed){
        quotaUsed = _quotaUsed;
    }

    public String getQuotaUsagePercent(){
        return quotaUsagePercent;
    }

    public void setQuotaUsagePercent(String _quotaUsagePercent){
        quotaUsagePercent = _quotaUsagePercent;
    }
}
