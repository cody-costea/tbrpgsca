/*
Copyright (C) AD 2017 Claudiu-Stefan Costea

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

import java.util.ArrayList;

import android.content.Context;
import android.graphics.drawable.AnimationDrawable;
import android.media.MediaPlayer;
import android.os.Parcel;
import android.os.Parcelable;

public class Ability implements Parcelable {
	protected String name;
	protected int trg, hpc, mpc, spc, lvrq, atki, hpdmg, mpdmg, spdmg, dmgtype,
			element, qty, mqty, rqty, tqty, rstate[], soundId, animSprId;
	protected final int originId;
	protected boolean steal, absorb, range, restore;
	protected Actor.State[] state;
	protected MediaPlayer media;
	protected Context mediaContext;
	protected AnimationDrawable animDrw;

	protected Ability(Parcel in) {
		this.name = in.readString();
		this.trg = in.readInt();
		this.hpc = in.readInt();
		this.mpc = in.readInt();
		this.spc = in.readInt();
		this.lvrq = in.readInt();
		this.atki = in.readInt();
		this.hpdmg = in.readInt();
		this.mpdmg = in.readInt();
		this.spdmg = in.readInt();
		this.dmgtype = in.readInt();
		this.element = in.readInt();
		this.qty = in.readInt();
		this.mqty = in.readInt();
		this.rqty = in.readInt();
		this.tqty = in.readInt();
		this.steal = in.readByte() != 0;
		this.absorb = in.readByte() != 0;
		this.range = in.readByte() != 0;
		this.restore = in.readByte() != 0;
		this.state = in.createTypedArray(Actor.State.CREATOR);
		this.rstate = in.createIntArray();
		this.originId = in.readInt();
		this.soundId = in.readInt();
		this.animSprId = in.readInt();
	}

	public static final Creator<Ability> CREATOR = new Creator<Ability>() {
		@Override
		public Ability createFromParcel(Parcel in) {
			return new Ability(in);
		}

		@Override
		public Ability[] newArray(int size) {
			return new Ability[size];
		}
	};

	public String getName() {
		return this.name;
	}

	public int getTrg() {
		return this.trg;
	}

	public Ability setQty(int qty) {
		if (this.mqty > 0 && qty > this.mqty)
			this.qty = this.mqty;
		else
			this.qty = qty;
		return this;
	}

	public int getHpCost() {
		return this.hpc;
	}

	public int getMpCost() {
		return this.mpc;
	}

	public int getRpCost() {
		return this.spc;
	}

	public int getAtkMod() {
		return this.atki;
	}

	public int getHpDmg() {
		return this.hpdmg;
	}

	public int getMpDmg() {
		return this.mpdmg;
	}

	public int geRpDmg() {
		return this.spdmg;
	}

	public int getDmgType() {
		return this.dmgtype;
	}

	public int getElement() {
		return this.element;
	}

	public int getQty() {
		return this.qty;
	}

	public int getMaxQty() {
		return this.mqty;
	}

	public boolean isStealing() {
		return this.steal;
	}

	public Actor.State[] getStates() {
		return this.state;
	}

	public int[] getRemovedStatesOriginIds() {
		return this.rstate;
	}

	public boolean isAbsorbing() {
		return this.absorb;
	}

	public boolean isRanged() {
		return this.range;
	}

	public int getOriginId() {
		return this.originId;
	}

	public int getSoundId() {
		return this.soundId;
	}

    public int getAnimationDrawableId() {
        return this.animSprId;
    }

    public AnimationDrawable getAnimationDrawable(Context act) {
	    if (this.animDrw == null && this.animSprId > 0)
	        this.animDrw = (AnimationDrawable)act.getResources().getDrawable(this.animSprId);
	    return this.animDrw;
    }

	public Ability setSoundId(int soundId) {
		this.soundId = soundId;
		return this.cleanSound();
	}

	public Ability playSound(Context c) {
		if (this.soundId != 0) {
			if (this.mediaContext != null && this.mediaContext != c)
				this.cleanSound();
			if (this.media == null) {
				this.mediaContext = c;
				this.media = MediaPlayer.create(c, this.soundId);
			}
			this.media.start();
		}
		return this;
	}

	public Ability cleanSound() {
		if (this.media != null) {
			this.media.release();
			this.media = null;
		}
		return this;
	}

	public String execute(Actor user, Actor target, boolean applyCosts) {
		String s = "";
		int dmg = (int) (Math.random() * 4);
		int res = (target.res[this.element] < 7) ? target.res[this.element]
				: -1;
		switch (this.dmgtype) {
		case 1:
			dmg += (this.atki + ((user.def + user.atk) / 2))
					/ (target.def * res + 1);
			break;
		case 2:
			dmg += (this.atki + user.wis) / (target.spi * res + 1);
			break;
		case 3:
			dmg += (this.atki + user.spi) / (target.wis * res + 1);
			break;
		case 4:
			dmg += (this.atki + ((user.agi + user.atk) / 2))
					/ (((target.agi + target.def) / 2) * res + 1);
			break;
		case 5:
			dmg += (this.atki + ((user.wis + user.atk) / 2))
					/ (((target.spi + target.def) / 2) * res + 1);
			break;
		case 6:
			dmg += (this.atki + ((user.agi + user.wis + user.atk) / 3))
					/ (((target.agi + target.spi) / 2) * res + 1);
			break;
		case 7:
			dmg += (this.atki + ((user.spi + user.atk + user.def) / 3))
					/ (((target.wis + target.def) / 2) * res + 1);
			break;
		default:
			dmg += (this.atki + user.atk) / (target.def * res + 1);
		}
		if (this.dmgtype == 2
				|| this.dmgtype == 3
				|| (this.qty > 0 && this.mqty < 1)
				|| (this.dmgtype != 4 && (int) (Math.random() * 13 + user.agi / 5) > 2 + target.agi / 4)
				|| (this.dmgtype == 4 && (int) (Math.random() * 13 + user.agi / 3) > 2 + target.agi / 4)) {
			int dmghp = (this.hpdmg != 0) ? (((this.hpdmg < 0 ? -1 : 1) * dmg) + this.hpdmg)
					: 0;
			int dmgmp = (this.mpdmg != 0) ? (((this.mpdmg < 0 ? -1 : 1) * dmg) + this.mpdmg)
					: 0;
			int dmgsp = (this.spdmg != 0) ? (((this.spdmg < 0 ? -1 : 1) * dmg) + this.spdmg)
					: 0;
			if (res < 0) {
				dmghp = -dmghp;
				dmgmp = -dmgmp;
				dmgsp = -dmgsp;
			}
			target.hp -= dmghp;
			target.mp -= dmgmp;
			target.sp -= dmgsp;
			if (this.absorb) {
				user.hp += dmghp / 2;
				user.mp += dmgmp / 2;
				user.sp += dmgsp / 2;
			}
			if (applyCosts) {
				user.hp -= this.hpc;
				user.mp -= this.mpc;
				user.sp -= this.spc;
				if (this.qty > 0)
					this.qty--;
			}
			boolean c = false;
			if (dmghp != 0 || dmgmp != 0 || dmgsp != 0)
				s += (", " + target.name + " suffers");
			if (dmghp != 0) {
				s += (" ");
				if (dmghp < 1)
					s += "+";
				s += -dmghp + " HP";
				c = true;
			}
			if (dmgmp != 0) {
				if (c)
					s += ",";
				s += " ";
				if (dmgmp < 1)
					s += "+";
				s += -dmgmp + " MP";
				c = true;
			}
			if (dmgsp != 0) {
				if (c)
					s += ",";
				s += " ";
				if (dmgsp < 1)
					s += "+";
				s += -dmgsp + " RP";
			}
			if (this.state != null)
				for (Actor.State state : this.state)
						state.inflict(target, false);
			if (target.currentState != null && this.rstate != null)
				for (int i = 0; i < this.rstate.length; i++) {
					for (Actor.State aState : target.currentState)
						if (aState.originId == this.rstate[i]) {
							aState.remove(target, false, false);
							break;
						}
			}
			if (this.steal
					&& target.items != null
					&& target.items != user.items
					&& target.items.size() > 0
					&& ((int) (Math.random() * 12 + user.agi / 4) > 4 + target.agi / 3)) {
				int itemId = (int) (Math.random() * target.items.size());
				if (itemId < target.items.size()
						&& target.items.get(itemId).qty > 0) {
					boolean found = false;
					if (user.items != null) {
						for (int i = 0; i < user.items.size(); i++)
							if (user.items.get(i).originId == target.items
									.get(itemId).originId) {
								user.items.get(i).qty++;
								found = true;
								break;
							}
					}
					else
						user.items = new ArrayList<Ability>();
					if (!found) {
						user.items.add(new Ability(target.items.get(itemId)));
						user.items.get(user.items.size() - 1).qty = 1;
					}
					target.items.get(itemId).qty--;
					s += ", " + target.items.get(itemId).name + " stolen";
					if (target.items.get(itemId).qty == 0)
						target.items.remove(itemId);
				}
			}
		} else
			s += ", but misses";

		s += target.applyState(false);
		s += user.checkStatus();
		return s;
	}

	public void replenish() {
		if (this.mqty > 0)
			this.qty = this.mqty;
	}

	public int getLvRq() {
		return Math.abs(this.lvrq);
	}

	public Ability() {
		this(0, "Ability", 0, 0, true, false, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, false,
				new Actor.State[] {}, new Actor.State[] {});
	}

	public Ability(int id, String name, int animDrwId, int soundId, boolean range, int hpdmg, int mpdmg,
				   int spdmg, int trg, int element, boolean restoreKO, Actor.State state[], Actor.State rstate[]) {
		this(id, name, animDrwId, soundId, false, range, 0, 0, 0, 0, 0, 0, hpdmg, mpdmg, spdmg, trg,
				element, 0, -1, false, restoreKO, state, rstate);
	}

	public Ability(int id, String name, int animDrwId, int soundId, boolean steal, boolean range,
				   int lvrq, int hpc, int mpc, int spc, int dmgtype, int atkp, int hpdmg,
				   int mpdmg, int spdmg, int trg, int element, boolean absorb,
				   Actor.State state[], Actor.State rstate[]) {
		this(id, name, animDrwId, soundId, steal, range, lvrq, hpc, mpc, spc, dmgtype, atkp, hpdmg,
				mpdmg, spdmg, trg, element, -1, -1, absorb, false, state, rstate);
	}

	public void checkQty() {
		if (this.qty < this.mqty && this.rqty > -1) {
			if (this.tqty > this.rqty) {
				this.tqty = 0;
				this.qty++;
			} else
				this.tqty++;
		}
	}

	public Ability(int id, String name, int animDrwId, int soundId, boolean steal, boolean range,
				   int lvrq, int hpc, int mpc, int spc, int dmgtype, int atkp, int hpdmg,
				   int mpdmg, int spdmg, int trg, int element, int mqty, int rqty,
				   boolean absorb, boolean restoreKO, Actor.State state[], Actor.State rstate[]) {
		this.originId = id;
		this.name = name;
		this.animSprId = animDrwId;
		this.soundId = soundId;
		this.steal = steal;
		this.lvrq = lvrq;
		this.hpc = hpc;
		this.mpc = mpc;
		this.spc = spc;
		this.atki = atkp;
		this.hpdmg = hpdmg;
		this.mpdmg = mpdmg;
		this.spdmg = spdmg;
		this.absorb = absorb;
		this.dmgtype = dmgtype;
		this.trg = trg;
		this.state = state;
		if (rstate != null) {
			this.rstate = new int[rstate.length];
			for (int i = 0; i < rstate.length; i++)
				this.rstate[i] = rstate[i].originId;
		}
		this.range = range;
		this.element = element;
		this.qty = mqty > 0 ? mqty : 0;
		this.mqty = mqty;
		this.rqty = rqty;
		this.tqty = 0;
		this.restore = restoreKO;
	}

	public Ability(Ability cloned) {
		this(cloned.originId, cloned.name, cloned.animSprId, cloned.soundId, cloned.steal,
                cloned.range,  cloned.lvrq, cloned.hpc, cloned.mpc, cloned.spc, cloned.dmgtype,
				cloned.atki, cloned.hpdmg, cloned.mpdmg, cloned.spdmg, cloned.trg, cloned.element,
                cloned.mqty, cloned.rqty, cloned.absorb, cloned.restore, cloned.state, null);
		this.qty = cloned.qty;
		this.rstate = cloned.rstate;
	}

	@Override
	public int describeContents() {
		return 0;
	}

	@Override
	public void writeToParcel(Parcel dest, int flags) {
		dest.writeString(this.name);
		dest.writeInt(this.trg);
		dest.writeInt(this.hpc);
		dest.writeInt(this.mpc);
		dest.writeInt(this.spc);
		dest.writeInt(this.lvrq);
		dest.writeInt(this.atki);
		dest.writeInt(this.hpdmg);
		dest.writeInt(this.mpdmg);
		dest.writeInt(this.spdmg);
		dest.writeInt(this.dmgtype);
		dest.writeInt(this.element);
		dest.writeInt(this.qty);
		dest.writeInt(this.mqty);
		dest.writeInt(this.rqty);
		dest.writeInt(this.tqty);
		dest.writeByte((byte) (this.steal ? 1 : 0));
		dest.writeByte((byte) (this.absorb ? 1 : 0));
		dest.writeByte((byte) (this.range ? 1 : 0));
		dest.writeByte((byte) (this.restore ? 1 : 0));
		dest.writeTypedArray(this.state, flags);
		dest.writeIntArray(this.rstate);
		dest.writeInt(this.originId);
		dest.writeInt(this.soundId);
		dest.writeInt(this.animSprId);
	}

	@Override
	public boolean equals(Object eq) {
		return eq != null && (eq instanceof Ability)
				&& this.originId == ((Ability)eq).originId;
	}
}