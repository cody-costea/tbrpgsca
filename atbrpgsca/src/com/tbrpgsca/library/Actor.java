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

public class Actor extends Job {
	protected String name;
	protected boolean active = true, reflect = false;
    protected State state[];
    protected int auto = 0, hp, mp, sp, level, maxlv, exp, maxp, atk, def, spi, wis, agi;
    protected int mres[], res[] = { 3, 3, 3, 3, 3, 3, 3, 3 };

    protected ArrayList<Integer> jobSkills = new ArrayList<Integer>();

	public String getName() {
		return this.name;
	}

	public void setName(String name) {
		this.name = name;
	}

	public boolean isActive() {
		return this.active;
	}

	public void setActive(boolean active) {
		this.active = active;
	}

	public boolean isReflecting() {
		return this.reflect;
	}

	public void setReflect(boolean reflect) {
		this.reflect = reflect;
	}

	public State[] getState() {
		return this.state;
	}

	public int getAuto() {
		return this.auto;
	}

	public void setAuto(int auto) {
		this.auto = auto;
	}

	public int getHP() {
		return this.hp;
	}

	public void setHP(int hp) {
		if (hp > this.maxp)
			this.hp = maxhp;
		else
			this.hp = hp;
	}

	public int getMP() {
		return this.mp;
	}

	public void setMP(int mp) {
		if (mp > this.maxmp)
			this.mp = this.maxmp;
		else
			this.mp = mp;
	}

	public int getSP() {
		return this.sp;
	}

	public void setSP(int sp) {
		if (sp > this.maxsp)
			this.sp = this.maxsp;
		else
			this.sp = sp;
	}

	public int getLevel() {
		return this.level;
	}

	public void setLevel(int level) {
		if (level > this.maxlv)
			level = this.maxlv;
		while (this.level < level) {
			this.exp = this.maxp;
			this.levelUp();
		}
	}

	public int getMaxLevel() {
		return this.maxlv;
	}

	public void setMaxLevel(int maxlv) {
		this.maxlv = maxlv;
	}

	public int getExp() {
		return this.exp;
	}

	public void setExp(int exp) {
		this.exp = exp;
		this.levelUp();
	}

	public int getMaxExp() {
		return this.maxp;
	}

	public int getAtk() {
		return this.atk;
	}

	public void setAtk(int atk) {
		this.atk = atk;
	}

	public int getDef() {
		return this.def;
	}

	public void setDef(int def) {
		this.def = def;
	}

	public int getSpi() {
		return this.spi;
	}

	public void setSpi(int spi) {
		this.spi = spi;
	}

	public int getWis() {
		return this.wis;
	}

	public void setWis(int wis) {
		this.wis = wis;
	}

	public int getAgi() {
		return this.agi;
	}

	public void setAgi(int agi) {
		this.agi = agi;
	}

	public int[] getMres() {
		return this.mres;
	}

	public int[] getRes() {
		return this.res;
	}

    public void setMaxAgi(int magi) {
        this.magi = magi;
    }

    public void setMaxSpi(int mspi) {
        this.mspi = mspi;
    }

    public void setMaxWis(int mwis) {
        this.mwis = mwis;
    }

    public void setMaxDef(int mdef) {
        this.mdef = mdef;
    }

    public void setMaxAtk(int matk) {
        this.matk = matk;
    }

    public void setMaxSP(int maxsp) {
        this.maxsp = maxsp;
    }

    public void setMaxMP(int maxmp) {
        this.maxmp = maxmp;
    }

    public void setMaxHP(int maxhp) {
        this.maxhp = maxhp;
    }

    public void setRaceName(String rname) {
        this.rname = rname;
    }

    public void setJobName(String jname) {
        this.jname = jname;
    }

    public void setHPinc(int hpp) {
        this.hpp = hpp;
    }

    public void setMPinc(int mpp) {
        this.mpp = mpp;
    }

    public void setSPinc(int spp) {
        this.spp = spp;
    }

    public void setAtkInc(int atkp) {
        this.atkp = atkp;
    }

    public void setDefInc(int defp) {
        this.defp = defp;
    }

    public void setWisInc(int wisp) {
        this.wisp = wisp;
    }

    public void setSpiInc(int spip) {
        this.spip = spip;
    }

    public void setAgiInc(int agip) {
        this.agip = agip;
    }

	public ArrayList<Integer> getJobSkills() {
		return this.jobSkills;
	}

	public void setRace(Race race, boolean init) {
		if (init) {
			this.raceStats(race.maxhp, race.maxmp, race.maxsp, race.matk, race.mdef, race.mwis, race.mspi, race.magi);
			this.stats(1, this.maxlv);
		} else
			this.raceStats(this.maxhp + race.maxhp, this.maxmp + race.maxmp, this.maxsp + race.maxsp,
					this.matk + race.matk, this.mdef + race.mdef, this.mwis + race.mwis, this.mspi + race.mspi,
					this.magi + race.magi);
		this.setRes(race.rres, true);
		this.raceSkills = race.raceSkills;
		this.rname = race.rname;
	}

	public void changeJob(Job job, boolean add) {
		this.jname = job.jname;
		if (add)
			this.raceStats(this.maxhp + job.maxhp, this.maxmp + job.maxmp, this.maxsp + job.maxsp, this.matk + job.matk,
					this.mdef + job.mdef, this.mwis + job.mwis, this.mspi + job.mspi, this.magi + job.magi);
		this.jobStats(job.hpp, job.mpp, job.spp, job.atkp, job.defp, job.wisp, job.spip, job.agip);
		if (!add) {
			this.removeSkills();
			this.resetRes();
		}
		this.setRes(job.rres, false);
		this.addSkills(job.raceSkills);
		this.setSprName(job.getSprName());
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
		this("Actor", 5);
	}

	public Actor(String name, int maxlv) {
		this.name = name;
		this.state = this.AddStates();
		this.stats(1, maxlv);
	}

	public Actor(String name, String race, String jname, int lv, int maxlv, int maxhp, int maxmp, int maxsp, int atk,
			int def, int wis, int spi, int agi, int hpp, int mpp, int spp, int atkp, int defp, int wisp, int spip,
			int agip) {
		this(name, race, jname, lv, maxlv, maxhp, maxmp, maxsp, atk, def, wis, spi, agi, hpp, mpp, spp, atkp, defp,
				wisp, spip, agip, new int[] {});
	}

	public Actor(String name, String race, String jname, int lv, int maxlv, int maxhp, int maxmp, int maxsp, int atk,
			int def, int wis, int spi, int agi, int hpp, int mpp, int spp, int atkp, int defp, int wisp, int spip,
			int agip, int[] newRes) {
		this(name, race, jname, lv, maxlv, maxhp, maxmp, maxsp, atk, def, wis, spi, agi, hpp, mpp, spp, atkp, defp,
				wisp, spip, agip, newRes, new int[] {});
	}

	public Actor(String name, String race, String jname, int lv, int maxlv, int maxhp, int maxmp, int maxsp, int atk,
			int def, int wis, int spi, int agi, int hpp, int mpp, int spp, int atkp, int defp, int wisp, int spip,
			int agip, int[] newRes, int[] newSkill) {
		super(jname, maxhp, maxmp, maxsp, atk, def, wis, spi, agi, hpp, mpp, spp, atkp, defp, wisp, spip, agip, newRes,
				newSkill);
		this.name = name;
		this.rname = race;
		this.state = this.AddStates();
		this.raceSkills = newSkill;
		this.stats(lv, maxlv);
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

	public void copy(Actor cloned) {
		this.name = cloned.name;
		this.jname = cloned.jname;
		this.rname = cloned.rname;
		this.raceStats(cloned.maxhp, cloned.maxmp, cloned.maxsp, cloned.atk, cloned.def, cloned.wis, cloned.spi,
				cloned.agi);
		this.jobStats(cloned.hpp, cloned.mpp, cloned.spp, cloned.atkp, cloned.defp, cloned.wisp, cloned.spip,
				cloned.agip);
		this.stats(cloned.level, cloned.maxlv);
		for (int i = 0; i < state.length; i++) {
			this.state[i].dur = cloned.state[i].dur;
			this.state[i].res = cloned.state[i].res;
		}
		for (int i = 0; i < res.length; i++) {
			this.rres[i] = cloned.rres[i];
			this.mres[i] = cloned.mres[i];
			this.res[i] = cloned.res[i];
		}
		this.jobSkills.clear();
		this.raceSkills = cloned.raceSkills;
		this.addSkills(cloned.jobSkills);
		this.setSprName(cloned.getSprName());
	}

	public void addSkills(ArrayList<Integer> newSkill) {
		for (int i = 0; i < newSkill.size(); i++)
			if (!this.jobSkills.contains(newSkill.get(i)))
				this.jobSkills.add(newSkill.get(i));
	}

	@Override
	public void setResistance(int[] newRes) {
		this.mres = new int[] { 3, 3, 3, 3, 3, 3, 3, 3 };
		for (int i = 0; i < newRes.length && i < this.mres.length; i++)
			this.mres[i] = newRes[i];
	}

	private State[] AddStates() {
		State state[] = new State[11];

		state[0] = new State("Regen", false, false, false, -1, 10, 0, 0, 0, 2, 0, 0, 0, false);
		state[1] = new State("Poison", false, false, false, 10, -7, 0, -2, 0, -2, 0, 0, 0, false);
		state[2] = new State("Clarity", false, false, false, -1, 0, 7, 0, 0, 0, 1, 1, 0, false);
		state[3] = new State("Dizziness", false, false, false, 3, 0, -7, 0, 0, 0, -1, -1, 0, false);
		state[4] = new State("Vigour", false, false, false, -1, 0, 0, 7, 1, 0, 0, 0, 1, false);
		state[5] = new State("Weakness", false, false, false, 5, 0, 0, -7, -1, 0, 0, 0, -1, false);
		state[6] = new State("Berserk", false, true, false, 7, 0, 0, 0, 5, -3, 0, 0, 3, false);
		state[7] = new State("Confusion", false, false, true, 3, 0, 0, 0, 0, 0, 0, 0, 0, false);
		state[8] = new State("Sleep", true, false, false, 5, 0, 0, 0, 0, -3, 0, 0, -3, false);
		state[9] = new State("Stun", true, false, false, 1, 0, 0, 0, 0, -1, 0, 0, -1, false);
		state[10] = new State("Reflect", false, false, false, 7, 0, 0, 0, 0, 0, 0, 0, 0, true);

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

	public class State {
		protected boolean inactive, confusion, auto, reflect;
		protected String name;
		protected int res, mdur, dur, hpm, mpm, spm, atkm, defm, spim, wism, agim;
		protected int resm[] = { 0, 0, 0, 0, 0, 0, 0, 0 };

		public boolean isInactive() {
			return this.inactive;
		}

		public void setInactive(boolean inactive) {
			this.inactive = inactive;
		}

		public boolean isConfusing() {
			return this.confusion;
		}

		public void setConfusion(boolean confusion) {
			this.confusion = confusion;
		}

		public boolean isAuto() {
			return this.auto;
		}

		public void setAuto(boolean auto) {
			this.auto = auto;
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

		public void setRes(int res) {
			this.res = res;
		}

		public int getMaxDur() {
			return this.mdur;
		}

		public void setMaxDur(int mdur) {
			this.mdur = mdur;
		}

		public int getDur() {
			return this.dur;
		}

		public void setDur(int dur) {
			this.dur = dur;
		}

		public int getHPmod() {
			return this.hpm;
		}

		public void setHPmod(int hpm) {
			this.hpm = hpm;
		}

		public int getMPmod() {
			return this.mpm;
		}

		public void setMPmod(int mpm) {
			this.mpm = mpm;
		}

		public int getSPmod() {
			return this.spm;
		}

		public void setSPmod(int spm) {
			this.spm = spm;
		}

		public int getAtkMod() {
			return this.atkm;
		}

		public void setAtkMod(int atkm) {
			this.atkm = atkm;
		}

		public int getDefMod() {
			return this.defm;
		}

		public void setDefMod(int defm) {
			this.defm = defm;
		}

		public int getSpiMod() {
			return this.spim;
		}

		public void setSpiMod(int spim) {
			this.spim = spim;
		}

		public int getWisMod() {
			return this.wism;
		}

		public void setWisMod(int wism) {
			this.wism = wism;
		}

		public int getAgiMod() {
			return this.agim;
		}

		public void setAgiMod(int agim) {
			this.agim = agim;
		}

		public int[] getResMod() {
			return this.resm;
		}

		public State(String name, boolean inactive, boolean auto, boolean confusion, int dur, int hpm, int mpm, int spm,
				int atkm, int defm, int wism, int spim, int agim, boolean reflect) {
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
						s += dmgsp + " SP";
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
	}

	public void addSkills(int[] newSkill) {
		for (int j = 0; j < newSkill.length; j++)
			if (!this.jobSkills.contains(newSkill[j]))
				this.jobSkills.add(newSkill[j]);
	}

	public void removeSkills() {
		this.jobSkills.clear();
	}

}