package com.fiuba.app.udrive.model;

import java.util.Date;
import java.text.SimpleDateFormat;

public class File {

    private Integer id;
    private String name;
    private Integer size;
    private Character type;
    private Boolean shared;
    private String lastModDate;
    private Integer cantItems;

    public File(String name, Integer size, Character type, Boolean shared, String lastModDate, Integer cantItems){
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

    public String getLastModDate() {
        return lastModDate;
    }

    public Integer getCantItems() {
        return cantItems;
    }

    public String getLastModDateFormated(){
        //String DATE_FORMAT = "dd/MM/yyyy";
        //SimpleDateFormat sdf = new SimpleDateFormat(DATE_FORMAT);
        //sdf.format(this.getLastModDate());
        return getLastModDate();
    }

    public boolean isFile(){
        return !isDir();
    }

    public boolean isDir(){
        return this.getType() == 'd' ? true : false;
    }


}