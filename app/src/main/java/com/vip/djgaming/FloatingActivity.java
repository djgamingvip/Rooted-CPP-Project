package com.vip.djgaming;

import static android.app.ProgressDialog.show;

import android.app.Service;
import android.content.Intent;
import android.graphics.PixelFormat;
import android.graphics.Typeface;
import android.graphics.drawable.GradientDrawable;
import android.os.Build;
import android.os.IBinder;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

public class FloatingActivity extends Service {

    private WindowManager windowManager;
    private View floatingView;
    private WindowManager.LayoutParams params;
    private LinearLayout linear1, linear2, icon, l1, l2, l3;
    private Switch switch1, switch2, switch3, switch4, switch5, switch6, switch7, switch8;
    private TextView textview1, textview2, textview3, textview4, textview5;
    private ImageView imageview1, imageview3;

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    private void setBg(View v, int radius, int color) {
        GradientDrawable shape = new GradientDrawable();
        shape.setCornerRadius(radius);
        shape.setColor(color);
        v.setBackground(shape);
    }

    private void setBgWithStroke(View v, int radius, int backgroundColor, int strokeWidth, int strokeColor) {
        GradientDrawable shape = new GradientDrawable();
        shape.setCornerRadius(radius);
        shape.setColor(backgroundColor);
        shape.setStroke(strokeWidth, strokeColor);
        v.setBackground(shape);
    }

    @Override
    public void onCreate() {
        super.onCreate();

        floatingView = LayoutInflater.from(this).inflate(R.layout.floating, null);

        int layout_parms;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            layout_parms = WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY;
        } else {
            layout_parms = WindowManager.LayoutParams.TYPE_PHONE;
        }

        params = new WindowManager.LayoutParams(
                WindowManager.LayoutParams.WRAP_CONTENT,
                WindowManager.LayoutParams.WRAP_CONTENT,
                layout_parms,
                WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE |
                        WindowManager.LayoutParams.FLAG_WATCH_OUTSIDE_TOUCH |
                        WindowManager.LayoutParams.FLAG_LAYOUT_NO_LIMITS |
                        WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL,
                PixelFormat.TRANSLUCENT);

        params.gravity = Gravity.TOP | Gravity.START;
        params.x = 0;
        params.y = 100;

        windowManager = (WindowManager) getSystemService(WINDOW_SERVICE);
        windowManager.addView(floatingView, params);

        initializeViews();
        setupClickListeners();
        setupTouchListeners();

        setBg(linear1, 20, 0xFF000000);
        setBg(l1, 10, 0xFF1A1A1A);
        setBg(l2, 10, 0xFF1A1A1A);
        setBg(l3, 10, 0xFF1A1A1A);
        setBgWithStroke(linear2, 15, 0xFF333333, 2, 0xFF00FF00);

        textview1.setTypeface(Typeface.createFromAsset(getAssets(), "anonymous.ttf"), Typeface.BOLD);
        textview2.setTypeface(Typeface.createFromAsset(getAssets(), "anonymous.ttf"), Typeface.BOLD);
    }

    //Delete Path
    public boolean deleteRootFile(String path) {
        try {
            java.lang.Process su = Runtime.getRuntime().exec("su");
            DataOutputStream os = new DataOutputStream(su.getOutputStream());

            os.writeBytes("mount -o remount,rw " + path + "\n");
            os.writeBytes("rm -f " + path + "\n");
            os.writeBytes("mount -o remount,ro " + path + "\n");
            os.writeBytes("exit\n");
            os.flush();
            su.waitFor();
            Toast.makeText(this, "File deleted successfully", Toast.LENGTH_SHORT).show();
        } catch (Exception e) {
            Toast.makeText(this, e.getMessage(), Toast.LENGTH_SHORT).show();
        }
        return false;
    }

    //Copy File From Assets To Path
    public boolean copyAssetFileToPath(String filename, String path, String realname) {
        try {
            InputStream inputStream = getAssets().open(filename);
            byte[] buffer = new byte[inputStream.available()];
            inputStream.read(buffer);
            inputStream.close();

            File tempFile = new File(getCacheDir(), "temp_file_temp.txt");
            FileOutputStream fos = new FileOutputStream(tempFile);
            fos.write(buffer);
            fos.close();

            java.lang.Process su = Runtime.getRuntime().exec("su");
            DataOutputStream outputStream = new DataOutputStream(su.getOutputStream());

            outputStream.writeBytes("mount -o remount,rw " + path + "\n");
            outputStream.writeBytes("cp " + tempFile.getAbsolutePath() + " " + path + realname + "\n");
            outputStream.writeBytes("chmod 644 " + path + realname + "\n");
            outputStream.writeBytes("mount -o remount,ro " + path + "\n");
            outputStream.writeBytes("exit\n");
            outputStream.flush();
            su.waitFor();

            Toast.makeText(FloatingActivity.this, "success", Toast.LENGTH_SHORT).show();
        } catch (Exception e) {
            Toast.makeText(FloatingActivity.this, e.getMessage(), Toast.LENGTH_SHORT).show();
        }
        return false;
    }

    //UnzipFromAssets
    public boolean unzipAssetToRoot(String filename, String path) {
        try {
            // Copy ZIP from assets to cache
            InputStream is = getAssets().open(filename);
            File zipFile = new File(getCacheDir(), "temp.zip");

            FileOutputStream fos = new FileOutputStream(zipFile);
            byte[] buffer = new byte[1024];
            int len;
            while ((len = is.read(buffer)) > 0) {
                fos.write(buffer, 0, len);
            }
            fos.close();
            is.close();

            // Root unzip
            Process su = Runtime.getRuntime().exec("su");
            DataOutputStream os = new DataOutputStream(su.getOutputStream());

            os.writeBytes("mount -o remount,rw " + path + "\n");
            os.writeBytes("unzip -o " + zipFile.getAbsolutePath() + " -d " + path + "\n");
            os.writeBytes("chmod -R 755 " + path + "\n");
            os.writeBytes("mount -o remount,ro " + path + "\n");
            os.writeBytes("exit\n");
            os.flush();

            su.waitFor();

            Toast.makeText(this, "Unzip success", Toast.LENGTH_SHORT).show();
            return true;

        } catch (Exception e) {
            Toast.makeText(this, e.getMessage(), Toast.LENGTH_SHORT).show();
            return false;
        }
    }

    private void initializeViews() {
        linear1 = floatingView.findViewById(R.id.linear1);
        linear2 = floatingView.findViewById(R.id.linear2);
        icon = floatingView.findViewById(R.id.icon);
        l1 = floatingView.findViewById(R.id.l1);
        l2 = floatingView.findViewById(R.id.l2);
        l3 = floatingView.findViewById(R.id.l3);
        imageview1 = floatingView.findViewById(R.id.imageview1);
        imageview3 = floatingView.findViewById(R.id.imageview3);
        textview1 = floatingView.findViewById(R.id.textview1);
        textview2 = floatingView.findViewById(R.id.textview2);
        textview3 = floatingView.findViewById(R.id.textview3);
        textview4 = floatingView.findViewById(R.id.textview4);
        textview5 = floatingView.findViewById(R.id.textview5);
        switch1 = floatingView.findViewById(R.id.switch1);
        switch2 = floatingView.findViewById(R.id.switch2);
        switch3 = floatingView.findViewById(R.id.switch3);
        switch4 = floatingView.findViewById(R.id.switch4);
        switch5 = floatingView.findViewById(R.id.switch5);
        switch6 = floatingView.findViewById(R.id.switch6);
        switch7 = floatingView.findViewById(R.id.switch7);
        switch8 = floatingView.findViewById(R.id.switch8);

        l2.setVisibility(View.GONE);
        l3.setVisibility(View.GONE);
        linear1.setVisibility(View.GONE);
    }

    private void setupClickListeners() {
        imageview1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                closeFloatingMenu();
            }
        });

        imageview3.setOnClickListener(v -> stopSelf());

        icon.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                openFloatingMenu();
            }
        });

        TextView textview3 = floatingView.findViewById(R.id.textview3);
        TextView textview4 = floatingView.findViewById(R.id.textview4);
        TextView textview5 = floatingView.findViewById(R.id.textview5);

        textview3.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View _view){
                l1.setVisibility(View.VISIBLE);
                l2.setVisibility(View.GONE);
                l3.setVisibility(View.GONE);
                textview3.setTypeface(Typeface.createFromAsset(getAssets(), "product.ttf"), Typeface.BOLD);
                textview4.setTypeface(Typeface.createFromAsset(getAssets(), "product.ttf"), Typeface.NORMAL);
                textview5.setTypeface(Typeface.createFromAsset(getAssets(), "product.ttf"), Typeface.NORMAL);
                textview3.setTextColor(0xFF00FF00);
                textview4.setTextColor(0xFF9E9E9E);
                textview5.setTextColor(0xFF9E9E9E);
            }
        });

        textview4.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View _view){
                l1.setVisibility(View.GONE);
                l2.setVisibility(View.VISIBLE);
                l3.setVisibility(View.GONE);
                textview3.setTypeface(Typeface.createFromAsset(getAssets(), "product.ttf"), Typeface.NORMAL);
                textview4.setTypeface(Typeface.createFromAsset(getAssets(), "product.ttf"), Typeface.BOLD);
                textview5.setTypeface(Typeface.createFromAsset(getAssets(), "product.ttf"), Typeface.NORMAL);
                textview3.setTextColor(0xFF9E9E9E);
                textview4.setTextColor(0xFF00FF00);
                textview5.setTextColor(0xFF9E9E9E);
            }
        });

        textview5.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View _view){
                l1.setVisibility(View.GONE);
                l2.setVisibility(View.GONE);
                l3.setVisibility(View.VISIBLE);
                textview3.setTypeface(Typeface.createFromAsset(getAssets(), "product.ttf"), Typeface.NORMAL);
                textview4.setTypeface(Typeface.createFromAsset(getAssets(), "product.ttf"), Typeface.NORMAL);
                textview5.setTypeface(Typeface.createFromAsset(getAssets(), "product.ttf"), Typeface.BOLD);
                textview3.setTextColor(0xFF9E9E9E);
                textview4.setTextColor(0xFF9E9E9E);
                textview5.setTextColor(0xFF00FF00);
            }
        });

        switch1.setOnCheckedChangeListener((buttonView, isChecked) -> {
            if (isChecked) {
                if(copyAssetFileToPath("abcd.txt","/data/user/0/com.dts.freefiremax/files/contentcache/Compulsory/android/gameassetbundles/","djgamingvip")) {
                    return;
                } else {
                    return;
                }
            } else {
                Toast.makeText(FloatingActivity.this, "OFF", Toast.LENGTH_SHORT).show();
            }
        });

        switch2.setOnCheckedChangeListener((buttonView, isChecked) -> {
            if (isChecked) {
                if(deleteRootFile("/data/user/0/com.dts.freefiremax/files/contentcache/Compulsory/android/gameassetbundles/djgamingvip")) {
                    return;
                } else {
                    return;
                }
            } else {
                Toast.makeText(FloatingActivity.this, "OFF", Toast.LENGTH_SHORT).show();
            }
        });

        switch3.setOnCheckedChangeListener((buttonView, isChecked) -> {
            if (isChecked) {
                if(unzipAssetToRoot("file.zip","data/user/0/com.dts.freefiremax/files/contentcache/Compulsory/android/gameassetbundles/")) {
                    return;
                } else {
                    return;
                }
            } else {
                Toast.makeText(FloatingActivity.this, "OFF", Toast.LENGTH_SHORT).show();
            }
        });

        switch4.setOnCheckedChangeListener((buttonView, isChecked) -> {
            if (isChecked) {
                //djgamingvip-On Statement
            } else {
                //djgamingvip-Off Statement
            }
        });
    }

    private void setupTouchListeners() {
        icon.setOnTouchListener(new View.OnTouchListener() {
            private int initialX, initialY;
            private float initialTouchX, initialTouchY;
            private boolean isDragging = false;
            private final int TOUCH_SLOP = 10;
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                switch (event.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        initialX = params.x;
                        initialY = params.y;
                        initialTouchX = event.getRawX();
                        initialTouchY = event.getRawY();
                        isDragging = false;
                        return true;

                    case MotionEvent.ACTION_MOVE:
                        float deltaX = Math.abs(event.getRawX() - initialTouchX);
                        float deltaY = Math.abs(event.getRawY() - initialTouchY);

                        if (deltaX > TOUCH_SLOP || deltaY > TOUCH_SLOP) {
                            isDragging = true;
                            params.x = initialX + (int) (event.getRawX() - initialTouchX);
                            params.y = initialY + (int) (event.getRawY() - initialTouchY);
                            windowManager.updateViewLayout(floatingView, params);
                        }
                        return true;

                    case MotionEvent.ACTION_UP:
                        if (!isDragging) {
                            openFloatingMenu();
                        }
                        return true;
                }
                return false;
            }
        });

        linear1.setOnTouchListener(new View.OnTouchListener() {
            private int initialX, initialY;
            private float initialTouchX, initialTouchY;
            private boolean isDragging = false;
            private final int TOUCH_SLOP = 10;

            @Override
            public boolean onTouch(View v, MotionEvent event) {
                switch (event.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        initialX = params.x;
                        initialY = params.y;
                        initialTouchX = event.getRawX();
                        initialTouchY = event.getRawY();
                        isDragging = false;
                        return true;

                    case MotionEvent.ACTION_MOVE:
                        float deltaX = Math.abs(event.getRawX() - initialTouchX);
                        float deltaY = Math.abs(event.getRawY() - initialTouchY);

                        if (deltaX > TOUCH_SLOP || deltaY > TOUCH_SLOP) {
                            isDragging = true;
                            params.x = initialX + (int) (event.getRawX() - initialTouchX);
                            params.y = initialY + (int) (event.getRawY() - initialTouchY);
                            windowManager.updateViewLayout(floatingView, params);
                        }
                        return true;

                    case MotionEvent.ACTION_UP:
                        return isDragging;
                }
                return false;
            }
        });
    }

    private void openFloatingMenu() {
        linear1.setVisibility(View.VISIBLE);
        icon.setVisibility(View.GONE);

        updateWindowFlags(WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL |
                WindowManager.LayoutParams.FLAG_WATCH_OUTSIDE_TOUCH);
    }

    private void closeFloatingMenu() {
        linear1.setVisibility(View.GONE);
        icon.setVisibility(View.VISIBLE);

        updateWindowFlags(WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE |
                WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL |
                WindowManager.LayoutParams.FLAG_WATCH_OUTSIDE_TOUCH);
    }

    private void updateWindowFlags(int flags) {
        params.flags = flags;
        try {
            windowManager.updateViewLayout(floatingView, params);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        if (floatingView != null) {
            windowManager.removeView(floatingView);
        }
    }
}