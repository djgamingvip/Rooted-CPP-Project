package com.vip.djgaming;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.util.AttributeSet;
import android.view.View;

import java.util.ArrayList;
import java.util.Random;

public class GlowingTriangleParticles extends View {

    private Paint paint;
    private ArrayList<Triangle> triangles;
    private Random random = new Random();
    private int numTriangles = 40;

    public GlowingTriangleParticles(Context context) {
        super(context);
        init();
    }

    public GlowingTriangleParticles(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    private void init() {
        paint = new Paint(Paint.ANTI_ALIAS_FLAG);
        paint.setColor(Color.parseColor("#FF0000"));
        paint.setStyle(Paint.Style.FILL);
        paint.setShadowLayer(25, 0, 0, Color.parseColor("#FF0000"));
        setLayerType(LAYER_TYPE_SOFTWARE, paint);

        triangles = new ArrayList<>();
        for (int i = 0; i < numTriangles; i++) {
            triangles.add(new Triangle());
        }
    }


    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        canvas.drawColor(Color.parseColor("#FF121212"));

        for (Triangle t : triangles) {
            t.update(getWidth(), getHeight());
            t.draw(canvas, paint);
        }

        postInvalidateDelayed(16);
    }

    private class Triangle {
        float x, y, dx, dy, size, angle, rotationSpeed;
        Path path = new Path();

        Triangle() {
            reset(1080, 1920);
        }

        void reset(int width, int height) {
            x = random.nextInt(width);
            y = random.nextInt(height);
            dx = (random.nextFloat() - 0.5f) * 4;
            dy = (random.nextFloat() - 0.5f) * 4;
            size = 5 + random.nextInt(10);
            angle = random.nextInt(360);
            rotationSpeed = (random.nextFloat() - 0.5f) * 4;
        }

        void update(int width, int height) {
            x += dx;
            y += dy;
            angle += rotationSpeed;

            if (x < -size || x > width + size || y < -size || y > height + size) {
                reset(width, height);
            }
        }

        void draw(Canvas canvas, Paint paint) {
            path.reset();
            path.moveTo(x, y - size);
            path.lineTo(x + size, y + size);
            path.lineTo(x - size, y + size);
            path.close();

            canvas.save();
            canvas.rotate(angle, x, y);
            canvas.drawPath(path, paint);
            canvas.restore();
        }
    }

    public static void addToLayout(Context context, android.view.ViewGroup layout) {
        GlowingTriangleParticles particleView = new GlowingTriangleParticles(context);
        particleView.setLayoutParams(
                new android.view.ViewGroup.LayoutParams(
                        android.view.ViewGroup.LayoutParams.MATCH_PARENT,
                        android.view.ViewGroup.LayoutParams.MATCH_PARENT
                )
        );
        layout.addView(particleView);
    }
}