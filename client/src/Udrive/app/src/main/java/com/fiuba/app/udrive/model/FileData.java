package com.fiuba.app.udrive.model;


import java.util.ArrayList;

public class FileData {
    private UserBasicData owner;
    private File file;
    private UserBasicData updatedBy;
    private double updatedFromLatitude;
    private double updatedFromLongitude;
    private ArrayList<UserBasicData> collaborators;
    private ArrayList<Tag> tags;

    public FileData(UserBasicData owner, File file, UserBasicData updatedBy,
                    double updatedFromLatitude, double updatedFromLongitude,
                    ArrayList<UserBasicData> collaborators, ArrayList<Tag> tags) {
        this.owner = owner;
        this.file = file;
        this.updatedBy = updatedBy;
        this.updatedFromLatitude = updatedFromLatitude;
        this.updatedFromLongitude = updatedFromLongitude;
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

    public double getUpdatedFromLatitude() {
        return updatedFromLatitude;
    }

    public void setUpdatedFromLatitude(double updatedFromLatitude) {
        this.updatedFromLatitude = updatedFromLatitude;
    }

    public double getUpdatedFromLongitude() {
        return updatedFromLongitude;
    }

    public void setUpdatedFromLongitude(double updatedFromLongitude) {
        this.updatedFromLongitude = updatedFromLongitude;
    }

    public ArrayList<UserBasicData> getCollaborators() {
        return collaborators;
    }

    public void setCollaborators(ArrayList<UserBasicData> collaborators) {
        this.collaborators = collaborators;
    }

    public ArrayList<Tag> getTags() {
        return tags;
    }

    public void setTags(ArrayList<Tag> tags) {
        this.tags = tags;
    }
}
