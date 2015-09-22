package com.fiuba.app.udrive.network;

import com.squareup.okhttp.OkHttpClient;

import retrofit.RestAdapter;
import retrofit.client.OkClient;

public abstract class AbstractService {
    private static final String BASE_URL = "http://quick-entities.herokuapp.com";
    private static RestAdapter restAdapter = new RestAdapter.Builder()
                                                    .setEndpoint(BASE_URL)
                                                    .setClient(new OkClient(new OkHttpClient()))
                                                    .setLogLevel(RestAdapter.LogLevel.FULL).build();

    protected <T> T createService(Class<T> service) {
        return restAdapter.create(service);
    }

}
