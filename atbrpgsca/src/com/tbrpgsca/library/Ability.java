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

import java.util.ArrayList;

import android.os.Parcel;
import android.os.Parcelable;

public class Ability implements Parcelable {
	protected String name;
	protected int trg, hpc, mpc, spc, lvrq, atki, hpdmg, mpdmg, spdmg, dmgtype,
			element, qty, mqty, rqty, tqty, originId;
	protected boolean steal, absorb, range, state[], rstate[];

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
		this.state = in.createBooleanArray();
		this.rstate = in.createBooleanArray();
		this.originId = in.readInt();
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

	public boolean[] getState() {
		return this.state;
	}

	public boolean[] getRstate() {
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
			for (int i = 1; i < this.state.length; i++)
				if (this.state[i])
					target.state[i - 1].inflict();
			for (int i = 1; i < this.rstate.length; i++)
				if (this.rstate[i]) {
					target.state[i - 1].remove();
				}
			if (this.steal
					&& target.items != null
					&& target.items.size() > 0
					&& target.items != user.items
					&& ((int) (Math.random() * 12 + user.agi / 4) > 4 + target.agi / 3)) {
				int itemId = (int) (Math.random() * target.items.size());
				if (itemId < target.items.size()
						&& target.items.get(itemId).qty > 0) {
					boolean found = false;
					if (user.items != null)
						for (int i = 0; i < user.items.size(); i++)
							if (user.items.get(i).originId == target.items
									.get(itemId).originId) {
								user.items.get(i).qty++;
								found = true;
								break;
							} else
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
		this(0, "Ability", true, false, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, false,
				new boolean[] {}, new boolean[] {});
	}

	public Ability(int id, String name, boolean steal, int hpdmg, int mpdmg,
			int spdmg, int trg, int element, boolean state[], boolean rstate[]) {
		this(id, name, steal, true, 0, 0, 0, 0, 0, 0, hpdmg, mpdmg, spdmg, trg,
				element, 0, -1, false, state, rstate);
	}

	public Ability(int id, String name, boolean steal, boolean range, int lvrq,
			int hpc, int mpc, int spc, int dmgtype, int atkp, int hpdmg,
			int mpdmg, int spdmg, int trg, int element, boolean absorb,
			boolean state[], boolean rstate[]) {
		this(id, name, steal, range, lvrq, hpc, mpc, spc, dmgtype, atkp, hpdmg,
				mpdmg, spdmg, trg, element, -1, -1, absorb, state, rstate);
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

	public Ability(int id, String name, boolean steal, boolean range, int lvrq,
			int hpc, int mpc, int spc, int dmgtype, int atkp, int hpdmg,
			int mpdmg, int spdmg, int trg, int element, int mqty, int rqty,
			boolean absorb, boolean state[], boolean rstate[]) {
		this.originId = id;
		this.name = name;
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
		this.rstate = rstate;
		this.range = range;
		this.element = element;
		this.qty = mqty > 0 ? mqty : 0;
		this.mqty = mqty;
		this.rqty = rqty;
		this.tqty = 0;
	}

	public Ability(Ability cloned) {
		this(cloned.originId, cloned.name, cloned.steal, cloned.range,
				cloned.lvrq, cloned.hpc, cloned.mpc, cloned.spc,
				cloned.dmgtype, cloned.atki, cloned.hpdmg, cloned.mpdmg,
				cloned.spdmg, cloned.trg, cloned.element, cloned.mqty,
				cloned.rqty, cloned.absorb, cloned.state, cloned.rstate);
		this.qty = cloned.qty;
		this.originId = cloned.originId;
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
		dest.writeBooleanArray(this.state);
		dest.writeBooleanArray(this.rstate);
		dest.writeInt(this.originId);
	}
}