package com.fiuba.app.udrive.model;

import java.io.Serializable;

public class FolderData implements Serializable {
    private String dirName;

    public FolderData(String dirName){
        this.dirName = dirName;
    }

    public String getDirName() {
        return dirName;
    }

    public void setDirName(String name) {
        this.dirName = name;
    }

}

