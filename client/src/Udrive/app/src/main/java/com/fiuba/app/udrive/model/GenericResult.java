package com.fiuba.app.udrive.model;

public class GenericResult {
    private int resultCode = 0;

    public GenericResult(int _resultCode){
        resultCode = _resultCode;
    }

    public int getResultCode(){
        return resultCode;
    }

    public void setResultCode(int _resultCode){
        resultCode = _resultCode;
    }

}
