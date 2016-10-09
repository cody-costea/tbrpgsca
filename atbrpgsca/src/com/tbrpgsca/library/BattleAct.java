/*
Copyright (C) 2016 Claudiu-Stefan Costea

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

import com.tbrpgsca.library.R;

import android.os.Bundle;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.drawable.AnimationDrawable;
import android.text.method.ScrollingMovementMethod;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.Spinner;
import android.widget.TextView;

public class BattleAct extends Activity {

	private static Ability[] Skill;
	private static Ability[] Item;
	private static Actor[] Battler;
	private static int result = 0, surprise;

	private int target, current = 7, difference = 0;
	private int waitTime[] = { 0, 0, 0 };

	private Spinner targetBox, skillBox, itemBox;
	private ArrayAdapter<String> targetList, skillList, itemList;
	private TextView skillCost, updText;
	private Button Act, Use, Auto, Escape;
	private ImageView imgActor[] = new ImageView[8];

	private ArrayList<Ability> cSkills = new ArrayList<Ability>();
	private ArrayList<Ability> crItems = new ArrayList<Ability>();

	public static void InitiateBattle(Activity act, Actor[] party,
			Actor[] enemy, Ability[] skill, Ability[] item, int surprise,
			boolean escapable) {
		Intent btInt = new Intent(act, BattleAct.class);
		btInt.putExtra("Escape", escapable);
		BattleAct.Battler = new Actor[8];
		BattleAct.Skill = skill;
		BattleAct.Item = item;
		BattleAct.surprise = surprise;
		for (int i = 0; i < party.length; i++)
			if (party[i] != null && party[i].maxhp > 0)
				BattleAct.Battler[i] = party[i];
		for (int k = 0; k < enemy.length; k++)
			if (enemy[k] != null && enemy[k].maxhp > 0) {
				BattleAct.Battler[k + 4] = new Actor(enemy[k]);
				BattleAct.Battler[k + 4].auto = 2;
			}
		act.startActivityForResult(btInt, 1);
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		if (BattleAct.Skill == null) {
			BattleAct.Skill = new Ability[] {
					new Ability("Attack", true, false, 0, 0, 0, 0, 0, 1, 10, 0,
							0, 0, 1, false, new boolean[] {}, new boolean[] {}),
					new Ability("Defend", true, true, 0, 0, 0, 0, 3, -1, 0, 0,
							-1, -2, 0, false, new boolean[] {},
							new boolean[] {}) };
		}
		if (BattleAct.Battler == null) {
			BattleAct.Item = new Ability[] { new Ability("Potion", true, -25,
					0, 0, 0, 0, new boolean[] {}, new boolean[] {}) };
			BattleAct.Item[0].qty = 10;
			BattleAct.Battler = new Actor[] {
					new Actor("Cody", "Half-Elf", "Hero", 1, 3, 50, 10, 15, 13,
							12, 9, 7, 5, 1, 1, 1, 1, 0, 1, 0, 0, new int[] {},
							BattleAct.Item, new Ability[] { new Ability(
									"Poison", true, true, 2, 0, 0, 5, 4, 2, 3,
									0, 9, 0, 1, false, new boolean[] { false,
											false, true }, new boolean[] {
											false, true }) }, false),
					new Actor("Stephanie", "Human", "Sorceress", 1, 3, 45, 20,
							5, 13, 12, 5, 5, 1, 1, 1, 1, 1, 0, 0, 0, 1,
							new int[] {}, BattleAct.Item,
							new Ability[] { new Ability("Fireball", true, true,
									2, 0, 7, 0, 2, 1, 15, 0, 0, 0, 2, false,
									new boolean[] {}, new boolean[] {}) },
							false),
					new Actor("George", "Half-Orc", "Templar", 1, 3, 47, 15,
							15, 13, 12, 5, 10, 7, 1, 1, 1, 0, 1, 0, 1, 0,
							new int[] {}, BattleAct.Item,
							new Ability[] { new Ability("Heal", false, true, 2,
									0, 7, 0, 3, -1, -25, 0, 0, 0, 6, false,
									new boolean[] { true }, new boolean[] {
											false, false, true, false, false,
											false, false, false, false, false,
											true }) }, false),
					new Actor("Victoria", "Elf", "Valkyrie", 1, 3, 55, 7, 13,
							17, 12, 5, 7, 3, 1, 1, 1, 1, 1, 0, 0, 0,
							new int[] {}, BattleAct.Item,
							new Ability[] { new Ability("Smash", true, false,
									2, 5, 0, 2, 0, 1, 12, 0, 1, 0, 1, false,
									new boolean[] { false, false, false, false,
											false, false, false, false, false,
											false, true }, new boolean[] {}) },
							false),
					new Actor("Lizard", "Lizard", "Lizard", 3, 3, 50, 15, 10,
							13, 12, 9, 7, 5, 1, 1, 1, 1, 0, 1, 0, 0,
							new int[] {}, null, new Ability[] { new Ability(
									"Fireball", true, true, 2, 0, 7, 0, 2, 1,
									15, 0, 0, 0, 2, false, new boolean[] {},
									new boolean[] {}) }, false),
					new Actor("Goblin", "Goblin", "Goblin", 3, 3, 45, 5, 20,
							13, 12, 5, 5, 1, 1, 1, 1, 1, 0, 0, 0, 1,
							new int[] {}, null, new Ability[] { new Ability(
									"Poison", true, true, 2, 0, 0, 5, 4, 2, 3,
									0, 9, 0, 1, false, new boolean[] { false,
											false, true }, new boolean[] {
											false, true }) }, false),
					new Actor("Ogre", "Ogre", "Ogre", 3, 3, 55, 7, 13, 17, 12,
							5, 7, 3, 1, 1, 1, 1, 1, 0, 0, 0, new int[] {},
							null, new Ability[] { new Ability("Smash", true,
									false, 2, 5, 0, 2, 0, 1, 12, 0, 1, 0, 1,
									false, new boolean[] { false, false, false,
											false, false, false, false, false,
											false, false, true },
									new boolean[] {}) }, false),
					new Actor("Troll", "Troll", "Troll", 3, 3, 47, 15, 15, 13,
							12, 5, 10, 7, 1, 1, 1, 0, 1, 0, 1, 0, new int[] {},
							null, new Ability[] { new Ability("Heal", false,
									true, 2, 0, 7, 0, 3, -1, -25, 0, 0, 0, 6,
									false, new boolean[] { true },
									new boolean[] { false, false, true, false,
											false, false, false, false, false,
											false, true }) }, false) };

			for (int i = 4; i < 8; i++)
				BattleAct.Battler[i].auto = 2;
			BattleAct.surprise = 0;
		}
		BattleAct.result = 0;
		this.setContentView(R.layout.activity_battle);
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
		Bundle extra = this.getIntent().getExtras();
		this.Escape
				.setEnabled((extra != null && extra.containsKey("Escape")) ? extra
						.getBoolean("Escape") : true);
		if (BattleAct.surprise < 0) {
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
			if (BattleAct.Battler[i] != null && BattleAct.Battler[i].maxhp > 0)
				this.playSpr(i, BattleAct.Battler[i].hp > 0 ? 0 : -1);
			else if (i < 4)
				this.difference++;
		}
		if (BattleAct.surprise < 0) {
			for (int i = 0; i < (BattleAct.Battler.length / 2)
					- this.difference; i++)
				if (BattleAct.Battler[i] != null)
					BattleAct.Battler[i].active = false;
			this.endTurn();
		} else if (BattleAct.surprise > 0) {
			for (int i = (BattleAct.Battler.length / 2); i < BattleAct.Battler.length; i++)
				if (BattleAct.Battler[i] != null)
					BattleAct.Battler[i].active = false;
			this.endTurn();
		} else
			this.endTurn();
		this.refreshTargetBox();
		this.targetBox.setSelection((BattleAct.Battler.length / 2)
				- this.difference);
		this.refreshSkillBox();
		this.refreshItemBox();
	}

	private void refreshTargetBox() {
		this.targetList.clear();
		String s;
		for (int i = 0; i < BattleAct.Battler.length; i++) {
			if (BattleAct.Battler[i] != null && BattleAct.Battler[i].maxhp > 0) {
				s = BattleAct.Battler[i].name + " (HP: "
						+ BattleAct.Battler[i].hp;
				if (i < (BattleAct.Battler.length / 2) - this.difference)
					s += ", MP: " + BattleAct.Battler[i].mp + ", RP: "
							+ BattleAct.Battler[i].sp;
				s += ")";
				this.targetList.add(s);
			}
		}
		this.targetList.notifyDataSetChanged();
	}

	private void refreshSkillBox() {
		this.skillList.clear();
		this.cSkills.clear();
		for (int i = 0; i < BattleAct.Skill.length; i++)
			this.addAbility(BattleAct.Skill[i], this.skillList, this.cSkills);
		if (BattleAct.Battler[this.current].raceSkills != null)
			for (int i = 0; i < Battler[this.current].raceSkills.length; i++)
				this.addAbility(BattleAct.Battler[this.current].raceSkills[i],
						this.skillList, this.cSkills);
		if (BattleAct.Battler[this.current].jobSkills != null)
			for (int i = 0; i < BattleAct.Battler[this.current].jobSkills
					.size(); i++)
				this.addAbility(
						BattleAct.Battler[this.current].jobSkills.get(i),
						this.skillList, this.cSkills);
		this.skillList.notifyDataSetChanged();
		if (this.skillBox.getSelectedItemPosition() > 1)
			this.skillBox.setSelection(0);
	}

	private void addAbility(Ability ability, ArrayAdapter<String> abilityList,
			ArrayList<Ability> abilities) {
		String trg;
		ability.checkQty();
		if (!(ability.getLvRq() > BattleAct.Battler[this.current].level)
				&& !(ability.mqty > -1 && ability.qty < 1)
				&& !(ability.mpc > BattleAct.Battler[this.current].mp)
				&& !(ability.spc > BattleAct.Battler[this.current].sp)
				&& !(ability.hpc >= BattleAct.Battler[this.current].hp)) {
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
		if (BattleAct.Battler[this.current].items != null)
			for (int i = 0; i < BattleAct.Battler[this.current].items.length; i++)
				this.addAbility(BattleAct.Battler[this.current].items[i],
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
				&& (BattleAct.Battler[3] != null && BattleAct.Battler[3].hp > 0)
				&& ((BattleAct.Battler[0] != null && BattleAct.Battler[0].hp > 0) || (BattleAct.Battler[2] != null && BattleAct.Battler[2].hp > 0))
				&& this.current > (BattleAct.Battler.length / 2) - 1
				&& this.difference == 0 && !ability.range)
			this.target = 3;
		if (this.target == 2
				&& (BattleAct.Battler[0] != null && BattleAct.Battler[0].hp > 0)
				&& ((BattleAct.Battler[1] != null && BattleAct.Battler[1].hp > 0) || (BattleAct.Battler[3] != null && BattleAct.Battler[3].hp > 0))
				&& this.current > (BattleAct.Battler.length / 2) - 1
				&& this.difference < 2 && !ability.range)
			this.target = 0;
		if (this.target == 4
				&& (BattleAct.Battler[6] != null && BattleAct.Battler[6].hp > 0)
				&& ((BattleAct.Battler[5] != null && BattleAct.Battler[5].hp > 0) || (BattleAct.Battler[2] != null && BattleAct.Battler[7].hp > 0))
				&& !ability.range)
			this.target = 6;
		if (this.target == 7
				&& ((BattleAct.Battler[6] != null && BattleAct.Battler[6].hp > 0) || (BattleAct.Battler[4] != null && BattleAct.Battler[4].hp > 0))
				&& (BattleAct.Battler[5] != null && BattleAct.Battler[5].hp > 0)
				&& !ability.range)
			this.target = 5;
		while ((BattleAct.Battler[this.target] != null && BattleAct.Battler[this.target].hp < 1)
				&& !(ability.state.length > 0 && ability.state[0] && ((ability.hpdmg < 0 && BattleAct.Battler[this.target].mres[ability.element] < 7) || (ability.hpdmg > 0 && BattleAct.Battler[this.target].mres[ability.element] == 7)))) {
			if (ability.hpdmg < 0)
				this.target--;
			else
				this.target++;
			if (this.target < 0)
				this.target = (BattleAct.Battler.length / 2) - 1
						- this.difference;
			if (this.target > BattleAct.Battler.length - 1 - this.difference)
				this.target = (BattleAct.Battler.length / 2) - this.difference;
		}
		int a = this.target, b = this.target;
		if (ability.trg > 1) {
			a = 0;
			b = 6;
		}
		if (this.current > (BattleAct.Battler.length / 2) - 1 - this.difference
				&& ability.trg < -1) {
			a = (BattleAct.Battler.length / 2);
			b = BattleAct.Battler.length - 1;
		}
		if (this.current < (BattleAct.Battler.length / 2) - this.difference
				&& ability.trg < -1) {
			a = 0;
			b = (BattleAct.Battler.length / 2) - 1;
		}
		if (this.target > (BattleAct.Battler.length / 2) - 1 - this.difference
				&& ability.trg == 1) {
			a = (BattleAct.Battler.length / 2);
			b = BattleAct.Battler.length - 1;
		}
		if (this.target < (BattleAct.Battler.length / 2) - this.difference
				&& ability.trg == 1) {
			a = 0;
			b = (BattleAct.Battler.length / 2) - 1;
		}
		if (ability.trg == -1) {
			a = this.current;
			b = this.current;
		}
		this.updText.append("\n" + Battler[this.current].name + " performs "
				+ ability.name);
		BattleAct.Battler[this.current].hp -= ability.hpc;
		BattleAct.Battler[this.current].mp -= ability.mpc;
		BattleAct.Battler[this.current].sp -= ability.spc;
		if (ability.qty > 0)
			ability.qty--;
		int trg;
		boolean act = true;
		for (int i = a; i <= b; i++)
			if ((BattleAct.Battler[i] != null && BattleAct.Battler[i].hp > 0)
					|| (ability.state.length > 0 && ability.state[0])) {
				if (BattleAct.Battler[i].reflect && ability.dmgtype == 2
						&& i != this.current) {
					this.updText.append(", which is reflected");
					trg = this.current;
				} else
					trg = i;
				if (act) {
					this.playSpr(this.current, 3);
					act = false;
				}
				this.updText.append(ability
						.execute(BattleAct.Battler[this.current],
								BattleAct.Battler[trg]));
				boolean ko = BattleAct.Battler[trg].hp < 1;
				if (trg != this.current)
					this.playSpr(trg, ko ? 2 : 1);
			}
		this.waitTime[0] = this.waitTime[1] > this.waitTime[2] ? this.waitTime[1]
				: this.waitTime[2];
		if (BattleAct.Battler[this.current].hp < 1)
			this.playSpr(this.current, 2);
		if (this.current < (BattleAct.Battler.length / 2) - this.difference) {
			BattleAct.Battler[this.current].exp++;
			BattleAct.Battler[this.current].levelUp();
		}
		this.updText.append(".");
	}

	private void useItem() {
		this.executeSkill(this.crItems.get(this.itemBox
				.getSelectedItemPosition()));
	}

	private void executeAI() {
		this.setAItarget(this.getAIskill(this.checkAIheal()));
	}

	private boolean checkAIheal() {
		boolean nHeal = false;
		int a = (this.current > (Battler.length / 2) - 1 - this.difference) ? (BattleAct.Battler.length / 2)
				- this.difference
				: 0;
		int b = (this.current < (Battler.length / 2) - this.difference) ? (BattleAct.Battler.length / 2)
				- 1 - this.difference
				: (BattleAct.Battler.length / 2) - 1 - this.difference;
		for (int i = a; i <= b; i++)
			if (BattleAct.Battler[i].hp < (BattleAct.Battler[i].maxhp / 3)) {
				nHeal = true;
				break;
			}
		if (nHeal) {
			nHeal = false;
			for (int i = 0; i < BattleAct.Battler[this.current].raceSkills.length; i++) {
				Ability s = BattleAct.Battler[this.current].raceSkills[i];
				if (s.hpdmg < 0 && s.mpc <= BattleAct.Battler[this.current].mp
						&& s.hpc <= BattleAct.Battler[this.current].hp
						&& s.spc <= BattleAct.Battler[this.current].sp
						&& BattleAct.Battler[this.current].level >= s.getLvRq()) {
					nHeal = true;
					break;
				}
			}
			if (!nHeal)
				for (int i = 0; i < BattleAct.Battler[this.current].jobSkills
						.size(); i++) {
					Ability s = BattleAct.Battler[this.current].jobSkills
							.get(i);
					if (s.hpdmg < 0
							&& s.mpc <= BattleAct.Battler[this.current].mp
							&& s.hpc <= BattleAct.Battler[this.current].hp
							&& s.spc <= BattleAct.Battler[this.current].sp
							&& BattleAct.Battler[this.current].level >= s
									.getLvRq()) {
						nHeal = true;
						break;
					}
				}
		}
		return nHeal;
	}

	private Ability getAIskill(boolean heal) {
		Ability s = (heal) ? BattleAct.Skill[1] : BattleAct.Skill[0];
		Ability a;
		for (int i = 0; i < BattleAct.Battler[this.current].jobSkills.size()
				|| i < BattleAct.Battler[this.current].raceSkills.length; i++)
			for (int j = 0; j < 2; j++) {
				if (j == 0 && i < Battler[this.current].raceSkills.length)
					a = Battler[this.current].raceSkills[i];
				else if (j == 1 && i < Battler[this.current].jobSkills.size())
					a = Battler[this.current].jobSkills.get(i);
				else
					continue;
				if (a.mpc <= BattleAct.Battler[this.current].mp
						&& a.hpc < BattleAct.Battler[this.current].hp
						&& a.spc <= BattleAct.Battler[this.current].sp
						&& BattleAct.Battler[this.current].level >= a.getLvRq())
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
		if (((this.current > (BattleAct.Battler.length / 2) - 1
				- this.difference) && (ability.hpdmg > 0))
				|| ((this.current < (BattleAct.Battler.length / 2)
						- this.difference) && (ability.hpdmg < 1))) {
			a = 0;
			b = (BattleAct.Battler.length / 2) - 1;
		} else {
			a = (BattleAct.Battler.length / 2);
			b = BattleAct.Battler.length - 1;
		}
		if (BattleAct.Battler[this.current].auto < 0)
			if (a == (BattleAct.Battler.length / 2)) {
				a = 0;
				b = (BattleAct.Battler.length / 2) - 1;
			} else {
				a = (BattleAct.Battler.length / 2);
				b = BattleAct.Battler.length - 1;
			}
		this.target = a;
		while ((BattleAct.Battler[this.target] == null || BattleAct.Battler[target].hp < 1)
				&& (ability.hpdmg > 1) && this.target < b)
			this.target++;

		for (int i = this.target; i <= b; ++i)
			if (BattleAct.Battler[i] != null
					&& (BattleAct.Battler[i].hp < BattleAct.Battler[target].hp)
					&& (BattleAct.Battler[i].hp > 0))
				this.target = i;
		this.executeSkill(ability);
	}

	@SuppressWarnings("deprecation")
	private void endTurn() {
		this.imgActor[this.current].setBackgroundDrawable(null);
		if (BattleAct.Battler[this.current] != null)
			BattleAct.Battler[this.current].active = false;

		boolean reset = true;
		for (int i = 0; i < BattleAct.Battler.length; i++) {
			if (BattleAct.Battler[i] != null && BattleAct.Battler[i].active) {
				reset = false;
				break;
			}
		}
		if (reset) {
			for (int i = 0; i < BattleAct.Battler.length; i++)
				if (BattleAct.Battler[i] != null && BattleAct.Battler[i].hp > 0) {
					this.updText.append(BattleAct.Battler[i].applyState(true));
					if (BattleAct.Battler[i].hp < 1)
						this.playSpr(i, 2);
				}
		}
		for (this.current = 0; this.current < BattleAct.Battler.length; this.current++) {
			if (BattleAct.Battler[this.current] != null
					&& BattleAct.Battler[this.current].active)
				break;
		}
		if (this.current < 6)
			for (int i = this.current + 1; i < BattleAct.Battler.length; i++)
				if (BattleAct.Battler[this.current] != null
						&& BattleAct.Battler[i] != null
						&& (BattleAct.Battler[this.current].agi < BattleAct.Battler[i].agi)
						&& BattleAct.Battler[i].active)
					this.current = i;

		if (!this.checkEnd()) {
			if (BattleAct.Battler[this.current] != null
					&& BattleAct.Battler[this.current].active) {
				if (BattleAct.Battler[this.current].auto == 0) {
					this.setCurrent();
					this.refreshItemBox();
				}
				BattleAct.Battler[this.current].applyState(false);
				if (BattleAct.Battler[this.current].auto != 0
						&& BattleAct.Battler[this.current].hp > 0) {
					this.executeAI();
					this.endTurn();
				}
				this.refreshTargetBox();
			} else
				this.endTurn();
		}
	}

	private void setCurrent() {
		this.skillCost.setText(BattleAct.Battler[this.current].name + ": "
				+ BattleAct.Battler[this.current].hp + "/"
				+ BattleAct.Battler[this.current].maxhp + " HP, "
				+ BattleAct.Battler[this.current].mp + "/"
				+ BattleAct.Battler[this.current].maxmp + " MP, "
				+ BattleAct.Battler[this.current].sp + "/"
				+ BattleAct.Battler[this.current].maxsp + " RP");
		this.imgActor[this.current].setBackgroundResource(R.drawable.current);
		this.refreshSkillBox();
	}

	private boolean checkEnd() {
		boolean b = false;
		if ((BattleAct.Battler[0] == null || BattleAct.Battler[0].hp < 1)
				&& (BattleAct.Battler[1] == null || BattleAct.Battler[1].hp < 1)
				&& (BattleAct.Battler[2] == null || BattleAct.Battler[2].hp < 1)
				&& (BattleAct.Battler[3] == null || BattleAct.Battler[3].hp < 1)) {
			this.endBt(-1);
			b = true;
		} else if ((BattleAct.Battler[4] == null || BattleAct.Battler[4].hp < 1)
				&& (BattleAct.Battler[5] == null || BattleAct.Battler[5].hp < 1)
				&& (BattleAct.Battler[6] == null || BattleAct.Battler[6].hp < 1)
				&& (BattleAct.Battler[7] == null || BattleAct.Battler[7].hp < 1)) {
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
		BattleAct.result = r;
		this.endingMsg(t, s);
	}

	private void endingMsg(String t, String s) {
		AlertDialog.Builder msg = new AlertDialog.Builder(this);
		msg.setCancelable(false);
		msg.setMessage(s);
		msg.setTitle(t);
		msg.setPositiveButton("Exit", new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface arg0, int arg1) {
				Intent outcome = new Intent();
				outcome.putExtra("Outcome", BattleAct.result);
				BattleAct.this.setResult(RESULT_OK, outcome);
				BattleAct.this.finish();
			}
		});
		AlertDialog dialog = msg.create();
		dialog.show();
	}

	private void playSpr(final int c, final int s) {
		if (BattleAct.Battler[c].getSprName() != "") {
			final String pos = ((c > (BattleAct.Battler.length / 2) - 1 && BattleAct.surprise >= 0) || (c < (BattleAct.Battler.length / 2) && BattleAct.surprise < 0)) ? "r"
					: "l";
			final AnimationDrawable sprAnim = Battler[c].getBtSprite(s, pos,
					this.waitTime, this);
			this.imgActor[c].postDelayed(new Runnable() {
				@Override
				public void run() {
					if (s == 2 || s < 0 || BattleAct.Battler[c].hp > 0
							|| !BattleAct.this.imgActor[c].getTag().equals(2)) {
						sprAnim.stop();
						sprAnim.setOneShot(true);
						BattleAct.this.imgActor[c].setImageDrawable(sprAnim);
						sprAnim.start();
						BattleAct.this.imgActor[c].setTag(s);
					}
				}
			}, this.waitTime[0]);
		}
	}

	private boolean imgClick(int i) {
		boolean a = false;
		int s = (i > (BattleAct.Battler.length / 2) - 1) ? i - this.difference
				: i;
		if (this.target == i) {
			this.executeSkill(this
					.getAIskill((i > BattleAct.Battler.length / 2 - 1 && this.current < BattleAct.Battler.length / 2)
							|| (i < Battler.length / 2 && this.current > Battler.length / 2 - 1) ? false
							: true));
			a = true;
		} else if (BattleAct.Battler[i] != null
				&& BattleAct.Battler[i].maxhp > 0)
			BattleAct.this.targetBox.setSelection(s);
		return a;
	}

	private OnClickListener cAction = new OnClickListener() {
		@Override
		public void onClick(final View v) {
			BattleAct.this.waitTime[0] = 0;
			boolean act = true;
			boolean surprised = BattleAct.surprise < 0;
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

	@Override
	public void onBackPressed() {
		this.endBt(-1);
	}

	@Override
	protected void onDestroy() {
		for (Actor player : BattleAct.Battler)
			if (player != null)
				player.setSprites(this, false, null);
		BattleAct.Battler = null;
		BattleAct.Skill = null;
		BattleAct.Item = null;
		super.onDestroy();
	}

}