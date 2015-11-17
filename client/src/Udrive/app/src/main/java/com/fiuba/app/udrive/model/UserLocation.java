package com.fiuba.app.udrive.model;

public class UserLocation {
    private double GPSLatitude = 0.0;
    private double GPSLongitude = 0.0;

    public UserLocation(double GPSLatitude, double GPSLongitude) {
        this.GPSLatitude = GPSLatitude;
        this.GPSLongitude = GPSLongitude;
    }

    public double getGPSLatitude() {
        return GPSLatitude;
    }

    public void setGPSLatitude(double GPSLatitude) {
        this.GPSLatitude = GPSLatitude;
    }

    public double getGPSLongitude() {
        return GPSLongitude;
    }

    public void setGPSLongitude(double GPSLongitude) {
        this.GPSLongitude = GPSLongitude;
    }
}
