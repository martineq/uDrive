package com.fiuba.app.udrive.network;

import android.content.Context;

import com.fiuba.app.udrive.model.GenericResult;
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

public class SignUpService extends AbstractService {

    private interface SignUpServiceApi {
        @POST("/signup")
        void signUp(@Body UserData userData, Callback<GenericResult> result);
    }

    private SignUpServiceApi mSignUpServiceApi;

    public SignUpService(Context context) {
        super(context);
        mSignUpServiceApi = createService(SignUpServiceApi.class, null);
    }

    public void signUp(UserData userData, final ServiceCallback<GenericResult> result){
        mSignUpServiceApi.signUp(userData, new Callback<GenericResult>(){
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
