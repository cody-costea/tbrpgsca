/*
Copyright (C) 2015 Claudiu-Stefan Costea

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
	public String rname;
	public int maxhp, maxmp, maxsp, matk, mdef, mwis, mspi, magi;
	public int rres[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	public int[] raceSkills;

	public Race() {
		this("Humanoid", 25, 25, 25, 10, 10, 10, 10, 10);
	}

	public Race(String rname, int maxhp, int maxmp, int maxsp, int atk, int def, int wis, int spi, int agi,
			int[] newRes, int[] newSkill) {
		this.rname = rname;
		this.raceStats(maxhp, maxmp, maxsp, atk, def, wis, spi, agi);
		this.raceSkills = newSkill;
		this.setResistance(newRes);
	}

	public Race(String rname, int maxhp, int maxmp, int maxsp, int atk, int def, int wis, int spi, int agi) {
		this(rname, maxhp, maxmp, maxsp, atk, def, wis, spi, agi, new int[] {});
	}

	public Race(String rname, int maxhp, int maxmp, int maxsp, int atk, int def, int wis, int spi, int agi,
			int[] newRes) {
		this(rname, maxhp, maxmp, maxsp, atk, def, wis, spi, agi, newRes, new int[] {});
	}

	protected void raceStats(int maxhp, int maxmp, int maxsp, int atk, int def, int wis, int spi,
			int agi/* , int[] newSkill */) {
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