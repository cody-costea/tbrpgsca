package com.tbrpgsca.library;

public class State {
	boolean inactive,confusion,auto,reflect;
	String name;
	int res,mdur,dur,hpm,mpm,spm,atkm,defm,spim,wism,agim;
    int resm[]={0,0,0,0,0,0,0,0};
	
	public State(String name,boolean inactive,boolean auto,boolean confusion,int dur,int hpm,int mpm,int spm,
			int atkm,int defm, int wism, int spim, int agim,boolean reflect){
		this.name=name;
		this.mdur=dur;
		this.dur=0;
		this.res=0;
		this.hpm=hpm;
		this.mpm=mpm;
		this.spm=spm;
		this.atkm=atkm;
		this.defm=defm;
		this.spim=spim;
		this.wism=wism;
		this.agim=agim;
		this.inactive=inactive;
		this.confusion=confusion;
		this.auto=(confusion)?true:auto;
		this.reflect=reflect;
	}
	
	public void inflict(){
		int rnd = (int) (Math.random() * 10);
		if (dur>-2&&rnd>res) dur=mdur;
	}
	
	public String apply(Actor actor, boolean consume){
		String s="";
		if (dur!=0&&actor.hp>0) {
			if (consume){
				int rnd = (int) (Math.random() * 3);
				int dmghp=((actor.maxhp+rnd)*hpm)/100;
				int dmgmp=((actor.maxmp+rnd)*mpm)/100;
				int dmgsp=((actor.maxsp+rnd)*spm)/100;
				actor.hp+=dmghp;
				actor.mp+=dmgmp;
				actor.sp+=dmgsp;
				boolean c=false;
	            if (dmghp!=0||dmgmp!=0||dmgsp!=0) s+=(name+" causes " + actor.name);            
	            if (dmghp!=0) {s+=(" "); if (dmghp>=0) s+="+"; s+=dmghp + " HP"; c=true;}
	            if (dmgmp!=0) {if (c) s+=","; s+=" "; if (dmgmp>=0) s+="+"; s+=dmgmp + " MP"; c=true;}
	            if (dmgsp!=0) {if (c) s+=","; s+=" "; if (dmgsp>=0) s+="+"; s+=dmgsp + " SP";}
				if (dur>0) dur--;
			}
			actor.atk=actor.matk+atkm;
			actor.def=actor.mdef+defm;
			actor.spi=actor.mspi+spim;
			actor.wis=actor.mwis+wism;
			actor.agi=actor.magi+agim;
			for (int i=0;i<actor.res.length;i++)
				{actor.res[i]+=this.resm[i];actor.checkRes(i);}
			if (inactive) actor.active=false;
			if (reflect) actor.reflect=true;
			if (auto&&actor.auto<2) actor.auto=1;
			if (confusion) actor.auto=(actor.auto<2)?-1:-2;
		}
		return s;
	}
	
	public void remove(){
		if (dur>-2) dur=0;
	}
}