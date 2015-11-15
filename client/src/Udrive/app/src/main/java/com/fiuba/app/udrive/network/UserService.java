package com.fiuba.app.udrive.network;

import android.content.Context;
import android.telecom.Call;

import com.fiuba.app.udrive.model.File;
import com.fiuba.app.udrive.model.GenericResult;
import com.fiuba.app.udrive.model.MyPhoto;
import com.fiuba.app.udrive.model.UserAccount;
import com.fiuba.app.udrive.model.UserData;
import com.fiuba.app.udrive.model.UserFullName;
import com.fiuba.app.udrive.model.UserProfile;

import java.util.List;

import retrofit.Callback;
import retrofit.RetrofitError;
import retrofit.client.Response;
import retrofit.http.Body;
import retrofit.http.GET;
import retrofit.http.POST;
import retrofit.http.PUT;
import retrofit.http.Path;

public class UserService extends AbstractService {

    private interface UserServiceApi {

        @POST("/token")
        void getToken(@Body UserData userData, Callback<UserAccount> uAccountCb);

        @POST("/signup")
        void signUp(@Body UserData userData, Callback<GenericResult> result);

        @GET("/profile/{userId}")
        void getProfile(@Path("userId") int userId, Callback<UserProfile> uProfile);

        @PUT("/photo/{userId}")
        void updatePhoto(@Path("userId") int userId, @Body MyPhoto photo, Callback<GenericResult> result);

        @PUT("/userfullname/{userId}")
        void updateFullName(@Path("userId") int userId, @Body UserFullName uFullName, Callback<GenericResult> result);

    }

    private UserServiceApi mUserServiceApi;

    public UserService(Context context) {
        super(context);
        this.mUserServiceApi = createService(UserServiceApi.class, null);
    }

    public UserService(String token, Context context) {
        super(context);
        this.mUserServiceApi = createService(UserServiceApi.class, token);
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

    public void getProfile(int userId, final ServiceCallback<UserProfile> uProfile){
        mUserServiceApi.getProfile(userId, new Callback<UserProfile>() {
            @Override
            public void success(UserProfile userProfile, Response response) {
                uProfile.onSuccess(userProfile, response.getStatus());
            }

            @Override
            public void failure(RetrofitError error) {
                int status;
                if (error.getKind() == RetrofitError.Kind.NETWORK) {
                    status = 503;
                } else
                    status = error.getResponse().getStatus();
                uProfile.onFailure(error.getMessage(), status);
            }
        });
    }

    public void updatePhoto(int userId, MyPhoto photo, final ServiceCallback<GenericResult> res){
        mUserServiceApi.updatePhoto(userId, photo, new Callback<GenericResult>() {
            @Override
            public void success(GenericResult result, Response response) {
                res.onSuccess(result, response.getStatus());
            }

            @Override
            public void failure(RetrofitError error) {
                int status;
                if (error.getKind() == RetrofitError.Kind.NETWORK) {
                    status = 503;
                } else
                    status = error.getResponse().getStatus();
                res.onFailure(error.getMessage(), status);
            }
        });
    }

    public void updateFullName(int userId, UserFullName userFullName, final ServiceCallback<GenericResult> res){
        mUserServiceApi.updateFullName(userId, userFullName, new Callback<GenericResult>() {
            @Override
            public void success(GenericResult result, Response response) {
                res.onSuccess(result, response.getStatus());
            }

            @Override
            public void failure(RetrofitError error) {
                int status;
                if (error.getKind() == RetrofitError.Kind.NETWORK) {
                    status = 503;
                } else
                    status = error.getResponse().getStatus();
                res.onFailure(error.getMessage(), status);
            }
        });
    }

}
