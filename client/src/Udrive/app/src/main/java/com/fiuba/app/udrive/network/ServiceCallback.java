package com.fiuba.app.udrive.network;

public interface ServiceCallback<T> {
    void onSuccess(T object, int status);
    void onFailure(String message, int status);
}
