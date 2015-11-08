package com.fiuba.app.udrive.network;

import java.io.FileOutputStream;
import java.lang.reflect.Type;
import retrofit.converter.ConversionException;
import retrofit.converter.Converter;
import retrofit.mime.TypedInput;
import retrofit.mime.TypedOutput;

public class FileOutputStreamConverter implements Converter {

    private String fullPath;

    public FileOutputStreamConverter(String fullPath) {
        this.fullPath = fullPath;
    }

    @Override
    public Object fromBody(TypedInput body, Type type) throws ConversionException {
        try {
            java.io.File file = new java.io.File(fullPath);
            FileOutputStream outputStream = new FileOutputStream(file);

            int read = 0;
            byte[] bytes = new byte[1024];

            while ((read = body.in().read(bytes)) != -1) {
                outputStream.write(bytes, 0, read);
            }
            return outputStream;
        } catch (Exception e) {
            throw new ConversionException(e);
        }
    }

    @Override
    public TypedOutput toBody(Object object) {
        return null;
    }
}

