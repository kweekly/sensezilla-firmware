package com.example.image;



import java.util.List;

import org.opencv.core.Size;
import org.opencv.highgui.Highgui;
import org.opencv.highgui.VideoCapture;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Rect;
import android.hardware.Camera;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceHolder.Callback;
import android.view.SurfaceView;

public abstract class CameraView extends SurfaceView implements SurfaceHolder.Callback, Runnable{
	private SurfaceHolder       mHolder;
    private VideoCapture        mCamera;

    public CameraView(Context context) {
        super(context);
        mHolder = getHolder();
        mHolder.addCallback(this);
    }

    public boolean openCamera() {
        synchronized (this) {
	        releaseCamera();
	        
	        Camera cam = Camera.open();
	        List<Camera.Size> sizes = cam.getParameters().getSupportedPreviewSizes();
	        cam.release();
	        for (int s = 0; s < sizes.size(); s++) {
	        	Log.d("CSIZE",sizes.get(s).width+"x"+sizes.get(s).height);
	        }
	        
	        mCamera = new VideoCapture(Highgui.CV_CAP_ANDROID);

	        if (!mCamera.isOpened()) {
	            mCamera.release();
	            mCamera = null;
	            return false;
	        }
	    }
        return true;
    }
    
    public void releaseCamera() {
        synchronized (this) {
	        if (mCamera != null) {
	                mCamera.release();
	                mCamera = null;
            }
        }
    }
    
    public void setupCamera(int width, int height) {
        synchronized (this) {
            if (mCamera != null && mCamera.isOpened()) {
                List<Size> sizes = mCamera.getSupportedPreviewSizes();
                int mFrameWidth = width;
                int mFrameHeight = height;

                // selecting optimal camera preview size
                /*
                {
                    double minDiff = Double.MAX_VALUE;
                    for (Size size : sizes) {
                        if (Math.abs(size.height - height) < minDiff) {
                            mFrameWidth = (int) size.width;
                            mFrameHeight = (int) size.height;
                            minDiff = Math.abs(size.height - height);
                        }
                    }
                }*/
                mFrameWidth = 640;
                mFrameHeight = 480;

                mCamera.set(Highgui.CV_CAP_PROP_FRAME_WIDTH, mFrameWidth);
                mCamera.set(Highgui.CV_CAP_PROP_FRAME_HEIGHT, mFrameHeight);
                mCamera.set(Highgui.CV_CAP_PROP_ANDROID_WHITE_BALANCE, Highgui.CV_CAP_ANDROID_WHITE_BALANCE_DAYLIGHT);
                
            }
        }

    }
    
    public void surfaceChanged(SurfaceHolder _holder, int format, int width, int height) {
        setupCamera(width, height);
    }

    public void surfaceCreated(SurfaceHolder holder) {
        (new Thread(this)).start();
    }

    public void surfaceDestroyed(SurfaceHolder holder) {
        releaseCamera();
    }

    protected abstract void processFrame(VideoCapture capture, Canvas canvas);

    public void run() {
        while (true) {
        	Canvas canvas = null;

            
            synchronized (this) {
                if (mCamera == null) {
                    break;
                }

                if (!mCamera.grab()) {
                    break;
                }
                canvas = mHolder.lockCanvas();
                if (canvas != null)
                	processFrame(mCamera, canvas);
            }


            if (canvas != null) {
                 mHolder.unlockCanvasAndPost(canvas);
            }
        }
    }
}
