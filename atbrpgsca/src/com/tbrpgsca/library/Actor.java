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

import android.os.Parcel;
import android.os.Parcelable;

import java.util.ArrayList;

public class Actor extends Job implements Parcelable {
	protected String name;
	protected boolean active = true, reflect = false;
	protected State state[];
	protected int auto = 0, hp, mp, sp, level, maxlv, exp, maxp, atk, def, spi,
			wis, agi, mres[], res[] = { 3, 3, 3, 3, 3, 3, 3, 3 };

	protected ArrayList<Ability> jobSkills = new ArrayList<Ability>(), items = null;

	protected Actor(Parcel in) {
		this.name = in.readString();
		this.active = in.readByte() != 0;
		this.reflect = in.readByte() != 0;
		this.state = in.createTypedArray(State.CREATOR);
		this.auto = in.readInt();
		this.rname = in.readString();
		this.maxhp = in.readInt();
		this.maxmp = in.readInt();
		this.maxsp = in.readInt();
		this.matk = in.readInt();
		this.mdef = in.readInt();
		this.mwis = in.readInt();
		this.mspi = in.readInt();
		this.magi = in.readInt();
		this.rres = in.createIntArray();
		this.raceSkills = in.createTypedArray(Ability.CREATOR);
		this.jname = in.readString();
		this.sprName = in.readString();
		this.hpp = in.readInt();
		this.mpp = in.readInt();
		this.spp = in.readInt();
		this.atkp = in.readInt();
		this.defp = in.readInt();
		this.wisp = in.readInt();
		this.spip = in.readInt();
		this.agip = in.readInt();
		this.hp = in.readInt();
		this.mp = in.readInt();
		this.sp = in.readInt();
		this.level = in.readInt();
		this.maxlv = in.readInt();
		this.exp = in.readInt();
		this.maxp = in.readInt();
		this.atk = in.readInt();
		this.def = in.readInt();
		this.spi = in.readInt();
		this.wis = in.readInt();
		this.agi = in.readInt();
		this.mres = in.createIntArray();
		this.res = in.createIntArray();
		this.jobSkills = in.createTypedArrayList(Ability.CREATOR);
		this.items = in.createTypedArrayList(Ability.CREATOR);
		this.originId = in.readInt();
	}

	public static final Creator<Actor> CREATOR = new Creator<Actor>() {
		@Override
		public Actor createFromParcel(Parcel in) {
			return new Actor(in);
		}

		@Override
		public Actor[] newArray(int size) {
			return new Actor[size];
		}
	};

	public String getName() {
		return this.name;
	}

	public Actor setName(String name) {
		this.name = name;
		return this;
	}

	public boolean isActive() {
		return this.active;
	}

	public Actor setActive(boolean active) {
		this.active = active;
		return this;
	}

	public boolean isReflecting() {
		return this.reflect;
	}

	public Actor setReflect(boolean reflect) {
		this.reflect = reflect;
		return this;
	}

	public State[] getState() {
		return this.state;
	}

	public int getAuto() {
		return this.auto;
	}

	public Actor setAuto(int auto) {
		this.auto = auto;
		return this;
	}

	public int getHP() {
		return this.hp;
	}

	public Actor setHP(int hp) {
		this.hp = (hp > this.maxp) ? this.maxhp : hp;
		return this;
	}

	public int getMP() {
		return this.mp;
	}

	public Actor setMP(int mp) {
		this.mp = (mp > this.maxmp) ? this.maxmp : mp;
		return this;
	}

	public int getRP() {
		return this.sp;
	}

	public Actor setRP(int sp) {
		this.sp = (sp > this.maxsp) ? this.maxsp : sp;
		return this;
	}

	public int getLevel() {
		return this.level;
	}

	public Actor setLevel(int level) {
		if (level > this.maxlv)
			level = this.maxlv;
		while (this.level < level) {
			this.exp = this.maxp;
			this.levelUp();
		}
		return this;
	}

	public int getMaxLevel() {
		return this.maxlv;
	}

	public Actor setMaxLevel(int maxlv) {
		this.maxlv = maxlv;
		return this;
	}

	public int getExp() {
		return this.exp;
	}

	public Actor setExp(int exp) {
		this.exp = exp;
		this.levelUp();
		return this;
	}

	public int getMaxExp() {
		return this.maxp;
	}

	public int getAtk() {
		return this.atk;
	}

	public Actor setAtk(int atk) {
		this.atk = atk;
		return this;
	}

	public int getDef() {
		return this.def;
	}

	public Actor setDef(int def) {
		this.def = def;
		return this;
	}

	public int getSpi() {
		return this.spi;
	}

	public Actor setSpi(int spi) {
		this.spi = spi;
		return this;
	}

	public int getWis() {
		return this.wis;
	}

	public Actor setWis(int wis) {
		this.wis = wis;
		return this;
	}

	public int getAgi() {
		return this.agi;
	}

	public Actor setAgi(int agi) {
		this.agi = agi;
		return this;
	}

	public int[] getMres() {
		return this.mres;
	}

	public int[] getRes() {
		return this.res;
	}

	public Actor setMaxAgi(int magi) {
		this.magi = magi;
		return this;
	}

	public Actor setMaxSpi(int mspi) {
		this.mspi = mspi;
		return this;
	}

	public Actor setMaxWis(int mwis) {
		this.mwis = mwis;
		return this;
	}

	public Actor setMaxDef(int mdef) {
		this.mdef = mdef;
		return this;
	}

	public Actor setMaxAtk(int matk) {
		this.matk = matk;
		return this;
	}

	public Actor setMaxRP(int maxsp) {
		this.maxsp = maxsp;
		return this;
	}

	public Actor setMaxMP(int maxmp) {
		this.maxmp = maxmp;
		return this;
	}

	public Actor setMaxHP(int maxhp) {
		this.maxhp = maxhp;
		return this;
	}

	public Actor setRaceName(String rname) {
		this.rname = rname;
		return this;
	}

	public Actor setJobName(String jname) {
		this.jname = jname;
		return this;
	}

	public Actor setHPinc(int hpp) {
		this.hpp = hpp;
		return this;
	}

	public Actor setMPinc(int mpp) {
		this.mpp = mpp;
		return this;
	}

	public Actor setRPinc(int spp) {
		this.spp = spp;
		return this;
	}

	public Actor setAtkInc(int atkp) {
		this.atkp = atkp;
		return this;
	}

	public Actor setDefInc(int defp) {
		this.defp = defp;
		return this;
	}

	public Actor setWisInc(int wisp) {
		this.wisp = wisp;
		return this;
	}

	public Actor setSpiInc(int spip) {
		this.spip = spip;
		return this;
	}

	public Actor setAgiInc(int agip) {
		this.agip = agip;
		return this;
	}

	public ArrayList<Ability> getExtraSkills() {
		return this.jobSkills;
	}

	public ArrayList<Ability> getItems() {
		return this.items;
	}

	public Actor setItems(ArrayList<Ability> items) {
		this.items = items;
		return this;
	}

	public Actor setBaseSkills(Ability[] raceSkills) {
		this.setBaseSkills(raceSkills, true);
		return this;
	}

	protected void setBaseSkills(Ability[] raceSkills, boolean clone) {
		this.raceSkills = new Ability[raceSkills.length];
		for (int i = 0; i < raceSkills.length; i++)
			this.raceSkills[i] = clone ? new Ability(raceSkills[i])
					: raceSkills[i];
	}

	public Actor setRace(Race race, boolean init) {
		if (init) {
			this.raceStats(race.maxhp, race.maxmp, race.maxsp, race.matk,
					race.mdef, race.mwis, race.mspi, race.magi);
			this.stats(1, this.maxlv);
		} else
			this.raceStats(this.maxhp + race.maxhp, this.maxmp + race.maxmp,
					this.maxsp + race.maxsp, this.matk + race.matk, this.mdef
							+ race.mdef, this.mwis + race.mwis, this.mspi
							+ race.mspi, this.magi + race.magi);
		this.setRes(race.rres, true);
		this.setBaseSkills(race.raceSkills, true);
		this.rname = race.rname;
		return this;
	}

	public Actor changeJob(Job job, boolean add) {
		this.jname = job.jname;
		if (add)
			this.raceStats(this.maxhp + job.maxhp, this.maxmp + job.maxmp,
					this.maxsp + job.maxsp, this.matk + job.matk, this.mdef
							+ job.mdef, this.mwis + job.mwis, this.mspi
							+ job.mspi, this.magi + job.magi);
		this.jobStats(job.hpp, job.mpp, job.spp, job.atkp, job.defp, job.wisp,
				job.spip, job.agip);
		if (!add) {
			this.jobSkills.clear();
			this.resetRes();
		}
		this.setRes(job.rres, false);
		if (job.raceSkills != null)
			this.addSkills(job.raceSkills, true, true);
		this.setSprName(job.getSprName());
		return this;
	}

	public void checkRes(int r) {
		if (r < 8 && r >= 0) {
			if (this.rres[r] > 7)
				this.rres[r] = 7;
			if (this.rres[r] < 0)
				this.rres[r] = 0;
			if (this.mres[r] > 7)
				this.mres[r] = 7;
			if (this.mres[r] < 0)
				this.mres[r] = 0;
			if (this.res[r] > 7)
				this.res[r] = 7;
			if (this.res[r] < 0)
				this.res[r] = 0;
		}
	}

	public String applyState(boolean consume) {
		String s = "";
		if (this.auto < 2 && this.auto > -2)
			this.auto = 0;
		else
			this.auto = 2;
		if (consume && this.hp > 0)
			this.active = true;
		this.atk = this.matk;
		this.def = this.mdef;
		this.spi = this.mspi;
		this.wis = this.mwis;
		this.agi = this.magi;
		for (int i = 0; i < this.res.length; i++)
			this.res[i] = this.mres[i];
		this.reflect = false;
		boolean c = false;
		for (int i = 0; i < this.state.length; i++)
			if (this.state[i].dur != 0 && this.hp > 0) {
				String r = this.state[i].apply(this, consume);
				if (r.length() > 0) {
					if (c)
						s += ", ";
					if (consume && !c) {
						s += "\n";
						c = true;
					}
					s += r;
				}
			}
		s += checkStatus();
		if (c && consume)
			s += ".";
		return s;
	}

	public String checkStatus() {
		String s = "";
		if (this.hp > this.maxhp)
			this.hp = this.maxhp;
		if (this.mp > this.maxmp)
			this.mp = this.maxmp;
		if (this.sp > this.maxsp)
			this.sp = this.maxsp;
		if (this.hp < 1) {
			s += " (and falls unconcious)";
			this.active = false;
			this.sp = 0;
			for (int i = 0; i < this.state.length; i++)
				this.state[i].remove();
		}
		if (this.hp < -this.maxhp)
			this.hp = -this.maxhp;
		if (this.mp < 0)
			this.mp = 0;
		if (this.sp < 0)
			this.sp = 0;
		return s;
	}

	public Actor() {
		this(0, "Actor", 5);
	}

	public Actor(int id, String name, int maxlv) {
		this.originId = id;
		this.name = name;
		this.state = this.AddStates();
		this.stats(1, maxlv);
	}

	public Actor(int id, String name, String race, String jname, int lv,
			int maxlv, int maxhp, int maxmp, int maxsp, int atk, int def,
			int wis, int spi, int agi, int hpp, int mpp, int spp, int atkp,
			int defp, int wisp, int spip, int agip) {
		this(id, name, race, jname, lv, maxlv, maxhp, maxmp, maxsp, atk, def,
				wis, spi, agi, hpp, mpp, spp, atkp, defp, wisp, spip, agip,
				new int[] {}, new Ability[] {}, new Ability[] {}, true);
	}

	public Actor(int id, String name, String race, String jname, int lv,
			int maxlv, int maxhp, int maxmp, int maxsp, int atk, int def,
			int wis, int spi, int agi, int hpp, int mpp, int spp, int atkp,
			int defp, int wisp, int spip, int agip, int[] newRes) {
		this(id, name, race, jname, lv, maxlv, maxhp, maxmp, maxsp, atk, def,
				wis, spi, agi, hpp, mpp, spp, atkp, defp, wisp, spip, agip,
				newRes, new Ability[] {}, new Ability[] {}, true);
	}

	public Actor(int id, String name, String race, String jname, int lv,
			int maxlv, int maxhp, int maxmp, int maxsp, int atk, int def,
			int wis, int spi, int agi, int hpp, int mpp, int spp, int atkp,
			int defp, int wisp, int spip, int agip, int[] newRes,
			Ability[] items, Ability[] newSkill) {
		this(id, name, race, jname, lv, maxlv, maxhp, maxmp, maxsp, atk, def,
				wis, spi, agi, hpp, mpp, spp, atkp, defp, wisp, spip, agip,
				newRes, items, newSkill, true);
	}

	public Actor(int id, String name, Race race, Job job, Ability[] items,
			int lv, int maxlv) {
		this(id, name, race.rname, job.jname, lv, maxlv, race.maxhp,
				race.maxmp, race.maxsp, race.matk, race.mdef, race.mwis,
				race.mspi, race.magi, job.hpp, job.mpp, job.spp, job.atkp,
				job.defp, job.wisp, job.spip, job.agip, race.rres, items,
				race.raceSkills, true);
		this.addSkills(job.raceSkills, true, true);
	}

	protected Actor(int id, String name, String race, String jname, int lv,
			int maxlv, int maxhp, int maxmp, int maxsp, int atk, int def,
			int wis, int spi, int agi, int hpp, int mpp, int spp, int atkp,
			int defp, int wisp, int spip, int agip, int[] newRes,
			Ability[] items, Ability[] newSkill, boolean cloneSkills) {
		super(id, jname, maxhp, maxmp, maxsp, atk, def, wis, spi, agi, hpp,
				mpp, spp, atkp, defp, wisp, spip, agip, newRes, null);
		this.name = name;
		this.rname = race;
		this.state = this.AddStates();
		if (items != null) {
			this.items = new ArrayList<Ability>(items.length);
			for (int i = 0; i < items.length; i++)
				this.items.add(items[i]);
		}
		if (newSkill != null)
			this.setBaseSkills(newSkill, cloneSkills);
		this.stats(lv, maxlv);
	}

	public Actor(int id, String name, String race, String jname, int lv,
			int maxlv, int maxhp, int maxmp, int maxsp, int atk, int def,
			int wis, int spi, int agi, int hpp, int mpp, int spp, int atkp,
			int defp, int wisp, int spip, int agip, int[] newRes,
			Ability[] items, Ability[] newSkill, int[] skills) {
		this(id, name, race, jname, lv, maxlv, maxhp, maxmp, maxsp, atk, def,
				wis, spi, agi, hpp, mpp, spp, atkp, defp, wisp, spip, agip,
				newRes, items, null);
		this.raceSkills = new Ability[skills.length];
		for (int i = 0; i < skills.length; i++)
			this.raceSkills[i] = new Ability(newSkill[skills[i]]);

	}

	private void setRes(int nres[], boolean race) {
		for (int i = 0; i < this.res.length; i++) {
			if (race)
				this.rres[i] = nres[i];
			else
				this.mres[i] += nres[i];
			this.checkRes(i);
		}
		if (race) {
			this.resetRes();
			this.setRes(nres, false);
		}
	}

	private void resetRes() {
		for (int i = 0; i < this.mres.length; i++)
			this.mres[i] = 3 + rres[i];
	}

	private void stats(int lv, int maxlv) {
		this.resetRes();
		this.maxlv = maxlv;
		this.maxp = 5;
		if (lv > 1) {
			this.exp = (int) (this.maxp * (Math.pow(lv - 1, 2)));
		} else
			this.exp = 0;
		this.level = 1;
		this.levelUp();
		this.recover();
		if (this.hp < 1)
			this.active = false;
	}

	public void recover() {
		this.hp = this.maxhp;
		this.mp = this.maxmp;
		this.sp = 0;
		this.active = true;
		this.atk = this.matk;
		this.def = this.mdef;
		this.spi = this.mspi;
		this.wis = this.mwis;
		this.agi = this.magi;
		for (int i = 0; i < this.state.length; i++)
			this.state[i].remove();
		this.applyState(false);
	}

	public Actor(Actor cloned) {
		this.name = cloned.name;
		this.jname = cloned.jname;
		this.rname = cloned.rname;
		this.state = cloned.state;
		this.raceStats(cloned.maxhp, cloned.maxmp, cloned.maxsp, cloned.matk,
				cloned.mdef, cloned.mwis, cloned.mspi, cloned.magi);
		this.jobStats(cloned.hpp, cloned.mpp, cloned.spp, cloned.atkp,
				cloned.defp, cloned.wisp, cloned.spip, cloned.agip);
		for (int i = 0; i < res.length; i++) {
			this.rres[i] = cloned.rres[i];
			this.mres[i] = cloned.mres[i];
			this.res[i] = cloned.res[i];
		}
		this.jobSkills.clear();
		this.raceSkills = new Ability[cloned.raceSkills.length];
		for (int i = 0; i < this.raceSkills.length; i++)
			this.raceSkills[i] = new Ability(cloned.raceSkills[i]);
		this.addSkills(cloned.jobSkills.toArray(new Ability[0]), false, true);
		if (cloned.items != null) {
			this.items = new ArrayList<Ability>(cloned.items.size());
			for (int i = 0; i < cloned.items.size(); i++)
				this.items.add(new Ability(cloned.items.get(i)));
		}
		this.setSprName(cloned.getSprName());
		this.level = cloned.level;
		this.maxlv = cloned.maxlv;
		this.resetRes();
		this.recover();
		this.originId = cloned.originId;
	}
	
	@Override
	public void setResistance(int[] newRes) {
		this.mres = new int[] { 3, 3, 3, 3, 3, 3, 3, 3 };
		for (int i = 0; i < newRes.length && i < this.mres.length; i++)
			this.mres[i] = newRes[i];
	}

	private State[] AddStates() {
		State state[] = new State[11];
		state[0] = new State(1, "Regen", false, false, false, -1, 10, 0, 0, 0,
				2, 0, 0, 0, false);
		state[1] = new State(2, "Poison", false, false, false, 10, -7, 0, -2,
				0, -2, 0, 0, 0, false);
		state[2] = new State(3, "Clarity", false, false, false, -1, 0, 7, 0, 0,
				0, 1, 1, 0, false);
		state[3] = new State(4, "Dizziness", false, false, false, 3, 0, -7, 0,
				0, 0, -1, -1, 0, false);
		state[4] = new State(5, "Vigour", false, false, false, -1, 0, 0, 7, 1,
				0, 0, 0, 1, false);
		state[5] = new State(6, "Weakness", false, false, false, 5, 0, 0, -7,
				-1, 0, 0, 0, -1, false);
		state[6] = new State(7, "Berserk", false, true, false, 7, 0, 0, 0, 5,
				-3, 0, 0, 3, false);
		state[7] = new State(8, "Confusion", false, false, true, 3, 0, 0, 0, 0,
				0, 0, 0, 0, false);
		state[8] = new State(9, "Sleep", true, false, false, 5, 0, 0, 0, 0, -3,
				0, 0, -3, false);
		state[9] = new State(10, "Stun", true, false, false, 1, 0, 0, 0, 0, -1,
				0, 0, -1, false);
		state[10] = new State(11, "Reflect", false, false, false, 7, 0, 0, 0,
				0, 0, 0, 0, 0, true);

		return state;
	}

	public void levelUp() {
		while (this.maxp <= this.exp && this.level < this.maxlv) {
			this.maxp *= 2;
			this.level++;
			this.maxhp += 3;
			this.maxmp += 2;
			this.maxsp += 2;
			this.matk++;
			this.mdef++;
			this.mwis++;
			this.mspi++;
			this.magi++;
			if (this.level % 2 == 1) {
				this.maxhp += this.hpp;
				this.maxmp += this.mpp;
				this.maxsp += this.spp;
				this.matk += this.atkp;
				this.mdef += this.defp;
				this.mwis += this.wisp;
				this.mspi += this.spip;
				this.magi += this.agip;
			}
		}
	}

	@Override
	public int describeContents() {
		return 0;
	}

	@Override
	public void writeToParcel(Parcel dest, int flags) {
		dest.writeString(this.name);
		dest.writeByte((byte) (this.active ? 1 : 0));
		dest.writeByte((byte) (this.reflect ? 1 : 0));
		dest.writeTypedArray(this.state, flags);
		dest.writeInt(this.auto);
		dest.writeString(this.rname);
		dest.writeInt(this.maxhp);
		dest.writeInt(this.maxmp);
		dest.writeInt(this.maxsp);
		dest.writeInt(this.matk);
		dest.writeInt(this.mdef);
		dest.writeInt(this.mwis);
		dest.writeInt(this.mspi);
		dest.writeInt(this.magi);
		dest.writeIntArray(this.rres);
		dest.writeTypedArray(this.raceSkills, flags);
		dest.writeString(this.jname);
		dest.writeString(this.sprName);
		dest.writeInt(this.hpp);
		dest.writeInt(this.mpp);
		dest.writeInt(this.spp);
		dest.writeInt(this.atkp);
		dest.writeInt(this.defp);
		dest.writeInt(this.wisp);
		dest.writeInt(this.spip);
		dest.writeInt(this.agip);
		dest.writeInt(this.hp);
		dest.writeInt(this.mp);
		dest.writeInt(this.sp);
		dest.writeInt(this.level);
		dest.writeInt(this.maxlv);
		dest.writeInt(this.exp);
		dest.writeInt(this.maxp);
		dest.writeInt(this.atk);
		dest.writeInt(this.def);
		dest.writeInt(this.spi);
		dest.writeInt(this.wis);
		dest.writeInt(this.agi);
		dest.writeIntArray(this.mres);
		dest.writeIntArray(this.res);
		dest.writeTypedList(this.jobSkills);
		dest.writeTypedList(this.items);
		dest.writeInt(this.originId);
	}

	public static class State implements Parcelable {
		protected boolean inactive, confusion, auto, reflect;
		protected String name;
		protected int res, mdur, dur, hpm, mpm, spm, atkm, defm, spim, wism,
				agim, originId, resm[] = { 0, 0, 0, 0, 0, 0, 0, 0 };

		protected State(Parcel in) {
			this.inactive = in.readByte() != 0;
			this.confusion = in.readByte() != 0;
			this.auto = in.readByte() != 0;
			this.reflect = in.readByte() != 0;
			this.name = in.readString();
			this.res = in.readInt();
			this.mdur = in.readInt();
			this.dur = in.readInt();
			this.hpm = in.readInt();
			this.mpm = in.readInt();
			this.spm = in.readInt();
			this.atkm = in.readInt();
			this.defm = in.readInt();
			this.spim = in.readInt();
			this.wism = in.readInt();
			this.agim = in.readInt();
			this.resm = in.createIntArray();
			this.originId = in.readInt();
		}

		public static final Creator<State> CREATOR = new Creator<State>() {
			@Override
			public State createFromParcel(Parcel in) {
				return new State(in);
			}

			@Override
			public State[] newArray(int size) {
				return new State[size];
			}
		};

		public boolean isInactive() {
			return this.inactive;
		}

		public State setInactive(boolean inactive) {
			this.inactive = inactive;
			return this;
		}

		public boolean isConfusing() {
			return this.confusion;
		}

		public State setConfusion(boolean confusion) {
			this.confusion = confusion;
			return this;
		}

		public boolean isAuto() {
			return this.auto;
		}

		public State setAuto(boolean auto) {
			this.auto = auto;
			return this;
		}

		public boolean isReflecting() {
			return this.reflect;
		}

		public void setReflection(boolean reflect) {
			this.reflect = reflect;
		}

		public String getName() {
			return this.name;
		}

		public void setName(String name) {
			this.name = name;
		}

		public int getRes() {
			return this.res;
		}

		public State setRes(int res) {
			this.res = res;
			return this;
		}

		public int getMaxDur() {
			return this.mdur;
		}

		public State setMaxDur(int mdur) {
			this.mdur = mdur;
			return this;
		}

		public int getDur() {
			return this.dur;
		}

		public State setDur(int dur) {
			this.dur = dur;
			return this;
		}

		public int getHPmod() {
			return this.hpm;
		}

		public State setHPmod(int hpm) {
			this.hpm = hpm;
			return this;
		}

		public int getMPmod() {
			return this.mpm;
		}

		public State setMPmod(int mpm) {
			this.mpm = mpm;
			return this;
		}

		public int getRPmod() {
			return this.spm;
		}

		public State setRPmod(int spm) {
			this.spm = spm;
			return this;
		}

		public int getAtkMod() {
			return this.atkm;
		}

		public State setAtkMod(int atkm) {
			this.atkm = atkm;
			return this;
		}

		public int getDefMod() {
			return this.defm;
		}

		public State setDefMod(int defm) {
			this.defm = defm;
			return this;
		}

		public int getSpiMod() {
			return this.spim;
		}

		public State setSpiMod(int spim) {
			this.spim = spim;
			return this;
		}

		public int getWisMod() {
			return this.wism;
		}

		public State setWisMod(int wism) {
			this.wism = wism;
			return this;
		}

		public int getAgiMod() {
			return this.agim;
		}

		public State setAgiMod(int agim) {
			this.agim = agim;
			return this;
		}

		public int[] getResMod() {
			return this.resm;
		}

		public int getOriginId() {
			return this.originId;
		}

		public State(int id, String name, boolean inactive, boolean auto,
				boolean confusion, int dur, int hpm, int mpm, int spm,
				int atkm, int defm, int wism, int spim, int agim,
				boolean reflect) {
			this.originId = id;
			this.name = name;
			this.mdur = dur;
			this.dur = 0;
			this.res = 0;
			this.hpm = hpm;
			this.mpm = mpm;
			this.spm = spm;
			this.atkm = atkm;
			this.defm = defm;
			this.spim = spim;
			this.wism = wism;
			this.agim = agim;
			this.inactive = inactive;
			this.confusion = confusion;
			this.auto = (confusion) ? true : auto;
			this.reflect = reflect;
		}

		public void inflict() {
			int rnd = (int) (Math.random() * 10);
			if (this.dur > -2 && rnd > res)
				this.dur = this.mdur;
		}

		public String apply(Actor actor, boolean consume) {
			String s = "";
			if (this.dur != 0 && actor.hp > 0) {
				if (consume) {
					int rnd = (int) (Math.random() * 3);
					int dmghp = ((actor.maxhp + rnd) * this.hpm) / 100;
					int dmgmp = ((actor.maxmp + rnd) * this.mpm) / 100;
					int dmgsp = ((actor.maxsp + rnd) * this.spm) / 100;
					actor.hp += dmghp;
					actor.mp += dmgmp;
					actor.sp += dmgsp;
					boolean c = false;
					if (dmghp != 0 || dmgmp != 0 || dmgsp != 0)
						s += (this.name + " causes " + actor.name);
					if (dmghp != 0) {
						s += (" ");
						if (dmghp >= 0)
							s += "+";
						s += dmghp + " HP";
						c = true;
					}
					if (dmgmp != 0) {
						if (c)
							s += ",";
						s += " ";
						if (dmgmp >= 0)
							s += "+";
						s += dmgmp + " MP";
						c = true;
					}
					if (dmgsp != 0) {
						if (c)
							s += ",";
						s += " ";
						if (dmgsp >= 0)
							s += "+";
						s += dmgsp + " RP";
					}
					if (this.dur > 0)
						this.dur--;
				}
				actor.atk = actor.matk + this.atkm;
				actor.def = actor.mdef + this.defm;
				actor.spi = actor.mspi + this.spim;
				actor.wis = actor.mwis + this.wism;
				actor.agi = actor.magi + this.agim;
				for (int i = 0; i < actor.res.length; i++) {
					actor.res[i] += this.resm[i];
					actor.checkRes(i);
				}
				if (this.inactive)
					actor.active = false;
				if (this.reflect)
					actor.reflect = true;
				if (this.auto && actor.auto < 2)
					actor.auto = 1;
				if (this.confusion)
					actor.auto = (actor.auto < 2) ? -1 : -2;
			}
			return s;
		}

		public void remove() {
			if (this.dur > -2)
				this.dur = 0;
		}

		@Override
		public int describeContents() {
			return 0;
		}

		@Override
		public void writeToParcel(Parcel dest, int flags) {
			dest.writeByte((byte) (this.inactive ? 1 : 0));
			dest.writeByte((byte) (this.confusion ? 1 : 0));
			dest.writeByte((byte) (this.auto ? 1 : 0));
			dest.writeByte((byte) (this.reflect ? 1 : 0));
			dest.writeString(this.name);
			dest.writeInt(this.res);
			dest.writeInt(this.mdur);
			dest.writeInt(this.dur);
			dest.writeInt(this.hpm);
			dest.writeInt(this.mpm);
			dest.writeInt(this.spm);
			dest.writeInt(this.atkm);
			dest.writeInt(this.defm);
			dest.writeInt(this.spim);
			dest.writeInt(this.wism);
			dest.writeInt(this.agim);
			dest.writeIntArray(this.resm);
			dest.writeInt(this.originId);
		}
	}

	public Actor addExtraSkills(Ability[] newSkill) {
		this.addSkills(newSkill, true, true);
		return this;
	}

	protected void addSkills(Ability[] newSkill, boolean noDuplicates,
			boolean clone) {
		ArrayList<Integer> origins = null;
		int i;
		if (noDuplicates) {
			origins = new ArrayList<Integer>(this.jobSkills.size()
					+ this.raceSkills.length);
			for (i = 0; i < this.jobSkills.size(); i++) {
				origins.add(this.jobSkills.get(i).originId);
			}
			for (i = 0; i < this.raceSkills.length; i++) {
				origins.add(this.raceSkills[i].originId);
			}
		}
		for (i = 0; i < newSkill.length; i++)
			if ((!noDuplicates)
					|| (!origins.contains(newSkill[i].originId) && !this.jobSkills
							.contains(newSkill[i])))
				this.jobSkills.add(clone ? new Ability(newSkill[i])
						: newSkill[i]);
	}

	public Actor removeExtraSkills() {
		this.jobSkills.clear();
		return this;
	}
}