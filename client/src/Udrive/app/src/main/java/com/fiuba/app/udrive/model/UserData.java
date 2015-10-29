package com.fiuba.app.udrive.model;

import java.io.Serializable;

public class UserData implements Serializable {
    private String email;
    private String password;

    public UserData(String _email, String _password){
        email = _email.toLowerCase();
        password = _password;
    }

    public String getEmail(){
        return email;
    }

    public void setEmail(String _email){
        email = _email.toLowerCase();
    }

    public String getPassword(){
        return password;
    }

    public void setPassword(String _password){
        password = _password;
    }
}

