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

public class Ability {
	protected String name;
	protected int trg, hpc, mpc, spc, lvrq, atki, hpdmg, mpdmg, spdmg, dmgtype, element, qty, mqty;
	protected boolean battle, absorb, range;
    protected boolean[] state, rstate;
    protected Ability origin = null;
    
    public Ability getOrigin() {
    	return this.origin;
    }

    public String getName() {
        return this.name;
    }

    public int getTrg() {
        return this.trg;
    }

    public void setQty(int qty) {
        if (this.mqty > 0 && qty > this.mqty)
            this.qty = this.mqty;
        else
            this.qty = qty;
    }

    public int getHpCost() {
        return this.hpc;
    }

    public int getMpCost() {
        return this.mpc;
    }

    public int getSpCost() {
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

    public int getSpDmg() {
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

    public boolean isBattleOnly() {
        return this.battle;
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

	public String execute(Actor user, Actor target) {
		String s = "";
		int dmg = (int) (Math.random() * 4);
		int res = (target.res[this.element] < 7) ? target.res[this.element] : -1;
		switch (this.dmgtype) {
		case 1:
			dmg += (this.atki * ((user.def + user.atk) / 2)) / (target.def * res + 1);
			break;
		case 3:
			dmg += (this.atki * user.wis) / (target.spi * res + 1);
			break;
		case 2:
			dmg += (this.atki * user.spi) / (target.wis * res + 1);
			break;
		case 4:
			dmg += (this.atki * ((user.agi + user.atk) / 2)) / (((target.agi + target.def) / 2) * res + 1);
			break;
		case 5:
			dmg += (this.atki * ((user.wis + user.atk) / 2)) / (((target.spi + target.def) / 2) * res + 1);
			break;
		case 6:
			dmg += (this.atki * ((user.agi + user.wis + user.atk) / 3)) / (((target.agi + target.spi) / 2) * res + 1);
			break;
		case 7:
			dmg += (this.atki * ((user.spi + user.atk + user.def) / 3)) / (((target.wis + target.def) / 2) * res + 1);
			break;
		default:
			dmg += (this.atki * user.atk) / (target.def * res + 1);
		}
		if (this.dmgtype == 2 || this.dmgtype == 3
				|| (this.dmgtype != 4 && (int) (Math.random() * 13 + user.agi / 5) > 2 + target.agi / 4)
				|| (this.dmgtype == 4 && (int) (Math.random() * 13 + user.agi / 3) > 2 + target.agi / 4)) {
			int dmghp = (this.hpdmg != 0) ? (dmg + this.hpdmg) : 0;
			int dmgmp = (this.mpdmg != 0) ? (dmg + this.mpdmg) : 0;
			int dmgsp = (this.spdmg != 0) ? (dmg + this.spdmg) : 0;
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
		} else
			s += ", but misses";

		s += target.applyState(false);
		s += user.checkStatus();
		return s;
	}

    public void replenish() {
        this.qty = this.mqty;
    }

	public int getLvRq() {
		return Math.abs(this.lvrq);
	}

	public Ability() {
		this("Ability", true, false, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, false, new boolean[] {}, new boolean[] {});
	}

	public Ability(String name, boolean battle, int hpdmg, int mpdmg, int spdmg, int trg, int element, boolean state[],
			boolean rstate[]) {
		this(name, battle, true, 0, 0, 0, 0, 0, 0, hpdmg, mpdmg, spdmg, trg, element, false, state, rstate);
	}

	public Ability(String name, boolean battle, boolean range, int lvrq, int hpc, int mpc, int spc, int dmgtype,
				   int atkp, int hpdmg, int mpdmg, int spdmg, int trg, int element, boolean absorb, boolean state[],
				   boolean rstate[]) {
		this(name, battle, range, lvrq, hpc, mpc, spc, dmgtype, atkp, hpdmg, mpdmg, spdmg, trg, element, -1, absorb, state, rstate);
	}

	public Ability(String name, boolean battle, boolean range, int lvrq, int hpc, int mpc, int spc, int dmgtype,
			int atkp, int hpdmg, int mpdmg, int spdmg, int trg, int element,  int mqty, boolean absorb, boolean state[],
			boolean rstate[]) {
		this.name = name;
		this.battle = battle;
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
		this.qty = 0;
		this.mqty = mqty;
	}
	
	public Ability(Ability cloned) {
		this(cloned.name, cloned.battle, cloned.range, cloned.lvrq, cloned.hpc, cloned.mpc, cloned.spc, cloned.dmgtype,
				cloned.atki, cloned.hpdmg, cloned.mpdmg, cloned.spdmg, cloned.trg, cloned.element, cloned.mqty, cloned.absorb,
				cloned.state, cloned.rstate);
		this.origin = cloned;
	}
}