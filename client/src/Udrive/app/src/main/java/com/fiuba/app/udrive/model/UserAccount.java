package com.fiuba.app.udrive.model;

import java.io.Serializable;

public class UserAccount extends UserData implements Serializable {
    private String token = null;
    private int userId;
    private String quotaAvailable = null;

     public UserAccount(final String email, final String _token, int _userId, String _quotaAvailable){
        super(email, null);
        token = _token;
        userId = _userId;
        quotaAvailable = _quotaAvailable;
    }

    public String getToken(){
        return token;
    }

    public void setToken(String token){
        this.token = token;
    }

    public int getUserId(){
        return userId;
    }

    public void setUserId(int userId){
        this.userId = userId;
    }

    public String getQuotaAvailable() {
        return quotaAvailable;
    }

    public void setQuotaAvailable(String quotaAvailable) {
        this.quotaAvailable = quotaAvailable;
    }
}
