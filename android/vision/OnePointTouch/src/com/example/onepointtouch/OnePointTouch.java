package com.example.onepointtouch;

import android.os.Bundle;
import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.support.v4.app.NavUtils;

public class OnePointTouch extends Activity {

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(new myView(this));
    }
    public class myView extends View{
    	Bitmap mBitmap = null;
    	Paint mPaint = null;
    	int mPosX = 0;
    	int mPosY = 0;
    	int action = -1;
    	long mActionTime = 0L;
    	public myView(Context context) {
			super(context);
			// TODO Auto-generated constructor stub
			this.setFocusable(true);
			this.setFocusableInTouchMode(true);
			mBitmap = BitmapFactory.decodeResource(getResources(), R.drawable.ic_launcher);
			mPaint = new Paint();
		}
    	protected void onDraw(Canvas canvas){
    		mPaint.setColor(Color.BLUE);
    		canvas.drawText("当前X坐标:"+mPosX, 0, 20, mPaint);
    		canvas.drawText("当前Y坐标:"+mPosY, 0, 40, mPaint);
    		canvas.drawText("事件触发时间:"+mActionTime, 0, 60, mPaint);
    		switch(action){
    		case MotionEvent.ACTION_DOWN:
    			mPaint.setColor(Color.BLUE);
        		canvas.drawBitmap(mBitmap, mPosX, mPosY, mPaint);
    			mPaint.setColor(Color.RED);
    			canvas.drawRect(490, 0, 540, 50, mPaint);
    			break;
    		case MotionEvent.ACTION_MOVE:
    			mPaint.setColor(Color.BLUE);
        		canvas.drawBitmap(mBitmap, mPosX, mPosY, mPaint);
    			mPaint.setColor(Color.GREEN);
    			canvas.drawRect(490, 0, 540, 50, mPaint);
    			break;
    		case MotionEvent.ACTION_UP:
    			break;
    		}
    	}
    	public boolean onTouchEvent(MotionEvent event){
    		action = event.getAction();
    		mPosX = (int) event.getX();
    		mPosY = (int) event.getY();
    		switch(action){
    		case MotionEvent.ACTION_DOWN:
    			Log.v("test","ACTION_DOWN");
    			break;
    		case MotionEvent.ACTION_MOVE:
    			Log.v("test","ACTION_MOVE");
    			break;
    		case MotionEvent.ACTION_UP:
    			Log.v("test","ACTION_UP");
    			break;
    		}
    		mActionTime = event.getEventTime();
    		postInvalidate();
			return true;
    	}
    };
}
