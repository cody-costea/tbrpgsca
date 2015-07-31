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
package com.tbrpgsca.demo;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.text.SpannableString;
import android.text.util.Linkify;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;

import com.tbrpgsca.library.Ability;
import com.tbrpgsca.library.Actor;
import com.tbrpgsca.library.BattleAct;
import com.tbrpgsca.library.Job;
import com.tbrpgsca.library.R;
import com.tbrpgsca.library.Race;

public class DemoAct extends Activity {

	private Ability[] Skill;
	private Ability[] Item;
	private Race[] pcRace;
	private Job[] pcJob;
	private Actor[] Player;
	private Actor[] Party;
	private Actor[][] Enemy;

	private EditText name[] = new EditText[3];
	private Spinner jobBox[] = new Spinner[3], raceBox[] = new Spinner[3];
	private ArrayAdapter<String> jobList, raceList;
	private Button Begin, About, Exit;
	private int level = 0;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		this.setContentView(R.layout.activity_demo);

		this.Skill = AddSkills();
		this.Item = AddItems();
		this.pcRace = AddRaces();
		this.pcJob = AddJobs();
		this.Player = AddCharacters();
		this.Enemy = AddEnemies(this.Player);
		this.Party = new Actor[4];

		this.raceList = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item);
		this.jobList = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item);
		this.raceBox[0] = (Spinner) findViewById(R.id.RaceBox1);
		this.raceBox[1] = (Spinner) findViewById(R.id.RaceBox2);
		this.raceBox[2] = (Spinner) findViewById(R.id.RaceBox3);
		for (int i = 0; i < this.pcRace.length; i++)
			this.raceList.add(this.pcRace[i].rname);
		for (int i = 0; i < 3; i++)
			this.raceBox[i].setAdapter(this.raceList);
		this.raceBox[0].setSelection(1);
		this.raceBox[1].setSelection(0);
		this.raceBox[2].setSelection(2);
		this.jobBox[0] = (Spinner) findViewById(R.id.JobBox1);
		this.jobBox[1] = (Spinner) findViewById(R.id.JobBox2);
		this.jobBox[2] = (Spinner) findViewById(R.id.JobBox3);
		for (int i = 0; i < this.pcJob.length; i++)
			this.jobList.add(this.pcJob[i].jname);
		for (int i = 0; i < 3; i++)
			this.jobBox[i].setAdapter(this.jobList);
		this.jobBox[0].setSelection(0);
		this.jobBox[1].setSelection(5);
		this.jobBox[2].setSelection(3);
		this.name[0] = (EditText) findViewById(R.id.NameField1);
		this.name[1] = (EditText) findViewById(R.id.NameField2);
		this.name[2] = (EditText) findViewById(R.id.NameField3);
		this.Begin = (Button) findViewById(R.id.StartBt);
		this.About = (Button) findViewById(R.id.InfoBt);
		this.Exit = (Button) findViewById(R.id.QuitBt);
		this.Begin.setOnClickListener(this.cAction);
		this.About.setOnClickListener(this.cAction);
		this.Exit.setOnClickListener(this.cAction);
		this.Party[0] = this.Player[1];
		this.Party[1] = this.Player[2];
		this.Party[2] = this.Player[3];
		this.Party[3] = null;
		for (int i = 0; i < this.Item.length; i++)
			this.Item[i].qty = 5;
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		this.getMenuInflater().inflate(R.menu.battle, menu);
		return true;
	}

	private void setPlayer(int p) {
		String n = this.name[p - 1].getText().toString();
		if (n.equals(""))
			switch (p) {
			case 1:
				n = "Cody";
				break;
			case 2:
				n = "George";
				break;
			case 3:
				n = "Stephen";
				break;
			}
		this.Player[p].name = n;
		if (this.raceBox[p - 1].isEnabled())
			this.Player[p].setRace(this.pcRace[this.raceBox[p - 1].getSelectedItemPosition()], true);
		this.Player[p].changeJob(this.pcJob[this.jobBox[p - 1].getSelectedItemPosition()], false);
		this.raceBox[p - 1].setEnabled(false);
	}

	private void beginBt() {
		for (int i = 1; i <= 3; i++)
			this.setPlayer(i);
		int surprise = 0;
		if (this.level % 2 == 1)
			surprise = -1;
		if (this.level == 2)
			surprise = 1;
		BattleAct.InitiateBattle(this, this.Party, this.Enemy[this.level], this.Skill, this.Item, surprise,
				this.level % 2 == 0);
		for (int i = 1; i < this.Player.length; i++)
			this.Player[i].recover();
	}

	private OnClickListener cAction = new OnClickListener() {
		@Override
		public void onClick(final View v) {
			switch (v.getId()) {
			case R.id.StartBt:
				DemoAct.this.beginBt();
				break;
			case R.id.InfoBt:
				DemoAct.this.displayMsg("About", getString(R.string.msg_about));
				break;
			case R.id.QuitBt:
				DemoAct.this.finish();
				break;
			}
		}
	};

	private void displayMsg(String t, String s) {
		SpannableString text = new SpannableString(s);
		Linkify.addLinks(text, Linkify.WEB_URLS);
		AlertDialog.Builder msg = new AlertDialog.Builder(this);
		msg.setCancelable(false);
		msg.setMessage(text);
		msg.setTitle(t);
		msg.setPositiveButton("Ok", new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface arg0, int arg1) {

			}
		});
		AlertDialog dialog = msg.create();
		dialog.show();
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		if (requestCode >= 0) {
			if (resultCode == RESULT_OK) {
				int result = data.getExtras().getInt("Outcome");
				if (result > 0 && DemoAct.this.level < 5)
					DemoAct.this.level++;
			}
		}
	}        
    	
	private static boolean nostate[] = {false};
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
    
    private Race[] AddRaces()
    {
    	
    	Race[] pcRace = new Race[4];
    			
    	pcRace[0]=new Race("Elf", 40,25,10, 7,5,15,12,11, new int[]{0,-1,1,1,1,1});
    	pcRace[2]=new Race("Human", 47,15,13, 9,11,9,11,10, new int[]{0,0,0,0,0,0,-1,1});
    	pcRace[1]=new Race("Half-Orc", 55,7,13, 17,12,5,7,9, new int[]{0,1,-1,-1,-1,-1});
    	pcRace[3]=new Race("Gnome", 40,15,20, 12,8,10,5,15, new int[]{0,0,0,0,0,0,1,-1});
    	return pcRace;
    	
    }
	
	private Job[] AddJobs()
	{
		
		Job pcJob[]=new Job[15];
		
		pcJob[1]=new Job("Berserker",1,0,0, 1,0,0,0,1, new int[]{0,1,-1,-1,-1,-1}, new int[]{8,9,10,11,12,14});
		pcJob[2]=new Job("Sorcerer",0,1,0, 0,0,1,0,1, new int[]{0,-1,1,1,1,1,1,-1}, new int[]{23,24,25,26,27,28,29,30,31,32,33,34,35,38});
        pcJob[2].setSprName("Wizard");
        pcJob[3]=new Job("Monk",0,1,0, 0,1,0,1,0, new int[]{0,0,0,0,0,0,-7,7}, new int[]{2,3,4,5,6,7});
        pcJob[4]=new Job("Rogue",0,0,1, 1,0,0,0,1, new int[]{}, new int[]{15,16,17,18,22,38});
        pcJob[5]=new Job("Alchemist",0,1,0, 0,0,1,0,1, new int[]{}, new int[]{23,24,25,26,27,28,30,31,32,33,15,16,17,18,20});
        pcJob[6]=new Job("Dragoon",1,0,0, 1,0,1,0,0, new int[]{0,0,1,1,1,1,0,-1}, new int[]{8,9,10,11,23,24,25,26,27,28,30,31,32,33,41,42,43,44,50});
        pcJob[7]=new Job("Knight",1,0,0, 0,1,0,1,0, new int[]{0,1,-1,-1,-1,-1,-7,7}, new int[]{2,3,4,5,8,9,10,11,13});
        pcJob[8]=new Job("Warden",0,0,1, 0,0,0,1,1, new int[]{0,0,0,0,0,0,-2,2}, new int[]{2,3,4,5,15,16,17,18,21});
        pcJob[8].setSprName("Bard");
        pcJob[9]=new Job("Shaman",0,1,0, 0,0,1,1,0, new int[]{0,0,0,0,0,0,7,-7}, new int[]{52,53,54,55,23,24,29,34,49});
        pcJob[10]=new Job("Swashbuckler",0,0,1, 1,0,0,0,1, new int[]{}, new int[]{8,9,10,11,15,16,17,18,19});
        pcJob[11]=new Job("Reaver",1,0,0, 1,0,0,0,0, new int[]{0,0,0,0,0,0,7,-7}, new int[]{8,9,11,23,24,29,15,16,18,45,46});
        pcJob[12]=new Job("Ninja",0,0,1, 0,0,0,0,1, new int[]{}, new int[]{8,9,11,15,16,17,2,3,5,40});
        pcJob[13]=new Job("Crusader",1,0,0, 0,0,0,1,0, new int[]{0,0,0,0,0,0,-7,7}, new int[]{8,9,10,2,3,4,56,36,51,37,47});
        pcJob[13].setSprName("Templar");
        pcJob[14]=new Job("Druid",0,1,0, 0,0,1,0,0, new int[]{0,0,1,1,1,1,-1,-1}, new int[]{23,25,26,27,28,30,31,32,33,52,53,54,15,16,18,48});
        pcJob[0]=new Job("Hero",1,1,1, 0,0,0,0,0, new int[]{}, new int[]{8,9,10,11,23,24,25,26,27,28,29,41,42,43,44,45,30,31,32,33,34,36,51,37,2,3,4,5,15,16,17,18});
        	
        return pcJob;
    }
    
    private Actor[] AddCharacters()
    {
        
    	int maxlv = 5;
    	
    	Actor npc[] = new Actor[8];
    	
    	npc[0]=new Actor("","","", 0,0, 0,0,0, 0,0,0,0,0, 0,0,0, 0,0,0,0,0);
    	
    	npc[1]=new Actor("Cody",maxlv);        
    	npc[2]=new Actor("George",maxlv);    	
    	npc[3]=new Actor("Stephen",maxlv);
    	
        npc[4]=new Actor("Ogre","Ogre","Ogre", 3,maxlv, 55,7,13, 17,12,5,7,3, 1,1,1, 1,1,0,0,0, new int[]{0,1}, new int[]{9,11});
        npc[4].state[5].res=9;npc[4].state[10].dur=-2;
        npc[5]=new Actor("Lizard", "Lizard", "Lizard", 3, maxlv, 50,15,10, 13,12,9,7,5, 1,1,1, 1,0,1,0,0, new int[]{0,0,7,1}, new int[]{23,30});
        npc[6]=new Actor("Goblin", "Goblin", "Goblin", 3, maxlv, 45,5,20, 13,12,5,5,1, 1,1,1, 1,0,0,0,1, new int[]{}, new int[]{15});
        npc[7]=new Actor("Troll", "Troll", "Troll", 3, maxlv, 47,15,15, 13,12,5,10,7, 1,1,1, 0,1,0,1,0, new int[]{}, new int[]{2});
        npc[7].state[0].dur=-3;
        
        return npc;
    }
    
    private Ability[] AddSkills()
    {
        
        Ability ability[] = new Ability[57];
    	
    	ability[0]=new Ability("Attack", true,false, 0, 0,0,0, 0,1, 10,0,0, 0,1, false, nostate, confsleep);
        ability[1]=new Ability("Defend", true,true, 0, 0,0,0, 3,-1, 0,0,-3, -1,0, false, nostate, nostate);
        ability[2]=new Ability("Heal", false,true, 1, 0,3,0, 3,-1, -25,0,0, 0,6, false, revive, nostate);
        ability[3]=new Ability("Meditate", true,true, 1, 0,0,3, 3,-1, -3,-7,0, -1,6, false, nostate, dizziness);
        ability[4]=new Ability("Cure", false,true, 3, 0,7,0, 3,-1, -17,0,0, 0,7, false, revive, cure);
        ability[5]=new Ability("Clarity", true,true, 3, 0,0,7, 3,-1, 0,-3,0, 0,6, false, clarity, madness);
        ability[6]=new Ability("Regen", false,true, 4, 0,10,0, 3,-1, -37,0,0, 0,6, false, rvregen, poison);
        ability[7]=new Ability("Prayer", false,true, 5, 0,7,0, 3,-1, -23,0,0, 1,0, false, revive, nostate);
        ability[8]=new Ability("Smite", true,false, 1, 1,0,2, 1,1, 4,3,0, 0,1, false, nostate, confclarity);
        ability[9]=new Ability("Hit", true,false, 1, 3,0,1, 0,1, 12,0,0, 0,1, false, nostate, confsleep);
        ability[10]=new Ability("Bash", true,false, 3, 3,0,5, 1,1, 7,5,0, 0,1, false, dizziness, confclarity);
        ability[11]=new Ability("Smash", true,false, 3, 5,0,3, 0,1, 18,0,1, 0,1, false, nostate, confsleep);
        ability[12]=new Ability("Berserk", true,false, 4, 7,0,4, 3,1, 0,0,0, -1,0, false, berserk, weakness);
        ability[13]=new Ability("Shock", true,false, 4, 4,0,7, 1,1, 10,5,0, 0,7, false, dizzystun, confclarity);
        ability[14]=new Ability("Crush", true,false, 5, 7,4,0, 0,1, 25,0,2, 0,1, false, stun, confsleep);
        ability[15]=new Ability("Strike", true,true, 1, 0,0,3, 4,1, 13,0,0, 0,1, false, nostate, confsleep);
        ability[16]=new Ability("Weaken", true,true, 1, 0,0,3, 4,1, 3,0,9, 0,1, false, weakness, vigour);
        ability[17]=new Ability("Dash", true,true, 3, 0,0,7, 4,1, 15,0,0, 0,1, false, nostate, confsleep);
        ability[18]=new Ability("Poison", true,true, 3, 0,0,5, 4,2, 5,0,7, 0,1, false, poison, regen);
        ability[19]=new Ability("Pierce", true,true, 4, 0,0,10, 4,2, 15,0,0, 0,1, false, nostate, confsleep);
        ability[20]=new Ability("Toxic Gas", true,true, 4, 0,0,10, 6,3, 1,1,1, 1,1, false, tpoison, tregen);
        ability[21]=new Ability("Cheer", true,true, 4, 0,10,5, 3,-1, 0,0,-5, -2,0, false, vigour, cure);
        ability[22]=new Ability("Venom Blade", true,false, 5, 0,0,10, 4,1, 17,0,3, 0,1, false, poison, conftregen);
        ability[23]=new Ability("Absorb", true,true, 1, 0,0,3, 2,1, 0,7,0, 0,6, true, nostate, nostate);
        ability[24]=new Ability("Drain", true,true, 3, 0,10,0, 2,1, 15,0,0, 0,6, true, nostate, nostate);
        ability[25]=new Ability("Fireball", true,true, 1, 0,3,0, 2,1, 11,0,0, 0,2, false, nostate, sleep);
        ability[26]=new Ability("Iceshard", true,true, 1, 0,3,0, 2,1, 11,0,0, 0,3, false, nostate, sleep);
        ability[27]=new Ability("Lighting", true,true, 1, 0,3,0, 2,1, 11,0,0, 0,4, false, nostate, sleep);
        ability[28]=new Ability("Rock", true,true, 1, 0,3,0, 2,1, 11,0,0, 0,5, false, nostate, sleep);
        ability[29]=new Ability("Darkness", true,true, 1, 0,3,0, 2,1, 11,0,0, 0,6, false, nostate, sleep);
        ability[30]=new Ability("Flame", true,true, 3, 0,5,0, 2,1, 15,0,0, 1,2, false, nostate, sleep);
        ability[31]=new Ability("Blizzard", true,true, 3, 0,5,0, 2,1, 15,0,0, 1,3, false, nostate, sleep);
        ability[32]=new Ability("Storm", true,true, 3, 0,5,0, 2,1, 15,0,0, 1,4, false, nostate, sleep);
        ability[33]=new Ability("Earthquake", true,true, 3, 0,5,0, 2,1, 15,0,0, 1,5, false, nostate, sleep);
        ability[34]=new Ability("Eclipse", true,true, 3, 0,5,0, 2,1, 13,0,0, 1,6, false, nostate, sleep);
        ability[35]=new Ability("Flare", true,true, 5, 0,12,0, 2,2, 25,0,0, 0,1, false, nostate, sleep);
        ability[36]=new Ability("Light Ray", true,true, 1, 0,3,0, 3,1, 11,0,0, 0,7, false, nostate, sleep);
        ability[37]=new Ability("Light Burst", true,true, 3, 0,5,0, 3,1, 15,0,0, 1,7, false, nostate, sleep);
        ability[38]=new Ability("Confusion", true,true, 5, 0,15,0, 2,1, 0,0,0, 0,6, false, confusion, clarity);
        ability[39]=new Ability("Sleep", true,true, 4, 0,0,17, 4,1, 3,0,17, 0,1, false, sleep, clarity);
        ability[40]=new Ability("Slash", true,true, 5, 0,10,10, 4,1, 15,0,0, 1,1, false, nostate, confsleep);
        ability[41]=new Ability("Fire Wpn", true,false, 2, 0,3,2, 5,1, 17,0,0, 0,2, false, nostate, confsleep);
        ability[42]=new Ability("Ice Wpn", true,false, 2, 0,3,2, 5,1, 17,0,0, 0,3, false, nostate, confsleep);
        ability[43]=new Ability("Electric Wpn", true,false, 2, 0,3,2, 5,1, 17,0,0, 0,4, false, nostate, confsleep);
        ability[44]=new Ability("Stone Wpn", true,false, 2, 0,3,2, 5,1, 17,0,0, 0,5, false, nostate, confsleep);
        ability[45]=new Ability("Dark Wpn", true,false, 2, 0,3,2, 5,1, 17,0,0, 0,6, false, nostate, confsleep);
        ability[46]=new Ability("Vampiric Wpn", true,false, 5, 0,10,5, 5,1, 21,0,0, 0,6, true, nostate, confsleep);
        ability[47]=new Ability("Reflect", true,true, 5, 0,10,0, 3,1, 0,0,0, 0,0, false, reflect, nostate);
        ability[48]=new Ability("Meteor", true,true, 5, 0,17,0, 2,1, 19,0,0, 1,1, false, nostate, sleep);
        ability[49]=new Ability("Syphon", true,true, 4, 0,15,0, 2,1, 13,0,3, 1,6, true, nostate, nostate);
        ability[50]=new Ability("Dragon Breath", true,false, 4, 0,13,7, 5,1, 15,0,0, 1,1, false, nostate, confsleep);
        ability[51]=new Ability("Light Wpn", true,false, 2, 0,3,2, 7,1, 17,0,0, 0,7, false, nostate, confsleep);
        ability[52]=new Ability("Heal", false,true, 1, 0,3,0, 2,-1, -25,0,0, 0,7, false, revive, nostate);
        ability[53]=new Ability("Meditate", true,true, 1, 0,0,2, 2,-1, -3,-7,0, -1,7, false, nostate, dizziness);
        ability[54]=new Ability("Cure", false,true, 3, 0,7,0, 2,-1, -17,0,0, 0,7, false, revive, cure);
        ability[55]=new Ability("Clarity", true,true, 3, 0,0,7, 2,-1, 0,-3,0, 0,7, false, clarity, madness);
        ability[56]=new Ability("Absorb", true,true, 1, 0,0,3, 3,1, 0,7,0, 0,7, true, nostate, nostate);
        
        
        return ability;
    }
    
    private Ability[] AddItems()
    {        
        
        Ability item[] = new Ability[9];
    	
    	item[0]=new Ability("Potion", true, -25,0,0, 0,0, nostate, nostate);
        item[1]=new Ability("Ether", true, 0,-10,0, 0,0, nostate, nostate);
        item[2]=new Ability("Tonic", true, 0,0,-10, 0,0, nostate, nostate);
        item[3]=new Ability("Antidote", true, 0,0,0, 0,0, nostate, poison);
        item[4]=new Ability("Hi-Ether", true, 0,-25,0, 0,0, nostate, dizziness);
        item[5]=new Ability("Hi-Tonic", true, 0,-20,0, 0,0, vigour, weakness);
        item[6]=new Ability("Panacea", true, 0,0,0, 0,0, nostate, cure);
        item[7]=new Ability("Elixir", true, -100,-100,-100, 0,0, revive, nostate);
        item[8]=new Ability("Hi-Potion", true, -50,0,0, 0,0, revive, poison);
        
        return item;        
    }
    
    private Actor[][] AddEnemies(Actor[] actors)
    {

		Actor enemy[][]=new Actor[6][4];
    	
    	enemy[0][0]=actors[6];
		enemy[0][1]=actors[4];
		enemy[0][2]=null;
		enemy[0][3]=null;
		
		enemy[1][0]=actors[5];
		enemy[1][1]=actors[7];
		enemy[1][2]=null;
		enemy[1][3]=null;
		
		enemy[2][0]=actors[5];
		enemy[2][1]=actors[4];
		enemy[2][2]=actors[6];
		enemy[2][3]=null;
				
		enemy[3][0]=actors[5];
		enemy[3][1]=actors[7];
		enemy[3][2]=actors[4];
		enemy[3][3]=null;
		
		enemy[4][0]=actors[5];
		enemy[4][1]=actors[6];
		enemy[4][2]=actors[4];
		enemy[4][3]=actors[7];
		
		enemy[5][0]=actors[5];
		enemy[5][1]=actors[4];
		enemy[5][2]=actors[6];
		enemy[5][3]=actors[7];
		
		return enemy;
    }
    
}