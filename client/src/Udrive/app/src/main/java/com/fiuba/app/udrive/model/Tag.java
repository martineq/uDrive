package com.fiuba.app.udrive.model;

public class Tag {
    private String tagName = null;

    public Tag(String tagName){
        this.tagName = tagName;
    }

    public String getTagName() {
        return tagName;
    }

    public void setTagName(String tagName) {
        this.tagName = tagName;
    }
}
