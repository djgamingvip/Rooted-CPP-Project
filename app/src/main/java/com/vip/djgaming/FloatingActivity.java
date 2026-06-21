package com.vip.djgaming;

import android.app.Service;
import android.content.Intent;
import android.graphics.PixelFormat;
import android.graphics.Typeface;
import android.os.Build;
import android.os.IBinder;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.Switch;
import android.widget.TextView;

public class FloatingActivity extends Service {

    private WindowManager windowManager;
    private View floatingView;
    private WindowManager.LayoutParams params;
    private LinearLayout linear1, linear2, l1, l2, l3, linear_close, linear_minimize;
    private RelativeLayout icon;
    private Switch switch1, switch2, switch3, switch4, switch5, switch6, switch7, switch8, switch9, switch10, switch11, switch12;
    private TextView textview1, textview2, textview3, textview4, textview5;
    private ImageView imageview2;

    @Override
    public IBinder onBind(Intent intent) {
        return null;
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

        try {
            textview1.setTypeface(Typeface.createFromAsset(getAssets(), "anonymous.ttf"), Typeface.BOLD);
            textview2.setTypeface(Typeface.createFromAsset(getAssets(), "anonymous.ttf"), Typeface.BOLD);
            textview3.setTypeface(Typeface.createFromAsset(getAssets(), "tajawal_medium.ttf"), Typeface.BOLD);
            textview4.setTypeface(Typeface.createFromAsset(getAssets(), "tajawal_medium.ttf"), Typeface.NORMAL);
            textview5.setTypeface(Typeface.createFromAsset(getAssets(), "tajawal_medium.ttf"), Typeface.NORMAL);
        } catch (Exception e) {
            // Fonts not found
        }

        textview3.setTextColor(0xFFFF0000);

        setBg(linear_close, "#461117", 5f, 24f, 24f, 5f);
        setBg(linear_minimize, "#373A43", 24f, 5f, 5f, 24f);
    }

    private void setBg(View view, String hexColor, float tl, float tr, float br, float bl) {
        android.graphics.drawable.GradientDrawable shape = new android.graphics.drawable.GradientDrawable();
        shape.setShape(android.graphics.drawable.GradientDrawable.RECTANGLE);
        shape.setColor(android.graphics.Color.parseColor(hexColor));
        shape.setCornerRadii(new float[] { tl, tl, tr, tr, br, br, bl, bl });
        view.setBackground(shape);
    }

    private void initializeViews() {
        linear1 = floatingView.findViewById(R.id.linear1);
        linear2 = floatingView.findViewById(R.id.linear2);
        linear_close = floatingView.findViewById(R.id.linear_close);
        linear_minimize = floatingView.findViewById(R.id.linear_minimize);
        icon = floatingView.findViewById(R.id.icon);
        l1 = floatingView.findViewById(R.id.l1);
        l2 = floatingView.findViewById(R.id.l2);
        l3 = floatingView.findViewById(R.id.l3);
        imageview2 = floatingView.findViewById(R.id.imageview2);
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
        switch9 = floatingView.findViewById(R.id.switch9);
        switch10 = floatingView.findViewById(R.id.switch10);
        switch11 = floatingView.findViewById(R.id.switch11);
        switch12 = floatingView.findViewById(R.id.switch12);

        l2.setVisibility(View.GONE);
        l3.setVisibility(View.GONE);
        linear1.setVisibility(View.GONE);
    }

    private void setupClickListeners() {
        imageview2.setOnClickListener(view -> closeFloatingMenu());
        linear_close.setOnClickListener(v -> stopSelf());

        icon.setOnClickListener(view -> openFloatingMenu());

        textview3.setOnClickListener(view -> {
            l1.setVisibility(View.VISIBLE);
            l2.setVisibility(View.GONE);
            l3.setVisibility(View.GONE);
            updateTabs(textview3, textview4, textview5);
        });

        textview4.setOnClickListener(view -> {
            l1.setVisibility(View.GONE);
            l2.setVisibility(View.VISIBLE);
            l3.setVisibility(View.GONE);
            updateTabs(textview4, textview3, textview5);
        });

        textview5.setOnClickListener(view -> {
            l1.setVisibility(View.GONE);
            l2.setVisibility(View.GONE);
            l3.setVisibility(View.VISIBLE);
            updateTabs(textview5, textview3, textview4);
        });

        setupSwitchListeners();
    }

    private void updateTabs(TextView active, TextView inactive1, TextView inactive2) {
        try {
            active.setTypeface(Typeface.createFromAsset(getAssets(), "tajawal_medium.ttf"), Typeface.BOLD);
            active.setTextColor(0xFFFF0000);
            inactive1.setTypeface(Typeface.createFromAsset(getAssets(), "tajawal_medium.ttf"), Typeface.NORMAL);
            inactive1.setTextColor(0xFF9E9E9E);
            inactive2.setTypeface(Typeface.createFromAsset(getAssets(), "tajawal_medium.ttf"), Typeface.NORMAL);
            inactive2.setTextColor(0xFF9E9E9E);
        } catch (Exception e) {
            // Font not found
        }
    }

    private void setupSwitchListeners() {
        switch1.setOnCheckedChangeListener((buttonView, isChecked) -> {});
        switch2.setOnCheckedChangeListener((buttonView, isChecked) -> {});
        switch3.setOnCheckedChangeListener((buttonView, isChecked) -> {});
        switch4.setOnCheckedChangeListener((buttonView, isChecked) -> {});
        switch5.setOnCheckedChangeListener((buttonView, isChecked) -> {});
        switch6.setOnCheckedChangeListener((buttonView, isChecked) -> {});
        switch7.setOnCheckedChangeListener((buttonView, isChecked) -> {});
        switch8.setOnCheckedChangeListener((buttonView, isChecked) -> {});
        switch9.setOnCheckedChangeListener((buttonView, isChecked) -> {});
        switch10.setOnCheckedChangeListener((buttonView, isChecked) -> {});
        switch11.setOnCheckedChangeListener((buttonView, isChecked) -> {});
        switch12.setOnCheckedChangeListener((buttonView, isChecked) -> {});
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
            try {
                windowManager.removeView(floatingView);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }
}