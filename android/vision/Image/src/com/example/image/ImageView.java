package com.example.image;

import org.opencv.android.Utils;
import org.opencv.calib3d.Calib3d;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.MatOfDouble;
import org.opencv.core.MatOfPoint;
import org.opencv.core.MatOfPoint2f;
import org.opencv.core.MatOfPoint3f;
import org.opencv.core.Point;
import org.opencv.core.Point3;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.highgui.Highgui;
import org.opencv.highgui.VideoCapture;
import org.opencv.imgproc.Imgproc;

import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.util.Log;
import android.view.MotionEvent;
import android.view.SurfaceHolder;

public class ImageView extends CameraView{
	private Size mSizeRgba;
    
    private Mat mRgba;
    private Mat mIntermediateMat;
    
    private Point gp,yp,bp,pp;

    private Mat mSepiaKernel;
    
    private MatOfPoint3f objectPoints;
    private MatOfPoint2f imagePoints;
    private MatOfPoint3f devicePoints3;
    private MatOfPoint2f devicePoints2;
    private Mat cameraMatrix;
    MatOfDouble distCoeffs;
    
    public int action = MotionEvent.ACTION_UP;
    private int PosX = 0;
    private int PosY = 0;
    public int jumpflag = 0;
    
    public int power[] = {0,0,0,0};
    
    public ImageView(Context context) {
        super(context);
        this.setFocusable(true);
        this.setFocusableInTouchMode(true);

        mSepiaKernel = new Mat(4, 4, CvType.CV_32F);
        mSepiaKernel.put(0, 0, /* R */0.189f, 0.769f, 0.393f, 0f);
        mSepiaKernel.put(1, 0, /* G */0.168f, 0.686f, 0.349f, 0f);
        mSepiaKernel.put(2, 0, /* B */0.131f, 0.534f, 0.272f, 0f);
        mSepiaKernel.put(3, 0, /* A */0.000f, 0.000f, 0.000f, 1f);
    }
    
    public boolean onTouchEvent(MotionEvent event){
    	action = event.getAction();
    	PosX = (int) event.getX();
    	PosY = (int) event.getY();
    	return true;
    }

    @Override
	public void surfaceCreated(SurfaceHolder holder) {
        synchronized (this) {
            // initialize Mats before usage
            mRgba = new Mat();
            mIntermediateMat = new Mat();
            objectPoints = new MatOfPoint3f();
            imagePoints = new MatOfPoint2f();
            cameraMatrix = new Mat(3, 3, CvType.CV_64FC1);
            double temp[] = {-0.0764,0.11513,-0.03593,-0.0031,0.0};
            distCoeffs = new MatOfDouble(temp);
            gp = new Point(0,0);
            yp = new Point(0,0);
            bp = new Point(0,0);
            pp = new Point(0,0);
            Point3 landmark[] = {new Point3(SettingsActivity.gx,SettingsActivity.gy,SettingsActivity.gz), new Point3(SettingsActivity.yx,SettingsActivity.yy,SettingsActivity.yz), new Point3(SettingsActivity.bx,SettingsActivity.by,SettingsActivity.bz), new Point3(SettingsActivity.px,SettingsActivity.py,SettingsActivity.pz)};
            objectPoints.fromArray(landmark);
            devicePoints3 = new MatOfPoint3f();
            devicePoints2 = new MatOfPoint2f();
            cameraMatrix.put(0, 0, 2756.07, 0.0, 1490.84);
            cameraMatrix.put(1, 0, 0.0, 2922.16, 1893.17);
            cameraMatrix.put(2, 0, 0.0, 0.0, 1.0);
        }

        super.surfaceCreated(holder);
	}

	private void CreateAuxiliaryMats() {
        if (mRgba.empty())
            return;

        mSizeRgba = mRgba.size();
    }
	
	private boolean isGreen(double [] data){
		int green = 80;
		if((data[0]>green-15)&&(data[0]<green+15)&&(data[1]>120))
			return true;
		else
			return false;
	}
	
	private boolean isYellow(double [] data){
		int yellow = 40;
		if((data[0]>yellow-10)&&(data[0]<yellow+10)&&(data[1]>150))
			return true;
		else
			return false;
	}
	
	private boolean isBlue(double [] data){
		int blue = 160;
		if((data[0]>blue-20)&&(data[0]<blue+20)&&(data[1]>120))
			return true;
		else
			return false;
	}
	
	private boolean isPink(double [] data){
		int pink = 250;
		if((data[0]>pink-20)&&(data[0]<pink+20)&&(data[1]>100))
			return true;
		else
			return false;
	}

    @Override
    protected Bitmap processFrame(VideoCapture capture) {

        capture.retrieve(mRgba, Highgui.CV_CAP_ANDROID_COLOR_FRAME_RGBA);
        if (mSizeRgba == null)
            CreateAuxiliaryMats();
            
        Imgproc.cvtColor(mRgba, mIntermediateMat, Imgproc.COLOR_RGB2HSV_FULL);
        double [] data = new double[4];
        int greeni = 0, yellowi = 0, bluei = 0, pinki = 0;
        int greenj = 0, yellowj = 0, bluej = 0, pinkj = 0;
        int greennum = 0, yellownum = 0, bluenum = 0, orangenum = 0, pinknum = 0;
        
        for(int i = 5; i < mIntermediateMat.rows()-5; i+=5){
            for(int j = 5;j < mIntermediateMat.cols()-5; j+=5){
            	data = mIntermediateMat.get(i,j);
            	if(data[2]<100) continue;
            		
            	if(isGreen(data)){
            		greeni+=i;
            		greenj+=j;
            		greennum++;
            		//Core.rectangle(mRgba, new Point(j,i), new Point(j+5,i+5), new Scalar(0,255,0,255), -1);
            	}
            	if(isYellow(data)){
            		yellowi+=i;
            		yellowj+=j;
            		yellownum++;
            		//Core.rectangle(mRgba, new Point(j,i), new Point(j+5,i+5), new Scalar(255,255,0,255), -1);
            	}
            	if(isBlue(data)){
            		bluei+=i;
            		bluej+=j;
            		bluenum++;
            		//Core.rectangle(mRgba, new Point(j,i), new Point(j+5,i+5), new Scalar(0,0,255,255), -1);
            	}
            	if(isPink(data)){
            		pinki+=i;
            		pinkj+=j;
            		pinknum++;
            		//Core.rectangle(mRgba, new Point(j,i), new Point(j+5,i+5), new Scalar(255,0,100,255), -1);
            	}
            }
        }
        
        if(greennum > 0){
            gp.x = greenj/greennum;
            gp.y = greeni/greennum;
            Core.rectangle(mRgba, gp, new Point(gp.x+5,gp.y+5), new Scalar(0,255,0,255), -1);
        }
        if(yellownum > 0){
            yp.x = yellowj/yellownum;
            yp.y = yellowi/yellownum;
            Core.rectangle(mRgba, yp, new Point(yp.x+5,yp.y+5), new Scalar(255,255,0,255), -1);
        }
        if(bluenum > 0){
            bp.x = bluej/bluenum;
            bp.y = bluei/bluenum;
            Core.rectangle(mRgba, bp, new Point(bp.x+5,bp.y+5), new Scalar(0,0,255,255), -1);
        }
        if(pinknum > 0){
            pp.x = pinkj/pinknum;
            pp.y = pinki/pinknum;
            Core.rectangle(mRgba, pp, new Point(pp.x+5,pp.y+5), new Scalar(255,0,100,255), -1);
        }
        if((greennum > 0)&&(yellownum > 0)&&(bluenum > 0)&&(pinknum > 0)){
            Point point[] = {gp, yp, bp, pp};
            imagePoints.fromArray(point);
            Mat rvec = new Mat();
            Mat tvec = new Mat();
            Calib3d.solvePnP(objectPoints, imagePoints, cameraMatrix, distCoeffs, rvec, tvec);
            for(int i = 0; i < SettingsActivity.devicenumber; i++){
            	Scalar color = SetDevicePoints(i);
            	Calib3d.projectPoints(devicePoints3, rvec, tvec, cameraMatrix, distCoeffs, devicePoints2);
            	Point [] device = devicePoints2.toArray();
            	if((action==MotionEvent.ACTION_MOVE)&&(PosX>device[0].x-60)&&(PosX<device[0].x+60)&&(PosY>device[0].y-60)&&(PosY<device[0].y+30))
            		power[i] = 1 - power[i];
            	DrawTransparentCloud(mRgba, device, 30, power[i]);
            }
        }
        else
        	Core.putText(mRgba, "I can't find enough landmarks.", new Point(20,20), Core.FONT_HERSHEY_SIMPLEX, 1, new Scalar(255,0,0,255),2);
        if(action==MotionEvent.ACTION_DOWN){
        	jumpflag = 1;
        }
        
        Bitmap bmp = Bitmap.createBitmap(mRgba.cols(), mRgba.rows(), Bitmap.Config.ARGB_8888);

        try {
        	Utils.matToBitmap(mRgba, bmp);
            return bmp;
        } catch(Exception e) {
        	Log.e("org.opencv.samples.puzzle15", "Utils.matToBitmap() throws an exception: " + e.getMessage());
            bmp.recycle();
            return null;
        }
    }
    
    private void DrawLine(Mat img){
    	Point [] yaxis1 = new Point[9];
    	Point [] yaxis2 = new Point[9];
    	for(int i = 0; i < 9; i++){
    		yaxis1[i] = new Point(img.cols()/20,(i+1)*img.rows()/10);
    		yaxis2[i] = new Point(img.cols()*19/20,(i+1)*img.rows()/10);
    	}
    	int width = img.cols()/30;
    	int height = img.rows()*3/20;
    	Scalar red = new Scalar(255, 0, 0, 255);
    	Scalar blue = new Scalar(0, 0, 255, 255);
    	//draw x axis
    	Core.line(img, yaxis1[0], yaxis2[0], red, 3);
    	Core.line(img, yaxis1[8], yaxis2[8], red, 3);
    	for(int i = 1; i < 8; i++){
    		Core.line(img, yaxis1[i], yaxis2[i], red, 1);
    	}
    	//draw y axis
    	Core.line(img, yaxis1[0], yaxis1[8], red, 3);
    	Core.line(img, yaxis2[0], yaxis2[8], red, 3);
    	//draw mark
    	//for(int i = 1; i < 5; i++){
    		//Core.line(img, new Point(origin.x+i*6*width,origin.y), new Point(origin.x+i*6*width,origin.y-5), red, 2);
    	//}
    	//draw statics
    	//double statics[] = {2,1,2,3,1,2,4,6,7,6,6,5,7,8,9,7,6,7,5,6,5,3,2,1,1};
    	//for(int i = 0; i < 24; i++){
    		//Core.line(img, new Point(origin.x+i*width,height*(10-statics[i])/10), new Point(origin.x+(i+1)*width,height*(10-statics[i+1])/10), new Scalar(255, 0, 0, 255),2);
    	//}
    	//draw text
    	//String text = "monthly";
    	//Core.putText(img, text, new Point(0,divide), Core.FONT_HERSHEY_SIMPLEX, 1, blue,2);
    }

    private Scalar SetDevicePoints(int n){
    	Scalar color = new Scalar(0, 255, 0, 255);
    	Point3 device[] = {new Point3(SettingsActivity.dx[n],SettingsActivity.dy[n],SettingsActivity.dz[n])};
    	devicePoints3.fromArray(device);
    	return color;
    }
    
    private void DrawTransparentCloud(Mat img, Point [] center1, int size, int p){
    	Scalar color0 = new Scalar(0, 255, 0, 255);
    	Scalar color1 = new Scalar(255, 0, 0, 255);
    	Mat temp = new Mat();
    	Point center = center1[0];
    	double [] data1,data2;
    	int istart = (int) (center.y-2*size);
    	int iend = (int) (center.y+size);
    	int jstart = (int) (center.x-2*size);
    	int jend = (int) (center.x+2*size);
    	istart = (istart>0)?istart:0;
    	iend = (iend<img.rows())?iend:img.rows();
    	jstart = (jstart>0)?jstart:0;
    	jend = (jend<img.cols())?jend:img.cols();
    	img.copyTo(temp);
    	switch(p){
    	case 0:
    		Core.rectangle(temp, new Point(center.x-size,center.y), new Point(center.x+size,center.y+size), color0, -1);
        	Core.circle(temp, new Point(center.x-size,center.y), size, color0, -1);
        	Core.circle(temp, new Point(center.x+size,center.y), size, color0, -1);
        	Core.circle(temp, new Point(center.x+size/4, center.y-size), size, color0, -1);
        	break;
    	case 1:
    		Core.rectangle(temp, new Point(center.x-size,center.y), new Point(center.x+size,center.y+size), color1, -1);
        	Core.circle(temp, new Point(center.x-size,center.y), size, color1, -1);
        	Core.circle(temp, new Point(center.x+size,center.y), size, color1, -1);
        	Core.circle(temp, new Point(center.x+size/4, center.y-size), size, color1, -1);
        	break;
    	}
    	for(int i = istart; i < iend; i++){
    		for(int j = jstart; j < jend; j++){
    			data1 = img.get(i, j);
    			data2 = temp.get(i, j);
    			for(int k = 0; k < 3; k++)
    				data1[k] = (data1[k] + data2[k])/2;
    			img.put(i, j, data1);
    		}
    	}
    	switch(p){
    	case 0:
    		Core.putText(mRgba, "Off", new Point(center.x,center.y), Core.FONT_HERSHEY_SIMPLEX, 1, color1, 2);
    		break;
    	case 1:
    		Core.putText(mRgba, "On", new Point(center.x,center.y), Core.FONT_HERSHEY_SIMPLEX, 1, color0, 2);
    		break;
    	}
    }
    private void DrawTransparentPoly(Mat img, Point [] device, Scalar color){
    	Mat temp = new Mat();
    	int pointnumber = device.length;
    	int istart = (int) device[0].y;
    	int iend = (int) device[0].y;
    	int jstart = (int) device[0].x;
    	int jend = (int) device[0].x;
    	for(int k = 1; k < pointnumber; k++){
    		if(istart > device[k].y) istart = (int) device[k].y;
    		if(iend < device[k].y) iend = (int) device[k].y;
    		if(jstart > device[k].x) jstart = (int) device[k].x;
    		if(jend < device[k].x) jend = (int) device[k].x;
    	}
    	istart = (istart>0)?istart:0;
    	iend = (iend<img.rows())?iend:img.rows();
    	jstart = (jstart>0)?jstart:0;
    	jend = (jend<img.cols())?jend:img.cols();
    	double [] data1,data2;
    	img.copyTo(temp);
    	MatOfPoint point = new MatOfPoint();
    	point.fromArray(device);
    	Core.fillConvexPoly(temp, point, color);
    	for(int i = istart; i < iend; i++){
    		for(int j = jstart; j < jend; j++){
    			data1 = img.get(i, j);
    			data2 = temp.get(i, j);
    			for(int k = 0; k < 3; k++)
    				data1[k] = (data1[k] + data2[k])/2;
    			img.put(i, j, data1);
    		}
    	}
    }
    @Override
    public void run() {
        super.run();

        synchronized (this) {
            // Explicitly deallocate Mats
            if (mRgba != null)
                mRgba.release();
            if (mIntermediateMat != null)
                mIntermediateMat.release();

            mRgba = null;
            mIntermediateMat = null;
        }
    }
}
