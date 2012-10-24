package edu.berkeley.sensezilla.energyeyes;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.ArrayList;

import android.os.Bundle;
import android.os.Handler;
import android.app.Activity;
import android.content.pm.ActivityInfo;
import android.graphics.PixelFormat;
import android.util.Log;
import android.view.Menu;
import android.view.ViewGroup.LayoutParams;
import android.view.Window;
import android.view.WindowManager;
import android.widget.FrameLayout;

import com.google.zxing.*;

public class EnergyActivity extends Activity {
	private Handler httpHandler = new Handler();
	private Runnable httpThreadTask = new Runnable() {
		public void run() {
			try{
				URL url  = new URL("http://sensezilla.berkeley.edu:7500/qrdata");
				HttpURLConnection conn = (HttpURLConnection) url.openConnection();
				BufferedReader cin = new BufferedReader(new InputStreamReader(conn.getInputStream()));
				String buf;
				while ( (buf = cin.readLine()) != null ) {
					String[] pts = buf.split(":");
					boolean found = false;
					double val = Double.parseDouble(pts[1]);
					synchronized(reslist) {
						for (MutableResult res : reslist) {
							if ( res.tag.equals(pts[0])) {
								res.value = val;
								found = true;
								break;
							}
						}
						if (!found) {
							reslist.add(new MutableResult(pts[0],val));
						}
					}
				}
				cin.close();
			} catch (Exception exp) {
				Log.e(EnergyActivity.class.getSimpleName(), "Error fetching data", exp);
			}			
		}
	};
	
	private Runnable httpTask = new Runnable() {
		public void run() {
			httpHandler.postDelayed(httpTask, 1000);
			new Thread(httpThreadTask).start();
			
		}
	};
	
	
	CustomCameraView cameraView;
	EnergyView energyView;
	
	ArrayList<MutableResult> reslist = new ArrayList<MutableResult>();
	long timeout = 2000;
	
	public void newResults(Result[] results) {
		synchronized (reslist) {
			for (Result r :results ) {
				MutableResult mr = new MutableResult(r);
				boolean found = false;
				for ( MutableResult m2 : reslist) {
					if ( m2.update(mr) ) {
						found = true;
						break;
					}
				}
			}
		}
	}
	
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
        energyView = new EnergyView( this );
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
        addContentView( energyView, new LayoutParams(LayoutParams.WRAP_CONTENT,LayoutParams.WRAP_CONTENT ));
        onResume();
        httpHandler.postDelayed(httpTask, 1);
    }

    @Override
    public void onDestroy() {
    	super.onDestroy();
    	httpHandler.removeCallbacks(httpTask);
    }
    
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.activity_energy, menu);
        return true;
    }
    
    @Override
    protected void onResume() {
    super.onResume();
    httpHandler.postDelayed(httpTask, 1);
    }

    @Override
    protected void onPause() {
    super.onPause();
    httpHandler.removeCallbacks(httpTask);
    }
}

class MutableResult {
	int x1,y1,x2,y2;
	String tag;
	long lastseen;
	
	double value;
	
	public MutableResult(String t, double v) {
		tag = t;
		value = v;
		x1 = x2 = y1 = y2 = 0;
		lastseen = 0;
	}
	
	public MutableResult(String t, ResultPoint[] rps) {
		tag = t;
		x1 = 100000;
		y1 = 100000;
		x2 = 0;
		y2 = 0;
		for (int i =0; i < rps.length; i++) 
		{
			if (rps[i].getX() < x1 ) {
				x1 = (int)rps[i].getX();
			}
			
			if ( rps[i].getY() < y1 ) {
				y1 = (int)rps[i].getY();
			}
			
			if (rps[i].getX() > x2 ) {
				x2 = (int)rps[i].getX();
			}
			
			if ( rps[i].getY() > y2 ) {
				y2 = (int)rps[i].getY();
			}
		}
		lastseen = System.currentTimeMillis();
	}
	
	public MutableResult(Result r) {
		this(r.getText(),r.getResultPoints());
	}

	public boolean update(MutableResult m2) {
		if ( !tag.equals(m2.tag) )
			return false;
		else {
			x1 = m2.x1;
			x2 = m2.x2;
			y1 = m2.y1;
			y2 = m2.y2;
			lastseen = System.currentTimeMillis();
			return true;
		}

	}
}