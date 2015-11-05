package com.fiuba.app.udrive.network;
import android.content.Context;

import com.fiuba.app.udrive.model.File;
import com.fiuba.app.udrive.model.FolderData;

import java.net.ContentHandler;
import java.util.List;
import retrofit.Callback;
import retrofit.RetrofitError;
import retrofit.client.Response;
import retrofit.http.Body;
import retrofit.http.GET;
import retrofit.http.Multipart;
import retrofit.http.POST;
import retrofit.http.Part;
import retrofit.http.Path;
import retrofit.mime.TypedFile;

public class FilesService extends AbstractService {

    private interface FilesServiceApi {
        @GET("/info/users/{userId}/dir/{dirId}")
        void getFiles(@Path("userId") int userId, @Path("dirId") int dirId, Callback<List<File>> files);


        @Multipart
        @POST("/file/users/{userId}/dir/{dirId}")
        void upload(@Path("userId") int userId,
                    @Path("dirId") int dirId,
                    @Part("file") TypedFile file,
                    @Part("fileName") String name,
                    Callback<List<File>> files);

        @POST("/users/{userId}/dir/{dirId}")
        void addFolder(@Path("userId") int userId,
                       @Path("dirId") int dirId,
                       @Body FolderData folderData,
                       Callback<List<File>> files);
    }

    private FilesServiceApi mFilesServiceApi;

    public FilesService(final String token, Context context) {
        super(context);
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

    public void upload(int userId, int dirId,String filePath, final ServiceCallback<List<File>> serviceCallback) {
        try{
            java.io.File file = new java.io.File(filePath);
            TypedFile typedFile = new TypedFile("multipart/form-data", file);
            String  name = file.getName();
            mFilesServiceApi.upload(userId,dirId,typedFile,name, new Callback<List<File>>() {
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
        }catch (Exception e){
            serviceCallback.onFailure("No se pudo cargar el archivo",1);
        }

    }

    public void addFolder(int userId, int dirId,String folderName, final ServiceCallback<List<File>> serviceCallback) {
        try{
            FolderData folderData = new FolderData(folderName);
            mFilesServiceApi.addFolder(userId, dirId, folderData, new Callback<List<File>>() {
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
        }catch (Exception e){
            serviceCallback.onFailure("No se pudo crear la carpeta",1);
        }

    }
}
