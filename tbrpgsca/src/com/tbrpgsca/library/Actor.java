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

import java.util.ArrayList;

public class Actor extends Job {
    public String name;
    public boolean active=true,reflect=false;
    public State state[];
    public int auto=0,hp,mp,sp,level,maxlv,exp,maxp,atk,def,spi,wis,agi;
    public int mres[],res[]={3,3,3,3,3,3,3,3};
    
    public void setRace(Race race, Boolean init)
    {
    	if (init) {
    		this.raceStats(race.maxhp, race.maxmp, race.maxsp, race.matk, race.mdef, race.mwis, race.mspi, race.magi);    		
    		this.stats(1, this.maxlv);    		
    	} else
    		this.raceStats(this.maxhp+race.maxhp, this.maxmp+race.maxmp, this.maxsp+race.maxsp,
    				this.matk+race.matk, this.mdef+race.mdef, this.mwis+race.mwis, this.mspi+race.mspi, this.magi+race.magi);
    	this.setRes(race.rres,true);
    	this.addSkills(race.skill);
        this.rname = race.rname;
    }
    
    public void changeJob(Job job, Boolean add)
    {
        this.jname = job.jname;
        if (add) this.raceStats(this.maxhp+job.maxhp, this.maxmp+job.maxmp, this.maxsp+job.maxsp,
        		this.matk+job.matk, this.mdef+job.mdef, this.mwis+job.mwis, this.mspi+job.mspi, this.magi+job.magi);
        this.jobStats(job.hpp,job.mpp,job.spp, job.atkp,job.defp,job.wisp,job.spip,job.agip);
        if (!add) {this.removeSkills();this.resetRes();}
        this.setRes(job.rres,false);        
        this.addSkills(job.skill);
        this.setSprName(job.getSprName());
    }
    
    public void checkRes(int r)
    {
    	if (r<8&&r>=0){
    		if (this.rres[r]>7) this.rres[r]=7;
    		if (this.rres[r]<0) this.rres[r]=0;
    		if (this.mres[r]>7) this.mres[r]=7;
    		if (this.mres[r]<0) this.mres[r]=0;
    		if (this.res[r]>7) this.res[r]=7;
    		if (this.res[r]<0) this.res[r]=0;
    	}
    }
    
    public String applyState(boolean consume)
    {
    	String s="";
    	if (auto<2&&auto>-2) this.auto=0; else this.auto=2;
    	if (consume&&hp>0) active=true;
    	this.atk=this.matk;
    	this.def=this.mdef;
    	this.spi=this.mspi;
    	this.wis=this.mwis;
    	this.agi=this.magi;
    	for (int i=0;i<this.res.length;i++)
			this.res[i]=this.mres[i];
    	this.reflect=false;
    	boolean c=false;
    	for (int i=0;i<this.state.length;i++)
    		if (this.state[i].dur!=0&&this.hp>0)
    		{
    			String r=state[i].apply(this, consume);
    			if (r.length()>0)
    			{
    				if (c) s+=", ";
    				if (consume&&!c)
    				{
    					s+="\n";
    					c=true;
    				}
    				s+=r;
    			}
    		}
    	s+=checkStatus();
    	if (c&&consume) s+=".";
    	return s;
    }
    
    public String checkStatus()
    {
    	String s="";
    	if (this.hp>maxhp) hp=maxhp;
    	if (this.mp>maxmp) mp=maxmp;
    	if (this.sp>maxsp) sp=maxsp;
    	if (this.hp<1) {
            s+=" (and falls unconcious)";
            active=false;
    		for (int i=0;i<state.length;i++)
    			this.state[i].remove();
    	}
    	if (this.hp<-maxhp) hp=-maxhp;
    	if (this.mp<0) mp=0;
    	if (this.sp<0) sp=0;
    	return s;
    }
    
    public Actor()
    {
    	this("Actor",5);
    }
    
    public Actor(String name,int maxlv)
    {
    	this.name=name;
    	this.state = this.AddStates();
    	this.stats(1,maxlv);
    }
    
    public Actor(String name, String race, String jname, int lv, int maxlv,
            int maxhp, int maxmp, int maxsp, int atk, int def, int wis, int spi, int agi,
            int hpp, int mpp, int spp, int atkp, int defp, int wisp, int spip, int agip)
    {
    	this(name, race, jname, lv, maxlv, maxhp,maxmp,maxsp, atk,def,wis,spi,agi,
    			hpp,mpp,spp,atkp,defp,wisp,spip,agip,new int[]{});    	        
    }
    
    public Actor(String name, String race, String jname, int lv, int maxlv,
            int maxhp, int maxmp, int maxsp, int atk, int def, int wis, int spi,
            int agi, int hpp, int mpp, int spp, int atkp, int defp, int wisp,
            int spip, int agip, int[] newRes)
    {
    	this(name, race, jname, lv, maxlv, maxhp,maxmp,maxsp, atk,def,wis,spi,agi,
    			hpp,mpp,spp,atkp,defp,wisp,spip,agip,newRes,new int[]{});    	        
    }
    
    public Actor(String name, String race, String jname, int lv, int maxlv,
            int maxhp, int maxmp, int maxsp, int atk, int def, int wis, int spi,
            int agi, int hpp, int mpp, int spp, int atkp, int defp, int wisp,
            int spip, int agip, int[] newRes, int[] newSkill)
    {    	
    	super(jname, maxhp,maxmp,maxsp, atk,def,wis,spi,agi, hpp,mpp,spp, atkp,defp,wisp,spip,agip, newRes, newSkill);
    	this.name=name;
        this.rname=race;
        this.state = this.AddStates();
        this.stats(lv,maxlv);
    }
    
    private void setRes(int nres[],boolean race)
    {
    	for (int i=0;i<this.res.length;i++)
    		{
    			if (race) this.rres[i]=nres[i];
    			else this.mres[i]+=nres[i];
    			this.checkRes(i);
    		}
    	if (race) {resetRes();this.setRes(nres, false);}
    }
    
    private void resetRes()
    {
    	for (int i=0;i<this.mres.length;i++) this.mres[i]=3+rres[i];
    }
    
    private void stats(int lv,int maxlv)
    {    	
    	this.resetRes();
    	this.maxlv = maxlv;
        this.maxp = 5;
        if (lv>1) {
            this.exp = (int) (this.maxp * (Math.pow(lv-1,2)));
        } else this.exp = 0;
        this.level=1; this.levelUp();
        this.recover();
        if (hp<1) active=false;
    }
    
    public void recover()
    {
        this.hp=this.maxhp;
        this.mp=this.maxmp;
        this.sp=this.maxsp;
        this.active=true;
        this.atk=this.matk;
        this.def=this.mdef;
        this.spi=this.mspi;
        this.wis=this.mwis;
        this.agi=this.magi;
        for (int i=0;i<this.state.length;i++)
        	this.state[i].remove();
        this.applyState(false);
    }
    
    public void copy(Actor cloned)
    {
    	this.name=cloned.name;
        this.jname=cloned.jname;
        this.rname=cloned.rname;
        this.raceStats(cloned.maxhp,cloned.maxmp,cloned.maxsp, cloned.atk,cloned.def,cloned.wis,cloned.spi,cloned.agi);
        this.jobStats(cloned.hpp,cloned.mpp,cloned.spp, cloned.atkp,cloned.defp,cloned.wisp,cloned.spip,cloned.agip);
        this.stats(cloned.level,cloned.maxlv);
        for (int i=0;i<state.length;i++)
        {
        	this.state[i].dur=cloned.state[i].dur;
        	this.state[i].res=cloned.state[i].res;
        }
        for (int i=0;i<res.length;i++)
        {
        	this.rres[i]=cloned.rres[i];
			this.mres[i]=cloned.mres[i];
			this.res[i]=cloned.res[i];
		}
        this.skill.clear();
        this.addSkills(cloned.skill);
        this.setSprName(cloned.getSprName());
    }
    
    public void addSkills(ArrayList<Ability> newSkill)
    {
    	for (int i=0;i<newSkill.size();i++)
        	if (!this.skill.contains(newSkill.get(i))) 
        			this.skill.add(newSkill.get(i));
    }
    
    @Override
    public void setResistance(int[] newRes)
    {
    	this.mres=new int[]{3,3,3,3,3,3,3,3};
    	for (int i=0;i<newRes.length&&i<this.mres.length;i++)
    		this.mres[i]=newRes[i];
    }
    
    private State[] AddStates() {    	
    	State state[] = new State[11];
    	
    	state[0]=new State("Regen", false,false,false, -1, 10,0,0, 0,2,0,0,0, false);
    	state[1]=new State("Poison", false,false,false, 10, -7,0,-2, 0,-2,0,0,0, false);
    	state[2]=new State("Clarity", false,false,false, -1, 0,7,0, 0,0,1,1,0, false);
    	state[3]=new State("Dizziness", false,false,false, 3, 0,-7,0, 0,0,-1,-1,0, false);
    	state[4]=new State("Vigour", false,false,false, -1, 0,0,7, 1,0,0,0,1, false);
    	state[5]=new State("Weakness", false,false,false, 5, 0,0,-7, -1,0,0,0,-1, false);
    	state[6]=new State("Berserk", false,true,false, 7, 0,0,0, 5,-3,0,0,3, false);
    	state[7]=new State("Confusion", false,false,true, 3, 0,0,0, 0,0,0,0,0, false);
    	state[8]=new State("Sleep", true,false,false, 5, 0,0,0, 0,-3,0,0,-3, false);
    	state[9]=new State("Stun", true,false,false, 1, 0,0,0, 0,-1,0,0,-1, false);
    	state[10]=new State("Reflect", false,false,false, 7, 0,0,0, 0,0,0,0,0, true);
    	
    	return state;
    }
    
    public void levelUp()
    {        
        while (this.maxp<=this.exp&&this.level<this.maxlv)
        {
                this.maxp *= 2;
                this.level++;
                this.maxhp+=3;
                this.maxmp+=2;
                this.maxsp+=2;
                this.matk++;
                this.mdef++;
                this.mwis++;
                this.mspi++;
                this.magi++;
            if (this.level%2==1) {
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
}