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
import org.mozilla.javascript.Context;
import org.mozilla.javascript.Scriptable;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.drawable.AnimationDrawable;
import android.os.Parcelable;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.Spinner;
import android.widget.TextView;

public class ArenaAct extends Activity {
	private static Parcelable[] cachedParty = null, cachedEnemy = null;
	private static Performance[] cachedSkill = null;
	private static ArrayList<Performance> cachedItem = null;
	private static String[] cachedScript = null;

	private Performance[] Skill;
	private ArrayList<Performance> Item;
	private Actor[] Battler;

	private String[] jScript;
	private Context jsContext;
	private Scriptable jsScope;

	private int result = 0, surprise = 0, target, current = 7, difference = 0,
			waitTime[] = { 0, 0, 0 };

	private Spinner targetBox, skillBox, itemBox;
	private ArrayAdapter<String> targetList, skillList, itemList;

	private TextView skillCost, updText;
	private Button Act, Use, Auto, Escape;
	private ImageView imgActor[] = new ImageView[8];

	private ArrayList<Performance> cSkills = new ArrayList<Performance>(),
			crItems = new ArrayList<Performance>();

	//private final PorterDuffColorFilter crColor = new PorterDuffColorFilter(Color.YELLOW, PorterDuff.Mode.MULTIPLY);

	private MediaPlayer songPlayer;

	private OnClickListener cAction = new OnClickListener() {
		@Override
		public void onClick(final View v) {
			ArenaAct.this.waitTime[0] = 0;
			boolean act = true;
			boolean surprised = ArenaAct.this.surprise < 0;
			ArenaAct.this.target = ArenaAct.this.targetBox
					.getSelectedItemPosition();
			if ((ArenaAct.this.target > 2 && ArenaAct.this.difference > 0)
					|| (ArenaAct.this.target > 0 && ArenaAct.this.difference > 2)
					|| (ArenaAct.this.target > 1 && ArenaAct.this.difference > 1))
				ArenaAct.this.target += ArenaAct.this.difference;
			switch (v.getId()) {
			case R.id.ActBt:
				ArenaAct.this.executeSkill();
				break;
			case R.id.UseBt:
				ArenaAct.this.useItem();
				break;
			case R.id.AutoBt:
				ArenaAct.this.executeAI();
				break;
			case R.id.RunBt:
				ArenaAct.this.endBt(0);
				break;
			case R.id.ImgPlayer1:
				act = (surprised) ? ArenaAct.this.imgClick(6) : ArenaAct.this
						.imgClick(0);
				break;
			case R.id.ImgPlayer3:
				act = (surprised) ? ArenaAct.this.imgClick(7) : ArenaAct.this
						.imgClick(1);
				break;
			case R.id.ImgPlayer2:
				act = (surprised) ? ArenaAct.this.imgClick(4) : ArenaAct.this
						.imgClick(2);
				break;
			case R.id.ImgPlayer4:
				act = (surprised) ? ArenaAct.this.imgClick(5) : ArenaAct.this
						.imgClick(3);
				break;
			case R.id.ImgEnemy3:
				act = (surprised) ? ArenaAct.this.imgClick(1) : ArenaAct.this
						.imgClick(4);
				break;
			case R.id.ImgEnemy1:
				act = (surprised) ? ArenaAct.this.imgClick(0) : ArenaAct.this
						.imgClick(5);
				break;
			case R.id.ImgEnemy4:
				act = (surprised) ? ArenaAct.this.imgClick(3) : ArenaAct.this
						.imgClick(6);
				break;
			case R.id.ImgEnemy2:
				act = (surprised) ? ArenaAct.this.imgClick(2) : ArenaAct.this
						.imgClick(7);
				break;
			}
			if (act)
				ArenaAct.this.endTurn();
		}
	};

	public static void PlayDemo(Activity act) {
		ArenaAct.InitiateBattle(act, 0, 0, null, null, null, null, null, 0, true,
				false);
	}

	public static void InitiateBattle(Activity act, int arenaDrawableId, int songId,
									  Actor[] party, Actor[] enemy, Performance[] skills,
									  ArrayList<Performance> items, String[] scripts,
									  int surprise, boolean escapable, boolean staticCache) {
		if (staticCache) {
			ArenaAct.cachedParty = party;
			ArenaAct.cachedEnemy = enemy;
			ArenaAct.cachedSkill = skills;
			ArenaAct.cachedItem = items;
			ArenaAct.cachedScript = scripts;
		}
		Intent btInt = new Intent(act, ArenaAct.class);
		btInt.putExtra("Escape", escapable);
		btInt.putExtra("Surprise", surprise);
		btInt.putExtra("Party", party);
		btInt.putExtra("Enemy", enemy);
		btInt.putExtra("Skill", skills);
		btInt.putParcelableArrayListExtra("Item", items);
		btInt.putExtra("Script", scripts);
		btInt.putExtra("Arena", arenaDrawableId);
		btInt.putExtra("Song", songId);
		act.startActivityForResult(btInt, 1);
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		Bundle extra = this.getIntent().getExtras();
		if (extra.containsKey("Surprise"))
			this.surprise = extra.getInt("Surprise");
		if (ArenaAct.cachedItem != null)
			this.Item = ArenaAct.cachedItem;
		else if (extra.containsKey("Item"))
			this.Item = extra.getParcelableArrayList("Item");
		if (ArenaAct.cachedScript != null)
			this.jScript = ArenaAct.cachedScript;
		else if (extra.containsKey("Script"))
			this.jScript = extra.getStringArray("Script");
		Parcelable[] party = null;
		Parcelable[] enemy = null;
		if (ArenaAct.cachedParty != null && ArenaAct.cachedEnemy != null) {
			party = ArenaAct.cachedParty;
			enemy = ArenaAct.cachedEnemy;
		} else if (extra.containsKey("Party") && extra.containsKey("Enemy")) {
			party = extra.getParcelableArray("Party");
			enemy = extra.getParcelableArray("Enemy");
		}
		if (party != null && enemy != null) {
			ArenaAct.this.Battler = new Actor[8];
			for (int i = 0; i < party.length && i < 4; i++)
				if (party[i] != null) {
					ArenaAct.this.Battler[i] = (Actor) party[i];
					if (this.Item != null)
						this.Battler[i].items = this.Item;
				}
			for (int k = 0; k < enemy.length && k < 4; k++)
				if (enemy[k] != null) {
					ArenaAct.this.Battler[k + 4] = ArenaAct.cachedEnemy == null ? (Actor) enemy[k]
							: new Actor((Actor) enemy[k]);
					ArenaAct.this.Battler[k + 4].auto = 2;
				}
		}
		if (ArenaAct.cachedSkill != null)
			this.Skill = ArenaAct.cachedSkill;
		else if (extra.containsKey("Skill")) {
			Parcelable[] skill = extra.getParcelableArray("Skill");
			if (skill != null) {
				this.Skill = new Performance[skill.length];
				System.arraycopy(skill, 0, this.Skill, 0, skill.length);
			}
		}
		if (ArenaAct.this.Skill == null) {
			ArenaAct.this.Skill = new Performance[] {
					new Performance(0, "Attack", 0, 0, false, false, 0, 0, 0, 0, 0, 1,
							10, 0, -3, 0, 1, false, null, null),
					new Performance(0, "Defend", 0, 0, false, true, 0, 0, 0, 0, 3, -1, 0,
							-2, -1, -1, 0, false, null, null) };
		}
		Actor.State regen = new Actor.State(1, "Regen", false, false, false, -1, 0, 10, 0, 0, 0,
				2, 0, 0, 0, false);
		Actor.State poison = new Actor.State(2, "Poison", false, false, false, 10, 0, -7, 0, -2,
				0, -2, 0, 0, 0, false);
		Actor.State dizziness = new Actor.State(4, "Dizziness", false, false, false, 3, 0, 0, -7, 0,
				0, 0, -1, -1, 0, false);
		if (ArenaAct.this.Battler == null) {
			ArrayList<Performance> items = new ArrayList<Performance>(
					Arrays.asList(new Performance(1, "Potion", 0, 0, false, -25, 0, 0, 0,
							0, false, null, null).setQty(10)));
			ArenaAct.this.Battler = new Actor[] {
					new Actor(0, "Cody", "Half-Elf", "Hero", 1, 3, 50, 10, 15,
							13, 12, 9, 7, 5, 1, 1, 1, 1, 0, 1, 0, 0,
							new int[] {}, items, new Performance[] { new Performance(0,
									"Steal", 0, 0, true, true, 2, 0, 0, 0, 3, 0, 0,
									0, 0, 0, 1, 5, 3, false, false, null, null) }, null),
					new Actor(0, "Stephanie", "Human", "Sorceress", 1, 3, 45,
							20, 5, 13, 12, 5, 5, 1, 1, 1, 1, 1, 0, 0, 0, 1,
							new int[] {}, items, new Performance[] { new Performance(0,
									"Iceshard", 0, 0, false, true, 2, 0, 7, 0, 2, 1,
									15, 0, 0, 0, 3, false, null,null) }, null),
					new Actor(0, "George", "Half-Orc", "Templar", 1, 3, 47, 15,
							15, 13, 12, 5, 10, 7, 1, 1, 1, 0, 1, 0, 1, 0,
							new int[] {}, items, new Performance[] { new Performance(0,
									"Regen", 0, 0, false, true, 2, 0, 7, 0, 3, 1,
									-25, 0, 0, 0, 6, -1,-1, false, true, new Actor.State[] { regen }, new Actor.State[] { poison, dizziness }) }, false),
					new Actor(0, "Victoria", "Elf", "Valkyrie", 1, 3, 55, 7,
							13, 17, 12, 5, 7, 3, 1, 1, 1, 1, 1, 0, 0, 0,
							new int[] {}, items, new Performance[] { new Performance(0,
									"Bash", 0, 0, false, false, 2, 0, 0, 5, 1, 1, 12,
									0, -5, 0, 1, false, new Actor.State[] { dizziness }, null) }, null),
					new Actor(0, "Lizard", "Lizard", "Lizard", 3, 3, 50, 15,
							10, 13, 12, 9, 7, 5, 1, 1, 1, 1, 0, 1, 0, 0,
							new int[] {}, new ArrayList<Performance>(Arrays.asList(new Performance(
									new Performance(4, "Ether", 0, 0, true, 0, -10, 0, 0,
											0, false, null,null)).setQty(2))),
							new Performance[] { new Performance(0, "Fireball", 0, 0, false,
									true, 2, 0, 7, 0, 2, 1, 15, 0, 0, 0, 2,
									false, null, null) }, null),
					new Actor(0, "Goblin", "Goblin", "Goblin", 3, 3, 45, 5, 20,
							13, 12, 5, 5, 1, 1, 1, 1, 1, 0, 0, 0, 1,
							new int[] {}, new ArrayList<Performance>(
									Arrays.asList(new Performance(new Performance(2,
											"Antidote", 0, 0, true, 0, 0, 0, 0, 0, false,
											null, new Actor.State[] { poison }))
											.setQty(3))),
							new Performance[] { new Performance(0, "Poison", 0, 0, false,
									true, 2, 0, 0, 5, 4, 2, 3, 0, 9, 0, 1, 3,
									7, false, false, new Actor.State[] { poison },
									new Actor.State[] { regen }) }, null),
					new Actor(0, "Ogre", "Ogre", "Ogre", 3, 3, 55, 7, 13, 17,
							12, 5, 7, 3, 1, 1, 1, 1, 1, 0, 0, 0, new int[] {},
							new ArrayList<Performance>(Arrays.asList(new Performance(
									items.get(0)).setQty(2))),
							new Performance[] { new Performance(0, "Smash", 0, 0, false,
									false, 2, 2, 0, 3, 0, 1, 12, 0, -7, 0, 1,
									false, new Actor.State[] { dizziness }, null) }, null),
					new Actor(0, "Troll", "Troll", "Troll", 3, 3, 47, 15, 15,
							13, 12, 5, 10, 7, 1, 1, 1, 0, 1, 0, 1, 0,
							new int[] {}, new ArrayList<Performance>(
									Arrays.asList(new Performance(new Performance(3,
											"Elixir", 0, 0, true, -100, -100, -100,
											0, 0, true, null, null)).setQty(1))),
							new Performance[] { new Performance(0, "Heal", 0, 0, false, true,
									2, 0, 7, 0, 3, 1, -25, 0, 0, 0, 6, -1, -1, false,
									true, null, new Actor.State[] { dizziness }) }, null) };
			for (int i = 4; i < 8; i++)
				ArenaAct.this.Battler[i].auto = 2;
			ArenaAct.this.surprise = 0;
		}
		ArenaAct.this.result = 0;
		int songId = extra.getInt("Song", 0);
		if (songId != 0 && this.songPlayer == null) {
			songPlayer = MediaPlayer.create(this, songId);
			songPlayer.setLooping(true);
			songPlayer.start();
		}
		this.setContentView(R.layout.activity_battle);
		int arenaId = extra.getInt("Arena", 0);
		if (arenaId != 0)
			((ImageView)this.findViewById(R.id.ImgArena)).setBackgroundDrawable(this.getResources().getDrawable(arenaId));
		this.targetBox = (Spinner) this.findViewById(R.id.TargetBox);
		this.skillBox = (Spinner) this.findViewById(R.id.SkillBox);
		this.itemBox = (Spinner) this.findViewById(R.id.ItemBox);
		this.skillCost = (TextView) this.findViewById(R.id.SkillCost);
		this.updText = (TextView) this.findViewById(R.id.ItemCost);
		this.Act = (Button) this.findViewById(R.id.ActBt);
		this.Use = (Button) this.findViewById(R.id.UseBt);
		this.Escape = (Button) this.findViewById(R.id.RunBt);
		this.Auto = (Button) this.findViewById(R.id.AutoBt);
		this.skillList = new ArrayAdapter<String>(this,
				android.R.layout.simple_spinner_item);
		this.itemList = new ArrayAdapter<String>(this,
				android.R.layout.simple_spinner_item);
		this.targetList = new ArrayAdapter<String>(this,
				android.R.layout.simple_spinner_item);
		this.targetBox.setAdapter(this.targetList);
		this.skillBox.setAdapter(this.skillList);
		this.itemBox.setAdapter(this.itemList);
		this.Act.setOnClickListener(this.cAction);
		this.Use.setOnClickListener(this.cAction);
		this.Escape.setOnClickListener(this.cAction);
		this.Auto.setOnClickListener(this.cAction);
		this.updText.setMovementMethod(new ScrollingMovementMethod());
		this.Escape
				.setEnabled((extra != null && extra.containsKey("Escape")) ? extra
						.getBoolean("Escape") : true);
		if (ArenaAct.this.surprise < 0) {
			this.imgActor[0] = (ImageView) findViewById(R.id.ImgEnemy1);
			this.imgActor[1] = (ImageView) findViewById(R.id.ImgEnemy3);
			this.imgActor[2] = (ImageView) findViewById(R.id.ImgEnemy2);
			this.imgActor[3] = (ImageView) findViewById(R.id.ImgEnemy4);
			this.imgActor[4] = (ImageView) findViewById(R.id.ImgPlayer2);
			this.imgActor[5] = (ImageView) findViewById(R.id.ImgPlayer4);
			this.imgActor[6] = (ImageView) findViewById(R.id.ImgPlayer1);
			this.imgActor[7] = (ImageView) findViewById(R.id.ImgPlayer3);
		} else {
			this.imgActor[0] = (ImageView) findViewById(R.id.ImgPlayer1);
			this.imgActor[1] = (ImageView) findViewById(R.id.ImgPlayer3);
			this.imgActor[2] = (ImageView) findViewById(R.id.ImgPlayer2);
			this.imgActor[3] = (ImageView) findViewById(R.id.ImgPlayer4);
			this.imgActor[4] = (ImageView) findViewById(R.id.ImgEnemy3);
			this.imgActor[5] = (ImageView) findViewById(R.id.ImgEnemy1);
			this.imgActor[6] = (ImageView) findViewById(R.id.ImgEnemy4);
			this.imgActor[7] = (ImageView) findViewById(R.id.ImgEnemy2);
		}
		for (int i = 0; i < this.imgActor.length; i++) {
			this.imgActor[i].setOnClickListener(this.cAction);
			this.imgActor[i].setTag(0);
			if (ArenaAct.this.Battler[i] != null
					&& ArenaAct.this.Battler[i].maxhp > 0)
				this.playSpr(i, ArenaAct.this.Battler[i].hp > 0 ? 0 : -1, null, false);
			else if (i < 4)
				this.difference++;
		}
		if (ArenaAct.this.surprise < 0) {
			for (int i = 0; i < (ArenaAct.this.Battler.length / 2)
					- this.difference; i++)
				if (ArenaAct.this.Battler[i] != null)
					ArenaAct.this.Battler[i].active = false;
			this.endTurn();
		} else if (ArenaAct.this.surprise > 0) {
			for (int i = (ArenaAct.this.Battler.length / 2); i < ArenaAct.this.Battler.length; i++)
				if (ArenaAct.this.Battler[i] != null)
					ArenaAct.this.Battler[i].active = false;
			this.endTurn();
		} else
			this.endTurn();
		if (this.jScript != null) {
			this.jsContext = Context.enter();
			this.jsContext.setOptimizationLevel(-1);
			this.jsScope = this.jsContext.initStandardObjects();
			this.jsScope.put("Scene", this.jsScope,
					Context.javaToJS(this, this.jsScope));
			this.jsScope.put("Battler", this.jsScope,
					Context.javaToJS(this.Battler, this.jsScope));
			this.jsScope.put("Skill", this.jsScope,
					Context.javaToJS(this.Skill, this.jsScope));
			this.jsScope.put("Item", this.jsScope,
					Context.javaToJS(this.Item, this.jsScope));
			if (this.jScript.length > 0 && this.jScript[0] != null)
				try {
					this.jsContext.evaluateString(this.jsScope,
							this.jScript[0], "OnStart", 1, null);
				} catch (Exception ex) {
					Log.e("Rhino JS error", ex.getMessage());
				}
		}
		this.refreshTargetBox();
		this.targetBox.setSelection((ArenaAct.this.Battler.length / 2)
				- this.difference);
		this.refreshSkillBox();
		this.refreshItemBox();
	}

	private void refreshTargetBox() {
		this.targetList.clear();
		String s;
		for (int i = 0; i < ArenaAct.this.Battler.length; i++) {
			if (ArenaAct.this.Battler[i] != null
					&& ArenaAct.this.Battler[i].maxhp > 0) {
				s = ArenaAct.this.Battler[i].name + " (HP: "
						+ ArenaAct.this.Battler[i].hp;
				if (i < (ArenaAct.this.Battler.length / 2) - this.difference)
					s += ", MP: " + ArenaAct.this.Battler[i].mp + ", RP: "
							+ ArenaAct.this.Battler[i].sp;
				s += ")";
				this.targetList.add(s);
			}
		}
		this.targetList.notifyDataSetChanged();
	}

	private void refreshSkillBox() {
		this.skillList.clear();
		this.cSkills.clear();
		for (int i = 0; i < ArenaAct.this.Skill.length; i++)
			this.addAbility(ArenaAct.this.Skill[i], this.skillList,
					this.cSkills);
		if (ArenaAct.this.Battler[this.current].raceSkills != null)
			for (int i = 0; i < Battler[this.current].raceSkills.length; i++)
				this.addAbility(
						ArenaAct.this.Battler[this.current].raceSkills[i],
						this.skillList, this.cSkills);
		if (ArenaAct.this.Battler[this.current].jobSkills != null)
			for (int i = 0; i < ArenaAct.this.Battler[this.current].jobSkills
					.size(); i++)
				this.addAbility(
						ArenaAct.this.Battler[this.current].jobSkills.get(i),
						this.skillList, this.cSkills);
		this.skillList.notifyDataSetChanged();
		if (this.skillBox.getSelectedItemPosition() > 1)
			this.skillBox.setSelection(0);
	}

	private void addAbility(Performance ability, ArrayAdapter<String> abilityList,
							ArrayList<Performance> abilities) {
		String trg;
		ability.checkQty();
		if (!(ability.getLvRq() > ArenaAct.this.Battler[this.current].level)
				&& !(ability.mqty > -1 && ability.qty < 1)
				&& !(ability.mpc > ArenaAct.this.Battler[this.current].mp)
				&& !(ability.spc > ArenaAct.this.Battler[this.current].sp)
				&& !(ability.hpc >= ArenaAct.this.Battler[this.current].hp)) {
			if (ability.trg == 0)
				trg = "One";
			else
				trg = (ability.trg > 0) ? "All" : "Self";
			abilities.add(ability);
			abilityList
					.add(ability.name
							+ " ("
							+ (ability.qty > 0 ? "Qty:" + ability.qty + ";"
									: "")
							+ (ability.hpc > 0 || ability.mpc > 0
									|| ability.spc > 0 ? "Rq:"
									+ (ability.hpc > 0 ? (ability.hpc + "HP")
											: "")
									+ (ability.mpc > 0 ? ((ability.hpc > 0 ? ","
											: "") + (ability.mpc + "MP"))
											: "")
									+ (ability.spc > 0 ? (ability.hpc > 0
											|| ability.mpc > 0 ? "," : "")
											+ (ability.spc + "RP") : "") + ";"
									: "") + "Trg:" + trg + ")");
		}
	}

	private void refreshItemBox() {
		this.itemList.clear();
		this.crItems.clear();
		if (ArenaAct.this.Battler[this.current].items != null)
			for (int i = 0; i < ArenaAct.this.Battler[this.current].items
					.size(); i++)
				this.addAbility(
						ArenaAct.this.Battler[this.current].items.get(i),
						this.itemList, this.crItems);
		if (this.itemList.getCount() == 0) {
			this.Use.setEnabled(false);
			this.itemBox.setEnabled(false);
		} else {
			this.Use.setEnabled(true);
			this.itemBox.setEnabled(true);
		}
		this.itemList.notifyDataSetChanged();
	}

	private void executeSkill() {
		this.executeSkill(this.cSkills.get(this.skillBox
				.getSelectedItemPosition()));
	}

	private void executeSkill(Performance ability) {
		if (this.target == 1
				&& (ArenaAct.this.Battler[3] != null && ArenaAct.this.Battler[3].canGuard)
				&& ((ArenaAct.this.Battler[0] != null && ArenaAct.this.Battler[0].canGuard) || (ArenaAct.this.Battler[2] != null && ArenaAct.this.Battler[2].canGuard))
				&& this.current > (ArenaAct.this.Battler.length / 2) - 1
				&& this.difference == 0 && !ability.range)
			this.target = 3;
		if (this.target == 2
				&& (ArenaAct.this.Battler[0] != null && ArenaAct.this.Battler[0].canGuard)
				&& ((ArenaAct.this.Battler[1] != null && ArenaAct.this.Battler[1].canGuard) || (ArenaAct.this.Battler[3] != null && ArenaAct.this.Battler[3].canGuard))
				&& this.current > (ArenaAct.this.Battler.length / 2) - 1
				&& this.difference < 2 && !ability.range)
			this.target = 0;
		if (this.target == 4
				&& (ArenaAct.this.Battler[6] != null && ArenaAct.this.Battler[6].canGuard)
				&& ((ArenaAct.this.Battler[5] != null && ArenaAct.this.Battler[5].canGuard) || (ArenaAct.this.Battler[7] != null && ArenaAct.this.Battler[7].canGuard))
				&& !ability.range)
			this.target = 6;
		if (this.target == 7
				&& ((ArenaAct.this.Battler[6] != null && ArenaAct.this.Battler[6].canGuard) || (ArenaAct.this.Battler[4] != null && ArenaAct.this.Battler[4].canGuard))
				&& (ArenaAct.this.Battler[5] != null && ArenaAct.this.Battler[5].canGuard)
				&& !ability.range)
			this.target = 5;
		while ((ArenaAct.this.Battler[this.target] != null && ArenaAct.this.Battler[this.target].hp < 1)
				&& !(ability.restore && ((ability.hpdmg < 0 && ArenaAct.this.Battler[this.target].mres[ability.element] < 7)
						|| (ability.hpdmg > 0 && ArenaAct.this.Battler[this.target].mres[ability.element] == 7)))) {
			if (ability.hpdmg < 0)
				this.target--;
			else
				this.target++;
			if (this.target < 0)
				this.target = (ArenaAct.this.Battler.length / 2) - 1
						- this.difference;
			if (this.target > ArenaAct.this.Battler.length - 1
					- this.difference)
				this.target = (ArenaAct.this.Battler.length / 2)
						- this.difference;
		}
		int a = this.target, b = this.target;
		if (ability.trg > 1) {
			a = 0;
			b = 6;
		}
		if (this.current > (ArenaAct.this.Battler.length / 2) - 1
				- this.difference
				&& ability.trg < -1) {
			a = (ArenaAct.this.Battler.length / 2);
			b = ArenaAct.this.Battler.length - 1;
		}
		if (this.current < (ArenaAct.this.Battler.length / 2)
				- this.difference
				&& ability.trg < -1) {
			a = 0;
			b = (ArenaAct.this.Battler.length / 2) - 1;
		}
		if (this.target > (ArenaAct.this.Battler.length / 2) - 1
				- this.difference
				&& ability.trg == 1) {
			a = (ArenaAct.this.Battler.length / 2);
			b = ArenaAct.this.Battler.length - 1;
		}
		if (this.target < (ArenaAct.this.Battler.length / 2) - this.difference
				&& ability.trg == 1) {
			a = 0;
			b = (ArenaAct.this.Battler.length / 2) - 1;
		}
		if (ability.trg == -1) {
			a = this.current;
			b = this.current;
		}
		if (this.jScript != null && this.jScript.length > 3
				&& this.jScript[3] != null)
			try {
				this.jsScope.put("Performance", this.jsScope,
						Context.javaToJS(ability, this.jsScope));
				this.jsScope.put("Current", this.jsScope,
						Context.javaToJS(this.current, this.jsScope));
				this.jsScope.put("FirstTarget", this.jsScope,
						Context.javaToJS(a, this.jsScope));
				this.jsScope.put("LastTarget", this.jsScope,
						Context.javaToJS(b, this.jsScope));
				this.jsContext.evaluateString(this.jsScope, this.jScript[3],
						"BeforeAct", 1, null);
			} catch (Exception ex) {
				Log.e("Rhino JS error", ex.getMessage());
			}
		this.updText.append("\n" + Battler[this.current].name + " performs "
				+ ability.name);
		int trg;
		boolean act = true;
		boolean cost = true;
		for (int i = a; i <= b; i++)
			if (ArenaAct.this.Battler[i] != null
					&& (ArenaAct.this.Battler[i].hp > 0 || ability.restore)) {
				if (ArenaAct.this.Battler[i].reflect && ability.dmgtype == 2
						&& i != this.current) {
					this.updText.append(", which is reflected");
					trg = this.current;
				} else
					trg = i;
				this.updText.append(ability.execute(
						ArenaAct.this.Battler[this.current],
						ArenaAct.this.Battler[trg], cost));
				if (trg == this.current) {
                    this.playSpr(this.current, 3, ability, cost);
                    act = false;
                }
				else {
                    if (act) {
                        this.playSpr(this.current, 3, null, false);
                        act = false;
                    }
                    this.playSpr(trg, ArenaAct.this.Battler[trg].hp < 1 ? 2 : 1, ability, cost);
                }
				cost = false;
			}
		this.waitTime[0] = this.waitTime[1] > this.waitTime[2] ? this.waitTime[1]
				: this.waitTime[2];
		if (ArenaAct.this.Battler[this.current].hp < 1)
			this.playSpr(this.current, 2, null, false);
		if (this.current < (ArenaAct.this.Battler.length / 2)
				- this.difference) {
			ArenaAct.this.Battler[this.current].exp++;
			ArenaAct.this.Battler[this.current].levelUp();
		}
		this.updText.append(".");
		if (this.jScript != null && this.jScript.length > 4
				&& this.jScript[4] != null)
			try {
				this.jsScope.put("Performance", this.jsScope,
						Context.javaToJS(ability, this.jsScope));
				this.jsScope.put("Current", this.jsScope,
						Context.javaToJS(this.current, this.jsScope));
				this.jsScope.put("FirstTarget", this.jsScope,
						Context.javaToJS(a, this.jsScope));
				this.jsScope.put("LastTarget", this.jsScope,
						Context.javaToJS(b, this.jsScope));
				this.jsContext.evaluateString(this.jsScope, this.jScript[4],
						"AfterAct", 1, null);
			} catch (Exception ex) {
				Log.e("Rhino JS error", ex.getMessage());
			}
	}

	private void useItem() {
		Performance item = this.crItems.get(this.itemBox.getSelectedItemPosition());
		this.executeSkill(item);
		if (item.qty == 0)
			this.Battler[this.current].items.remove(item);
	}

	private void executeAI() {
		this.setAItarget(this.getAIskill(this.checkAIheal()));
	}

	private boolean checkAIheal() {
		boolean nHeal = false;
		int a = (this.current > (Battler.length / 2) - 1 - this.difference) ? (ArenaAct.this.Battler.length / 2)
				- this.difference
				: 0;
		int b = (this.current < (Battler.length / 2) - this.difference) ? (ArenaAct.this.Battler.length / 2)
				- 1 - this.difference
				: (ArenaAct.this.Battler.length / 2) - 1 - this.difference;
		for (int i = a; i <= b; i++)
			if (ArenaAct.this.Battler[i].hp < (ArenaAct.this.Battler[i].maxhp / 3)) {
				nHeal = true;
				break;
			}
		if (nHeal) {
			nHeal = false;
			for (int i = 0; i < ArenaAct.this.Battler[this.current].raceSkills.length; i++) {
				Performance s = ArenaAct.this.Battler[this.current].raceSkills[i];
				if (s.hpdmg < 0
						&& s.mpc <= ArenaAct.this.Battler[this.current].mp
						&& s.hpc <= ArenaAct.this.Battler[this.current].hp
						&& s.spc <= ArenaAct.this.Battler[this.current].sp
						&& ArenaAct.this.Battler[this.current].level >= s
								.getLvRq()) {
					nHeal = true;
					break;
				}
			}
			if (!nHeal)
				for (int i = 0; i < ArenaAct.this.Battler[this.current].jobSkills
						.size(); i++) {
					Performance s = ArenaAct.this.Battler[this.current].jobSkills
							.get(i);
					if (s.hpdmg < 0
							&& s.mpc <= ArenaAct.this.Battler[this.current].mp
							&& s.hpc <= ArenaAct.this.Battler[this.current].hp
							&& s.spc <= ArenaAct.this.Battler[this.current].sp
							&& ArenaAct.this.Battler[this.current].level >= s
									.getLvRq()) {
						nHeal = true;
						break;
					}
				}
		}
		return nHeal;
	}

	private Performance getAIskill(boolean heal) {
		Performance s = (heal) ? ArenaAct.this.Skill[1] : ArenaAct.this.Skill[0];
		Performance a;
		for (int i = 0; i < ArenaAct.this.Battler[this.current].jobSkills
				.size()
				|| i < ArenaAct.this.Battler[this.current].raceSkills.length; i++)
			for (int j = 0; j < 2; j++) {
				if (j == 0 && i < Battler[this.current].raceSkills.length)
					a = Battler[this.current].raceSkills[i];
				else if (j == 1 && i < Battler[this.current].jobSkills.size())
					a = Battler[this.current].jobSkills.get(i);
				else
					continue;
				if (a.mpc <= ArenaAct.this.Battler[this.current].mp
						&& a.hpc < ArenaAct.this.Battler[this.current].hp
						&& a.spc <= ArenaAct.this.Battler[this.current].sp
						&& ArenaAct.this.Battler[this.current].level >= a
								.getLvRq())
					if (heal) {
						if (a.hpdmg < s.hpdmg && a.hpdmg < 0)
							s = a;
					} else if (a.hpdmg > s.hpdmg)
						s = a;
			}
		return s;
	}

	private void setAItarget(Performance ability) {
		int a;
		int b;
		if (((this.current > (ArenaAct.this.Battler.length / 2) - 1
				- this.difference) && (ability.hpdmg > 0))
				|| ((this.current < (ArenaAct.this.Battler.length / 2)
						- this.difference) && (ability.hpdmg < 1))) {
			a = 0;
			b = (ArenaAct.this.Battler.length / 2) - 1;
		} else {
			a = (ArenaAct.this.Battler.length / 2);
			b = ArenaAct.this.Battler.length - 1;
		}
		if (ArenaAct.this.Battler[this.current].auto < 0)
			if (a == (ArenaAct.this.Battler.length / 2)) {
				a = 0;
				b = (ArenaAct.this.Battler.length / 2) - 1;
			} else {
				a = (ArenaAct.this.Battler.length / 2);
				b = ArenaAct.this.Battler.length - 1;
			}
		this.target = a;
		while ((ArenaAct.this.Battler[this.target] == null || ArenaAct.this.Battler[target].hp < 1)
				&& (ability.hpdmg > 1) && this.target < b)
			this.target++;

		for (int i = this.target; i <= b; ++i)
			if (ArenaAct.this.Battler[i] != null
					&& (ArenaAct.this.Battler[i].hp < ArenaAct.this.Battler[target].hp)
					&& (ArenaAct.this.Battler[i].hp > 0))
				this.target = i;
		this.executeSkill(ability);
	}

	@SuppressWarnings("deprecation")
	private void endTurn() {
		if (ArenaAct.this.Battler[this.current] != null) {
			ArenaAct.this.Battler[this.current].active = false;
			//this.imgActor[this.current].getBackground().setColorFilter(null);
			this.imgActor[this.current].setImageDrawable(null);
		}
		if (this.jScript != null && this.jScript.length > 2
				&& this.jScript[2] != null)
			try {
				this.jsScope.put("Current", this.jsScope,
						Context.javaToJS(this.current, this.jsScope));
				this.jsContext.evaluateString(this.jsScope, this.jScript[2],
						"OnEndTurn", 1, null);
			} catch (Exception ex) {
				Log.e("Rhino JS error", ex.getMessage());
			}

		boolean reset = true;
		for (this.current = 0; this.current < ArenaAct.this.Battler.length; this.current++) {
			if (ArenaAct.this.Battler[this.current] != null
					&& ArenaAct.this.Battler[this.current].active) {
				reset = false;
				break;
			}
		}

		int i;
		if (reset) {
			for (i = 0; i < ArenaAct.this.Battler.length; i++)
				if (ArenaAct.this.Battler[i] != null
						&& ArenaAct.this.Battler[i].hp > 0) {
					this.updText.append(ArenaAct.this.Battler[i]
							.applyState(true));
					if (ArenaAct.this.Battler[i].hp < 1)
						this.playSpr(i, 2, null, false);
				}
			for (this.current = 0; this.current < ArenaAct.this.Battler.length; this.current++)
				if (ArenaAct.this.Battler[this.current] != null
						&& ArenaAct.this.Battler[this.current].active)
					break;
		}
		
		if (this.current < 6)
			for (i = this.current + 1; i < ArenaAct.this.Battler.length; i++)
				if (ArenaAct.this.Battler[this.current] != null
						&& ArenaAct.this.Battler[i] != null
						&& (ArenaAct.this.Battler[this.current].agi < ArenaAct.this.Battler[i].agi)
						&& ArenaAct.this.Battler[i].active)
					this.current = i;

		if (this.jScript != null && this.jScript.length > 1
				&& this.jScript[1] != null)
			try {
				this.jsScope.put("Reset", this.jsScope,
						Context.javaToJS(reset, this.jsScope));
				this.jsScope.put("Current", this.jsScope,
						Context.javaToJS(this.current, this.jsScope));
				this.jsContext.evaluateString(this.jsScope, this.jScript[1],
						"OnBeginTurn", 1, null);
			} catch (Exception ex) {
				Log.e("Rhino JS error", ex.getMessage());
			}

		if (!this.checkEnd()) {
			if (ArenaAct.this.Battler[this.current] != null
					&& ArenaAct.this.Battler[this.current].active) {
				if (ArenaAct.this.Battler[this.current].auto == 0) {
					this.setCurrent();
					this.refreshItemBox();
				}
				ArenaAct.this.Battler[this.current].applyState(false);
				if (ArenaAct.this.Battler[this.current].auto != 0
						&& ArenaAct.this.Battler[this.current].hp > 0) {
					this.executeAI();
					this.endTurn();
				}
				this.refreshTargetBox();
			} else
				this.endTurn();
		}
	}

	private void setCurrent() {
		this.skillCost.setText(ArenaAct.this.Battler[this.current].name + ": "
				+ ArenaAct.this.Battler[this.current].hp + "/"
				+ ArenaAct.this.Battler[this.current].maxhp + " HP, "
				+ ArenaAct.this.Battler[this.current].mp + "/"
				+ ArenaAct.this.Battler[this.current].maxmp + " MP, "
				+ ArenaAct.this.Battler[this.current].sp + "/"
				+ ArenaAct.this.Battler[this.current].maxsp + " RP");
		if (this.imgActor[this.current].getDrawable() == null)
            ArenaAct.this.imgActor[ArenaAct.this.current].setImageResource(R.drawable.current);
		/*Drawable backSpr;
		if ((backSpr = this.imgActor[this.current].getBackground()) != null)
			backSpr.setColorFilter(this.crColor);*/
		this.refreshSkillBox();
	}

	private boolean checkEnd() {
		boolean b = false;
		if ((ArenaAct.this.Battler[0] == null || ArenaAct.this.Battler[0].hp < 1)
				&& (ArenaAct.this.Battler[1] == null || ArenaAct.this.Battler[1].hp < 1)
				&& (ArenaAct.this.Battler[2] == null || ArenaAct.this.Battler[2].hp < 1)
				&& (ArenaAct.this.Battler[3] == null || ArenaAct.this.Battler[3].hp < 1)) {
			this.endBt(-1);
			b = true;
		} else if ((ArenaAct.this.Battler[4] == null || ArenaAct.this.Battler[4].hp < 1)
				&& (ArenaAct.this.Battler[5] == null || ArenaAct.this.Battler[5].hp < 1)
				&& (ArenaAct.this.Battler[6] == null || ArenaAct.this.Battler[6].hp < 1)
				&& (ArenaAct.this.Battler[7] == null || ArenaAct.this.Battler[7].hp < 1)) {
			this.endBt(1);
			b = true;
		}
		return b;
	}

	private void endBt(int r) {
		String t;
		String s;
		if (r > 0) {
			t = "Victory";
			s = "The party has won the battle!";
		} else {
			t = "Defeat";
			if (r < 0)
				s = "The party has been defeated!";
			else
				s = "The party has fled!";
		}
		ArenaAct.this.result = r;
		this.endingMsg(t, s);
	}

	private void endingMsg(String t, String s) {
		(new AlertDialog.Builder(this).setCancelable(false).setMessage(s)
				.setTitle(t).setPositiveButton("Exit",
				new DialogInterface.OnClickListener() {
					@Override
					public void onClick(DialogInterface arg0, int arg1) {
						Intent outcome = new Intent();
						outcome.putExtra("Outcome", ArenaAct.this.result);
						if (ArenaAct.cachedItem == null
								&& ArenaAct.this.Item != null) {
							outcome.putParcelableArrayListExtra("Item",
									ArenaAct.this.Item);
							for (int i = 0; i < 4 - ArenaAct.this.difference; i++) {
								ArenaAct.this.Battler[i].items = null;
								ArenaAct.this.Battler[i].cleanStates();
							}
						}
						else for (int i = 0; i < 4 - ArenaAct.this.difference; i++)
								ArenaAct.this.Battler[i].cleanStates();
						if (ArenaAct.cachedParty == null) {
							Actor[] party = new Actor[] {
									ArenaAct.this.Battler[0],
									ArenaAct.this.Battler[1],
									ArenaAct.this.Battler[2],
									ArenaAct.this.Battler[3] };
							outcome.putExtra("Party", party);
						}
						ArenaAct.this.setResult(RESULT_OK, outcome);
						ArenaAct.this.finish();
					}
				})).create().show();
	}

	private void playSpr(final int c, final int s, final Performance a, final boolean playSnd) {
		if (!ArenaAct.this.Battler[c].sprName.equals("")) {
			final String pos = ((c > (ArenaAct.this.Battler.length / 2) - 1 && ArenaAct.this.surprise >= 0)
					|| (c < (ArenaAct.this.Battler.length / 2) && ArenaAct.this.surprise < 0))
					? "r" : "l";
			final AnimationDrawable sprAnim = Battler[c].getBtSprite(s, pos,
					this.waitTime, this);
			this.imgActor[c].postDelayed(new Runnable() {
				@Override
				public void run() {
					if (s == 2 || s < 0 || ArenaAct.this.Battler[c].hp > 0
							|| !ArenaAct.this.imgActor[c].getTag().equals(2)) {
                        if (a != null) {
							if (a.animSprId > 0) {
								final AnimationDrawable animDrw = (AnimationDrawable) ArenaAct.this.getResources().getDrawable(a.animSprId);
								int animDur = 0;
								for (int i = 0; i < animDrw.getNumberOfFrames(); i++)
									animDur += animDrw.getDuration(i);
								ArenaAct.this.imgActor[c].setImageDrawable(animDrw);
								animDrw.start();
								ArenaAct.this.imgActor[c].postDelayed(new Runnable() {
									@Override
									public void run() {
										if (c == ArenaAct.this.current)
											ArenaAct.this.imgActor[c].setImageResource(R.drawable.current);
										else
											ArenaAct.this.imgActor[c].setImageDrawable(null);
									}
								}, animDur);
							}
							if (playSnd)
								a.playSound(ArenaAct.this);
						}
						sprAnim.stop();
						sprAnim.setOneShot(true);
                        //sprAnim.setColorFilter(c == ArenaAct.this.current ? crColor : null);
						ArenaAct.this.imgActor[c].setBackgroundDrawable(sprAnim);
						sprAnim.start();
						ArenaAct.this.imgActor[c].setTag(s);
					}
				}
			}, this.waitTime[0]);
		}
	}

	private boolean imgClick(int i) {
		boolean a = false;
		int s = (i > (ArenaAct.this.Battler.length / 2) - 1) ? i
				- this.difference : i;
		if (this.target == i) {
			this.executeSkill(this
					.getAIskill((i > ArenaAct.this.Battler.length / 2 - 1 && this.current < ArenaAct.this.Battler.length / 2)
							|| (i < Battler.length / 2 && this.current > Battler.length / 2 - 1) ? false
							: true));
			a = true;
		} else if (ArenaAct.this.Battler[i] != null
				&& ArenaAct.this.Battler[i].maxhp > 0)
			ArenaAct.this.targetBox.setSelection(s);
		return a;
	}

	@Override
	public void onBackPressed() {
		this.endBt(-1);
	}

	@Override
	protected void onDestroy() {
		for (Performance ability : ArenaAct.this.Skill)
			ability.cleanSound();
		for (Actor player : ArenaAct.this.Battler)
			if (player != null) {
				player.setSprites(this, false, null);
				for (Performance ability : player.raceSkills)
					ability.cleanSound();
				for (Performance ability : player.jobSkills)
					ability.cleanSound();
			}
		ArenaAct.cachedParty = null;
		ArenaAct.cachedEnemy = null;
		ArenaAct.cachedSkill = null;
		ArenaAct.cachedItem = null;
		ArenaAct.cachedScript = null;
		if (this.jsContext != null)
			Context.exit();
		if (this.songPlayer != null)
			songPlayer.release();
		super.onDestroy();
	}
}