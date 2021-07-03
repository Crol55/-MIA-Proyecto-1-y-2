package com.example.cliente;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

import java.io.IOException;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.MultipartBody;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.RequestBody;
import okhttp3.Response;


public class MainActivity extends AppCompatActivity implements View.OnClickListener{

    EditText et_usuario ;
    EditText et_contrasena ;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        findViewById(R.id.button).setOnClickListener(this);
        et_usuario = findViewById(R.id.editText);
        et_contrasena = findViewById(R.id.editText2);
    }


    @Override
    public void onClick(View view) {

        if(view.getId() == R.id.button){
            System.out.println("si ya es hora de mandar la peticion post al servidor");

            String str = et_usuario.getText().toString();
            String str2 = et_contrasena.getText().toString();

            OkHttpClient cliente = new OkHttpClient();
            // String url = "http://192.168.43.3:8000/api/post/";
            String url   = "http://192.168.1.12:5000/prueba";
            RequestBody rb = new MultipartBody.Builder().setType(MultipartBody.FORM)
                    .addFormDataPart("username", str)
                    .addFormDataPart("contrasena", str2)
                    //.addFormDataPart("file", imagen_servidor.getName(), RequestBody.create(MediaType.parse("image/png"), imagen_servidor))
                    .build();

            Request request = new Request.Builder().url(url).post(rb).build();

            cliente.newCall(request).enqueue(new Callback() {
                @Override
                public void onFailure(Call call, IOException e) {
                    e.printStackTrace();
                }

                @Override
                public void onResponse(Call call, Response response) throws IOException {

                    System.out.println("paso algo aqui?");
                    if (response.isSuccessful()) {

                        final String respuesta = response.body().string();
                        System.out.println(respuesta);
                        MainActivity.this.runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                               // et_usuario.setVisibility(View.INVISIBLE);
                                //et_contrasena.setVisibility(View.INVISIBLE);
                                alamierda(respuesta);
                            }
                        });

                    } else {
                        System.out.println("error deplano?");
                    }
                }
            });


        }
    }

    public void alamierda(String tipo_usuario)
    {
        System.out.println("Que putas===" + tipo_usuario);
        if(tipo_usuario.equals("3") )
        {
            Intent intent =  new Intent(this, usuario.class);
            startActivity(intent);

        }else if(tipo_usuario.equals("1") ){
            Intent intent =  new Intent(this, admin.class);
            startActivity(intent);

        }else{
            Toast.makeText(this, "Credenciales Incorrectas al iniciar sesion", Toast.LENGTH_SHORT).show();
        }


    }

}
