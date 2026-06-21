package com.vip.djgaming;

import android.Manifest;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.graphics.Typeface;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.Settings;
import android.provider.Settings.Secure;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;

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
    private ProgressBar progressBar;

    private SharedPreferences sharedPreferences;
    private static final String PREF_NAME = "LoginPrefs";
    private static final int ACTION_MANAGE_OVERLAY_PERMISSION_REQUEST_CODE = 1;
    private String user, pass, deviceID;

    static {
        System.loadLibrary("djgaming");
    }

    // ============================================
    // NATIVE METHODS - FLY FEATURES
    // ============================================
    public static native String getMessage();
    public static native void setFly(boolean fly);
    public static native void setFlyMap(boolean flyMap);
    public static native void setFootballHack(boolean active);
    public static native void initNative();
    public static native String testNative();

    // ============================================
    // NATIVE METHODS - ESP FEATURES
    // ============================================
    public static native void setEsp(boolean enable);
    public static native void setEspLine(boolean enable);
    public static native void setEspBox(boolean enable);
    public static native void setEspName(boolean enable);
    public static native void setEspDistance(boolean enable);
    public static native void setEspHealth(boolean enable);
    public static native void setEspSkeleton(boolean enable);
    public static native void setEspTarget(boolean enable);
    public static native void setEnemyCounter(boolean enable);
    public static native void setShowFov(boolean enable);
    public static native void setEspDirection(boolean enable);
    public static native void setFovDirection(float fov);
    public static native void setEspOffsetX(float offset);

    // ============================================
    // NATIVE METHODS - AIMBOT FEATURES
    // ============================================
    public static native void setAimbot(boolean active);
    public static native void setAimSilent(boolean active);
    public static native void setAimMagnet(boolean active);
    public static native void setAimFov(int fov);
    public static native void setHeadshotRate(int rate);
    public static native void setAimSmooth(float smooth);
    public static native void setTargetMode(int mode);
    public static native void setWhenAim(int when);

    // ============================================
    // NATIVE METHODS - DRAWING
    // ============================================
    public static native void drawESP(Object espView, Object canvas);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);

        // Test native bridge
        try {
            String testResult = testNative();
            Log.d("MainActivity", "Native test: " + testResult);
            Toast.makeText(this, "Native bridge loaded!", Toast.LENGTH_SHORT).show();
        } catch (Exception e) {
            Log.e("MainActivity", "Native test failed: " + e.getMessage());
        }

        initNative();

        sharedPreferences = getSharedPreferences(PREF_NAME, MODE_PRIVATE);

        initialize(savedInstanceState);
        GlowingTriangleView(back);
        requestStoragePermission();
        checkOverlayPermission();

        etUsername.setText(sharedPreferences.getString("username", ""));
        etPassword.setText(sharedPreferences.getString("password", ""));

        try {
            textview1.setText(getMessage());
        } catch (Exception e) {
            textview1.setText("DJ Gaming VIP");
        }

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
            public void beforeTextChanged(CharSequence charSequence, int i, int i1, int i2) {}

            @Override
            public void onTextChanged(CharSequence charSequence, int i, int i1, int i2) {}
        });

        etPassword.addTextChangedListener(new TextWatcher() {
            @Override
            public void afterTextChanged(Editable editable) {
                textview2.setText("");
            }

            @Override
            public void beforeTextChanged(CharSequence charSequence, int i, int i1, int i2) {}

            @Override
            public void onTextChanged(CharSequence charSequence, int i, int i1, int i2) {}
        });

        try {
            textview1.setTypeface(Typeface.createFromAsset(getAssets(), "font_style.ttf"), Typeface.BOLD);
        } catch (Exception e) {
            // Font not found
        }

        subscribe();
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
        progressBar = findViewById(R.id.progressBar);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            Window window = getWindow();
            window.addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
            window.setStatusBarColor(Color.parseColor("#FF000000"));
        }
    }

    private void subscribe() {
        final AlertDialog dialog = new AlertDialog.Builder(MainActivity.this).create();
        View inflate = getLayoutInflater().inflate(R.layout.subscribe, null);
        dialog.setView(inflate);
        dialog.getWindow().setBackgroundDrawableResource(android.R.color.transparent);

        final Button btnLater = inflate.findViewById(R.id.btnLater);
        final Button btnSubscribe = inflate.findViewById(R.id.btnSubscribe);

        dialog.show();
        dialog.setCancelable(false);
        btnLater.setOnClickListener(v -> dialog.dismiss());
        btnSubscribe.setOnClickListener(v -> {
            Intent intent = new Intent(Intent.ACTION_VIEW);
            intent.setData(Uri.parse("https://youtube.com/@djgamingvip"));
            startActivity(intent);
            dialog.dismiss();
        });
    }

    private void GlowingTriangleView(android.view.ViewGroup layout) {
        try {
            GlowingTriangleParticles.addToLayout(this, layout);
        } catch (Exception e) {
            // Glowing triangle not available
        }
    }

    private void login() {
        user = etUsername.getText().toString().trim();
        pass = etPassword.getText().toString().trim();

        if (user.isEmpty() || pass.isEmpty()) {
            textview2.setText("Please enter username and password");
            return;
        }

        progressBar.setVisibility(View.VISIBLE);
        btnLogin.setEnabled(false);
        textview2.setText("Verifying...");
        textview2.setTextColor(Color.YELLOW);

        deviceID = Secure.getString(getApplicationContext().getContentResolver(), Secure.ANDROID_ID);

        new Thread(() -> {
            try {
                URL url = new URL("https://djgamingvip.myvippanel.shop/login.php");
                HttpURLConnection conn = (HttpURLConnection) url.openConnection();
                conn.setRequestMethod("POST");
                conn.setDoOutput(true);
                conn.setDoInput(true);
                conn.setConnectTimeout(10000);
                conn.setReadTimeout(10000);

                String postData = "username=" + user +
                        "&password=" + pass +
                        "&deviceID=" + deviceID;

                OutputStream os = conn.getOutputStream();
                os.write(postData.getBytes("UTF-8"));
                os.flush();
                os.close();

                BufferedReader br = new BufferedReader(new InputStreamReader(conn.getInputStream()));
                StringBuilder response = new StringBuilder();
                String line;
                while ((line = br.readLine()) != null) {
                    response.append(line);
                }
                br.close();

                final String result = response.toString();
                runOnUiThread(() -> handleLoginResponse(result));

            } catch (Exception e) {
                e.printStackTrace();
                runOnUiThread(() -> {
                    progressBar.setVisibility(View.GONE);
                    btnLogin.setEnabled(true);
                    textview2.setText("Network error: " + e.getMessage());
                    textview2.setTextColor(Color.RED);
                });
            }
        }).start();
    }

    private void handleLoginResponse(String response) {
        progressBar.setVisibility(View.GONE);
        btnLogin.setEnabled(true);

        try {
            JSONObject jsonResponse = new JSONObject(response);
            boolean success = jsonResponse.getBoolean("success");
            String message = jsonResponse.getString("message");

            if (success) {
                JSONObject userObj = jsonResponse.getJSONObject("user");

                SharedPreferences.Editor editor = sharedPreferences.edit();
                editor.putString("username", userObj.getString("username"));
                editor.putString("password", userObj.getString("password"));
                editor.putString("status", userObj.getString("status"));
                editor.putString("validUntil", userObj.getString("validUntil"));
                editor.putString("deviceID", userObj.getString("deviceID"));
                editor.putString("access", userObj.getString("access"));
                editor.putBoolean("isLoggedIn", true);
                editor.apply();

                textview2.setText("Login successful! Redirecting...");
                textview2.setTextColor(Color.GREEN);

                Intent intent = new Intent(MainActivity.this, FloatingActivity.class);
                startService(intent);

            } else {
                textview2.setText(message);
                textview2.setTextColor(Color.RED);
                Toast.makeText(this, message, Toast.LENGTH_LONG).show();
            }

        } catch (JSONException e) {
            e.printStackTrace();
            textview2.setText("Error: " + e.getMessage());
            textview2.setTextColor(Color.RED);
            Toast.makeText(this, "Error parsing response", Toast.LENGTH_LONG).show();
        }
    }

    private void checkOverlayPermission() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            if (!Settings.canDrawOverlays(this)) {
                Intent intent = new Intent(Settings.ACTION_MANAGE_OVERLAY_PERMISSION,
                        Uri.parse("package:" + getPackageName()));
                startActivityForResult(intent, ACTION_MANAGE_OVERLAY_PERMISSION_REQUEST_CODE);
            }
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

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == ACTION_MANAGE_OVERLAY_PERMISSION_REQUEST_CODE) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                if (Settings.canDrawOverlays(this)) {
                    Toast.makeText(this, "Overlay permission granted", Toast.LENGTH_SHORT).show();
                } else {
                    Toast.makeText(this, "Overlay permission denied", Toast.LENGTH_SHORT).show();
                }
            }
        }
    }
}