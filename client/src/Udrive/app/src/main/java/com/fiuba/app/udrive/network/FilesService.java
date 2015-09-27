package com.fiuba.app.udrive.network;
import com.fiuba.app.udrive.model.File;
import java.util.List;
import retrofit.Callback;
import retrofit.RetrofitError;
import retrofit.client.Response;
import retrofit.http.GET;

public class FilesService extends AbstractService {

    private interface FilesServiceApi {
        @GET("/files")
        void getFiles(Callback<List<File>> files);
    }

    private FilesServiceApi mFilesServiceApi;

    public FilesService(final String token) {
        this.mFilesServiceApi = createService(FilesServiceApi.class, token);
    }

    public void getFiles(final ServiceCallback<List<File>> serviceCallback) {
        mFilesServiceApi.getFiles(new Callback<List<File>>() {
            @Override
            public void success(List<File> files, Response response) {
                serviceCallback.onSuccess(files);
            }

            @Override
            public void failure(RetrofitError error) {
                serviceCallback.onFailure(error.getMessage());
            }
        });
    }
}
