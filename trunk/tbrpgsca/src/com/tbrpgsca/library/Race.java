/*This file has been written by Claudiu-Stefan Costea,
 * and is released under the terms of the LGPL-2.1
 */
package com.tbrpgsca.library;

import java.util.ArrayList;

public class Race {
	public String rname;
	public int maxhp,maxmp,maxsp,matk,mdef,mwis,mspi,magi;
	public int resm[]={0,0,0,0,0,0,0,0};
    public ArrayList<Ability> skill = new ArrayList<Ability>();
	
	public Race(){
		this("Humanoid", 25, 25, 25, 10, 10, 10, 10, 10);
	}
	
	public Race(String rname, int maxhp, int maxmp, int maxsp, int atk,
    		int def, int wis, int spi, int agi) {
		this.rname = rname;
		this.raceStats(maxhp, maxmp, maxsp, atk, def, wis, spi, agi);
		this.addSkills();
	}
	
	public Race(String rname, int maxhp, int maxmp, int maxsp, int atk,
    		int def, int wis, int spi, int agi, int[] newSkill) {
		this(rname,maxhp, maxmp, maxsp, atk, def, wis, spi, agi);
		this.addSkills(newSkill);
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
    
    public void addSkills(int[] newSkill){
    	for (int j=0;j<newSkill.length;j++)
    		if (!this.skill.contains(newSkill[j]))
    			this.skill.add(DataApp.Skill[newSkill[j]]);
    }
    
    public void addSkills(){
    	this.addSkills(new int[]{0,1});
    }
    
    public void removeSkills(boolean all){
    	if (all) {this.skill.clear();this.addSkills();}
    	else
    		for (int i=this.skill.size()-1;i>=0;i--)
    			if (this.skill.get(i).lvrq>0)
    				this.skill.remove(i);
    }
    
    public void removeSkills(){
    	this.removeSkills(false);
    }
    
}