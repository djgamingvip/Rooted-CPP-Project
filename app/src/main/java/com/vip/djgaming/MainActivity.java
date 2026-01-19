package com.vip.djgaming;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Typeface;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.Settings;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;
import android.content.SharedPreferences;
import android.graphics.Color;

import com.google.firebase.auth.FirebaseAuth;
import com.google.firebase.database.ChildEventListener;
import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import java.text.SimpleDateFormat;
import java.util.Date;

public class MainActivity extends AppCompatActivity {
    private RelativeLayout background;
    private LinearLayout back;
    private LinearLayout linear1;
    private LinearLayout linear2;
    private TextView textview1;
    private TextView textview2;
    private EditText etUsername;
    private EditText etPassword;
    private Button btnLogin;

    private FirebaseDatabase firebase = FirebaseDatabase.getInstance("https://injector-admin-cec7d-default-rtdb.firebaseio.com");
    private DatabaseReference djgamingvip = firebase.getReference("djgamingvip");
    private ChildEventListener userdatachildlistener;
    private SharedPreferences save;
    private static final int ACTION_MANAGE_OVERLAY_PERMISSION_REQUEST_CODE = 1;

    public void checkOverlayPermission() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            if (!Settings.canDrawOverlays(this)) {
                Intent intent = new Intent(Settings.ACTION_MANAGE_OVERLAY_PERMISSION,
                        Uri.parse("package:" + getPackageName()));
                startActivityForResult(intent, ACTION_MANAGE_OVERLAY_PERMISSION_REQUEST_CODE);
            }
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        initialize(savedInstanceState);
        GlowingTriangleView(back);
        requestStoragePermission();

        etUsername.setText(save.getString("user",""));
        etPassword.setText(save.getString("pass",""));

        btnLogin.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                login();
            }
        });

        etUsername.addTextChangedListener(new TextWatcher() {
            @Override
            public void afterTextChanged(Editable editable) {
                textview2.setText("");
            }

            @Override
            public void beforeTextChanged(CharSequence charSequence, int i, int i1, int i2) {

            }

            @Override
            public void onTextChanged(CharSequence charSequence, int i, int i1, int i2) {

            }
        });

        etPassword.addTextChangedListener(new TextWatcher() {
            @Override
            public void afterTextChanged(Editable editable) {
                textview2.setText("");
            }

            @Override
            public void beforeTextChanged(CharSequence charSequence, int i, int i1, int i2) {

            }

            @Override
            public void onTextChanged(CharSequence charSequence, int i, int i1, int i2) {

            }
        });

        textview1.setTypeface(Typeface.createFromAsset(getAssets(), "font_style.ttf"), Typeface.BOLD);
        textview2.setTypeface(Typeface.createFromAsset(getAssets(), "sansation_regular.ttf"), Typeface.BOLD);
    }

    private void initialize(Bundle savedInstanceState) {
        background = findViewById(R.id.background);
        back = findViewById(R.id.back);
        linear1 = findViewById(R.id.linear1);
        linear2 = findViewById(R.id.linear2);
        textview1 = findViewById(R.id.textview1);
        textview2 = findViewById(R.id.textview2);
        etUsername = findViewById(R.id.etUsername);
        etPassword = findViewById(R.id.etPassword);
        btnLogin = findViewById(R.id.btnLogin);
        save = getSharedPreferences("save", MODE_PRIVATE);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            Window window = getWindow();
            window.addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
            window.setStatusBarColor(Color.parseColor("#FF000000"));
        }
    }

    private void GlowingTriangleView(android.view.ViewGroup layout) {
        GlowingTriangleParticles.addToLayout(this, layout);
    }

    private void login() {
        String username = etUsername.getText().toString();
        String password = etPassword.getText().toString();
        if (username.isEmpty() || password.isEmpty()) {
            Toast.makeText(MainActivity.this, "Please enter username and password", Toast.LENGTH_SHORT).show();
        } else  {
            djgamingvip.child(username).addListenerForSingleValueEvent(new ValueEventListener() {
                @Override
                public void onDataChange(@NonNull DataSnapshot snapshot) {
                    if (snapshot.exists()) {
                        String user = snapshot.child("username").getValue(String.class);
                        String pass = snapshot.child("password").getValue(String.class);
                        String rgtime = snapshot.child("rgtime").getValue(String.class);
                        String validity = snapshot.child("validity").getValue(String.class);
                        save.edit().putString("user",user).commit();
                        save.edit().putString("pass",pass).commit();
                        save.edit().putString("rgtime",rgtime).commit();
                        save.edit().putString("validity",validity).commit();
                        if (user.equals(username) && pass.equals(password)) {
                            if(snapshot.child("status").getValue(String.class).equals("active")) {
                                SimpleDateFormat sdf = new SimpleDateFormat("dd-MM-yyyy HH:mm");
                                Date now = new Date();
                                try {
                                    Date expiryDate = sdf.parse(validity);
                                    if (now.after(expiryDate)) {
                                        textview2.setText("Key Expired!");
                                        return;
                                    }
                                } catch(Exception e) {
                                    textview2.setText("Invalid Date Format");
                                    return;
                                }
                                if(snapshot.child("access").getValue(String.class).equals("1")) {
                                    if(snapshot.child("device").getValue(String.class).equals("null")) {
                                        String deviceId = Settings.Secure.getString(getContentResolver(), Settings.Secure.ANDROID_ID);
                                        java.util.HashMap<String, Object> map = new java.util.HashMap<>();
                                        map.put("device", deviceId);
                                        djgamingvip.child(user).updateChildren(map);
                                        floating();
                                    } else {
                                        String deviceId = Settings.Secure.getString(getContentResolver(), Settings.Secure.ANDROID_ID);
                                        if(snapshot.child("device").getValue(String.class).equals(deviceId)) {
                                            floating();
                                        } else {
                                            textview2.setText("Key Already Used!");
                                        }
                                    }
                                } else if (snapshot.child("access").getValue(String.class).equals("∞")) {
                                    floating();
                                }
                            } else {
                                textview2.setText("User Deactivate!");
                            }
                        } else {
                            textview2.setText("Invalid Credentials!");
                        }
                    } else {
                        textview2.setText("User Not Found!");
                    }
                }

                @Override
                public void onCancelled(@NonNull DatabaseError error) {
                    Toast.makeText(MainActivity.this, "Permission Denied", Toast.LENGTH_SHORT).show();
                }
            });
        }
    }

    private void floating() {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.M || Settings.canDrawOverlays(this)) {
            startService(new Intent(this, FloatingActivity.class));
        } else {
            checkOverlayPermission();
        }
    }

    private void requestStoragePermission() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            if (!Environment.isExternalStorageManager()) {
                Intent intent = new Intent(Settings.ACTION_MANAGE_ALL_FILES_ACCESS_PERMISSION);
                startActivity(intent);
            }
        } else {
            if (ContextCompat.checkSelfPermission(this, Manifest.permission.READ_EXTERNAL_STORAGE)
                    != PackageManager.PERMISSION_GRANTED ||
                    ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE)
                            != PackageManager.PERMISSION_GRANTED) {

                ActivityCompat.requestPermissions(this,
                        new String[]{ Manifest.permission.READ_EXTERNAL_STORAGE,
                                Manifest.permission.WRITE_EXTERNAL_STORAGE }, 1);
            }
        }
    }

}
