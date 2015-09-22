package com.fiuba.app.udrive.model;

import java.io.Serializable;

public class UserData implements Serializable {
    private String email;
    private String password;

    public UserData(String email, String password){
        this.email = email;
        this.password = password;
    }

    public String getEmail(){
        return email;
    }

    public void setEmail(String email){
        this.email = email;
    }

    public String getPassword(){
        return password;
    }

    public void setPassword(String password){
        this.password = password;
    }
}

