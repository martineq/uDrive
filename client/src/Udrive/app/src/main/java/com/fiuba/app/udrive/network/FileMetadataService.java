package com.fiuba.app.udrive.network;

import android.content.Context;

import com.fiuba.app.udrive.model.GenericResult;
import com.fiuba.app.udrive.model.MyPhoto;
import com.fiuba.app.udrive.model.Tag;
import com.fiuba.app.udrive.model.UserAccount;
import com.fiuba.app.udrive.model.UserData;
import com.fiuba.app.udrive.model.UserFullName;
import com.fiuba.app.udrive.model.UserProfile;

import java.util.List;

import retrofit.Callback;
import retrofit.RetrofitError;
import retrofit.client.Response;
import retrofit.http.Body;
import retrofit.http.DELETE;
import retrofit.http.GET;
import retrofit.http.PUT;
import retrofit.http.Path;

public class FileMetadataService extends AbstractService {

    private interface FileTagServiceApi {

        // Gets the user's tag set
        @GET("/filetags/{userId}")
        void getTags(@Path("userId") int userId, Callback<List<Tag>> tags);

        // Updates tag set for the given user ID
        @PUT("/filetags/{userId}")
        void addTag(@Path("userId") int userId, @Body Tag tag, Callback<Tag> tagNew);

        // Deletes a tag specified by the tag ID for the given user ID
        @DELETE("/filetags/tags/{tagId}/users/{userId}")
        void deleteTag(@Path("tagId") int tagId, @Path("userId") int userId, Callback<GenericResult> result);

        // Updates the tag set for the given user ID and file ID
        @PUT("/filetags/files/{fileId}/users/{userId}")
        void updateFileTags(@Path("fileId") int fileId, @Path("userId") int userId, @Body List<Tag> tagList,
                        Callback<GenericResult> result);

    }

    private FileTagServiceApi mFileTagServiceApi;

    public FileMetadataService(Context context) {
        super(context);
        this.mFileTagServiceApi = createService(FileTagServiceApi.class, null);
    }

    public FileMetadataService(String token, Context context) {
        super(context);
        this.mFileTagServiceApi = createService(FileTagServiceApi.class, token);
    }

    public void getTags(int userId, final ServiceCallback<List<Tag>> tags){
        mFileTagServiceApi.getTags(userId, new Callback<List<Tag>>() {
            @Override
            public void success(List<Tag> tagList, Response response) {
                tags.onSuccess(tagList, response.getStatus());
            }

            @Override
            public void failure(RetrofitError error) {
                int status;
                if (error.getKind() == RetrofitError.Kind.NETWORK) {
                    status = 503;
                } else
                    status = error.getResponse().getStatus();
                tags.onFailure(error.getMessage(), status);
            }
        });
    }


    public void addTag(int userId, Tag tag, final ServiceCallback<Tag> tagNew){
        mFileTagServiceApi.addTag(userId, tag, new Callback<Tag>() {
            @Override
            public void success(Tag tNew, Response response) {
                tagNew.onSuccess(tNew, response.getStatus());
            }

            @Override
            public void failure(RetrofitError error) {
                int status;
                if (error.getKind() == RetrofitError.Kind.NETWORK) {
                    status = 503;
                } else
                    status = error.getResponse().getStatus();
                tagNew.onFailure(error.getMessage(), status);
            }
        });
    }

    public void deleteTag(int tagId, int userId, final ServiceCallback<GenericResult> result){
        mFileTagServiceApi.deleteTag(tagId, userId, new Callback<GenericResult>() {
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

    // Passes the final tag list corresponding to a file.
    // So the server just updates the tag list associated to a file instead of removing one by one of them.
    public void updateFileTags(int fileId, int userId, @Body List<Tag> tagList,
                        final ServiceCallback<GenericResult> result){
        mFileTagServiceApi.updateFileTags(fileId, userId, tagList, new Callback<GenericResult>() {
            @Override
            public void success(GenericResult genericResult, Response response) {
                result.onSuccess(genericResult, response.getStatus());
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
