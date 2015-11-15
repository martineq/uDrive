package com.fiuba.app.udrive.model;


import java.io.Serializable;

public class Collaborator implements Serializable {

    private Integer id;
    private String mFirstName;
    private String mLastName;
    private String mEmail;

    public Collaborator(String firstName,String lastName, String eMail){
        this.mFirstName = firstName;
        this.mLastName = lastName;
        this.mEmail = eMail;
    }

    public String getmFirstName() {
        return mFirstName;
    }

    public void setmFirstName(String mFirstName) {
        this.mFirstName = mFirstName;
    }

    public String getmLastName() {
        return mLastName;
    }

    public void setmLastName(String mLastName) {
        this.mLastName = mLastName;
    }

    public String getmEmail() {
        return mEmail;
    }

    public void setmEmail(String mEmail) {
        this.mEmail = mEmail;
    }

    public Integer getId() {
        return id;
    }

    public void setId(Integer id) {
        this.id = id;
    }

}
