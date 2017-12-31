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
package com.tbrpgsca.library;

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

public class BattleAct extends Activity {
	private static Parcelable[] cachedParty = null, cachedEnemy = null;
	private static Ability[] cachedSkill = null;
	private static ArrayList<Ability> cachedItem = null;
	private static String[] cachedScript = null;

	private Ability[] Skill;
	private ArrayList<Ability> Item;
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

	private ArrayList<Ability> cSkills = new ArrayList<Ability>(),
			crItems = new ArrayList<Ability>();

	//private final PorterDuffColorFilter crColor = new PorterDuffColorFilter(Color.YELLOW, PorterDuff.Mode.MULTIPLY);

	private MediaPlayer songPlayer;

	private OnClickListener cAction = new OnClickListener() {
		@Override
		public void onClick(final View v) {
			BattleAct.this.waitTime[0] = 0;
			boolean act = true;
			boolean surprised = BattleAct.this.surprise < 0;
			BattleAct.this.target = BattleAct.this.targetBox
					.getSelectedItemPosition();
			if ((BattleAct.this.target > 2 && BattleAct.this.difference > 0)
					|| (BattleAct.this.target > 0 && BattleAct.this.difference > 2)
					|| (BattleAct.this.target > 1 && BattleAct.this.difference > 1))
				BattleAct.this.target += BattleAct.this.difference;
			switch (v.getId()) {
			case R.id.ActBt:
				BattleAct.this.executeSkill();
				break;
			case R.id.UseBt:
				BattleAct.this.useItem();
				break;
			case R.id.AutoBt:
				BattleAct.this.executeAI();
				break;
			case R.id.RunBt:
				BattleAct.this.endBt(0);
				break;
			case R.id.ImgPlayer1:
				act = (surprised) ? BattleAct.this.imgClick(6) : BattleAct.this
						.imgClick(0);
				break;
			case R.id.ImgPlayer2:
				act = (surprised) ? BattleAct.this.imgClick(7) : BattleAct.this
						.imgClick(1);
				break;
			case R.id.ImgPlayer3:
				act = (surprised) ? BattleAct.this.imgClick(4) : BattleAct.this
						.imgClick(2);
				break;
			case R.id.ImgPlayer4:
				act = (surprised) ? BattleAct.this.imgClick(5) : BattleAct.this
						.imgClick(3);
				break;
			case R.id.ImgEnemy1:
				act = (surprised) ? BattleAct.this.imgClick(2) : BattleAct.this
						.imgClick(4);
				break;
			case R.id.ImgEnemy2:
				act = (surprised) ? BattleAct.this.imgClick(3) : BattleAct.this
						.imgClick(5);
				break;
			case R.id.ImgEnemy3:
				act = (surprised) ? BattleAct.this.imgClick(0) : BattleAct.this
						.imgClick(6);
				break;
			case R.id.ImgEnemy4:
				act = (surprised) ? BattleAct.this.imgClick(1) : BattleAct.this
						.imgClick(7);
				break;
			}
			if (act)
				BattleAct.this.endTurn();
		}
	};

	public static void PlayDemo(Activity act) {
		BattleAct.InitiateBattle(act, 0, 0, null, null, null, null, null, 0, true,
				false);
	}

	public static void InitiateBattle(Activity act, int arenaDrawableId, int songId,
									  Actor[] party, Actor[] enemy, Ability[] skills,
									  ArrayList<Ability> items, String[] scripts,
									  int surprise, boolean escapable, boolean staticCache) {
		if (staticCache) {
			BattleAct.cachedParty = party;
			BattleAct.cachedEnemy = enemy;
			BattleAct.cachedSkill = skills;
			BattleAct.cachedItem = items;
			BattleAct.cachedScript = scripts;
		}
		Intent btInt = new Intent(act, BattleAct.class);
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
		if (BattleAct.cachedItem != null)
			this.Item = BattleAct.cachedItem;
		else if (extra.containsKey("Item"))
			this.Item = extra.getParcelableArrayList("Item");
		if (BattleAct.cachedScript != null)
			this.jScript = BattleAct.cachedScript;
		else if (extra.containsKey("Script"))
			this.jScript = extra.getStringArray("Script");
		Parcelable[] party = null;
		Parcelable[] enemy = null;
		if (BattleAct.cachedParty != null && BattleAct.cachedEnemy != null) {
			party = BattleAct.cachedParty;
			enemy = BattleAct.cachedEnemy;
		} else if (extra.containsKey("Party") && extra.containsKey("Enemy")) {
			party = extra.getParcelableArray("Party");
			enemy = extra.getParcelableArray("Enemy");
		}
		if (party != null && enemy != null) {
			BattleAct.this.Battler = new Actor[8];
			for (int i = 0; i < party.length && i < 4; i++)
				if (party[i] != null) {
					BattleAct.this.Battler[i] = (Actor) party[i];
					if (this.Item != null)
						this.Battler[i].items = this.Item;
				}
			for (int k = 0; k < enemy.length && k < 4; k++)
				if (enemy[k] != null) {
					BattleAct.this.Battler[k + 4] = BattleAct.cachedEnemy == null ? (Actor) enemy[k]
							: new Actor((Actor) enemy[k]);
					BattleAct.this.Battler[k + 4].auto = 2;
				}
		}
		if (BattleAct.cachedSkill != null)
			this.Skill = BattleAct.cachedSkill;
		else if (extra.containsKey("Skill")) {
			Parcelable[] skill = extra.getParcelableArray("Skill");
			if (skill != null) {
				this.Skill = new Ability[skill.length];
				System.arraycopy(skill, 0, this.Skill, 0, skill.length);
			}
		}
		if (BattleAct.this.Skill == null) {
			BattleAct.this.Skill = new Ability[] {
					new Ability(0, "Attack", 0, 0, false, false, 0, 0, 0, 0, 0, 1,
							10, 0, -3, 0, 1, false, null, null),
					new Ability(0, "Defend", 0, 0, false, true, 0, 0, 0, 0, 3, -1, 0,
							-2, -1, -1, 0, false, null, null) };
		}
		Actor.State regen = new Actor.State(1, "Regen", false, false, false, -1, 0, 10, 0, 0, 0,
				2, 0, 0, 0, false);
		Actor.State poison = new Actor.State(2, "Poison", false, false, false, 10, 0, -7, 0, -2,
				0, -2, 0, 0, 0, false);
		Actor.State dizziness = new Actor.State(4, "Dizziness", false, false, false, 3, 0, 0, -7, 0,
				0, 0, -1, -1, 0, false);
		if (BattleAct.this.Battler == null) {
			ArrayList<Ability> items = new ArrayList<Ability>(
					Arrays.asList(new Ability(1, "Potion", 0, 0, false, -25, 0, 0, 0,
							0, false, null, null).setQty(10)));
			BattleAct.this.Battler = new Actor[] {
					new Actor(0, "Cody", "Half-Elf", "Hero", 1, 3, 50, 10, 15,
							13, 12, 9, 7, 5, 1, 1, 1, 1, 0, 1, 0, 0,
							new int[] {}, items, new Ability[] { new Ability(0,
									"Steal", 0, 0, true, true, 2, 0, 0, 0, 3, 0, 0,
									0, 0, 0, 1, 5, 3, false, false, null, null) }, null),
					new Actor(0, "Stephanie", "Human", "Sorceress", 1, 3, 45,
							20, 5, 13, 12, 5, 5, 1, 1, 1, 1, 1, 0, 0, 0, 1,
							new int[] {}, items, new Ability[] { new Ability(0,
									"Iceshard", 0, 0, false, true, 2, 0, 7, 0, 2, 1,
									15, 0, 0, 0, 3, false, null,null) }, null),
					new Actor(0, "George", "Half-Orc", "Templar", 1, 3, 47, 15,
							15, 13, 12, 5, 10, 7, 1, 1, 1, 0, 1, 0, 1, 0,
							new int[] {}, items, new Ability[] { new Ability(0,
									"Regen", 0, 0, false, true, 2, 0, 7, 0, 3, 1,
									-25, 0, 0, 0, 6, -1,-1, false, true, new Actor.State[] { regen }, new Actor.State[] { poison, dizziness }) }, false),
					new Actor(0, "Victoria", "Elf", "Valkyrie", 1, 3, 55, 7,
							13, 17, 12, 5, 7, 3, 1, 1, 1, 1, 1, 0, 0, 0,
							new int[] {}, items, new Ability[] { new Ability(0,
									"Bash", 0, 0, false, false, 2, 0, 0, 5, 1, 1, 12,
									0, -5, 0, 1, false, new Actor.State[] { dizziness }, null) }, null),
					new Actor(0, "Lizard", "Lizard", "Lizard", 3, 3, 50, 15,
							10, 13, 12, 9, 7, 5, 1, 1, 1, 1, 0, 1, 0, 0,
							new int[] {}, new ArrayList<Ability>(Arrays.asList(new Ability(
									new Ability(4, "Ether", 0, 0, true, 0, -10, 0, 0,
											0, false, null,null)).setQty(2))),
							new Ability[] { new Ability(0, "Fireball", 0, 0, false,
									true, 2, 0, 7, 0, 2, 1, 15, 0, 0, 0, 2,
									false, null, null) }, null),
					new Actor(0, "Goblin", "Goblin", "Goblin", 3, 3, 45, 5, 20,
							13, 12, 5, 5, 1, 1, 1, 1, 1, 0, 0, 0, 1,
							new int[] {}, new ArrayList<Ability>(
									Arrays.asList(new Ability(new Ability(2,
											"Antidote", 0, 0, true, 0, 0, 0, 0, 0, false,
											null, new Actor.State[] { poison }))
											.setQty(3))),
							new Ability[] { new Ability(0, "Poison", 0, 0, false,
									true, 2, 0, 0, 5, 4, 2, 3, 0, 9, 0, 1, 3,
									7, false, false, new Actor.State[] { poison },
									new Actor.State[] { regen }) }, null),
					new Actor(0, "Ogre", "Ogre", "Ogre", 3, 3, 55, 7, 13, 17,
							12, 5, 7, 3, 1, 1, 1, 1, 1, 0, 0, 0, new int[] {},
							new ArrayList<Ability>(Arrays.asList(new Ability(
									items.get(0)).setQty(2))),
							new Ability[] { new Ability(0, "Smash", 0, 0, false,
									false, 2, 2, 0, 3, 0, 1, 12, 0, -7, 0, 1,
									false, new Actor.State[] { dizziness }, null) }, null),
					new Actor(0, "Troll", "Troll", "Troll", 3, 3, 47, 15, 15,
							13, 12, 5, 10, 7, 1, 1, 1, 0, 1, 0, 1, 0,
							new int[] {}, new ArrayList<Ability>(
									Arrays.asList(new Ability(new Ability(3,
											"Elixir", 0, 0, true, -100, -100, -100,
											0, 0, true, null, null)).setQty(1))),
							new Ability[] { new Ability(0, "Heal", 0, 0, false, true,
									2, 0, 7, 0, 3, 1, -25, 0, 0, 0, 6, -1, -1, false,
									true, null, new Actor.State[] { dizziness }) }, null) };
			for (int i = 4; i < 8; i++)
				BattleAct.this.Battler[i].auto = 2;
			BattleAct.this.surprise = 0;
		}
		BattleAct.this.result = 0;
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
		if (BattleAct.this.surprise < 0) {
			this.imgActor[0] = (ImageView) findViewById(R.id.ImgEnemy3);
			this.imgActor[1] = (ImageView) findViewById(R.id.ImgEnemy4);
			this.imgActor[2] = (ImageView) findViewById(R.id.ImgEnemy1);
			this.imgActor[3] = (ImageView) findViewById(R.id.ImgEnemy2);
			this.imgActor[4] = (ImageView) findViewById(R.id.ImgPlayer3);
			this.imgActor[5] = (ImageView) findViewById(R.id.ImgPlayer4);
			this.imgActor[6] = (ImageView) findViewById(R.id.ImgPlayer1);
			this.imgActor[7] = (ImageView) findViewById(R.id.ImgPlayer2);
		} else {
			this.imgActor[0] = (ImageView) findViewById(R.id.ImgPlayer1);
			this.imgActor[1] = (ImageView) findViewById(R.id.ImgPlayer2);
			this.imgActor[2] = (ImageView) findViewById(R.id.ImgPlayer3);
			this.imgActor[3] = (ImageView) findViewById(R.id.ImgPlayer4);
			this.imgActor[4] = (ImageView) findViewById(R.id.ImgEnemy1);
			this.imgActor[5] = (ImageView) findViewById(R.id.ImgEnemy2);
			this.imgActor[6] = (ImageView) findViewById(R.id.ImgEnemy3);
			this.imgActor[7] = (ImageView) findViewById(R.id.ImgEnemy4);
		}
		for (int i = 0; i < this.imgActor.length; i++) {
			this.imgActor[i].setOnClickListener(this.cAction);
			this.imgActor[i].setTag(0);
			if (BattleAct.this.Battler[i] != null
					&& BattleAct.this.Battler[i].maxhp > 0)
				this.playSpr(i, BattleAct.this.Battler[i].hp > 0 ? 0 : -1, null, false);
			else if (i < 4)
				this.difference++;
		}
		if (BattleAct.this.surprise < 0) {
			for (int i = 0; i < (BattleAct.this.Battler.length / 2)
					- this.difference; i++)
				if (BattleAct.this.Battler[i] != null)
					BattleAct.this.Battler[i].active = false;
			this.endTurn();
		} else if (BattleAct.this.surprise > 0) {
			for (int i = (BattleAct.this.Battler.length / 2); i < BattleAct.this.Battler.length; i++)
				if (BattleAct.this.Battler[i] != null)
					BattleAct.this.Battler[i].active = false;
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
		this.targetBox.setSelection((BattleAct.this.Battler.length / 2)
				- this.difference);
		this.refreshSkillBox();
		this.refreshItemBox();
	}

	private void refreshTargetBox() {
		this.targetList.clear();
		String s;
		for (int i = 0; i < BattleAct.this.Battler.length; i++) {
			if (BattleAct.this.Battler[i] != null
					&& BattleAct.this.Battler[i].maxhp > 0) {
				s = BattleAct.this.Battler[i].name + " (HP: "
						+ BattleAct.this.Battler[i].hp;
				if (i < (BattleAct.this.Battler.length / 2) - this.difference)
					s += ", MP: " + BattleAct.this.Battler[i].mp + ", RP: "
							+ BattleAct.this.Battler[i].sp;
				s += ")";
				this.targetList.add(s);
			}
		}
		this.targetList.notifyDataSetChanged();
	}

	private void refreshSkillBox() {
		this.skillList.clear();
		this.cSkills.clear();
		for (int i = 0; i < BattleAct.this.Skill.length; i++)
			this.addAbility(BattleAct.this.Skill[i], this.skillList,
					this.cSkills);
		if (BattleAct.this.Battler[this.current].raceSkills != null)
			for (int i = 0; i < Battler[this.current].raceSkills.length; i++)
				this.addAbility(
						BattleAct.this.Battler[this.current].raceSkills[i],
						this.skillList, this.cSkills);
		if (BattleAct.this.Battler[this.current].jobSkills != null)
			for (int i = 0; i < BattleAct.this.Battler[this.current].jobSkills
					.size(); i++)
				this.addAbility(
						BattleAct.this.Battler[this.current].jobSkills.get(i),
						this.skillList, this.cSkills);
		this.skillList.notifyDataSetChanged();
		if (this.skillBox.getSelectedItemPosition() > 1)
			this.skillBox.setSelection(0);
	}

	private void addAbility(Ability ability, ArrayAdapter<String> abilityList,
			ArrayList<Ability> abilities) {
		String trg;
		ability.checkQty();
		if (!(ability.getLvRq() > BattleAct.this.Battler[this.current].level)
				&& !(ability.mqty > -1 && ability.qty < 1)
				&& !(ability.mpc > BattleAct.this.Battler[this.current].mp)
				&& !(ability.spc > BattleAct.this.Battler[this.current].sp)
				&& !(ability.hpc >= BattleAct.this.Battler[this.current].hp)) {
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
		if (BattleAct.this.Battler[this.current].items != null)
			for (int i = 0; i < BattleAct.this.Battler[this.current].items
					.size(); i++)
				this.addAbility(
						BattleAct.this.Battler[this.current].items.get(i),
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

	private void executeSkill(Ability ability) {
		if (this.target == 1
				&& (BattleAct.this.Battler[3] != null && BattleAct.this.Battler[3].canGuard)
				&& ((BattleAct.this.Battler[0] != null && BattleAct.this.Battler[0].canGuard) || (BattleAct.this.Battler[2] != null && BattleAct.this.Battler[2].canGuard))
				&& this.current > (BattleAct.this.Battler.length / 2) - 1
				&& this.difference == 0 && !ability.range)
			this.target = 3;
		if (this.target == 2
				&& (BattleAct.this.Battler[0] != null && BattleAct.this.Battler[0].canGuard)
				&& ((BattleAct.this.Battler[1] != null && BattleAct.this.Battler[1].canGuard) || (BattleAct.this.Battler[3] != null && BattleAct.this.Battler[3].canGuard))
				&& this.current > (BattleAct.this.Battler.length / 2) - 1
				&& this.difference < 2 && !ability.range)
			this.target = 0;
		if (this.target == 4
				&& (BattleAct.this.Battler[6] != null && BattleAct.this.Battler[6].canGuard)
				&& ((BattleAct.this.Battler[5] != null && BattleAct.this.Battler[5].canGuard) || (BattleAct.this.Battler[7] != null && BattleAct.this.Battler[7].canGuard))
				&& !ability.range)
			this.target = 6;
		if (this.target == 7
				&& ((BattleAct.this.Battler[6] != null && BattleAct.this.Battler[6].canGuard) || (BattleAct.this.Battler[4] != null && BattleAct.this.Battler[4].canGuard))
				&& (BattleAct.this.Battler[5] != null && BattleAct.this.Battler[5].canGuard)
				&& !ability.range)
			this.target = 5;
		while ((BattleAct.this.Battler[this.target] != null && BattleAct.this.Battler[this.target].hp < 1)
				&& !(ability.restore && ((ability.hpdmg < 0 && BattleAct.this.Battler[this.target].mres[ability.element] < 7)
						|| (ability.hpdmg > 0 && BattleAct.this.Battler[this.target].mres[ability.element] == 7)))) {
			if (ability.hpdmg < 0)
				this.target--;
			else
				this.target++;
			if (this.target < 0)
				this.target = (BattleAct.this.Battler.length / 2) - 1
						- this.difference;
			if (this.target > BattleAct.this.Battler.length - 1
					- this.difference)
				this.target = (BattleAct.this.Battler.length / 2)
						- this.difference;
		}
		int a = this.target, b = this.target;
		if (ability.trg > 1) {
			a = 0;
			b = 6;
		}
		if (this.current > (BattleAct.this.Battler.length / 2) - 1
				- this.difference
				&& ability.trg < -1) {
			a = (BattleAct.this.Battler.length / 2);
			b = BattleAct.this.Battler.length - 1;
		}
		if (this.current < (BattleAct.this.Battler.length / 2)
				- this.difference
				&& ability.trg < -1) {
			a = 0;
			b = (BattleAct.this.Battler.length / 2) - 1;
		}
		if (this.target > (BattleAct.this.Battler.length / 2) - 1
				- this.difference
				&& ability.trg == 1) {
			a = (BattleAct.this.Battler.length / 2);
			b = BattleAct.this.Battler.length - 1;
		}
		if (this.target < (BattleAct.this.Battler.length / 2) - this.difference
				&& ability.trg == 1) {
			a = 0;
			b = (BattleAct.this.Battler.length / 2) - 1;
		}
		if (ability.trg == -1) {
			a = this.current;
			b = this.current;
		}
		if (this.jScript != null && this.jScript.length > 3
				&& this.jScript[3] != null)
			try {
				this.jsScope.put("Ability", this.jsScope,
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
			if (BattleAct.this.Battler[i] != null
					&& (BattleAct.this.Battler[i].hp > 0 || ability.restore)) {
				if (BattleAct.this.Battler[i].reflect && ability.dmgtype == 2
						&& i != this.current) {
					this.updText.append(", which is reflected");
					trg = this.current;
				} else
					trg = i;
				this.updText.append(ability.execute(
						BattleAct.this.Battler[this.current],
						BattleAct.this.Battler[trg], cost));
				if (trg == this.current) {
                    this.playSpr(this.current, 3, ability, cost);
                    act = false;
                }
				else {
                    if (act) {
                        this.playSpr(this.current, 3, null, false);
                        act = false;
                    }
                    this.playSpr(trg, BattleAct.this.Battler[trg].hp < 1 ? 2 : 1, ability, cost);
                }
				cost = false;
			}
		this.waitTime[0] = this.waitTime[1] > this.waitTime[2] ? this.waitTime[1]
				: this.waitTime[2];
		if (BattleAct.this.Battler[this.current].hp < 1)
			this.playSpr(this.current, 2, null, false);
		if (this.current < (BattleAct.this.Battler.length / 2)
				- this.difference) {
			BattleAct.this.Battler[this.current].exp++;
			BattleAct.this.Battler[this.current].levelUp();
		}
		this.updText.append(".");
		if (this.jScript != null && this.jScript.length > 4
				&& this.jScript[4] != null)
			try {
				this.jsScope.put("Ability", this.jsScope,
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
		Ability item = this.crItems.get(this.itemBox.getSelectedItemPosition());
		this.executeSkill(item);
		if (item.qty == 0)
			this.Battler[this.current].items.remove(item);
	}

	private void executeAI() {
		this.setAItarget(this.getAIskill(this.checkAIheal()));
	}

	private boolean checkAIheal() {
		boolean nHeal = false;
		int a = (this.current > (Battler.length / 2) - 1 - this.difference) ? (BattleAct.this.Battler.length / 2)
				- this.difference
				: 0;
		int b = (this.current < (Battler.length / 2) - this.difference) ? (BattleAct.this.Battler.length / 2)
				- 1 - this.difference
				: (BattleAct.this.Battler.length / 2) - 1 - this.difference;
		for (int i = a; i <= b; i++)
			if (BattleAct.this.Battler[i].hp < (BattleAct.this.Battler[i].maxhp / 3)) {
				nHeal = true;
				break;
			}
		if (nHeal) {
			nHeal = false;
			for (int i = 0; i < BattleAct.this.Battler[this.current].raceSkills.length; i++) {
				Ability s = BattleAct.this.Battler[this.current].raceSkills[i];
				if (s.hpdmg < 0
						&& s.mpc <= BattleAct.this.Battler[this.current].mp
						&& s.hpc <= BattleAct.this.Battler[this.current].hp
						&& s.spc <= BattleAct.this.Battler[this.current].sp
						&& BattleAct.this.Battler[this.current].level >= s
								.getLvRq()) {
					nHeal = true;
					break;
				}
			}
			if (!nHeal)
				for (int i = 0; i < BattleAct.this.Battler[this.current].jobSkills
						.size(); i++) {
					Ability s = BattleAct.this.Battler[this.current].jobSkills
							.get(i);
					if (s.hpdmg < 0
							&& s.mpc <= BattleAct.this.Battler[this.current].mp
							&& s.hpc <= BattleAct.this.Battler[this.current].hp
							&& s.spc <= BattleAct.this.Battler[this.current].sp
							&& BattleAct.this.Battler[this.current].level >= s
									.getLvRq()) {
						nHeal = true;
						break;
					}
				}
		}
		return nHeal;
	}

	private Ability getAIskill(boolean heal) {
		Ability s = (heal) ? BattleAct.this.Skill[1] : BattleAct.this.Skill[0];
		Ability a;
		for (int i = 0; i < BattleAct.this.Battler[this.current].jobSkills
				.size()
				|| i < BattleAct.this.Battler[this.current].raceSkills.length; i++)
			for (int j = 0; j < 2; j++) {
				if (j == 0 && i < Battler[this.current].raceSkills.length)
					a = Battler[this.current].raceSkills[i];
				else if (j == 1 && i < Battler[this.current].jobSkills.size())
					a = Battler[this.current].jobSkills.get(i);
				else
					continue;
				if (a.mpc <= BattleAct.this.Battler[this.current].mp
						&& a.hpc < BattleAct.this.Battler[this.current].hp
						&& a.spc <= BattleAct.this.Battler[this.current].sp
						&& BattleAct.this.Battler[this.current].level >= a
								.getLvRq())
					if (heal) {
						if (a.hpdmg < s.hpdmg && a.hpdmg < 0)
							s = a;
					} else if (a.hpdmg > s.hpdmg)
						s = a;
			}
		return s;
	}

	private void setAItarget(Ability ability) {
		int a;
		int b;
		if (((this.current > (BattleAct.this.Battler.length / 2) - 1
				- this.difference) && (ability.hpdmg > 0))
				|| ((this.current < (BattleAct.this.Battler.length / 2)
						- this.difference) && (ability.hpdmg < 1))) {
			a = 0;
			b = (BattleAct.this.Battler.length / 2) - 1;
		} else {
			a = (BattleAct.this.Battler.length / 2);
			b = BattleAct.this.Battler.length - 1;
		}
		if (BattleAct.this.Battler[this.current].auto < 0)
			if (a == (BattleAct.this.Battler.length / 2)) {
				a = 0;
				b = (BattleAct.this.Battler.length / 2) - 1;
			} else {
				a = (BattleAct.this.Battler.length / 2);
				b = BattleAct.this.Battler.length - 1;
			}
		this.target = a;
		while ((BattleAct.this.Battler[this.target] == null || BattleAct.this.Battler[target].hp < 1)
				&& (ability.hpdmg > 1) && this.target < b)
			this.target++;

		for (int i = this.target; i <= b; ++i)
			if (BattleAct.this.Battler[i] != null
					&& (BattleAct.this.Battler[i].hp < BattleAct.this.Battler[target].hp)
					&& (BattleAct.this.Battler[i].hp > 0))
				this.target = i;
		this.executeSkill(ability);
	}

	@SuppressWarnings("deprecation")
	private void endTurn() {
		if (BattleAct.this.Battler[this.current] != null) {
			BattleAct.this.Battler[this.current].active = false;
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
		for (this.current = 0; this.current < BattleAct.this.Battler.length; this.current++) {
			if (BattleAct.this.Battler[this.current] != null
					&& BattleAct.this.Battler[this.current].active) {
				reset = false;
				break;
			}
		}

		int i;
		if (reset) {
			for (i = 0; i < BattleAct.this.Battler.length; i++)
				if (BattleAct.this.Battler[i] != null
						&& BattleAct.this.Battler[i].hp > 0) {
					this.updText.append(BattleAct.this.Battler[i]
							.applyState(true));
					if (BattleAct.this.Battler[i].hp < 1)
						this.playSpr(i, 2, null, false);
				}
			for (this.current = 0; this.current < BattleAct.this.Battler.length; this.current++)
				if (BattleAct.this.Battler[this.current] != null
						&& BattleAct.this.Battler[this.current].active)
					break;
		}
		
		if (this.current < 6)
			for (i = this.current + 1; i < BattleAct.this.Battler.length; i++)
				if (BattleAct.this.Battler[this.current] != null
						&& BattleAct.this.Battler[i] != null
						&& (BattleAct.this.Battler[this.current].agi < BattleAct.this.Battler[i].agi)
						&& BattleAct.this.Battler[i].active)
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
			if (BattleAct.this.Battler[this.current] != null
					&& BattleAct.this.Battler[this.current].active) {
				if (BattleAct.this.Battler[this.current].auto == 0) {
					this.setCurrent();
					this.refreshItemBox();
				}
				BattleAct.this.Battler[this.current].applyState(false);
				if (BattleAct.this.Battler[this.current].auto != 0
						&& BattleAct.this.Battler[this.current].hp > 0) {
					this.executeAI();
					this.endTurn();
				}
				this.refreshTargetBox();
			} else
				this.endTurn();
		}
	}

	private void setCurrent() {
		this.skillCost.setText(BattleAct.this.Battler[this.current].name + ": "
				+ BattleAct.this.Battler[this.current].hp + "/"
				+ BattleAct.this.Battler[this.current].maxhp + " HP, "
				+ BattleAct.this.Battler[this.current].mp + "/"
				+ BattleAct.this.Battler[this.current].maxmp + " MP, "
				+ BattleAct.this.Battler[this.current].sp + "/"
				+ BattleAct.this.Battler[this.current].maxsp + " RP");
		if (this.imgActor[this.current].getDrawable() == null)
            BattleAct.this.imgActor[BattleAct.this.current].setImageResource(R.drawable.current);
		/*Drawable backSpr;
		if ((backSpr = this.imgActor[this.current].getBackground()) != null)
			backSpr.setColorFilter(this.crColor);*/
		this.refreshSkillBox();
	}

	private boolean checkEnd() {
		boolean b = false;
		if ((BattleAct.this.Battler[0] == null || BattleAct.this.Battler[0].hp < 1)
				&& (BattleAct.this.Battler[1] == null || BattleAct.this.Battler[1].hp < 1)
				&& (BattleAct.this.Battler[2] == null || BattleAct.this.Battler[2].hp < 1)
				&& (BattleAct.this.Battler[3] == null || BattleAct.this.Battler[3].hp < 1)) {
			this.endBt(-1);
			b = true;
		} else if ((BattleAct.this.Battler[4] == null || BattleAct.this.Battler[4].hp < 1)
				&& (BattleAct.this.Battler[5] == null || BattleAct.this.Battler[5].hp < 1)
				&& (BattleAct.this.Battler[6] == null || BattleAct.this.Battler[6].hp < 1)
				&& (BattleAct.this.Battler[7] == null || BattleAct.this.Battler[7].hp < 1)) {
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
		BattleAct.this.result = r;
		this.endingMsg(t, s);
	}

	private void endingMsg(String t, String s) {
		(new AlertDialog.Builder(this).setCancelable(false).setMessage(s)
				.setTitle(t).setPositiveButton("Exit",
				new DialogInterface.OnClickListener() {
					@Override
					public void onClick(DialogInterface arg0, int arg1) {
						Intent outcome = new Intent();
						outcome.putExtra("Outcome", BattleAct.this.result);
						if (BattleAct.cachedItem == null
								&& BattleAct.this.Item != null) {
							outcome.putParcelableArrayListExtra("Item",
									BattleAct.this.Item);
							for (int i = 0; i < 4 - BattleAct.this.difference; i++) {
								BattleAct.this.Battler[i].items = null;
								BattleAct.this.Battler[i].cleanStates();
							}
						}
						else for (int i = 0; i < 4 - BattleAct.this.difference; i++)
								BattleAct.this.Battler[i].cleanStates();
						if (BattleAct.cachedParty == null) {
							Actor[] party = new Actor[] {
									BattleAct.this.Battler[0],
									BattleAct.this.Battler[1],
									BattleAct.this.Battler[2],
									BattleAct.this.Battler[3] };
							outcome.putExtra("Party", party);
						}
						BattleAct.this.setResult(RESULT_OK, outcome);
						BattleAct.this.finish();
					}
				})).create().show();
	}

	private void playSpr(final int c, final int s, final Ability a, final boolean playSnd) {
		if (!BattleAct.this.Battler[c].sprName.equals("")) {
			final String pos = ((c > (BattleAct.this.Battler.length / 2) - 1 && BattleAct.this.surprise >= 0)
					|| (c < (BattleAct.this.Battler.length / 2) && BattleAct.this.surprise < 0))
					? "r" : "l";
			final AnimationDrawable sprAnim = Battler[c].getBtSprite(s, pos,
					this.waitTime, this);
			this.imgActor[c].postDelayed(new Runnable() {
				@Override
				public void run() {
					if (s == 2 || s < 0 || BattleAct.this.Battler[c].hp > 0
							|| !BattleAct.this.imgActor[c].getTag().equals(2)) {
                        if (a != null) {
							if (a.animSprId > 0) {
								final AnimationDrawable animDrw = (AnimationDrawable) BattleAct.this.getResources().getDrawable(a.animSprId);
								int animDur = 0;
								for (int i = 0; i < animDrw.getNumberOfFrames(); i++)
									animDur += animDrw.getDuration(i);
								BattleAct.this.imgActor[c].setImageDrawable(animDrw);
								animDrw.start();
								BattleAct.this.imgActor[c].postDelayed(new Runnable() {
									@Override
									public void run() {
										if (c == BattleAct.this.current)
											BattleAct.this.imgActor[c].setImageResource(R.drawable.current);
										else
											BattleAct.this.imgActor[c].setImageDrawable(null);
									}
								}, animDur);
							}
							if (playSnd)
								a.playSound(BattleAct.this);
						}
						sprAnim.stop();
						sprAnim.setOneShot(true);
                        //sprAnim.setColorFilter(c == BattleAct.this.current ? crColor : null);
						BattleAct.this.imgActor[c].setBackgroundDrawable(sprAnim);
						sprAnim.start();
						BattleAct.this.imgActor[c].setTag(s);
					}
				}
			}, this.waitTime[0]);
		}
	}

	private boolean imgClick(int i) {
		boolean a = false;
		int s = (i > (BattleAct.this.Battler.length / 2) - 1) ? i
				- this.difference : i;
		if (this.target == i) {
			this.executeSkill(this
					.getAIskill((i > BattleAct.this.Battler.length / 2 - 1 && this.current < BattleAct.this.Battler.length / 2)
							|| (i < Battler.length / 2 && this.current > Battler.length / 2 - 1) ? false
							: true));
			a = true;
		} else if (BattleAct.this.Battler[i] != null
				&& BattleAct.this.Battler[i].maxhp > 0)
			BattleAct.this.targetBox.setSelection(s);
		return a;
	}

	@Override
	public void onBackPressed() {
		this.endBt(-1);
	}

	@Override
	protected void onDestroy() {
		for (Ability ability : BattleAct.this.Skill)
			ability.cleanSound();
		for (Actor player : BattleAct.this.Battler)
			if (player != null) {
				player.setSprites(this, false, null);
				for (Ability ability : player.raceSkills)
					ability.cleanSound();
				for (Ability ability : player.jobSkills)
					ability.cleanSound();
			}
		BattleAct.cachedParty = null;
		BattleAct.cachedEnemy = null;
		BattleAct.cachedSkill = null;
		BattleAct.cachedItem = null;
		BattleAct.cachedScript = null;
		if (this.jsContext != null)
			Context.exit();
		if (this.songPlayer != null)
			songPlayer.release();
		super.onDestroy();
	}
}