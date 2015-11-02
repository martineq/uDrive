package com.fiuba.app.udrive.network;

import android.content.Context;

import com.fiuba.app.udrive.model.GenericResult;
import com.fiuba.app.udrive.model.UserAccount;
import com.fiuba.app.udrive.model.UserData;

import retrofit.Callback;
import retrofit.RetrofitError;
import retrofit.client.Response;
import retrofit.http.Body;
import retrofit.http.POST;

public class UserService extends AbstractService {

    private interface UserServiceApi {

        @POST("/token")
        void getToken(@Body UserData userData, Callback<UserAccount> uAccountCb);

        @POST("/signup")
        void signUp(@Body UserData userData, Callback<GenericResult> result);
    }

    private UserServiceApi mUserServiceApi;

    public UserService(Context context) {
        super(context);
        this.mUserServiceApi = createService(UserServiceApi.class, null);
    }

    public void getToken(UserData userData, final ServiceCallback<UserAccount> uAccountCb){
        mUserServiceApi.getToken(userData, new Callback<UserAccount>() {
            @Override
            public void success(UserAccount userAccount, Response response) {
                uAccountCb.onSuccess(userAccount, response.getStatus());
            }

            @Override
            public void failure(RetrofitError error) {
                int status;
                if (error.getKind() == RetrofitError.Kind.NETWORK) {
                    status = 503;
                } else
                    status = error.getResponse().getStatus();
                uAccountCb.onFailure(error.getMessage(), status);
            }
        });
    }

    public void signUp(UserData userData, final ServiceCallback<GenericResult> result){
        mUserServiceApi.signUp(userData, new Callback<GenericResult>() {
            @Override
            public void success(GenericResult res, Response response) {
                result.onSuccess(res, response.getStatus());
            }

            @Override
            public void failure(RetrofitError error) {
                int status;
                if (error.getKind() == RetrofitError.Kind.NETWORK) {
                    status = 503;
                } else
                    status = error.getResponse().getStatus();
                result.onFailure(error.getMessage(), status);
            }
        });
    }

}
