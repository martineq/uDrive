package com.fiuba.app.udrive.model;

/**
 * Created by eugenia on 20/09/15.
 */
public class UserData {
    private String email;
    private String password;

    public UserData(String email, String password){
        this.email = email;
        this.password = password;
    }

    public String getEmail() {
        return email;
    }

    public void setEmail(String email) {
        this.email = email;
    }

    public String getPassword() {
        return password;
    }

    public void setPassword(String password) {
        this.password = password;
    }
}

