package com.fiuba.app.udrive.model;


import java.io.Serializable;

public class UserFullName implements Serializable{
    private String firstName;
    private String lastName;

    public UserFullName(String firstname, String lastname){
        this.firstName = firstname;
        this.lastName = lastname;
    }

    public String getFirstname() {
        return firstName;
    }

    public void setFirstname(String firstname) {
        this.firstName = firstname;
    }

    public String getLastname() {
        return lastName;
    }

    public void setLastname(String lastname) {
        this.lastName = lastname;
    }
}