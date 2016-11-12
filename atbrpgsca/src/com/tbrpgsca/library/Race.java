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

public class Race {
	protected String rname;
	protected int maxhp, maxmp, maxsp, matk, mdef, mwis, mspi, magi, originId;

	protected int rres[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	protected Ability[] raceSkills;

	public String getRaceName() {
		return this.rname;
	}

	public int getMaxHP() {
		return this.maxhp;
	}

	public int getMaxMP() {
		return this.maxmp;
	}

	public int getMaxRP() {
		return this.maxsp;
	}

	public int getMaxAtk() {
		return this.matk;
	}

	public int getMaxDef() {
		return this.mdef;
	}

	public int getMaxWis() {
		return this.mwis;
	}

	public int getMaxSpi() {
		return this.mspi;
	}

	public int getMaxAgi() {
		return this.magi;
	}

	public int[] getRaceRes() {
		return this.rres;
	}

	public Ability[] getRaceSkills() {
		return this.raceSkills;
	}

	public int getOriginId() {
		return this.originId;
	}

	public Race() {
		this(0, "Humanoid", 25, 25, 25, 10, 10, 10, 10, 10);
	}

	public Race(int id, String rname, int maxhp, int maxmp, int maxsp, int atk,
			int def, int wis, int spi, int agi, int[] newRes, Ability[] newSkill) {
		this.originId = id;
		this.rname = rname;
		this.raceStats(maxhp, maxmp, maxsp, atk, def, wis, spi, agi);
		this.raceSkills = newSkill;
		this.setResistance(newRes);
	}

	public Race(int originId, String rname, int maxhp, int maxmp, int maxsp,
			int atk, int def, int wis, int spi, int agi) {
		this(originId, rname, maxhp, maxmp, maxsp, atk, def, wis, spi, agi,
				new int[] {});
	}

	public Race(int id, String rname, int maxhp, int maxmp, int maxsp, int atk,
			int def, int wis, int spi, int agi, int[] newRes) {
		this(id, rname, maxhp, maxmp, maxsp, atk, def, wis, spi, agi, newRes,
				new Ability[] {});
	}

	protected void raceStats(int maxhp, int maxmp, int maxsp, int atk, int def,
			int wis, int spi, int agi) {
		this.maxhp = maxhp;
		this.maxmp = maxmp;
		this.maxsp = maxsp;
		this.matk = atk;
		this.mdef = def;
		this.mwis = wis;
		this.mspi = spi;
		this.magi = agi;
	}

	public void setResistance(int[] newRes) {
		for (int i = 0; i < newRes.length && i < this.rres.length; i++)
			this.rres[i] = newRes[i];
	}
}