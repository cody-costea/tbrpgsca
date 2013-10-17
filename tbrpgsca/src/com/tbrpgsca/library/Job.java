/*This file has been written by Claudiu-Stefan Costea,
 * and is released under the terms of the LGPL-2.1
 */
package com.tbrpgsca.library;

import java.util.Locale;

import android.app.Activity;
import android.graphics.drawable.Drawable;
public class Job extends Race {
	public String jname;    
    String bSprite[] = new String[44];        
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
        setSprite(this.jname.toLowerCase(Locale.US));
        this.hpp = hpp;
        this.mpp = mpp;
        this.spp = spp;
        this.atkp = atkp;
        this.defp = defp;
        this.wisp = wisp;
        this.spip = spip;
        this.agip = agip;
    }    
    protected void setSprite(String spr) {
    	if (spr!="") spr="hero";
    	String[] sprName= {"act1","act2","act3","act4","act5","act6","act7",
    			"act8", "hit1","hit2","hit3","hit4","hit5","hit6", "fallen1",
    			"fallen2","fallen3","fallen4","fallen5","fallen6","fallen7","fallen8",
    			"actr1","actr2","actr3","actr4","actr5","actr6","actr7","actr8",
    			"hitr1","hitr2","hitr3","hitr4","hitr5","hitr6", "fallenr1", "fallenr2",
    			"fallenr3","fallenr4","fallenr5","fallenr6","fallenr7","fallenr8"};
        for (int i=0;i<sprName.length&&i<bSprite.length;i++) {
            this.bSprite[i] = ("b_"+spr+"_"+sprName[i]);
        }
    }
    public Drawable getBtSprite(int i, Activity context){
    	return context.getResources().getDrawable(context.getResources().getIdentifier(bSprite[i], "drawable", context.getPackageName()));
    }    
}