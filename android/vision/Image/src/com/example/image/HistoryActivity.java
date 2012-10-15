package com.example.image;

import android.os.Bundle;
import android.os.Handler;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Paint.Align;
import android.util.DisplayMetrics;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.TextView;
import android.support.v4.app.NavUtils;

public class HistoryActivity extends Activity {
	private Intent intent;
	private String s;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        intent = getIntent();
        s = intent.getStringExtra("flag");
        s = "The history information of device " + s;
        setContentView(new myView(this));
        getActionBar().setDisplayHomeAsUpEnabled(true);
    }
    
    public class myView extends View{
    	private Paint paint;
		public myView(Context context) {
			super(context);
			paint = new Paint();
		}
		protected void onDraw(Canvas canvas){
			DisplayMetrics dm = new DisplayMetrics();
			getWindowManager().getDefaultDisplay().getMetrics(dm);
			int width = dm.widthPixels;
			int height = dm.heightPixels;
			paint.setColor(Color.RED);
			paint.setTextAlign(Align.LEFT);
			paint.setTextSize(25);
			canvas.drawText(s, height/2, 20, paint);
		}
    };
    
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                NavUtils.navigateUpFromSameTask(this);
                return true;
        }
        return super.onOptionsItemSelected(item);
    }
}
