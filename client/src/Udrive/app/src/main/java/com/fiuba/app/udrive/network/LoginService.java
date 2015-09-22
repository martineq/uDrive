package com.fiuba.app.udrive.network;

import com.fiuba.app.udrive.model.UserAccount;
import com.fiuba.app.udrive.model.UserData;
import com.fiuba.app.udrive.network.AbstractService;
import com.fiuba.app.udrive.network.ServiceCallback;
import com.squareup.okhttp.OkHttpClient;

import retrofit.Callback;
import retrofit.RestAdapter;
import retrofit.RetrofitError;
import retrofit.client.OkClient;
import retrofit.client.Response;
import retrofit.http.Body;
import retrofit.http.GET;
import retrofit.http.POST;

public class LoginService extends AbstractService {

    private interface LoginServiceApi {
        @GET("/token/1") // After testing Supress /1 and decomment uData
        void getToken(/*@Body UserData uData, */Callback<UserAccount> uAccountCb);
    }

    private LoginServiceApi mLoginServiceApi;

    public LoginService() {
        this.mLoginServiceApi = createService(LoginServiceApi.class);
    }

    public void getToken(/*UserData uData, */final ServiceCallback<UserAccount> uAccountCb){
        mLoginServiceApi.getToken(/*uData, */new Callback<UserAccount>(){
            @Override
            public void success(UserAccount userAccount, Response response){
                uAccountCb.onSuccess(userAccount);
            }

            @Override
            public void failure(RetrofitError error){
                uAccountCb.onFailure(error.getMessage());
            }
        });
    }

}
