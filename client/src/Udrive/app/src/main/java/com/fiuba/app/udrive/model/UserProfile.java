package com.fiuba.app.udrive.model;

public class UserProfile extends UserData {
    private String firstname;
    private String lastname;
    private String photo = null; // TODO: check this.
    private String lastLocation = null; // TODO: Mobile GPS usage
    private int userId = 0;
    private String quotaAvailable = null;
    private String quotaTotal = null;
    private String quotaUsagePercent = null;

    public UserProfile(String _email, String _password, String _firstname, String _lastname,
                       String _photo, String _lastLocation, int _userId, String _quotaTotal, String _quotaAvailable,
                       String _quotaUsagePercent){
        super(_email.toLowerCase(), Util.encodePassword(_password));
        firstname = _firstname.toLowerCase();
        lastname = _lastname.toLowerCase();
        photo = _photo;
        lastLocation = _lastLocation;
        userId = _userId;
        quotaTotal = _quotaTotal;
        quotaAvailable = _quotaAvailable;
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

    public String getQuotaTotal(){
        return quotaTotal;
    }

    public void setQuotaTotal(String _quotaTotal){
        quotaTotal = _quotaTotal;
    }

    public String getQuotaAvailable(){
        return quotaAvailable;
    }

    public void setQuotaAvailable(String _quotaAvailable){
        quotaAvailable = _quotaAvailable;
    }

    public String getQuotaUsagePercent(){
        return quotaUsagePercent;
    }

    public void setQuotaUsagePercent(String _quotaUsagePercent){
        quotaUsagePercent = _quotaUsagePercent;
    }
}
