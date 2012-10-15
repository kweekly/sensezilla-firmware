package com.example.image;

import android.os.Bundle;
import android.app.Activity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.TextView;
import android.support.v4.app.NavUtils;

public class SettingsActivity extends Activity {
	
	public static int bx = 0;
	public static int by = 0;
	public static int bz = 0;
	public static int gx = 0;
	public static int gy = 0;
	public static int gz = 0;
	public static int yx = 0;
	public static int yy = 0;
	public static int yz = 0;
	public static int px = 0;
	public static int py = 0;
	public static int pz = 0;
	
	public static int devicenumber = 1;
	public static int dx[] = {0,0,0,0};
	public static int dy[] = {0,0,0,0};
	public static int dz[] = {0,0,0,0};

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_settings);
        getActionBar().setDisplayHomeAsUpEnabled(true);
    }
    
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                NavUtils.navigateUpFromSameTask(this);
                return true;
        }
        return super.onOptionsItemSelected(item);
    }
    public void updateMessage(View view){
    	TextView tv = (TextView) findViewById(R.id.blue_x);
    	bx = Integer.parseInt(tv.getText().toString());
    	tv = (TextView) findViewById(R.id.blue_y);
    	by = Integer.parseInt(tv.getText().toString());
    	tv = (TextView) findViewById(R.id.blue_z);
    	bz = Integer.parseInt(tv.getText().toString());
    	tv = (TextView) findViewById(R.id.green_x);
    	gx = Integer.parseInt(tv.getText().toString());
    	tv = (TextView) findViewById(R.id.green_y);
    	gy = Integer.parseInt(tv.getText().toString());
    	tv = (TextView) findViewById(R.id.green_z);
    	gz = Integer.parseInt(tv.getText().toString());
    	tv = (TextView) findViewById(R.id.yellow_x);
    	yx = Integer.parseInt(tv.getText().toString());
    	tv = (TextView) findViewById(R.id.yellow_y);
    	yy = Integer.parseInt(tv.getText().toString());
    	tv = (TextView) findViewById(R.id.yellow_z);
    	yz = Integer.parseInt(tv.getText().toString());
    	tv = (TextView) findViewById(R.id.pink_x);
    	px = Integer.parseInt(tv.getText().toString());
    	tv = (TextView) findViewById(R.id.pink_y);
    	py = Integer.parseInt(tv.getText().toString());
    	tv = (TextView) findViewById(R.id.pink_z);
    	pz = Integer.parseInt(tv.getText().toString());
    	tv = (TextView) findViewById(R.id.devicenumber);
    	devicenumber = Integer.parseInt(tv.getText().toString());
    	tv = (TextView) findViewById(R.id.d1_x);
    	dx[0] = Integer.parseInt(tv.getText().toString());
    	tv = (TextView) findViewById(R.id.d1_y);
    	dy[0] = Integer.parseInt(tv.getText().toString());
    	tv = (TextView) findViewById(R.id.d1_z);
    	dz[0] = Integer.parseInt(tv.getText().toString());
    	tv = (TextView) findViewById(R.id.d2_x);
    	dx[1] = Integer.parseInt(tv.getText().toString());
    	tv = (TextView) findViewById(R.id.d2_y);
    	dy[1] = Integer.parseInt(tv.getText().toString());
    	tv = (TextView) findViewById(R.id.d2_z);
    	dz[1] = Integer.parseInt(tv.getText().toString());
    	tv = (TextView) findViewById(R.id.d3_x);
    	dx[2] = Integer.parseInt(tv.getText().toString());
    	tv = (TextView) findViewById(R.id.d3_y);
    	dy[2] = Integer.parseInt(tv.getText().toString());
    	tv = (TextView) findViewById(R.id.d3_z);
    	dz[2] = Integer.parseInt(tv.getText().toString());
    	tv = (TextView) findViewById(R.id.d4_x);
    	dx[3] = Integer.parseInt(tv.getText().toString());
    	tv = (TextView) findViewById(R.id.d4_y);
    	dy[3] = Integer.parseInt(tv.getText().toString());
    	tv = (TextView) findViewById(R.id.d4_z);
    	dz[3] = Integer.parseInt(tv.getText().toString());
    }

}
