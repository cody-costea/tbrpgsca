/*This file has been written by Claudiu-Stefan Costea,
 * and is released under the terms of the LGPL-2.1
 */
package com.tbrpgsca.library;

public class Ability {
    public String name;
    public int trg,hpc,mpc,spc,lvrq,atki,hpdmg,mpdmg,spdmg,dmgtype,element,qty;
    public boolean battle,state[],rstate[],absorb,range;
        
    public String execute(Actor user, Actor target) {
    	String s="";
            int dmg = (int) (Math.random() * 4);
            int res = (target.res[element]<7)?target.res[element]:-1;
            switch (dmgtype) {
                case 1:
                   dmg += (this.atki*((user.def+user.atk)/2))/(target.def*res+1);
                   break;
                case 3:
                   dmg += (this.atki*user.wis)/(target.spi*res+1);
                   break;
                case 2:
                   dmg += (this.atki*user.spi)/(target.wis*res+1);
                   break;
                case 4:
                   dmg += (this.atki*((user.agi+user.atk)/2))/(((target.agi+target.def)/2)*res+1);
                   break;
                case 5:
                    dmg += (this.atki*((user.wis+user.atk)/2))/(((target.spi+target.def)/2)*res+1);
                    break;
                case 6:
                    dmg += (this.atki*((user.agi+user.wis+user.atk)/3))/(((target.agi+target.spi)/2)*res+1);
                    break;
                case 7:
                    dmg += (this.atki*((user.spi+user.atk+user.def)/3))/(((target.wis+target.def)/2)*res+1);
                    break;
                default:
                   dmg += (this.atki*user.atk)/(target.def*res+1);
            }            
            if (dmgtype==2||dmgtype==3||(dmgtype!=4&&(int) (Math.random() * 13+user.agi/5)>2+target.agi/4)
            		||(dmgtype==4&&(int) (Math.random() * 13+user.agi/3)>2+target.agi/4))
            {
            	int dmghp = (hpdmg!=0) ? (dmg + this.hpdmg) : 0;
            	int dmgmp = (mpdmg!=0) ? (dmg + this.mpdmg) : 0;
            	int dmgsp = (spdmg!=0) ? (dmg + this.spdmg) : 0;
            	if (res<0) {dmghp=-dmghp;dmgmp=-dmgmp;dmgsp=-dmgsp;}
            	target.hp -= dmghp;
            	target.mp -= dmgmp;
            	target.sp -= dmgsp;
            	if (absorb) {
                user.hp += dmghp;
                user.mp += dmgmp;
                user.sp += dmgsp;
            	}
            	boolean c=false;
            	if (dmghp!=0||dmgmp!=0||dmgsp!=0) s+=(", " + target.name+" suffers");            
            	if (dmghp!=0) {s+=(" "); if (dmghp<1) s+="+"; s+=-dmghp + " HP";  c=true;}
            	if (dmgmp!=0) {if (c) s+=","; s+=" "; if (dmgmp<1) s+="+"; s+=-dmgmp + " MP"; c=true;}
            	if (dmgsp!=0) {if (c) s+=","; s+=" "; if (dmgsp<1) s+="+"; s+=-dmgsp + " SP";}
            
            for (int i=1;i<this.state.length;i++) 
            	if (this.state[i])
            		target.state[i-1].inflict();
            
            for (int i=1;i<this.rstate.length;i++) 
            	if (this.rstate[i]) {
            		target.state[i-1].remove();
            	}
            } else s+=", but misses";

            s+=target.applyState(false);
            s+=user.checkStatus();
        return s;
    }
    
    public int getLvRq(){
    	return Math.abs(this.lvrq);
    }
    
    public Ability(){    	
    	this("Ability",true,false,0,0,0,0,0,1,0,0,0,0,1,false,new boolean[]{},new boolean[]{});
    }
    
    public Ability(String name, boolean battle, int hpdmg, int mpdmg, int spdmg,
    		int trg, int element, boolean state[], boolean rstate[]) {
        this(name, battle, true, 0, 0, 0, 0, 0, 0, hpdmg, mpdmg, spdmg, trg,element, false, state, rstate);
    }
    
    public Ability(String name, boolean battle,boolean range,int lvrq, int hpc, int mpc,
    		int spc, int dmgtype, int atkp, int hpdmg, int mpdmg, int spdmg,  int trg,
    		int element, boolean absorb, boolean state[], boolean rstate[]) {
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
        this.range=range;
        this.element=element;
        this.qty=0;
    }
}