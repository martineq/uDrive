package com.fiuba.app.udrive.network;

import com.fiuba.app.udrive.BuildConfig;
import com.squareup.okhttp.OkHttpClient;

import retrofit.RequestInterceptor;
import retrofit.RestAdapter;
import retrofit.client.OkClient;

public abstract class AbstractService {

    private static RestAdapter.Builder builder = new RestAdapter.Builder()
            .setEndpoint(BuildConfig.BASE_URL)
            .setClient(new OkClient(new OkHttpClient()))
            .setLogLevel(BuildConfig.RETROFIT_LOGGING);


    protected <T> T createService(Class<T> service, final String token) {
        if (token != null){
            builder.setRequestInterceptor(new RequestInterceptor() {
                @Override
                public void intercept(RequestFacade request) {
                    request.addHeader("Accept", "application/json");
                    request.addHeader("Authorization", token);
                }
            });
        }
        return builder.build().create(service);
    }

}
