package com.fiuba.app.udrive.model;

import java.io.Serializable;

public class UserAccount extends UserData implements Serializable {
    private String token = null;
    private int userId;

    public UserAccount(final String email, final String _token, int _userId){
        super(email, null);
        token = _token;
        userId = _userId;
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
