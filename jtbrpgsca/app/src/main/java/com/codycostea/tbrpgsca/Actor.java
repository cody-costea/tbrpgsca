/*
Copyright (C) AD 2013-2019 Claudiu-Stefan Costea

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
package com.codycostea.tbrpgsca;

import android.content.Context;
import android.content.res.Resources;
import android.graphics.drawable.AnimationDrawable;
import android.graphics.drawable.Drawable;
import android.os.Parcel;

import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;

public final class Actor extends Costume {

	protected final static int FLAG_ACTIVE = 2;
	protected final static int FLAG_REFLECTS = 4;
	protected final static int FLAG_GUARDS = 8;

	public final static int AUTO_NONE = 0;
	public final static int AUTO_CONFUSED = -1;
	public final static int AUTO_ENRAGED = 1;
	public final static int AUTO_ENEMY = -2;
	public final static int AUTO_ALLY = 2;

	protected Costume _race, _job;
	protected int _lv = 1, maxLv, _hp, _mp, _sp, _xp, _maxp, automatic, init;
	protected Hashtable _items, skillsQty, skillsQtyRgTurn, stateDur, equipment;
	protected Boolean _ranged;

	protected final Vector<Performance> _skills = new Vector();

	int[][] spritesDur = {
			{0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0}
	};
	private AnimationDrawable[][] sprites;

	protected Actor(final Parcel in) {
		super(in);
		this._race = in.readParcelable(Costume.class.getClassLoader());
		this._job = in.readParcelable(Costume.class.getClassLoader());
		this._lv = in.readInt();
		this.maxLv = in.readInt();
		this._hp = in.readInt();
		this._mp = in.readInt();
		this._sp = in.readInt();
		this._xp = in.readInt();
		this._maxp = in.readInt();
		this.automatic = in.readInt();
		this.init = in.readInt();
		byte tmp_ranged = in.readByte();
		this._ranged = tmp_ranged == 0 ? null : tmp_ranged == 1;
	}

	@Override
	public void writeToParcel(final Parcel dest, final int flags) {
		super.writeToParcel(dest, flags);
		dest.writeParcelable(this._race, flags);
		dest.writeParcelable(this._job, flags);
		dest.writeInt(this._lv);
		dest.writeInt(this.maxLv);
		dest.writeInt(this._hp);
		dest.writeInt(this._mp);
		dest.writeInt(this._sp);
		dest.writeInt(this._xp);
		dest.writeInt(this._maxp);
		dest.writeInt(this.automatic);
		dest.writeInt(this.init);
		dest.writeByte((byte) (this._ranged == null ? 0 : this._ranged ? 1 : 2));
	}

	@Override
	public int describeContents() {
		return 0;
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

	public Vector<Performance> getAvailableSkills() {
		return this._skills;
	}

	public Costume getRace() {
		return this._race;
	}

	public Actor setRace(final Costume race) {
		this.switchCostume(this._race, race);
		this._race = race;
		return this;
	}

	public Costume getJob() {
		return this._job;
	}

	public Actor setJob(final Costume job) {
		this.switchCostume(this._job, job);
		this._job = job;
		return this;
	}

	public int getCurrentLevel() {
		return this._lv;
	}

	public Actor setCurrentLevel(final int lv) {
		final int nLv = lv > this.maxLv ? this.maxLv : (lv < 1 ? 1 : lv);
		while (this._lv < nLv) {
			this._xp = this._maxp;
			this.levelUp();
		}
		return this;
	}

	public int getMaxLevel() {
		return this.maxLv;
	}

	public Actor setMaxLevel(final int lv) {
		this.maxLv = lv;
		return this;
	}

	public int getExperience() {
		return this._xp;
	}

	public Actor setExperience(final int xp) {
		this._xp = xp;
		this.levelUp();
		return this;
	}

	public int getExpLimit() {
		return this._maxp;
	}

	public Actor setExpLimit(final int xp) {
		//TODO: levels might be changed;
		this._maxp = xp;
		return this;
	}

	public int getCurrentHp() {
		return this._hp;
	}

	public int getCurrentMp() {
		return this._mp;
	}

	public int getCurrentSp() {
		return this._sp;
	}

	public Actor setCurrentHp(final int hp) {
		this._hp = hp > this.mHp ? this.mHp : (hp < 0 ? 0 : hp);
		return this;
	}

	public Actor setCurrentMp(final int mp) {
		this._mp = mp > this.mMp ? this.mMp : (mp < 0 ? 0 : mp);
		return this;
	}

	public Actor setCurrentSp(final int sp) {
		this._sp = sp > this.mSp ? this.mSp : (sp < 0 ? 0 : sp);
		return this;
	}

	public Boolean isRanged() {
		return this._ranged;
	}

	public boolean hasRange() {
		Boolean ranged = this._ranged;
		if (ranged == null) {
			final Hashtable equipment = this.equipment;
			final Hashtable states = this.stateDur;
			boolean r = super.hasRange() || this._job.hasRange() || this._race.hasRange();
			if (!r && equipment != null) {
				final Enumeration vEquip = equipment.elements();
				while (vEquip.hasMoreElements()) {
					Costume c = (Costume)vEquip.nextElement();
					if (c.hasRange()) {
						r = true;
						break;
					}
				}
			}
			if (!r && states != null) {
				final Enumeration sMasks = states.keys();
				while (sMasks.hasMoreElements()) {
					StateMask s = (StateMask)sMasks.nextElement();
					if (s.hasRange()) {
						r = true;
						break;
					}
				}
			}
			ranged = new Boolean(r);
			this._ranged = ranged;
		}
		return ranged != null && ranged.booleanValue();
	}

	public RolePlay setRange(final boolean range) {
		super.setRange(range);
		this._ranged = range ? new Boolean(true) : null;
		return this;
	}

	public Hashtable getItems() {
		Hashtable items = this._items;
		if (items == null) {
			items = new Hashtable();
			this._items = items;
		}
		return items;
	}

	public Actor setItems(final Hashtable items) {
		this._items = items;
		return this;
	}

	public boolean isActive() {
		return (this.flags & FLAG_ACTIVE) == FLAG_ACTIVE;
	}

	public Actor setActive(final boolean value) {
		if (value) {
			this.flags |= FLAG_ACTIVE;
		} else {
			int flags = this.flags;
			if ((flags & FLAG_ACTIVE) == FLAG_ACTIVE) {
				flags -= FLAG_ACTIVE;
				this.flags = flags;
			}
		}
		return this;
	}

	public boolean isReflecting() {
		return (this.flags & FLAG_REFLECTS) == FLAG_REFLECTS;
	}

	public Actor setReflecting(final boolean value) {
		if (value) {
			this.flags |= FLAG_REFLECTS;
		} else {
			int flags = this.flags;
			if ((flags & FLAG_REFLECTS) == FLAG_REFLECTS) {
				flags -= FLAG_REFLECTS;
				this.flags = flags;
			}
		}
		return this;
	}

	public boolean isGuarding() {
		return (this.flags & FLAG_GUARDS) == FLAG_GUARDS;
	}

	public Actor setGuarding(final boolean value) {
		if (value) {
			this.flags |= FLAG_GUARDS;
		} else {
			int flags = this.flags;
			if ((flags & FLAG_GUARDS) == FLAG_GUARDS) {
				flags -= FLAG_GUARDS;
				this.flags = flags;
			}
		}
		return this;
	}

	public int getAutoMode() {
		return this.automatic;
	}

	public Actor setAudoMode(final int auto) {
		this.automatic = auto;
		return this;
	}

	public int getInitiative() {
		return this.init;
	}

	public Actor setInitiative(final int init) {
		this.init = init;
		return this;
	}

	public void levelUp() {
		while (this._maxp <= this._xp && this._lv < this.maxLv) {
			this._maxp *= 2;
			this._lv++;
			this.mHp += 3;
			this.mMp += 2;
			this.mSp += 2;
			this.atk++;
			this.def++;
			this.wis++;
			this.spi++;
			this.agi++;
		}
	}

	void checkRegSkill(final Performance skill) {
		if (skill.rQty > 0) {
			Hashtable regSkills = this.skillsQtyRgTurn;
			if (regSkills == null) {
				regSkills = new Hashtable();
				this.skillsQtyRgTurn = regSkills;
			}
			regSkills.put(skill, new Integer(0));
		}
	}

	void switchCostume(final Costume oldRole, final Costume newRole) {
		if (oldRole != null) {
			this.updateSkills(true, oldRole.aSkills);
			this.updateAttributes(true, oldRole);
			this.updateResistance(true, oldRole.res, oldRole.stRes);
			this.updateStates(true, oldRole.aStates);
		}
		if (newRole != null) {
			this.updateStates(false, newRole.aStates);
			this.updateResistance(false, newRole.res, newRole.stRes);
			this.updateAttributes(false, newRole);
			this.updateSkills(false, newRole.aSkills);
		}
	}

	void updateStates(final boolean remove, final StateMask[] states) {
		if (states == null) {
			return;
		}
		final Hashtable aStates = this.stateDur;
		if (remove) {
			if (aStates == null) {
				return;
			}
			for (int i = 0; i < states.length; i++) {
				states[i].remove(this, true, true);
			}
		} else {
			for (int i = 0; i < states.length; i++) {
				states[i].inflict(this, true, true);
			}
		}
	}

	void updateAttributes(final boolean remove, final Costume role) {
		int i;
		if (remove) {
			i = -1;
			if (role.hasRange()) {
				this._ranged = null;
			}
		} else {
			if (role.hasRange()) {
				this._ranged = new Boolean(true);
			}
			i = 1;
		}
		this.mHp += i * role.mHp;
		this.mMp += i * role.mMp;
		this.mSp += i * role.mSp;
		this.atk += i * role.atk;
		this.def += i * role.def;
		this.mSp += i * role.spi;
		this.wis += i * role.wis;
		this.agi += i * role.agi;
	}

	void updateSkills(final boolean remove, final Performance[] abilities) {
		if (abilities == null) {
			return;
		}
		final Vector skills = this._skills;
		if (remove) {
			for (int i = 0; i < abilities.length; i++) {
				Performance k = abilities[i];
				skills.removeElement(k);
				if (k.rQty > 0) {
					Hashtable regSkills = this.skillsQtyRgTurn;
					if (regSkills != null) {
						regSkills.remove(k);
					}
				}
			}
		} else {
			for (int i = 0; i < abilities.length; i++) {
				Performance k = abilities[i];
				skills.addElement(k);
				if (k.mQty > 0) {
					Hashtable skillsQty = this.skillsQty;
					if (skillsQty == null) {
						skillsQty = new Hashtable();
						this.skillsQty = skillsQty;
					}
					skillsQty.put(k, new Integer(k.mQty));
					this.checkRegSkill(k);
				}
			}
		}
	}

	void updateResistance(final boolean remove, final Hashtable resMap, final Hashtable stResMap) {
		Object x, y = null;
		if (resMap != null) {
			Hashtable r = this.res;
			if (r == null) {
				if (remove) {
					return;
				} else {
					r = new Hashtable();
					this.res = r;
				}
			}
			final Enumeration rsIdx = resMap.keys();
			while (rsIdx.hasMoreElements()) {
				Integer i = ((Integer)rsIdx.nextElement());
				Enumeration rIdx = r.keys();
				while (rIdx.hasMoreElements()) {
					Integer k = ((Integer)rIdx.nextElement());
					if (!remove || i.intValue() == k.intValue()) {
						r.put(k, new Integer(((x = r.get(k)) == null ? 3 : ((Integer)x).intValue())
								+ (((remove) ? -1 : 1) * ((y = resMap.get(i)) == null
								? 0 : ((Integer)y).intValue()))));
					}
				}
			}
		}
		if (stResMap != null) {
			Hashtable rs = this.stRes;
			if (rs == null) {
				if (remove) {
					return;
				} else {
					rs = new Hashtable();
					this.stRes = rs;
				}
			}
			final Enumeration rsIdx = stResMap.keys();
			while (rsIdx.hasMoreElements()) {
				Integer i = ((Integer)rsIdx.nextElement());
				Enumeration rIdx = rs.keys();
				while (rIdx.hasMoreElements()) {
					Integer k = ((Integer)rIdx.nextElement());
					if (!remove || i == k) {
						rs.put(k, new Integer(((x = rs.get(k)) == null ? 3 : ((Integer)x).intValue())
								+ (((remove) ? -1 : 1) * ((y = resMap.get(i)) == null
								? 0 : ((Integer)y).intValue()))));
					}
				}
			}
		}
	}

	public String checkStatus() {
		String s = "";
		if (this._hp < 1) {
			s += ", " + this.name + " falls unconscious";
			this.setActive(false);
			this.setGuarding(false);
			this._sp = 0;
			final Hashtable sDur = this.stateDur;
			if (sDur != null) {
				final Enumeration sMasks = sDur.keys();
				while (sMasks.hasMoreElements()) {
					((StateMask)sMasks.nextElement()).remove(this, false, false);
				}
			}
		}
		return s;
	}

	public String applyStates(final boolean consume) {
		String s = "";
		if (!consume) {
			if (this.automatic < AUTO_ALLY && this.automatic > AUTO_ENEMY) {
				this.automatic = AUTO_NONE;
			}
			else {
				this.automatic = AUTO_ALLY;
			}
			if (this._hp > 0) {
				this.setGuarding(true);
			}
			this.setReflecting(false);
		}
		boolean c = false;
		final Hashtable sDur = this.stateDur;
		if (sDur != null) {
			final Enumeration sMasks = sDur.keys();
			while (sMasks.hasMoreElements()) {
				final StateMask state = (StateMask)sMasks.nextElement();
				if (((Integer)sDur.get(state)).intValue() > -3 && this._hp > 0) {
					final String r = state.apply(this, consume);
					if (r.length() > 0) {
						if (c) s += ", ";
						if (consume && !c) {
							c = true;
						}
						s += r;
					}
				}
			}
		}
		s += this.checkStatus();
		if (c && consume) s += ".";
		return s;
	}

	public void recover() {
		this._hp = this.mHp;
		this._mp = this.mMp;
		this._sp = 0;
		this.setActive(true);
		final Hashtable sDur = this.stateDur;
		if (sDur != null) {
			final Enumeration sMasks = sDur.keys();
			while (sMasks.hasMoreElements()) {
				final StateMask state = (StateMask)sMasks.nextElement();
				state.remove(this, true, false);
			}
			if (sDur.size() == 0) {
				this.stateDur = null;
			}
		}
		this.applyStates(false);
		final Hashtable res = this.res;
		if (res != null) {
			final Enumeration rsIdx = res.keys();
			while (rsIdx.hasMoreElements()) {
				Integer r = ((Integer)rsIdx.nextElement());
				if (((Integer)res.get(r)).intValue() == 3) {
					res.remove(r);
				}
			}
			if (res.size() == 0) {
				this.res = null;
			}
		}
		final Hashtable stRes = this.stRes;
		if (stRes != null) {
			final Enumeration rsIdx = stRes.keys();
			while (rsIdx.hasMoreElements()) {
				StateMask r = ((StateMask)rsIdx.nextElement());
				if (((Integer)res.get(r)).intValue() == 0) {
					res.remove(r);
				}
			}
			if (res.size() == 0) {
				this.res = null;
			}
		}
		final Hashtable skillQty = this.skillsQty;
		if (skillQty != null) {
			final Enumeration skIdx = skillQty.keys();
			while (skIdx.hasMoreElements()) {
				Performance ability = ((Performance)skIdx.nextElement());
				skillQty.put(ability, new Integer(ability.mQty));
			}
		}
	}

	public Costume unequipPos(final Object pos) {
		final Hashtable equipment = this.equipment;
		if (equipment == null) {
			return null;
		} else {
			final Object r = equipment.get(pos);
			final Costume c = r == null ? null : ((Costume)r);
			this.switchCostume(c, null);
			return c;
		}
	}

	public Object unequipItem(final Costume item) {
		final Hashtable e = this.equipment;
		if (e != null) {
			final Enumeration vEquip = equipment.keys();
			while (vEquip.hasMoreElements()) {
				Object k = vEquip.nextElement();
				if (e.get(k).equals(item)) {
					this.unequipPos(k);
					return k;
				}
			}
		}
		return null;
	}

	public Costume equipItem(final Object pos, final Costume item) {
		final Costume r = this.unequipPos(pos);
		Hashtable e = this.equipment;
		if (e == null) {
			e = new Hashtable();
			this.equipment = e;
		}
		e.put(pos, item);
		this.switchCostume(null, item);
		return r;
	}

	public Actor(final int id, final String name, final Costume race, final Costume job, final int level, final int maxLv,
				 final int mInit, final int mHp, final int mMp, final int mSp, final int atk, final int def, final int spi,
				 final int wis, final int agi, final boolean range, final Hashtable res, final Performance[] skills,
				 final StateMask[] states, final Hashtable stRes, final Hashtable items) {
		super(id, name, null, mHp, mMp, mSp, atk, def, spi, wis, agi, mInit, range, res, skills, states, stRes);
		this._xp = 0;
		this._maxp = 15;
		this.flags |= FLAG_ACTIVE | FLAG_GUARDS;
		this.automatic = AUTO_NONE;
		this.mInit = mInit;
		this.mHp = mHp;
		this.mMp = mMp;
		this.mSp = mSp;
		this.maxLv = maxLv;
		this.setRace(race);
		this.setJob(job);
		this.setCurrentLevel(level);
		this._hp = this.mHp;
		this._mp = this.mMp;
		this._sp = this.mSp;
		this._items = items;
	}

}