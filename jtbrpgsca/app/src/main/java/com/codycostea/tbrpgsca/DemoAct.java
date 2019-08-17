/*
Copyright (C) AD 2017 Claudiu-Stefan Costea

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
package com.codycostea.tbrpgsca;

import java.util.ArrayList;
import java.util.Arrays;

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

import com.codycostea.tbrpgsca.R;

public class DemoAct extends Activity {

	private Performance[] Skill;
	private ArrayList<Performance> Item;
	private RolePlay[] pcRace;
	private Costume[] pcJob;
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

		final StateMask[] state = this.AddStates();
		this.Skill = AddSkills(state);
		this.Item = AddItems(state);
		this.pcRace = AddRaces();
		this.pcJob = AddJobs();
		this.Player = AddCharacters(state);
		this.Enemy = AddEnemies(this.Player);
		this.Party = new Actor[4];

		this.raceList = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item);
		this.jobList = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item);
		this.raceBox[0] = (Spinner) findViewById(R.id.RaceBox1);
		this.raceBox[1] = (Spinner) findViewById(R.id.RaceBox2);
		this.raceBox[2] = (Spinner) findViewById(R.id.RaceBox3);
		for (int i = 0; i < this.pcRace.length; i++)
			this.raceList.add(this.pcRace[i].getRaceName());
		for (int i = 0; i < 3; i++)
			this.raceBox[i].setAdapter(this.raceList);
		this.raceBox[0].setSelection(1);
		this.raceBox[1].setSelection(0);
		this.raceBox[2].setSelection(2);
		this.jobBox[0] = (Spinner) findViewById(R.id.JobBox1);
		this.jobBox[1] = (Spinner) findViewById(R.id.JobBox2);
		this.jobBox[2] = (Spinner) findViewById(R.id.JobBox3);
		for (int i = 0; i < this.pcJob.length; i++)
			this.jobList.add(this.pcJob[i].getJobName());
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
		for (int i = 0; i < this.Item.size(); i++)
			this.Item.get(i).setQty(5);
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
		this.Player[p].setName(n);
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
		for (int i = 1; i < this.Player.length; i++)
			this.Player[i].recover();
		//ArenaAct.PlayDemo(this);
		ArenaAct.InitiateBattle(this, 0, 0, this.Party, this.Enemy[this.level], new Performance[] { this.Skill[0], this.Skill[1] },
				this.Item, null, surprise, this.level % 2 == 0, true);
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
				Bundle extra = data.getExtras();
				if (extra.containsKey("Party")) {
					System.arraycopy(extra.getParcelableArray("Party"), 0, this.Party, 0, 4);
					this.Player[1] = this.Party[0];
					this.Player[2] = this.Party[1];
					this.Player[3] = this.Party[2];
				}
				if (extra.containsKey("Item"))
					this.Item = extra.getParcelableArrayList("Item");
				if (extra.getInt("Outcome") > 0 && DemoAct.this.level < 5)
					DemoAct.this.level++;
			}
		}
	}

	private StateMask[] AddStates()
	{
		StateMask state[] = new StateMask[11];
		state[0] = new StateMask(1, "Regen", false, false, false, -1, 0, 10, 0, 0, 0,
				2, 0, 0, 0, false);
		state[1] = new StateMask(2, "Poison", false, false, false, 10, 0, -7, 0, -2,
				0, -2, 0, 0, 0, false);
		state[2] = new StateMask(3, "Clarity", false, false, false, -1, 0, 0, 7, 0, 0,
				0, 1, 1, 0, false);
		state[3] = new StateMask(4, "Dizziness", false, false, false, 3, 0, 0, -7, 0,
				0, 0, -1, -1, 0, false);
		state[4] = new StateMask(5, "Vigour", false, false, false, -1, 0, 0, 0, 7, 1,
				0, 0, 0, 1, false);
		state[5] = new StateMask(6, "Weakness", false, false, false, 5, 0, 0, 0, -7,
				-1, 0, 0, 0, -1, false);
		state[6] = new StateMask(7, "Berserk", false, true, false, 7, 0, 0, 0, 0, 5,
				-3, 0, 0, 3, false);
		state[7] = new StateMask(8, "Confusion", false, false, true, 3, 2, 0, 0, 0, 0,
				0, 0, 0, 0, false);
		state[8] = new StateMask(9, "Sleep", true, false, false, 5, 1, 0, 0, 0, 0, -3,
				0, 0, -3, false);
		state[9] = new StateMask(10, "Stun", true, false, false, 1, 0, 0, 0, 0, 0, -1,
				0, 0, -1, false);
		state[10] = new StateMask(11, "Reflect", false, false, false, 7, 0,0, 0, 0,
				0, 0, 0, 0, 0, true);
		return state;
	}

    private Costume[] AddRaces()
    {

		Costume[] pcRace = new Costume[4];
    			
    	pcRace[0]=new Costume(1, "Elf", 40,25,10, 7,5,15,12,11, new int[]{0,-1,1,1,1,1});
    	pcRace[2]=new Costume(2, "Human", 47,15,13, 9,11,9,11,10, new int[]{0,0,0,0,0,0,-1,1});
    	pcRace[1]=new Costume(3, "Half-Orc", 55,7,13, 17,12,5,7,9, new int[]{0,1,-1,-1,-1,-1});
    	pcRace[3]=new Costume(4, "Gnome", 40,15,20, 12,8,10,5,15, new int[]{0,0,0,0,0,0,1,-1});
    	return pcRace;
    	
    }
	
	private Costume[] AddJobs()
	{
		
		Costume pcJob[]=new Costume[15];
		
		pcJob[1]=new Costume(1, "Berserker",1,0,0, 1,0,0,0,1, new int[]{0,1,-1,-1,-1,-1}, new Performance[]{this.Skill[8],this.Skill[9],this.Skill[10],this.Skill[11],this.Skill[12],this.Skill[14]});
		pcJob[2]=new Costume(2, "Sorcerer",0,1,0, 0,0,1,0,1, new int[]{0,-1,1,1,1,1,1,-1}, new Performance[]{this.Skill[23],this.Skill[24],this.Skill[25],this.Skill[26],this.Skill[27],this.Skill[28],this.Skill[29],this.Skill[30],this.Skill[31],this.Skill[32],this.Skill[33],this.Skill[34],this.Skill[35],this.Skill[38]});
        pcJob[2].setSprName("Wizard");
        pcJob[3]=new Costume(3, "Monk",0,1,0, 0,1,0,1,0, new int[]{0,0,0,0,0,0,-7,7}, new Performance[]{this.Skill[2],this.Skill[3],this.Skill[4],this.Skill[5],this.Skill[6],this.Skill[7]});
        pcJob[4]=new Costume(4, "Rogue",0,0,1, 1,0,0,0,1, new int[]{}, new Performance[]{this.Skill[15],this.Skill[16],this.Skill[17],this.Skill[18],this.Skill[19],this.Skill[38]});
        pcJob[5]=new Costume(5, "Alchemist",0,1,0, 0,0,1,0,1, new int[]{}, this.Skill, new int[]{23,24,25,26,27,28,30,31,32,33,15,16,17,18,20});
        pcJob[6]=new Costume(6, "Dragoon",1,0,0, 1,0,1,0,0, new int[]{0,0,1,1,1,1,0,-1}, this.Skill, new int[]{8,9,10,11,23,24,25,26,27,28,30,31,32,33,41,42,43,44,50});
        pcJob[7]=new Costume(7, "Knight",1,0,0, 0,1,0,1,0, new int[]{0,1,-1,-1,-1,-1,-7,7}, this.Skill, new int[]{2,3,4,5,8,9,10,11,13});
        pcJob[8]=new Costume(8, "Warden",0,0,1, 0,0,0,1,1, new int[]{0,0,0,0,0,0,-2,2}, this.Skill, new int[]{2,3,4,5,15,16,17,18,21});
        pcJob[8].setSprName("Bard");
        pcJob[9]=new Costume(9, "Shaman",0,1,0, 0,0,1,1,0, new int[]{0,0,0,0,0,0,7,-7}, this.Skill, new int[]{52,53,54,55,23,24,29,34,49});
        pcJob[10]=new Costume(10, "Swashbuckler",0,0,1, 1,0,0,0,1, new int[]{}, this.Skill, new int[]{8,9,10,11,15,16,17,18,22});
        pcJob[11]=new Costume(11, "Reaver",1,0,0, 1,0,0,0,0, new int[]{0,0,0,0,0,0,7,-7}, this.Skill, new int[]{8,9,11,23,24,29,15,16,18,45,46});
        pcJob[12]=new Costume(12, "Ninja",0,0,1, 0,0,0,0,1, new int[]{}, this.Skill, new int[]{8,9,11,15,16,17,2,3,5,40});
        pcJob[13]=new Costume(13, "Crusader",1,0,0, 0,0,0,1,0, new int[]{0,0,0,0,0,0,-7,7}, this.Skill, new int[]{8,9,10,2,3,4,56,36,51,37,47});
        pcJob[13].setSprName("Templar");
        pcJob[14]=new Costume(14, "Druid",0,1,0, 0,0,1,0,0, new int[]{0,0,1,1,1,1,-1,-1}, this.Skill, new int[]{23,25,26,27,28,30,31,32,33,52,53,54,15,16,18,48});
        pcJob[0]=new Costume(15, "Hero",1,1,1, 0,0,0,0,0, new int[]{}, this.Skill, new int[]{8,9,10,11,23,24,25,26,27,28,29,41,42,43,44,45,30,31,32,33,34,36,51,37,2,3,4,5,15,16,17,18});
        	
        return pcJob;
    }
    
    private Actor[] AddCharacters(StateMask[] state)
    {
        
    	int maxlv = 5;
    	
    	Actor npc[] = new Actor[8];
    	
    	npc[0]=new Actor(0, "","","", 0,0, 0,0,0, 0,0,0,0,0, 0,0,0, 0,0,0,0,0);
    	
    	npc[1]=new Actor(1, "Cody",maxlv);
    	npc[2]=new Actor(2, "George",maxlv);
    	npc[3]=new Actor(3, "Stephen",maxlv);

        npc[4]=new Actor(4, "Ogre","Ogre","Ogre", 9,9, 55,7,13, 17,12,5,7,3, 1,1,1, 1,1,0,0,0, new int[]{0,1}, null, this.Skill, new int[]{9,11}, false)
        	.setItems(new ArrayList<Performance>(Arrays.asList(new Performance(this.Item.get(0)).setQty(2), new Performance(this.Item.get(1)).setQty(1))));
        npc[4].setStateRes(state[5], 9).addState(new StateMask(state[10]).setMaxDur(-2));
        npc[5]=new Actor(5, "Lizard", "Lizard", "Lizard", 9, 9, 50,15,10, 13,12,9,7,5, 1,1,1, 1,0,1,0,0, new int[]{0,0,7,1}, null, this.Skill, new int[]{23,30}, false)
        	.setItems(new ArrayList<Performance>(Arrays.asList(new Performance(this.Item.get(4)).setQty(1), new Performance(this.Item.get(8)).setQty(1))));
        npc[6]=new Actor(6, "Goblin", "Goblin", "Goblin", 9, 9, 45,5,20, 13,12,5,5,1, 1,1,1, 1,0,0,0,1, new int[]{}, null, this.Skill, new int[]{15}, false)
        	.setItems(new ArrayList<Performance>(Arrays.asList(new Performance(this.Item.get(3)).setQty(2), new Performance(this.Item.get(6)).setQty(1))));
        npc[7]=new Actor(7, "Troll", "Troll", "Troll", 9, 9, 47,15,15, 13,12,5,10,7, 1,1,1, 0,1,0,1,0, new int[]{}, null, this.Skill, new int[]{2}, false)
        	.setItems(new ArrayList<Performance>(Arrays.asList(new Performance(this.Item.get(7)).setQty(1))));
        npc[7].addState(new StateMask(state[0]).setMaxDur(-2));

        return npc;
    }
    
    private Performance[] AddSkills(StateMask[] state)
    {

		final StateMask[] confsleep = new StateMask[] { state[7], state[8] };
		final StateMask[] cure = new StateMask[] { state[1], state[3], state[5], state[6], state[7], state[8], state[9] };
		final StateMask[] dizziness = new StateMask[] { state[3] };
		final StateMask[] poison = new StateMask[] { state[1] };
		final StateMask[] regen = new StateMask[] { state[0] };
		final StateMask[] madness = new StateMask[] { state[3], state[6], state[7], state[8], state[9] };
		final StateMask[] clarity = new StateMask[] { state[2] };
		final StateMask[] weakness = new StateMask[] { state[5] };
		final StateMask[] vigour = new StateMask[] { state[4] };
		final StateMask[] tregen = new StateMask[] { state[0], state[2], state[4] };
		final StateMask[] berserk = new StateMask[] { state[6] };
		final StateMask[] stun = new StateMask[] { state[9] };
		final StateMask[] reflect = new StateMask[] { state[10] };
		final StateMask[] confusion = new StateMask[] { state[7] };
		final StateMask[] confclarity = new StateMask[] { state[2], state[7], state[8] };
		final StateMask[] conftregen = new StateMask[] { state[0], state[2], state[4], state[7], state[8] };
		final StateMask[] sleep = new StateMask[] { state[8] };
		final StateMask[] tpoison = new StateMask[] { state[1], state[3], state[5] };
		final StateMask[] dizzystun = new StateMask[] { state[3], state[9] };

        Performance ability[] = new Performance[57];
    	
    	ability[0]=new Performance(1, "Attack", 0, 0, false,false, 0, 0,0,0, 0,1, 10,0,-3, 0,1, false, null, confsleep);
        ability[1]=new Performance(2, "Defend", 0, 0, false,true, 0, 0,0,0, 3,1, 0,-1,-2, -1,0, false, null, null);
        ability[2]=new Performance(3, "Heal", 0, 0, false,true, 1, 0,3,0, 3,1, -25,0,0, 0,6, -1,-1, false, true, null, null);
        ability[3]=new Performance(4, "Meditate", 0, 0, false,true, 1, 0,0,0, 3,1, -1,-7,10, -1,6, false, null, dizziness);
        ability[4]=new Performance(5, "Cure", 0, 0, false,true, 3, 0,7,0, 3,1, -17,0,0, 0,7, -1, -1, false, true, null, cure);
        ability[5]=new Performance(6, "Clarity", 0, 0, false,true, 3, 0,0,7, 3,1, 0,-3,0, 0,6, false, clarity, madness);
        ability[6]=new Performance(7, "Regen", 0, 0, false,true, 4, 0,10,0, 3,1, -37,0,0, 0,6, -1, -1, false, true, regen, poison);
        ability[7]=new Performance(8, "Prayer", 0, 0, false,true, 5, 0,7,0, 3,1, -23,0,0, 1,0, -1, -1, false, true, null, null);
        ability[8]=new Performance(9, "Smite", 0, 0, false,false, 1, 1,0,2, 1,1, 4,3,-4, 0,1, false, null, confclarity);
        ability[9]=new Performance(10, "Hit", 0, 0, false,false, 1, 3,0,1, 0,1, 12,0,-4, 0,1, false, null, confsleep);
        ability[10]=new Performance(11, "Bash", 0, 0, false,false, 3, 3,0,5, 1,1, 7,5,-5, 0,1, false, dizziness, confclarity);
        ability[11]=new Performance(12, "Smash", 0, 0, false,false, 3, 5,0,3, 0,1, 18,0,-7, 0,1, false, null, confsleep);
        ability[12]=new Performance(13, "Berserk", 0, 0, false,false, 4, 7,0,4, 3,1, 0,0,0, -1,0, false, berserk, weakness);
        ability[13]=new Performance(14, "Shock", 0, 0, false,false, 4, 4,0,7, 1,1, 10,5,-9, 0,7, false, dizzystun, confclarity);
        ability[14]=new Performance(15, "Crush", 0, 0, false,false, 5, 7,4,0, 0,1, 25,0,-11, 0,1, false, stun, confsleep);
        ability[15]=new Performance(16, "Strike", 0, 0, false,true, 1, 0,0,0, 4,1, 13,0,-3, 0,1, 5,3, false, false, null, confsleep);
        ability[16]=new Performance(17, "Steal", 0, 0, true,true, 1, 0,0,0, 3,1, 0,0,0, 0,1, 5,3, false, false, null, null);
        ability[17]=new Performance(18, "Dash", 0, 0, false,true, 3, 0,0,0, 4,1, 15,0,-5, 0,1, 3,7, false, false, null, confsleep);
        ability[18]=new Performance(19, "Poison", 0, 0, false,true, 3, 0,0,0, 4,2, 5,0,2, 0,1, 3,7, false, false, poison, regen);
        ability[19]=new Performance(20, "Mug", 0, 0, true,true, 4, 0,0,0, 4,2, 15,0,-7, 0,1, 1,5, false, false, null, confsleep);
        ability[20]=new Performance(21, "Toxic Gas", 0, 0, false,true, 4, 0,10,0, 6,3, 1,1,1, 1,1, 3,7, false, false, tpoison, tregen);
        ability[21]=new Performance(22, "Cheer", 0, 0, false,true, 4, 0,10,0, 3,-1, 0,0,-5, -2,0, 3,7, false, false, vigour, cure);
        ability[22]=new Performance(23, "Venom Blade", 0, 0, false,false, 5, 0,0,10, 4,1, 17,0,-9, 0,1, 3,7, false, false, poison, conftregen);
        ability[23]=new Performance(24, "Absorb", 0, 0, false,true, 1, 0,0,3, 2,1, 0,7,-3, 0,6, true, null, null);
        ability[24]=new Performance(25, "Drain", 0, 0, false,true, 3, 0,10,0, 2,1, 15,0,-3, 0,6, true, null, null);
        ability[25]=new Performance(26, "Fireball", 0, 0, false,true, 1, 0,3,0, 2,1, 11,0,-5, 0,2, false, null, sleep);
        ability[26]=new Performance(27, "Iceshard", 0, 0, false,true, 1, 0,3,0, 2,1, 11,0,-5, 0,3, false, null, sleep);
        ability[27]=new Performance(28, "Lighting", 0, 0, false,true, 1, 0,3,0, 2,1, 11,0,-5, 0,4, false, null, sleep);
        ability[28]=new Performance(29, "Rock", 0, 0, false,true, 1, 0,3,0, 2,1, 11,0,-5, 0,5, false, null, sleep);
        ability[29]=new Performance(30, "Darkness", 0, 0, false,true, 1, 0,3,0, 2,1, 11,0,-5, 0,6, false, null, sleep);
        ability[30]=new Performance(31, "Flame", 0, 0, false,true, 3, 0,5,0, 2,1, 15,0,-7, 1,2, false, null, sleep);
        ability[31]=new Performance(32, "Blizzard", 0, 0, false,true, 3, 0,5,0, 2,1, 15,0,-7, 1,3, false, null, sleep);
        ability[32]=new Performance(33, "Storm", 0, 0, false,true, 3, 0,5,0, 2,1, 15,0,-7, 1,4, false, null, sleep);
        ability[33]=new Performance(34, "Earthquake", 0, 0, false,true, 3, 0,5,0, 2,1, 15,0,-7, 1,5, false, null, sleep);
        ability[34]=new Performance(35, "Eclipse", 0, 0, false,true, 3, 0,5,0, 2,1, 13,0,-7, 1,6, false, null, sleep);
        ability[35]=new Performance(36, "Flare", 0, 0, false,true, 5, 0,12,0, 2,2, 25,0,-9, 0,1, false, null, sleep);
        ability[36]=new Performance(37, "Light Ray", 0, 0, false,true, 1, 0,3,0, 3,1, 11,0,-5, 0,7, false, null, sleep);
        ability[37]=new Performance(38, "Light Burst", 0, 0, false,true, 3, 0,5,0, 3,1, 15,0,-7, 1,7, false, null, sleep);
        ability[38]=new Performance(39, "Confusion", 0, 0, false,true, 5, 0,15,0, 2,1, 0,0,0, 0,6, false, confusion, clarity);
        ability[39]=new Performance(40, "Sleep", 0, 0, false,true, 4, 0,0,17, 4,1, 3,0,17, 0,1, false, sleep, clarity);
        ability[40]=new Performance(41, "Slash", 0, 0, false,true, 5, 0,10,0, 4,1, 15,0,0, 1,1, 3,7, false, false, null, confsleep);
        ability[41]=new Performance(42, "Fire Wpn", 0, 0, false,false, 2, 0,3,2, 5,1, 17,0,-7, 0,2, false, null, confsleep);
        ability[42]=new Performance(43, "Ice Wpn", 0, 0, false,false, 2, 0,3,2, 5,1, 17,0,-7, 0,3, false, null, confsleep);
        ability[43]=new Performance(44, "Electric Wpn", 0, 0, false,false, 2, 0,3,2, 5,1, 17,0,-7, 0,4, false, null, confsleep);
        ability[44]=new Performance(45, "Stone Wpn", 0, 0, false,false, 2, 0,3,2, 5,1, 17,0,-7, 0,5, false, null, confsleep);
        ability[45]=new Performance(46, "Dark Wpn", 0, 0, false,false, 2, 0,3,2, 5,1, 17,0,-7, 0,6, false, null, confsleep);
        ability[46]=new Performance(47, "Vampiric Wpn", 0, 0, false,false, 5, 0,10,0, 5,1, 21,0,-9, 0,6, 3,7, true, false, null, confsleep);
        ability[47]=new Performance(48, "Reflect", 0, 0, false,true, 5, 0,10,0, 3,1, 0,0,0, 0,0, false, reflect, null);
        ability[48]=new Performance(49, "Meteor", 0, 0, false,true, 5, 0,17,0, 2,1, 19,0,-11, 1,1, false, null, sleep);
        ability[49]=new Performance(50, "Syphon", 0, 0, false,true, 4, 0,15,0, 2,1, 13,0,-3, 1,6, true, null, null);
        ability[50]=new Performance(51, "Dragon Breath", 0, 0, false,false, 4, 0,13,7, 5,1, 15,0,-11, 1,1, false, null, confsleep);
        ability[51]=new Performance(52, "Light Wpn", 0, 0, false,false, 2, 0,3,2, 7,1, 17,0,-5, 0,7, false, null, confsleep);
        ability[52]=new Performance(53, "Heal", 0, 0, false,true, 1, 0,3,0, 2,1, -25,0,0, 0,7, -1,-1, false, true, null, null);
        ability[53]=new Performance(54, "Meditate", 0, 0, false,true, 1, 0,0,2, 2,1, -3,-7,0, -1,7, false, null, dizziness);
        ability[54]=new Performance(55, "Cure", 0, 0, false,true, 3, 0,7,0, 2,1, -17,0,0, 0,7, -1,-1, false, true, null, cure);
        ability[55]=new Performance(56, "Clarity", 0, 0, false,true, 3, 0,0,7, 2,1, 0,-3,0, 0,7, false, clarity, madness);
        ability[56]=new Performance(57, "Absorb", 0, 0, false,true, 1, 0,0,3, 3,1, 0,7,-3, 0,7, true, null, null);
        
        return ability;
    }
    
    private ArrayList<Performance> AddItems(StateMask state[])
    {        
        
        ArrayList<Performance> item = new ArrayList<Performance>(9);
    	item.add(new Performance(1, "Potion", 0, 0, true, -25,0,0, 0,0, false, null, null));
        item.add(new Performance(2, "Ether", 0, 0, true, 0,-10,0, 0,0, false, null, null));
        item.add(new Performance(3, "Tonic", 0, 0, true, 0,0,-10, 0,0, false, null, null));
        item.add(new Performance(4, "Antidote", 0, 0, true, 0,0,0, 0,0, false, null, new StateMask[] { state[1] }));
        item.add(new Performance(5, "Hi-Ether", 0, 0, true, 0,-25,0, 0,0, false, null, new StateMask[] { state[3] }));
        item.add(new Performance(6, "Hi-Tonic", 0, 0, true, 0,-20,0, 0,0, false, new StateMask[] { state[4] }, new StateMask[] { state[5] }));
        item.add(new Performance(7, "Panacea", 0, 0, true, 0,0,0, 0,0, false, null, new StateMask[] { state[1], state[3], state[5], state[6], state[7], state[8], state[9] }));
        item.add(new Performance(8, "Elixir", 0, 0, true, -100,-100,-100, 0,0, true, null, null));
        item.add(new Performance(9, "Hi-Potion", 0, 0, true, -50,0,0, 0,0, true, null, null));
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