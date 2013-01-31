package edu.berkeley.sensezilla.energyeyes;

import java.io.IOException;


import com.google.zxing.*;
import com.google.zxing.common.HybridBinarizer;
import com.google.zxing.datamatrix.DataMatrixReader;
import com.google.zxing.multi.qrcode.QRCodeMultiReader;
import com.google.zxing.qrcode.QRCodeReader;

import android.content.Context;
import android.graphics.PixelFormat;
import android.graphics.YuvImage;
import android.hardware.Camera;
import android.hardware.Camera.Parameters;
import android.hardware.Camera.PreviewCallback;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceHolder.Callback;
import android.view.SurfaceView;

public class CustomCameraView extends SurfaceView  implements Callback {
	private static String TAG = CustomCameraView.class.getSimpleName(); 
    private Camera camera;
    
    EnergyActivity parent;
    
    public CustomCameraView( EnergyActivity p ) {
        super( p );
        parent = p;
        // We're implementing the Callback interface and want to get notified
        // about certain surface events.
        getHolder().addCallback( this );
        // We're changing the surface to a PUSH surface, meaning we're receiving
        // all buffer data from another component - the camera, in this case.
        getHolder().setType( SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS );
    }
 
    public void surfaceCreated( SurfaceHolder holder ) {
        // Once the surface is created, simply open a handle to the camera hardware.
        camera = Camera.open();
    }
 
    public void surfaceChanged( SurfaceHolder holder, int format, int width, int height ) {
        // This method is called when the surface changes, e.g. when it's size is set.
        // We use the opportunity to initialize the camera preview display dimensions.
        Camera.Parameters p = camera.getParameters();
        p.setPreviewSize( width, height );
        camera.setParameters( p );
 
        // We also assign the preview display to this surface...
        try {
            camera.setPreviewDisplay( holder );
        } catch( IOException e ) {
            e.printStackTrace();
        }

        
        // ...and start previewing. From now on, the camera keeps pushing preview
        // images to the surface.
        camera.startPreview();
        
        
        camera.setPreviewCallback(new PreviewCallback() {
			int troll = 0;
			public void onPreviewFrame(byte[] data, Camera camera) {
				if (troll % 10 == 0) {
					//Log.d(TAG,"TROLOLOLOLO "+data.length+" "+troll);
					Parameters params = camera.getParameters();
					PlanarYUVLuminanceSource source = new PlanarYUVLuminanceSource(data, params.getPreviewSize().width, params.getPreviewSize().height, 
															0, 0, params.getPreviewSize().width, params.getPreviewSize().height, false);
					BinaryBitmap bbmap = new BinaryBitmap(new HybridBinarizer(source));
					QRCodeMultiReader reader = new QRCodeMultiReader();
					
					try {
						Result[] results = reader.decodeMultiple(bbmap);
						for (int r = 0; r < results.length; r++ ) {
							Log.d(TAG,"RESULT: "+results[r].getText());
							for ( int rpi = 0; rpi < results[r].getResultPoints().length; rpi++) {
								ResultPoint rp = results[r].getResultPoints()[rpi];
								Log.d(TAG,"\t"+rp.getX()+","+rp.getY());
							}
						}
						parent.newResults(results);
					} catch (Exception e) {
						Log.e(TAG, "ERROR", e);
					}
					
					parent.energyView.invalidate(); 
					
        		}
				troll++;
			}
        });
    }
 
    public void surfaceDestroyed( SurfaceHolder holder ) {
        // Once the surface gets destroyed, we stop the preview mode and release
        // the whole camera since we no longer need it.
    	if (camera != null) {
    		camera.stopPreview();
    		camera.setPreviewCallback(null);
    		camera.release();
    		camera = null;
    	}
    }
}
