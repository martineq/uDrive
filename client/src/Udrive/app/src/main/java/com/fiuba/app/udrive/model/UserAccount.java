package com.fiuba.app.udrive.model;

import java.io.Serializable;

public class UserAccount extends UserData implements Serializable {
    private String token = null;
    private int id;

    public UserAccount(final String email, final String password, final String token, int id){
        super(email, password);
        this.token = token;
        this.id = id; // User ID
    }

    public String getToken(){
        return token;
    }

    public void setToken(String token){
        this.token = token;
    }

    public int getId(){
        return id;
    }

    public void setId(int id){
        this.id = id;
    }


}
