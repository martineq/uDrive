package com.fiuba.app.udrive.model;

import java.util.Date;

public class File {

    private Integer id;
    private String name;
    private Integer size;
    private Character type;
    private Boolean shared;
    private Date lastModDate;
    private Integer cantItems;

    public File(String name, Integer size, Character type, Boolean shared, Date lastModDate, Integer cantItems){
        this.name = name;
        this.size = size;
        this.type = type;
        this.shared = shared;
        this.lastModDate = lastModDate;
        this.cantItems = cantItems;
    }

    public Integer getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public Integer getSize() {
        return size;
    }

    public Character getType() {
        return type;
    }

    public Boolean getShared() {
        return shared;
    }

    public Date getLastModDate() {
        return lastModDate;
    }

    public Integer getCantItems() {
        return cantItems;
    }



}
