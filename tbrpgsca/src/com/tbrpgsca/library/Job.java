/*This file has been written by Claudiu-Stefan Costea,
 * and is released under the terms of the LGPL-2.1
 */
package com.tbrpgsca.library;

import java.util.Locale;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.AnimationDrawable;
import android.graphics.drawable.BitmapDrawable;
public class Job extends Race {
	public String jname,bsprite;
	DataApp data;
    public int hpp,mpp,spp,atkp,defp,wisp,spip,agip;
    
    public Job(){
    	this("Hero", 0, 0, 0, 0, 0, 0, 0, 0);
    }
    
    public Job(String jname, int hpp, int mpp, int spp, int atkp, int defp,
            int wisp, int spip, int agip){
    	this.jname=jname;
    	this.jobStats(hpp, mpp, spp, atkp, defp, wisp, spip, agip);
        this.skill.add(0);
        this.skill.add(1);
    }    
    
    protected void jobStats(int hpp, int mpp, int spp, int atkp, int defp,
            int wisp, int spip, int agip) {        
        this.bsprite=(this.jname!="")?"hero":jname.toLowerCase(Locale.US);
        this.hpp = hpp;
        this.mpp = mpp;
        this.spp = spp;
        this.atkp = atkp;
        this.defp = defp;
        this.wisp = wisp;
        this.spip = spip;
        this.agip = agip;
    }
        
    public AnimationDrawable getBtSprite(int i, String pos, boolean bkw, int[] time, Activity context){
    	Bitmap bmp=null; int h=128,w=h,nr=7; if (i>2) time[1]=0;//System.gc();
    	AnimationDrawable sprite=new AnimationDrawable();
    	/*BitmapFactory.Options opt = new BitmapFactory.Options();
    	opt.inPurgeable=true;opt.inDither=false;opt.inSampleSize=1;*/
    	if (i>0) { 
    		bmp = BitmapFactory.decodeResource(context.getResources(), context.getResources().getIdentifier("bt_"+bsprite+"_"+i+"_"+pos, "drawable", context.getPackageName())/*,opt*/);
    		h=bmp.getHeight();nr=bmp.getWidth()/h;w=bmp.getWidth()/nr;    		
    		for (int j=0;j<nr;j++){
    			int t=(i!=1||j!=0)?87:261;
    			sprite.addFrame(new BitmapDrawable(context.getResources(),Bitmap.createBitmap(bmp, j*w, 0, w, h)),t);
    			if (i>2) time[1]+=t;
    		}
    		for (int j=nr-2;bkw&&j>0;j--){
    			sprite.addFrame(new BitmapDrawable(context.getResources(),Bitmap.createBitmap(bmp, j*w, 0, w, h)),87);
    			if (i>2) time[1]+=87;
    		}
    	}
    	if (i!=2) {
    		if (i!=1) {
    			bmp = BitmapFactory.decodeResource(context.getResources(), context.getResources().getIdentifier("bt_"+bsprite+"_1_"+pos, "drawable", context.getPackageName())/*,opt*/);
    			h=bmp.getHeight();nr=bmp.getWidth()/h;w=bmp.getWidth()/nr;
    		}
    		sprite.addFrame(new BitmapDrawable(context.getResources(),Bitmap.createBitmap(bmp, 0, 0, w, h)),0);
    	}
    	bmp.recycle(); return sprite;
    }
}