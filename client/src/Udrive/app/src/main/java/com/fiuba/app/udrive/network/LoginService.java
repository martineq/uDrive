package com.fiuba.app.udrive.network;

import android.content.Context;

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
import retrofit.http.Path;

public class LoginService extends AbstractService {

    private interface LoginServiceApi {
        @POST("/token")
        void getToken(@Body UserData userData, Callback<UserAccount> uAccountCb);
    }

    private LoginServiceApi mLoginServiceApi;

    public LoginService(Context context) {
        super(context);
        this.mLoginServiceApi = createService(LoginServiceApi.class, null);
    }

    public void getToken(UserData userData, final ServiceCallback<UserAccount> uAccountCb){
        mLoginServiceApi.getToken(userData, new Callback<UserAccount>(){
            @Override
            public void success(UserAccount userAccount, Response response){
                uAccountCb.onSuccess(userAccount, response.getStatus());
            }

            @Override
            public void failure(RetrofitError error){
                int status;
                if (error.getKind() == RetrofitError.Kind.NETWORK) {
                    status = 503;
                } else
                    status = error.getResponse().getStatus();
                uAccountCb.onFailure(error.getMessage(), status);
            }
        });
    }

}
