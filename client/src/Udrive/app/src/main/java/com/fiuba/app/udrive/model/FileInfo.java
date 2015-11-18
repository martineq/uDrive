package com.fiuba.app.udrive.model;


import java.io.Serializable;
import java.util.ArrayList;

public class FileInfo implements Serializable {
    private UserBasicData owner;
    private File file;
    private UserBasicData updatedBy;
    private ArrayList<UserBasicData> collaborators;
    //private ArrayList<Tag> tags;
    private String tags = null;

    public FileInfo(UserBasicData owner, File file, UserBasicData updatedBy,
                    double updatedFromLatitude, double updatedFromLongitude,
                    ArrayList<UserBasicData> collaborators, /*ArrayList<Tag>*/ String tags) {
        this.owner = owner;
        this.file = file;
        this.updatedBy = updatedBy;
        this.collaborators = collaborators;
        this.tags = tags;
    }

    public UserBasicData getOwner() {
        return owner;
    }

    public void setOwner(UserBasicData owner) {
        this.owner = owner;
    }

    public File getFile() {
        return file;
    }

    public void setFile(File file) {
        this.file = file;
    }

    public UserBasicData getUpdatedBy() {
        return updatedBy;
    }

    public void setUpdatedBy(UserBasicData updatedBy) {
        this.updatedBy = updatedBy;
    }

    public ArrayList<UserBasicData> getCollaborators() {
        return collaborators;
    }

    public void setCollaborators(ArrayList<UserBasicData> collaborators) {
        this.collaborators = collaborators;
    }

    public /*ArrayList<Tag>*/ String getTags() {
        return tags;
    }

    public void setTags(/*ArrayList<Tag>*/ String tags) {
        this.tags = tags;
    }
}
