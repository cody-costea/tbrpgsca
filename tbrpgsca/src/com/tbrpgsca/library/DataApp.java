/*This file has been written by Claudiu-Stefan Costea,
 * and is released under the terms of the LGPL-2.1
 */
package com.tbrpgsca.library;

import android.app.Application;

public class DataApp extends Application {
	
    public Race[] pcRace = AddRaces();
	public Job[] pcJob = AddJobs();
	public Actor[] Player = AddCharacters();
    public Ability[] Skill = AddSkills();
    public Ability[] Item = AddItems();    
	public int party[]={1,2,3,4};
	public int enemy[][]=AddEnemies();        
    	
	private static boolean nstate[] = {false};
	private static boolean poison[] = {false,false,true};
	private static boolean regen[] = {false,true};
	private static boolean dizziness[] = {false,false,false,false,true};
	private static boolean madness[] = {false,false,false,false,true,false,false,true,true,true,true};
	private static boolean clarity[] = {false,false,false,true};
	private static boolean weakness[] = {false,false,false,false,false,false,true};
	private static boolean vigour[] = {false,false,false,false,false,true};
	private static boolean tpoison[] = {false,false,true,false,true,false,true};
	private static boolean tregen[] = {false,true,false,true,false,true};
	private static boolean revive[] = {true};
	private static boolean rvregen[] = {true,true};
	private static boolean berserk[] = {false,false,false,false,false,false,false,true};
	private static boolean confusion[] = {false,false,false,false,false,false,false,false,true};
	private static boolean confclarity[] = {false,false,false,true,false,false,false,false,true,true};
	private static boolean conftregen[] = {false,true,false,true,false,true,false,false,true,true};
	private static boolean cure[] = {false,false,true,false,true,false,true,true,true,true,true};
	private static boolean sleep[] = {false,false,false,false,false,false,false,false,false,true};
	private static boolean stun[] = {false,false,false,false,false,false,false,false,false,false,true};
	private static boolean confsleep[] = {false,false,false,false,false,false,false,false,true,true};
	private static boolean dizzystun[] = {false,false,false,false,true,false,false,false,false,false,true};
	private static boolean reflect[] = {false,false,false,false,false,false,false,false,false,false,false,true};
    
    protected static Race[] AddRaces(){
    	
    	Race[] pcRace = new Race[4];
    	
    	pcRace[0]=new Race("Elf", 40,25,10, 7,5,15,12,11);
    	pcRace[2]=new Race("Human", 47,15,13, 9,11,9,11,10);
    	pcRace[1]=new Race("Half-Orc", 55,7,13, 17,12,5,7,9);
    	pcRace[3]=new Race("Gnome", 40,15,20, 12,8,10,5,15);
    	return pcRace;
    	
    }
	
	protected static Job[] AddJobs() {
		
		Job pcJob[]=new Job[15];
		
		pcJob[1]=new Job("Warrior",1,0,0, 1,0,0,0,1);
		pcJob[1].skill.add(8);
        pcJob[1].skill.add(9);
        pcJob[1].skill.add(10);
        pcJob[1].skill.add(11);
        pcJob[1].skill.add(12);
        pcJob[1].skill.add(14);

        pcJob[2]=new Job("Sorcerer",0,1,0, 0,0,1,0,1);
        pcJob[2].skill.add(23);
        pcJob[2].skill.add(24);
        pcJob[2].skill.add(25);
        pcJob[2].skill.add(26);
        pcJob[2].skill.add(27);
        pcJob[2].skill.add(28);
        pcJob[2].skill.add(29);
        pcJob[2].skill.add(30);
        pcJob[2].skill.add(31);
        pcJob[2].skill.add(32);
        pcJob[2].skill.add(33);
        pcJob[2].skill.add(34);
        pcJob[2].skill.add(35);
        pcJob[2].skill.add(38);

        pcJob[3]=new Job("Monk",0,1,0, 0,1,0,1,0);
        pcJob[3].skill.add(2);
        pcJob[3].skill.add(3);
        pcJob[3].skill.add(4);
        pcJob[3].skill.add(5);
        pcJob[3].skill.add(6);
        pcJob[3].skill.add(7);

        pcJob[4]=new Job("Rogue",0,0,1, 1,0,0,0,1);
        pcJob[4].skill.add(15);
        pcJob[4].skill.add(16);
        pcJob[4].skill.add(17);
        pcJob[4].skill.add(18);
        pcJob[4].skill.add(22);
        pcJob[4].skill.add(38);

        pcJob[10]=new Job("Swashbuckler",0,0,1, 1,0,0,0,1);
        pcJob[10].skill.add(8);
        pcJob[10].skill.add(9);
        pcJob[10].skill.add(10);
        pcJob[10].skill.add(11);
        pcJob[10].skill.add(15);
        pcJob[10].skill.add(16);
        pcJob[10].skill.add(17);
        pcJob[10].skill.add(18);
        pcJob[10].skill.add(19);

        pcJob[5]=new Job("Alchemist",0,1,0, 0,0,1,0,1);
        pcJob[5].skill.add(23);
        pcJob[5].skill.add(24);
        pcJob[5].skill.add(25);
        pcJob[5].skill.add(26);
        pcJob[5].skill.add(27);
        pcJob[5].skill.add(28);
        pcJob[5].skill.add(30);
        pcJob[5].skill.add(31);
        pcJob[5].skill.add(32);
        pcJob[5].skill.add(33);
        pcJob[5].skill.add(15);
        pcJob[5].skill.add(16);
        pcJob[5].skill.add(17);
        pcJob[5].skill.add(18);
        pcJob[5].skill.add(20);
        
        pcJob[6]=new Job("Dragoon",1,0,0, 1,0,1,0,0);
        pcJob[6].skill.add(8);
        pcJob[6].skill.add(9);
        pcJob[6].skill.add(10);
        pcJob[6].skill.add(11);
        pcJob[6].skill.add(23);
        pcJob[6].skill.add(24);
        pcJob[6].skill.add(25);
        pcJob[6].skill.add(26);
        pcJob[6].skill.add(27);
        pcJob[6].skill.add(28);
        pcJob[6].skill.add(30);
        pcJob[6].skill.add(31);
        pcJob[6].skill.add(32);
        pcJob[6].skill.add(33);
        pcJob[6].skill.add(41);
        pcJob[6].skill.add(42);
        pcJob[6].skill.add(43);
        pcJob[6].skill.add(44);        
        pcJob[6].skill.add(50);        

        pcJob[7]=new Job("Knight",1,0,0, 0,1,0,1,0);
        pcJob[7].skill.add(2);
        pcJob[7].skill.add(3);
        pcJob[7].skill.add(4);
        pcJob[7].skill.add(5);
        pcJob[7].skill.add(8);
        pcJob[7].skill.add(9);
        pcJob[7].skill.add(10);
        pcJob[7].skill.add(11);
        pcJob[7].skill.add(13);

        pcJob[8]=new Job("Bard",0,0,1, 0,0,0,1,1);
        pcJob[8].skill.add(2);
        pcJob[8].skill.add(3);
        pcJob[8].skill.add(4);
        pcJob[8].skill.add(5);
        pcJob[8].skill.add(15);
        pcJob[8].skill.add(16);
        pcJob[8].skill.add(17);
        pcJob[8].skill.add(18);
        pcJob[8].skill.add(21);

        pcJob[9]=new Job("Shaman",0,1,0, 0,0,1,1,0);
        pcJob[9].skill.add(2);
        pcJob[9].skill.add(3);
        pcJob[9].skill.add(4);
        pcJob[9].skill.add(5);
        pcJob[9].skill.add(23);
        pcJob[9].skill.add(24);
        pcJob[9].skill.add(29);
        pcJob[9].skill.add(34);
        pcJob[9].skill.add(49);
        
        pcJob[11]=new Job("Reaver",1,0,0, 1,0,0,0,0);
        pcJob[11].skill.add(8);
        pcJob[11].skill.add(9);
        pcJob[11].skill.add(11);
        pcJob[11].skill.add(23);
        pcJob[11].skill.add(24);        
        pcJob[11].skill.add(29);
        pcJob[11].skill.add(15);
        pcJob[11].skill.add(16);
        pcJob[11].skill.add(18);
        pcJob[11].skill.add(45);
        pcJob[11].skill.add(46);
        
        pcJob[12]=new Job("Ninja",0,0,1, 0,0,0,0,1);
        pcJob[12].skill.add(8);
        pcJob[12].skill.add(9);
        pcJob[12].skill.add(11);
        pcJob[12].skill.add(15);
        pcJob[12].skill.add(16);
        pcJob[12].skill.add(17);
        pcJob[12].skill.add(2);
        pcJob[12].skill.add(3);
        pcJob[12].skill.add(5);
        pcJob[12].skill.add(40);
        
        pcJob[13]=new Job("Templar",1,0,0, 0,0,0,1,0);
        pcJob[13].skill.add(8);
        pcJob[13].skill.add(9);
        pcJob[13].skill.add(10);
        pcJob[13].skill.add(2);
        pcJob[13].skill.add(3);
        pcJob[13].skill.add(4);
        pcJob[13].skill.add(23);
        pcJob[13].skill.add(36);
        pcJob[13].skill.add(37);
        pcJob[13].skill.add(47);
        
        pcJob[14]=new Job("Druid",0,1,0, 0,0,1,0,0);
        pcJob[14].skill.add(23);
        pcJob[14].skill.add(25);
        pcJob[14].skill.add(26);
        pcJob[14].skill.add(27);
        pcJob[14].skill.add(28);
        pcJob[14].skill.add(30);
        pcJob[14].skill.add(31);
        pcJob[14].skill.add(32);
        pcJob[14].skill.add(33);
        pcJob[14].skill.add(2);
        pcJob[14].skill.add(3);
        pcJob[14].skill.add(4);
        pcJob[14].skill.add(15);
        pcJob[14].skill.add(16);
        pcJob[14].skill.add(18);
        pcJob[14].skill.add(48);
        
        pcJob[0]=new Job("Hero",1,1,1, 0,1,0,0,0);
        pcJob[0].skill.add(8);
        pcJob[0].skill.add(9);
        pcJob[0].skill.add(10);
        pcJob[0].skill.add(11);
        pcJob[0].skill.add(25);
        pcJob[0].skill.add(26);
        pcJob[0].skill.add(27);
        pcJob[0].skill.add(28);
        pcJob[0].skill.add(29);
        pcJob[0].skill.add(30);
        pcJob[0].skill.add(31);
        pcJob[0].skill.add(32);
        pcJob[0].skill.add(33);
        pcJob[0].skill.add(34);
        pcJob[0].skill.add(36);
        pcJob[0].skill.add(37);
        pcJob[0].skill.add(2);
        pcJob[0].skill.add(3);
        pcJob[0].skill.add(4);
        pcJob[0].skill.add(5);
        pcJob[0].skill.add(15);
        pcJob[0].skill.add(16);
        pcJob[0].skill.add(17);
        pcJob[0].skill.add(18);
        
        return pcJob;
    }
    
    protected static Actor[] AddCharacters() {
        
    	int maxlv = 5;
    	
    	Actor npc[] = new Actor[12];
    	
    	npc[0]=new Actor("","","", 0,0, 0,0,0, 0,0,0,0,0, 0,0,0, 0,0,0,0,0);
    	
    	npc[1]=new Actor("Cody",maxlv);
        
    	npc[2]=new Actor("George",maxlv);
    	
    	npc[3]=new Actor("Stephen",maxlv);
    	
        npc[4]=new Actor("Ogre","Ogre","Ogre", 3,maxlv, 55,7,13, 17,12,5,7,3, 1,1,1, 1,1,0,0,0);
        npc[4].skill.add(9);
        npc[4].skill.add(11);
        npc[4].state[5].res=9;
        npc[4].state[10].dur=-2;
        npc[5]=new Actor("Lizard", "Lizard", "Lizard", 3, maxlv, 50,15,10, 13,12,9,7,5, 1,1,1, 1,0,1,0,0);
        npc[5].skill.add(23);
        npc[5].skill.add(30);
        npc[5].mres[2]=7;
        npc[5].mres[3]=1;
        npc[6]=new Actor("Goblin", "Goblin", "Goblin", 3, maxlv, 45,5,20, 13,12,5,5,1, 1,1,1, 1,0,0,0,1);
        npc[6].skill.add(15);
        npc[7]=new Actor("Troll", "Troll", "Troll", 3, maxlv, 47,15,15, 13,12,5,10,7, 1,1,1, 0,1,0,1,0);
        npc[7].state[0].dur=-3;
        npc[7].skill.add(2);
        
        for (int i=npc.length-4;i<npc.length;i++)
        	npc[i]=new Actor();
        
        return npc;
    }
    
    protected static Ability[] AddSkills() {
        
        Ability ability[] = new Ability[51];
    	
    	ability[0]=new Ability("Attack", true,false, 0, 0,0,0, 0,1, 10,0,0, 0,1, false, nstate, confsleep);
        ability[1]=new Ability("Defend", true,true, 0, 0,0,0, 3,-1, 0,0,-3, -1,0, false, nstate, nstate);
        ability[2]=new Ability("Heal", false,true, 1, 0,3,0, 3,-1, -25,0,0, 0,6, false, revive, nstate);
        ability[3]=new Ability("Meditate", true,true, 1, 0,0,3, 3,-1, -3,-7,0, -1,0, false, nstate, dizziness);
        ability[4]=new Ability("Cure", false,true, 3, 0,7,0, 3,-1, -17,0,0, 0,7, false, revive, cure);
        ability[5]=new Ability("Clarity", true,true, 3, 0,0,7, 3,-1, 0,-3,0, 0,6, false, clarity, madness);
        ability[6]=new Ability("Prayer", false,true, 5, 0,7,0, 3,-1, -23,0,0, 1,6, false, revive, nstate);
        ability[7]=new Ability("Regen", false,true, 4, 0,10,0, 3,-1, -37,0,0, 0,6, false, rvregen, poison);
        ability[8]=new Ability("Smite", true,false, 1, 1,0,2, 1,1, 4,3,0, 0,1, false, nstate, confclarity);
        ability[9]=new Ability("Hit", true,false, 1, 3,0,1, 0,1, 12,0,0, 0,1, false, nstate, confsleep);
        ability[10]=new Ability("Bash", true,false, 3, 3,0,5, 1,1, 7,5,0, 0,1, false, dizziness, confclarity);
        ability[11]=new Ability("Smash", true,false, 3, 5,0,3, 0,1, 18,0,1, 0,1, false, nstate, confsleep);
        ability[12]=new Ability("Berserk", true,false, 4, 7,0,4, 3,1, 0,0,0, -1,0, false, berserk, weakness);
        ability[13]=new Ability("Shock", true,false, 4, 4,0,7, 1,1, 10,5,0, 0,7, false, dizzystun, confclarity);
        ability[14]=new Ability("Crush", true,false, 5, 7,4,0, 0,1, 25,0,2, 0,1, false, stun, confsleep);
        ability[15]=new Ability("Strike", true,true, 1, 0,0,3, 4,1, 13,0,0, 0,1, false, nstate, confsleep);
        ability[16]=new Ability("Weaken", true,true, 1, 0,0,3, 4,1, 3,0,9, 0,1, false, weakness, vigour);
        ability[17]=new Ability("Dash", true,true, 3, 0,0,7, 4,1, 15,0,0, 0,1, false, nstate, confsleep);
        ability[18]=new Ability("Poison", true,true, 3, 0,0,5, 4,2, 5,0,7, 0,1, false, poison, regen);
        ability[19]=new Ability("Pierce", true,true, 4, 0,0,10, 4,2, 15,0,0, 0,1, false, nstate, confsleep);
        ability[20]=new Ability("Toxic Gas", true,true, 4, 0,0,10, 6,3, 1,1,1, 1,1, false, tpoison, tregen);
        ability[21]=new Ability("Cheer", true,true, 4, 0,10,5, 3,-1, 0,0,-5, -2,0, false, vigour, cure);
        ability[22]=new Ability("Venom Blade", true,false, 4, 0,0,10, 4,1, 17,0,3, 0,1, false, poison, conftregen);
        ability[23]=new Ability("Absorb", true,true, 1, 0,0,3, 2,1, 0,7,0, 0,6, true, nstate, nstate);
        ability[24]=new Ability("Drain", true,true, 3, 0,10,0, 2,1, 15,0,0, 0,6, true, nstate, nstate);
        ability[25]=new Ability("Fireball", true,true, 1, 0,3,0, 2,1, 11,0,0, 0,2, false, nstate, sleep);
        ability[26]=new Ability("Iceshard", true,true, 1, 0,3,0, 2,1, 11,0,0, 0,3, false, nstate, sleep);
        ability[27]=new Ability("Lighting", true,true, 1, 0,3,0, 2,1, 11,0,0, 0,4, false, nstate, sleep);
        ability[28]=new Ability("Rock", true,true, 1, 0,3,0, 2,1, 11,0,0, 0,5, false, nstate, sleep);
        ability[29]=new Ability("Darkness", true,true, 1, 0,3,0, 2,1, 11,0,0, 0,6, false, nstate, sleep);
        ability[30]=new Ability("Flame", true,true, 3, 0,5,0, 2,1, 15,0,0, 1,2, false, nstate, sleep);
        ability[31]=new Ability("Blizzard", true,true, 3, 0,5,0, 2,1, 15,0,0, 1,3, false, nstate, sleep);
        ability[32]=new Ability("Storm", true,true, 3, 0,5,0, 2,1, 15,0,0, 1,4, false, nstate, sleep);
        ability[33]=new Ability("Earthquake", true,true, 3, 0,5,0, 2,1, 15,0,0, 1,5, false, nstate, sleep);
        ability[34]=new Ability("Eclipse", true,true, 3, 0,5,0, 2,1, 13,0,0, 1,6, false, nstate, sleep);
        ability[35]=new Ability("Flare", true,true, 5, 0,12,0, 2,2, 25,0,0, 0,1, false, nstate, sleep);
        ability[36]=new Ability("Light Ray", true,true, 1, 0,3,0, 3,1, 11,0,0, 0,7, false, nstate, sleep);
        ability[37]=new Ability("Sacred Fire", true,true, 3, 0,5,0, 3,1, 15,0,0, 1,7, false, nstate, sleep);
        ability[38]=new Ability("Confusion", true,true, 5, 0,15,0, 2,1, 0,0,0, 0,6, false, confusion, clarity);
        ability[39]=new Ability("Sleep", true,true, 3, 0,0,17, 4,1, 3,0,17, 0,1, false, sleep, clarity);
        ability[40]=new Ability("Slash", true,true, 5, 0,10,10, 4,1, 15,0,0, 1,1, false, nstate, confsleep);
        ability[41]=new Ability("Fire Wpn", true,false, 2, 0,3,2, 5,1, 17,0,0, 0,2, false, nstate, confsleep);
        ability[42]=new Ability("Ice Wpn", true,false, 2, 0,3,2, 5,1, 17,0,0, 0,3, false, nstate, confsleep);
        ability[43]=new Ability("Electric Wpn", true,false, 2, 0,3,2, 5,1, 17,0,0, 0,4, false, nstate, confsleep);
        ability[44]=new Ability("Stone Wpn", true,false, 2, 0,3,2, 5,1, 17,0,0, 0,5, false, nstate, confsleep);
        ability[45]=new Ability("Dark Wpn", true,false, 2, 0,3,2, 5,1, 17,0,0, 0,6, false, nstate, confsleep);
        ability[46]=new Ability("Vampiric Wpn", true,false, 5, 0,10,5, 5,1, 21,0,0, 0,6, true, nstate, confsleep);
        ability[47]=new Ability("Reflect", true,true, 5, 0,10,0, 3,1, 0,0,0, 0,0, false, reflect, nstate);
        ability[48]=new Ability("Meteor", true,true, 5, 0,17,0, 2,1, 19,0,0, 1,1, false, nstate, sleep);
        ability[49]=new Ability("Syphon", true,true, 4, 0,15,0, 2,1, 13,0,3, 1,6, true, nstate, nstate);
        ability[50]=new Ability("Dragon Breath", true,false, 4, 0,13,7, 5,1, 17,0,0, 1,1, false, nstate, confsleep);
        
        return ability;
    }
    
    protected static Ability[] AddItems() {        
        
        Ability item[] = new Ability[9];
    	
    	item[0]=new Ability("Potion", true, -25,0,0, 0,0, nstate, nstate);
        item[1]=new Ability("Ether", true, 0,-10,0, 0,0, nstate, nstate);
        item[2]=new Ability("Tonic", true, 0,0,-10, 0,0, nstate, nstate);
        item[3]=new Ability("Antidote", true, 0,0,0, 0,0, nstate, poison);
        item[4]=new Ability("Hi-Ether", true, 0,-25,0, 0,0, nstate, dizziness);
        item[5]=new Ability("Hi-Tonic", true, 0,-20,0, 0,0, vigour, weakness);
        item[6]=new Ability("Panacea", true, 0,0,0, 0,0, nstate, cure);
        item[7]=new Ability("Elixir", true, -100,-100,-100, 0,0, revive, nstate);
        item[8]=new Ability("Hi-Potion", true, -50,0,0, 0,0, revive, poison);
        
        return item;        
    }
    
    protected static State[] AddStates() {    	
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
    
    protected static int[][] AddEnemies(){

		int enemy[][]=new int[6][4];
    	
    	enemy[0][0]=6;
		enemy[0][1]=4;
		enemy[0][2]=0;
		enemy[0][3]=0;
		
		enemy[1][0]=5;
		enemy[1][1]=7;
		enemy[1][2]=0;
		enemy[1][3]=0;
		
		enemy[2][0]=5;
		enemy[2][1]=4;
		enemy[2][2]=6;
		enemy[2][3]=0;
				
		enemy[3][0]=5;
		enemy[3][1]=7;
		enemy[3][2]=4;
		enemy[3][3]=0;
		
		enemy[4][0]=5;
		enemy[4][1]=6;
		enemy[4][2]=4;
		enemy[4][3]=7;
		
		enemy[5][0]=5;
		enemy[5][1]=4;
		enemy[5][2]=6;
		enemy[5][3]=7;
		
		return enemy;
    }
    
    @Override
	public void onCreate() {
		super.onCreate();
	}
}