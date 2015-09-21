package com.fiuba.app.udrive.rest.service;

import com.fiuba.app.udrive.model.UserAccount;
import com.fiuba.app.udrive.model.UserData;
import com.fiuba.app.udrive.rest.model.TestRest;

import retrofit.Callback;
import retrofit.http.Body;
import retrofit.http.GET;
import retrofit.http.POST;

/**
 * Created by eugenia on 13/09/15.
 */
public interface RestService {
    @GET("/token")
    void getToken(@Body UserData credentials, Callback<UserAccount> uAccountCb);
}
