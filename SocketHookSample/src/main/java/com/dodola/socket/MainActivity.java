package com.dodola.socket;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Toast;

public class MainActivity extends Activity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        findViewById(R.id.button).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                SocketHook.enableSocketHook();
                Toast.makeText(MainActivity.this, "开启成功", Toast.LENGTH_SHORT).show();
            }
        });

        findViewById(R.id.newrequest).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        Log.e("HOOOOOOOOK", "respond:" + getURLResponse("https://www.baidu.com"));

                    }
                }).start();
            }

        });
    }


    private String getURLResponse(String urlString){
        HttpURLConnection conn = null;
        InputStream is = null;
        StringBuffer stringBuffer = new StringBuffer();
        try {
            URL url = new URL(urlString);
            conn = (HttpURLConnection)url.openConnection();
            conn.setRequestMethod("GET");
            is = conn.getInputStream();
            InputStreamReader isr = new InputStreamReader(is);
            BufferedReader bufferReader = new BufferedReader(isr);
            String inputLine;
            while((inputLine = bufferReader.readLine()) != null){
                stringBuffer.append(inputLine).append("\n");
            }

        } catch (Throwable e) {
            e.printStackTrace();
        } finally {
            if(is != null){
                try {
                    is.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            if(conn != null){
                conn.disconnect();
            }
        }

        return stringBuffer.toString();
    }



}

