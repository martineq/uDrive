package com.fiuba.app.udrive.network;

public interface ServiceCallback<T> {
    void onSuccess(T object);
    void onFailure(String message);
}
