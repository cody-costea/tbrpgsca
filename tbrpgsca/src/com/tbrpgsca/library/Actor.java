/*This file has been written by Claudiu-Stefan Costea,
 * and is released under the terms of the LGPL-2.1
 */
package com.tbrpgsca.library;

public class Actor extends Job {
    public String name;
    public boolean active=true,reflect=false;
    public State state[] = DataApp.AddStates();
    public int auto=0,hp,mp,sp,level,maxlv,exp,maxp,atk,def,spi,wis,agi;
    public int mres[]={3,3,3,3,3,3,3,3},res[]={3,3,3,3,3,3,3,3};
    
    public void setRace(Race race, Boolean init) {
    	if (init) {
    		this.raceStats(race.maxhp, race.maxmp, race.maxsp, race.matk, race.mdef, race.mwis, race.mspi, race.magi);    		
    		this.stats(1, this.maxlv);
    		for (int i=0;i<res.length;i++)
    			{this.mres[i]+=race.resm[i];checkRes(i);}
    	}
    	for (int i=0;i<race.skill.size();i++) {
        	if (!this.skill.contains(race.skill.get(i))) 
        			this.skill.add(race.skill.get(i));
        }
        this.rname = race.rname;
    }
    
    public void changeJob(Job job, Boolean init) {
        this.jname = job.jname;
        this.jobStats(job.hpp,job.mpp,job.spp, job.atkp,job.defp,job.wisp,job.spip,job.agip);
        for (int i=0;i<res.length;i++)
			{this.mres[i]+=job.resm[i];checkRes(i);}
        if (init) {
        	this.skill.clear();
        	this.skill.add(0);
        	this.skill.add(1);
        }
        
        for (int i=0;i<job.skill.size();i++) {
        	if (!this.skill.contains(job.skill.get(i))) 
        			this.skill.add(job.skill.get(i));
        }
    }
    
    public void checkRes(int r){
    	if (r<8&&r>=0){
    		if (mres[r]>7) mres[r]=7;
    		if (mres[r]<0) mres[r]=0;
    		if (res[r]>7) res[r]=7;
    		if (res[r]<0) res[r]=0;
    	}
    }
    
    public String applyState(boolean consume) {
    	String s="";
    	if (auto<2&&auto>-2) this.auto=0; else this.auto=2;
    	if (consume&&hp>0) active=true;
    	this.atk=matk;
    	this.def=mdef;
    	this.spi=mspi;
    	this.wis=mwis;
    	this.agi=magi;
    	for (int i=0;i<res.length;i++)
			this.res[i]=this.mres[i];
    	this.reflect=false;
    	boolean c=false;
    	for (int i=0;i<state.length;i++)
    		if (state[i].dur!=0&&this.hp>0) {
    			String r=state[i].apply(this, consume);
    			if (r.length()>0){
    				if (c) s+=", ";
    				if (consume&&!c) {s+="\n"; c=true;}
    				s+=r;
    			}
    		}
    	s+=checkStatus();
    	if (c&&consume) s+=".";
    	return s;
    }
    
    public String checkStatus() {
    	String s="";
    	//if (hp>0) active=true;
    	if (this.hp>maxhp)
    		hp=maxhp;
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
    
    public Actor(){
    	this("Actor",5);
    }
    
    public Actor(String name,int maxlv){
    	this.name=name;
    	stats(1,maxlv);
    }
    
    public Actor(String name, String race, String jname, int lv, int maxlv,
            int maxhp, int maxmp, int maxsp, int atk, int def, int wis, int spi, int agi,
            int hpp, int mpp, int spp, int atkp, int defp, int wisp, int spip, int agip){
    	super(jname, hpp,mpp,spp, atkp,defp,wisp,spip,agip);
    	this.name = name;
        //this.jname = jname;
        this.rname=race;
        this.raceStats(maxhp,maxmp,maxsp, atk,def,wis,spi,agi);
        //this.jobStats(hpp,mpp,spp, atkp,defp,wisp,spip,agip);
        stats(lv,maxlv);        
    }
    
    private void stats(int lv,int maxlv){
    	this.maxlv = maxlv;
        this.maxp = 5;
        if (lv>1) {
            this.exp = (int) (this.maxp * (Math.pow(lv-1,2)));
        } else this.exp = 0;
        this.level=1; this.levelUp();
        recover();
        if (hp<1) active=false;
    }
    
    public void recover(){

        hp=maxhp;
        mp=maxmp;
        sp=maxsp;
        active=true;
        atk=matk;
        def=mdef;
        spi=mspi;
        wis=mwis;
        agi=magi;
        for (int i=0;i<state.length;i++) state[i].remove();
        applyState(false);
    }
    
    public void copy(Actor cloned){
    	this.name = cloned.name;
        this.jname = cloned.jname;
        this.rname=cloned.rname;
        this.raceStats(cloned.maxhp,cloned.maxmp,cloned.maxsp, cloned.atk,cloned.def,cloned.wis,cloned.spi,cloned.agi);
        this.jobStats(cloned.hpp,cloned.mpp,cloned.spp, cloned.atkp,cloned.defp,cloned.wisp,cloned.spip,cloned.agip);
        stats(cloned.level,cloned.maxlv);
        for (int i=0;i<state.length;i++) {
        	this.state[i].dur=cloned.state[i].dur;
        	this.state[i].res=cloned.state[i].res;
        	}
        for (int i=0;i<res.length;i++){
			this.mres[i]=cloned.mres[i];
			this.res[i]=cloned.res[i];
		}
    }
    
    public void levelUp() {        
        while (this.maxp<=this.exp&&this.level<this.maxlv) {
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
            if (level%2==1) {
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