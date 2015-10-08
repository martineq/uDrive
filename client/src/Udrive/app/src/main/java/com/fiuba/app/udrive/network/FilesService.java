package com.fiuba.app.udrive.network;
import com.fiuba.app.udrive.model.File;
import java.util.List;
import retrofit.Callback;
import retrofit.RetrofitError;
import retrofit.client.Response;
import retrofit.http.GET;
import retrofit.http.Path;

public class FilesService extends AbstractService {

    private interface FilesServiceApi {
        @GET("/info/users/{userId}/dir/{dirId}")
        void getFiles(@Path("userId") int userId, @Path("dirId") int dirId, Callback<List<File>> files);
    }

    private FilesServiceApi mFilesServiceApi;

    public FilesService(final String token) {
        this.mFilesServiceApi = createService(FilesServiceApi.class, token);
    }

    public void getFiles(int userId, int dirId, final ServiceCallback<List<File>> serviceCallback) {
        mFilesServiceApi.getFiles(userId, dirId, new Callback<List<File>>() {
            @Override
            public void success(List<File> files, Response response) {
                serviceCallback.onSuccess(files, response.getStatus());
            }

            @Override
            public void failure(RetrofitError error) {
                int status;
                if (error.getKind() == RetrofitError.Kind.NETWORK) {
                    status = 503;
                } else
                    status = error.getResponse().getStatus();
                serviceCallback.onFailure(error.getMessage(), status);
            }
        });
    }
}
