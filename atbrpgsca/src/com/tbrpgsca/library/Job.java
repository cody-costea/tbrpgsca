/*
Copyright (C) 2016 Claudiu-Stefan Costea

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. 
*/
package com.tbrpgsca.library;

import java.util.Locale;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.AnimationDrawable;
import android.graphics.drawable.BitmapDrawable;

public class Job extends Race {
	public int getAgiInc() {
		return this.agip;
	}

	public int getSpiInc() {
		return this.spip;
	}

	public int getWisInc() {
		return this.wisp;
	}

	public int getDefInc() {
		return this.defp;
	}

	public int getAtkInc() {
		return this.atkp;
	}

	public int getRPinc() {
		return this.spp;
	}

	public int getMPinc() {
		return this.mpp;
	}

	public int getHPinc() {
		return this.hpp;
	}

	public String getJobName() {
		return this.jname;
	}

	protected String jname;
	private String sprName;
	protected int hpp, mpp, spp, atkp, defp, wisp, spip, agip;

	private AnimationDrawable bSprite[] = new AnimationDrawable[5];
	private int sprWait[] = new int[5];

	public Job() {
		this("Hero", 0, 0, 0, 0, 0, 0, 0, 0);
	}

	public Job(String jname, int hp, int mp, int sp, int atk, int def, int wis, int spi, int agi, int hpp, int mpp,
			int spp, int atkp, int defp, int wisp, int spip, int agip, int[] newRes, Ability[] newSkill) {
		super("", hp, mp, sp, atk, def, wis, spi, agi, newRes, newSkill);
		this.jname = jname;
		this.setSprName(this.jname);
		this.jobStats(hpp, mpp, spp, atkp, defp, wisp, spip, agip);
	}
	
	public Job(String jname, int hp, int mp, int sp, int atk, int def, int wis, int spi, int agi, int hpp, int mpp,
			int spp, int atkp, int defp, int wisp, int spip, int agip, int[] newRes, Ability[] newSkill, int[] skills) {
		this(jname, hpp, mpp, spp, atkp, defp, wisp, spip, agip, newRes, null);
		this.raceSkills = new Ability[skills.length];
		for (int i = 0; i < skills.length; i++)
			this.raceSkills[i] = newSkill[skills[i]];
	}

	public Job(String jname, int hpp, int mpp, int spp, int atkp, int defp, int wisp, int spip, int agip) {
		this(jname, hpp, mpp, spp, atkp, defp, wisp, spip, agip, new int[] {});
	}

	public Job(String jname, int hpp, int mpp, int spp, int atkp, int defp, int wisp, int spip, int agip,
			int[] newRes) {
		this(jname, hpp, mpp, spp, atkp, defp, wisp, spip, agip, newRes, new Ability[] {});
	}

	public Job(String jname, int hpp, int mpp, int spp, int atkp, int defp, int wisp, int spip, int agip, int[] newRes,
			Ability[] newSkill) {
		this(jname, 0, 0, 0, 0, 0, 0, 0, 0, hpp, mpp, spp, atkp, defp, wisp, spip, agip, newRes, newSkill);
	}
	
	public Job(String jname, int hpp, int mpp, int spp, int atkp, int defp, int wisp, int spip, int agip, int[] newRes,
			Ability[] newSkill, int[] skills) {
		this(jname, 0, 0, 0, 0, 0, 0, 0, 0, hpp, mpp, spp, atkp, defp, wisp, spip, agip, newRes, newSkill, skills);
	}

	protected void jobStats(int hpp, int mpp, int spp, int atkp, int defp, int wisp, int spip, int agip) {
		this.hpp = hpp;
		this.mpp = mpp;
		this.spp = spp;
		this.atkp = atkp;
		this.defp = defp;
		this.wisp = wisp;
		this.spip = spip;
		this.agip = agip;
	}

	protected void setSprName(String sprName, String pos, Activity act) {
		this.sprName = sprName.toLowerCase(Locale.US);
		this.setSprites(act, act != null, pos);
	}

	public void setSprName(String sprName) {
		this.setSprName(sprName, null, null);
	}

	public void setSprites(Activity act, boolean get, String pos) {
		for (int i = 0; i < 5; i++)
			if (get && this.sprName != null && this.sprName != "")
				this.setBtSprite(i - 1, pos, act);
			else
				this.bSprite[i] = null;
	}

	public String getSprName() {
		return this.sprName;
	}

	private Bitmap getBmpSpr(int i, String pos, int[] c, Activity context) {
		Bitmap bmp = BitmapFactory.decodeResource(context.getResources(),
				context.getResources().getIdentifier("bt_" + this.sprName + "_" + String.valueOf((i)) + "_" + pos,
						"drawable", context.getPackageName())/* ,opt */);
		c[0] = bmp.getHeight();
		c[2] = bmp.getWidth() / c[0];
		c[1] = bmp.getWidth() / c[2];
		return bmp;
	}

	private void setBtSprite(int i, String pos, Activity context) {
		Bitmap bmp = null;
		int x = i + 1;
		int p[] = { 128, 128, 7 };
		this.sprWait[x] = 0;
		this.bSprite[x] = new AnimationDrawable();
		if (i > 0) {
			if (i == 2) {
				bmp = getBmpSpr(1, pos, p, context);
				this.bSprite[x].addFrame(
						new BitmapDrawable(context.getResources(), Bitmap.createBitmap(bmp, 0, 0, p[1], p[0])), 261);
				this.sprWait[x] += 261;
			}
			bmp = getBmpSpr(i, pos, p, context);
			for (int j = 0; j < p[2]; j++) {
				int t = (i != 1 || j != 0) ? 87 : 261;
				this.bSprite[x].addFrame(
						new BitmapDrawable(context.getResources(), Bitmap.createBitmap(bmp, j * p[1], 0, p[1], p[0])),
						t);
				this.sprWait[x] += t;
			}
			for (int j = p[2] - 2; p[2] < 7 && j > (i == 1 ? 0 : -1); j--) {
				this.bSprite[x].addFrame(
						new BitmapDrawable(context.getResources(), Bitmap.createBitmap(bmp, j * p[1], 0, p[1], p[0])),
						87);
				this.sprWait[x] += 87;
			}
		}
		if (i != 2 && i >= 0) {
			if (i != 1)
				bmp = getBmpSpr(1, pos, p, context);
			this.bSprite[x].addFrame(
					new BitmapDrawable(context.getResources(), Bitmap.createBitmap(bmp, 0, 0, p[1], p[0])), 0);
		}
		if (i < 0) {
			bmp = getBmpSpr(2, pos, p, context);
			this.bSprite[x].addFrame(new BitmapDrawable(context.getResources(),
					Bitmap.createBitmap(bmp, (p[2] - 1) * p[1], 0, p[1], p[0])), 0);
		}
		bmp.recycle();
	}

	public AnimationDrawable getBtSprite(int i, String pos, int[] time, Activity act) {
		int s = i > 2 ? 1 : 2;
		time[s] = 0;
		i += 1;
		if (this.bSprite[i] == null && this.sprName != "")
			this.setBtSprite(i - 1, pos, act);
		time[s] += this.sprWait[i];
		return this.bSprite[i];
	}

}