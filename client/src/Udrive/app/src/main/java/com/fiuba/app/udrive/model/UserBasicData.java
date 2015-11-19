package com.fiuba.app.udrive.model;


import java.io.Serializable;

public class UserBasicData extends UserFullName implements Serializable {
    private String email = null;

    public UserBasicData(String firstname, String lastname, String email){
        super(firstname,lastname);
        this.email = email;
    }

    public String getEmail() {
        return email;
    }

    public void setEmail(String email) {
        this.email = email;
    }
}

