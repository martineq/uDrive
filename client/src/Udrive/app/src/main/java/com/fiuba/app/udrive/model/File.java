package com.fiuba.app.udrive.model;

/**
 * Represents a File in the app context with all its required attributes
 */
public class File {

    private Integer id;
    private String name;
    private Integer size;
    private Character type;
    private Boolean shared;
    private String lastModDate;
    private Integer cantItems;
    private Integer userOwner;
    private Integer lastVersion;

    public File(String name, Integer size, Character type, Boolean shared, String lastModDate, Integer cantItems, Integer userOwner){
        this.name = name;
        this.size = size;
        this.type = type;
        this.shared = shared;
        this.lastModDate = lastModDate;
        this.cantItems = cantItems;
        this.userOwner = userOwner;

    }

    public Integer getId() {
        return id;
    }

    public void setId(Integer id) {
        this.id = id;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
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

    /**
     * Checks the type of the entity
     * @return true if it is a directory, false otherwise.
     */
    public boolean isDir(){
        return this.getType() == 'd' ? true : false;
    }

    public Integer getUserOwner() {
        return userOwner;
    }

    public Integer getLastVersion() {
        return lastVersion;
    }


}