package com.fiuba.app.udrive.model;

import java.io.Serializable;

public class FolderData implements Serializable {
    private String name;

    public FolderData(String name){
        this.name = name;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }
}

