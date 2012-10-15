package edu.berkeley.sensezilla.energyeyes;

import android.os.Bundle;
import android.app.Activity;
import android.content.pm.ActivityInfo;
import android.graphics.PixelFormat;
import android.view.Menu;
import android.view.ViewGroup.LayoutParams;
import android.view.Window;
import android.view.WindowManager;
import android.widget.FrameLayout;

public class EnergyActivity extends Activity {
	CustomCameraView cameraView;
	CustomGLSurfaceView glView;
	
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate( savedInstanceState );
        
        // When working with the camera, it's useful to stick to one orientation.
        setRequestedOrientation( ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE );
     
        // Next, we disable the application's title bar...
        requestWindowFeature( Window.FEATURE_NO_TITLE );
        // ...and the notification bar. That way, we can use the full screen.
        getWindow().setFlags( WindowManager.LayoutParams.FLAG_FULLSCREEN,
                                WindowManager.LayoutParams.FLAG_FULLSCREEN );
     
        // Now let's create an OpenGL surface.
        glView = new CustomGLSurfaceView( this );
        // The renderer will be implemented in a separate class, GLView, which I'll show next.
        // Now set this as the main view.
        
     
        // Now also create a view which contains the camera preview...
        cameraView = new CustomCameraView( this );
        // ...and add it, wrapping the full screen size.
        /*
        setContentView( glView );
        addContentView( cameraView, new LayoutParams( LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT ) );
        */
        setContentView( cameraView, new LayoutParams( LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT ) );
        addContentView( glView, new LayoutParams(LayoutParams.WRAP_CONTENT,LayoutParams.WRAP_CONTENT ));
        onResume();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.activity_energy, menu);
        return true;
    }
    /*
    @Override
    protected void onResume() {
    super.onResume();
    glView.onResume();
    glView.bringToFront();
    }

    @Override
    protected void onPause() {
    super.onPause();
    glView.onPause();
    finish();
    }*/
}
