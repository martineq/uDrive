package com.fiuba.app.udrive.model;

/**
 * The purpose of this class is to represent a code for
 * trivial results coming as HTTP Responses.
 */
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
