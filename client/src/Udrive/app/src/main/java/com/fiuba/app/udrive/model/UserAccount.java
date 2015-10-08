package com.fiuba.app.udrive.model;

import java.io.Serializable;

public class UserAccount extends UserData implements Serializable {
    private String token = null;
    private int userId;

    public UserAccount(final String email, final String token, int userId){
        super(email, null);
        this.token = token;
        this.userId = userId; // User ID
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


}
