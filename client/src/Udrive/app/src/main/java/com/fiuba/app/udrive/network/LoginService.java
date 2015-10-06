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
        @GET("/token")
        void getToken(@Body UserData uData, Callback<UserAccount> uAccountCb);
    }

    private LoginServiceApi mLoginServiceApi;

    public LoginService() {
        this.mLoginServiceApi = createService(LoginServiceApi.class, null);
    }

    public void getToken(UserData uData, final ServiceCallback<UserAccount> uAccountCb){
        mLoginServiceApi.getToken(uData, new Callback<UserAccount>(){
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
