/*This file has been written by Claudiu-Stefan Costea,
 * and is released under the terms of the LGPL-2.1
 */
package com.tbrpgsca.library;

public class Race {
	public String rname;
	public int maxhp,maxmp,maxsp,matk,mdef,mwis,mspi,magi;
	public int res[]={3,3,3,3,3,3,3,3};
	
	public Race(){
		this.rname="Humanoid";
		this.raceStats(25, 25, 25, 10, 10, 10, 10, 10);
	}
	
	public Race(String rname, int maxhp, int maxmp, int maxsp, int atk,
    		int def, int wis, int spi, int agi) {
		this.rname = rname;
		this.raceStats(maxhp, maxmp, maxsp, atk, def, wis, spi, agi);
	}
    
    protected void raceStats(int maxhp, int maxmp, int maxsp, int atk,
    		int def, int wis, int spi, int agi) {
        
    	this.maxhp = maxhp;
        this.maxmp = maxmp;
        this.maxsp = maxsp;
        this.matk = atk;
        this.mdef = def;
        this.mwis = wis;
        this.mspi = spi;
        this.magi = agi;
    }
}
