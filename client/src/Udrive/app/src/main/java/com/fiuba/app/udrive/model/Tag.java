package com.fiuba.app.udrive.model;

public class Tag {
    private String tagName = null;
    private int tagId = 0;

    public Tag(String tagName, int tagId){
        this.tagName = tagName;
        this.tagId = tagId;
    }

    public String getTagName() {
        return tagName;
    }

    public void setTagName(String tagName) {
        this.tagName = tagName;
    }

    public int getTagId() {
        return tagId;
    }

    public void setTagId(int tagId) {
        this.tagId = tagId;
    }

}
