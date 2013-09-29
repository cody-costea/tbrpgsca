/*This file has been written by Claudiu-Stefan Costea,
 * and is released under the terms of the LGPL-2.1
 */
package com.tbrpgsca.library;

public class Ability {
    public String name;
    public int trg,hpc,mpc,spc,lvrq,atki,hpdmg,mpdmg,spdmg,dmgtype,element,qty;
    public boolean battle,state[],rstate[],absorb,range;
    
    private static boolean nstate[] = {};
    
    public String execute(Actor user, Actor target) {
    	String s="";
        //if ((this.hpc<=user.hp)&&(this.mpc<=user.mp)&&(this.spc<=user.sp)&&(this.lvrq<=user.level)) {
            int dmg = (int) (Math.random() * 4);
            switch (dmgtype) {
                case 1:
                   dmg += ((this.atki*user.def/2)+user.atk/2)/(target.def*target.res[element]+1);
                case 3:
                   dmg += (this.atki*user.wis)/(target.spi*target.res[element]+1);
                case 2:
                   dmg += (this.atki*user.spi)/(target.wis*target.res[element]+1);
                case 4:
                   dmg += ((this.atki*user.agi/2)+user.atk/2)/((target.agi/2+target.def/2)*target.res[element]+1);
                case 5:
                    dmg += ((this.atki*user.wis/2)+user.atk/2)/((target.spi/2+target.def/2)*target.res[element]+1);
                case 6:
                    dmg += ((this.atki*user.agi/2)+user.wis/2)/((target.agi/2+target.spi/2)*target.res[element]+1);
                case 7:
                    dmg += ((this.atki*user.spi/2)+user.atk/2)/((target.wis/2+target.def/2)*target.res[element]+1);
                default:
                   dmg += (this.atki*user.atk)/(target.def*target.res[element]+1);
            }            
            int dmghp = (hpdmg!=0) ? (dmg + this.hpdmg) : 0;
            int dmgmp = (mpdmg!=0) ? (dmg + this.mpdmg) : 0;
            int dmgsp = (spdmg!=0) ? (dmg + this.spdmg) : 0;
            if (target.res[element]<0) {dmghp=-dmghp;dmgmp=-dmgmp;dmgsp=-dmgsp;}
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

            s+=target.applyState(false);
            s+=user.checkStatus();
        //}
        return s;
    }
    
    public Ability(){    	
    	this("Ability",true,false,0,0,0,0,0,1,0,0,0,0,1,false,nstate,nstate);
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
