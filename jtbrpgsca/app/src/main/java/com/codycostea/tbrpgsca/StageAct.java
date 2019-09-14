/*
Copyright (C) AD 2013-2019 Claudiu-Stefan Costea

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

import java.util.Hashtable;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.text.SpannableString;
import android.text.util.Linkify;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;

public class StageAct extends Activity {

	private Performance[] Skill;
	private Costume[] pcRace;
	private Costume[] pcJob;
	private Actor[] Player;
	//private Actor[] Party;
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

		final StateMask[] state = new StateMask[] {
				new StateMask(1, "Regen", null, false, false, false, false, -1, 0, -10, 0, 0,
						0, 0, 0, 0, 2, 0, 0, 0, 0, false, null, null, null, null, null),
				new StateMask(2, "Poison", null, false, false, false, false, 10, 0, 7, 0, 2,
						0, 0, 0, 0, -2, 0, 0, 0, 0, false, null, null, null, null, null),
				new StateMask(3, "Clarity", null, false, false, false, false, -1, 0, 0, -7, 0,
						0, 0, 0, 0, 0, 1, 1, 0, 0, false, null, null, null, null, null),
				new StateMask(4, "Dizziness", null, false, false, false, false, 3, 0, 0, 7, 0,
						0, 0, 0, 0, 0, -1, -1, 0, 0, false, null, null, null, null, null),
				new StateMask(5, "Vigour", null, false, false, false, false, -1, 0, 0, 0, -7,
						0, 0, 0, 1, 0, 0, 0, 1, 0, false, null, null, null, null, null),
				new StateMask(6, "Weakness", null, false, false, false, false, 5, 0, 0, 0, 7,
						0, 0, 0, -1, 0, 0, 0, -1, 0, false, null, null, null, null, null),
				new StateMask(7, "Berserk", null, false, true, false, false, 7, 0, 0, 0, 0,
						0, 0, 0, 5, -3, 0, 0, 3, 0, false, null, null, null, null, null),
				new StateMask(8, "Confusion", null, false, false, true, false, 3, 2, 0, 0, 0,
						0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null, null),
				new StateMask(9, "Sleep", null, true, false, false, false, 5, 1, 0, 0, 0,
						0, 0, 0, 0, -3, 0, 0, -3, 0, false, null, null, null, null, null),
				new StateMask(10, "Stun", null, true, false, false, false, 1, 0, 0, 0, 0,
						0, 0, 0, 0, -1, 0, 0, -1, 0, false, null, null, null, null, null),
				new StateMask(11, "Reflect", null, false, false, false, true, 7, 0, 0, 0, 0,
						0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null, null)
		};

		final StateMask[] confsleep = new StateMask[] {state[7], state[8]};
		final StateMask[] cure = new StateMask[] {state[1], state[3], state[5], state[6], state[7], state[8], state[9]};
		final StateMask[] dizziness = new StateMask[] {state[3]};
		final StateMask[] poison = new StateMask[] {state[1]};
		final StateMask[] regen = new StateMask[] {state[0]};
		final StateMask[] madness = new StateMask[] {state[3], state[6], state[7], state[8], state[9]};
		final StateMask[] clarity = new StateMask[] {state[2]};
		final StateMask[] weakness = new StateMask[] {state[5]};
		final StateMask[] vigour = new StateMask[] {state[4]};
		final StateMask[] tregen = new StateMask[] {state[0], state[2], state[4]};
		final StateMask[] berserk = new StateMask[] {state[6]};
		final StateMask[] stun = new StateMask[] {state[9]};
		final StateMask[] reflect = new StateMask[] {state[10]};
		final StateMask[] confusion = new StateMask[] {state[7]};
		final StateMask[] confclarity = new StateMask[] {state[2], state[7], state[8]};
		final StateMask[] conftregen = new StateMask[] {state[0], state[2], state[4], state[7], state[8]};
		final StateMask[] sleep = new StateMask[] {state[8]};
		final StateMask[] tpoison = new StateMask[] {state[1], state[3], state[5]};
		final StateMask[] dizzystun = new StateMask[] {state[3], state[9]};

		final Integer ELEMENT_NONE = new Integer(0);
		final Integer ELEMENT_PHYSICAL = new Integer(1);
		final Integer ELEMENT_FIRE = new Integer(2);
		final Integer ELEMENT_WATER = new Integer(3);
		final Integer ELEMENT_WIND = new Integer(4);
		final Integer ELEMENT_EARTH = new Integer(5);
		final Integer ELEMENT_PSYCHIC = new Integer(6);
		final Integer ELEMENT_LIGHT = new Integer(7);

		final Performance[] skill = this.Skill = new Performance[] {
				new Performance(1, "Attack", 0, 0, false, false, 0, 0, 0, 0, Performance.DMG_TYPE_ATK, 1, 10, 0, -3, Performance.TRG_ONE, ELEMENT_PHYSICAL, -1, -1, false, false, null, confsleep),
				new Performance(2, "Defend", 0, 0, false, true, 0, 0, 0, 0, Performance.DMG_TYPE_SPI, 1, 0, -1, -2, Performance.TRG_SELF, ELEMENT_NONE, -1, -1, false, false, null, null),
				new Performance(3, "Heal", 0, 0, false, true, 1, 0, 3, 0, Performance.DMG_TYPE_SPI, 1, -25, 0, 0, Performance.TRG_ONE, ELEMENT_PSYCHIC, -1, -1, false, true, null, null),
				new Performance(4, "Meditate", 0, 0, false, true, 1, 0, 0, 0, Performance.DMG_TYPE_SPI, 1, -1, -7, 10, Performance.TRG_SELF, ELEMENT_PSYCHIC, -1, -1, false, false, null, dizziness),
				new Performance(5, "Cure", 0, 0, false, true, 3, 0, 7, 0, Performance.DMG_TYPE_SPI, 1, -17, 0, 0, Performance.TRG_ONE, ELEMENT_PSYCHIC, -1, -1, false, true, null, cure),
				new Performance(6, "Clariy", 0, 0, false, true, 3, 0, 0, 7, Performance.DMG_TYPE_SPI, 1, 0, -3, 0, Performance.TRG_ONE, ELEMENT_PSYCHIC, -1, -1, false, false, clarity, madness),
				new Performance(7, "Regen", 0, 0, false, true, 4, 0, 10, 0, Performance.DMG_TYPE_SPI, 1, -37, 0, 0, Performance.TRG_ONE, ELEMENT_PSYCHIC, -1, -1, false, true, regen, poison),
				new Performance(8, "Prayer", 0, 0, false, true, 5, 0, 7, 0, Performance.DMG_TYPE_SPI, 1, -23, 0, 0, Performance.TRG_ENEMY, ELEMENT_PSYCHIC, -1, -1, false, true, null, null),
				new Performance(9, "Smite", 0, 0, false, false, 1, 1, 0, 2, Performance.DMG_TYPE_ATK | Performance.DMG_TYPE_DEF, 1, 4, 3, -4, Performance.TRG_ONE, ELEMENT_PHYSICAL, -1, -1, false, false, null, confclarity),
				new Performance(10, "Hit", 0, 0, false, false, 1, 3, 0, 1, Performance.DMG_TYPE_ATK, 1, 12, 0, -4, Performance.TRG_ONE, ELEMENT_PHYSICAL, -1, -1, false, false, null, confsleep),
				new Performance(11, "Bash", 0, 0, false, false, 3, 3, 0, 5, Performance.DMG_TYPE_ATK | Performance.DMG_TYPE_DEF, 1, 7, 5, -5, Performance.TRG_ONE, ELEMENT_PHYSICAL, -1, -1, false, false, dizziness, confclarity),
				new Performance(12, "Smash", 0, 0, false, false, 3, 5, 0, 3, Performance.DMG_TYPE_ATK, 1, 18, 0, -7, Performance.TRG_ONE, ELEMENT_PHYSICAL, -1, -1, false, false, null, confsleep),
				new Performance(13, "Berserk", 0, 0, false, false, 4, 7, 0, 4, Performance.DMG_TYPE_SPI, 1, 0, 0, 0, Performance.TRG_SELF, ELEMENT_PSYCHIC, -1, -1, false, false, berserk, weakness),
				new Performance(14, "Shock", 0, 0, false, false, 4, 4, 0, 7, Performance.DMG_TYPE_ATK | Performance.DMG_TYPE_DEF, 1, 10, 5, -9, Performance.TRG_ONE, ELEMENT_PHYSICAL, -1, -1, false, false, dizzystun, confclarity),
				new Performance(15, "Crush", 0, 0, false, false, 5, 7, 4, 0, Performance.DMG_TYPE_ATK, 1, 25, 0, -11, Performance.TRG_ONE, ELEMENT_PHYSICAL, -1, -1, false, false, stun, confsleep),
				new Performance(16, "Strike", 0, 0, false, true, 1, 0, 0, 0, Performance.DMG_TYPE_ATK | Performance.DMG_TYPE_AGI, 1, 13, 0, -3, Performance.TRG_ONE, ELEMENT_PHYSICAL, 5, 3, false, false, null, confsleep),
				new Performance(17, "Steal", 0, 0, true, true, 1, 0, 0, 0, Performance.DMG_TYPE_WIS, 1, 0, 0, 0, Performance.TRG_ONE, ELEMENT_PHYSICAL, 5, 3, false, false, null, null),
				new Performance(18, "Dash", 0, 0, false, true, 3, 0, 0, 0, Performance.DMG_TYPE_ATK | Performance.DMG_TYPE_AGI, 1, 15, 0, -5, Performance.TRG_ONE, ELEMENT_PHYSICAL, 3, 7, false, false, null, confsleep),
				new Performance(19, "Poison", 0, 0, false, true, 3, 0, 0, 0, Performance.DMG_TYPE_WIS, 2, 5, 0, 2, Performance.TRG_ONE, ELEMENT_PHYSICAL, 3, 7, false, false, poison, regen),
				new Performance(20, "Mug", 0, 0, true, true, 4, 0, 0, 0, Performance.DMG_TYPE_ATK | Performance.DMG_TYPE_AGI, 2, 15, 0, -7, Performance.TRG_ONE, ELEMENT_PHYSICAL, 1, 5, false, false, null, confsleep),
				new Performance(21, "Toxic Gas", 0, 0, false, true, 4, 0, 10, 0, Performance.DMG_TYPE_WIS, 3, 1, 1, 1, Performance.TRG_ENEMY, ELEMENT_PHYSICAL, 3, 7, false, false, tpoison, tregen),
				new Performance(22, "Cheer", 0, 0, false, true, 4, 0, 10, 0, Performance.DMG_TYPE_SPI, -1, 0, 0, -5, Performance.TRG_PARTY, ELEMENT_PSYCHIC, 3, 7, false, false, vigour, cure),
				new Performance(23, "Venom Blade", 0, 0, false, false, 5, 0, 0, 10, Performance.DMG_TYPE_ATK | Performance.DMG_TYPE_AGI, 1, 17, 0, -9, Performance.TRG_ONE, ELEMENT_PHYSICAL, 3, 7, false, false, poison, conftregen),
				new Performance(24, "Absorb", 0, 0, false, true, 1, 0, 0, 3, Performance.DMG_TYPE_WIS, 1, 0, 7, -3, Performance.TRG_ONE, ELEMENT_PSYCHIC, -1, -1, true, false, null, null),
				new Performance(25, "Drain", 0, 0, false, true, 3, 0, 10, 0, Performance.DMG_TYPE_WIS, 1, 15, 0, -3, Performance.TRG_ONE, ELEMENT_PSYCHIC, -1, -1, true, false, null, null),
				new Performance(26, "Fireball", 0, 0, false, true, 1, 0, 3, 0, Performance.DMG_TYPE_WIS, 1, 11, 0, -5, Performance.TRG_ONE, ELEMENT_FIRE, -1, -1, false, false, null, sleep),
				new Performance(27, "Iceshard", 0, 0, false, true, 1, 0, 3, 0, Performance.DMG_TYPE_WIS, 1, 11, 0, -5, Performance.TRG_ONE, ELEMENT_WATER, -1, -1, false, false, null, sleep),
				new Performance(28, "Lighting", 0, 0, false, true, 1, 0, 3, 0, Performance.DMG_TYPE_WIS, 1, 11, 0, -5, Performance.TRG_ONE, ELEMENT_WIND, -1, -1, false, false, null, sleep),
				new Performance(29, "Rock", 0, 0, false, true, 1, 0, 3, 0, Performance.DMG_TYPE_WIS, 1, 11, 0, -5, Performance.TRG_ONE, ELEMENT_EARTH, -1, -1, false, false, null, sleep),
				new Performance(30, "Darkness", 0, 0, false, true, 1, 0, 3, 0, Performance.DMG_TYPE_WIS, 1, 11, 0, -5, Performance.TRG_ONE, ELEMENT_PSYCHIC, -1, -1, false, false, null, sleep),
				new Performance(31, "Flame", 0, 0, false, true, 3, 0, 5, 0, Performance.DMG_TYPE_WIS, 1, 15, 0, -7, Performance.TRG_ENEMY, ELEMENT_FIRE, -1, -1, false, false, null, sleep),
				new Performance(32, "Blizzard", 0, 0, false, true, 3, 0, 5, 0, Performance.DMG_TYPE_WIS, 1, 15, 0, -7, Performance.TRG_ENEMY, ELEMENT_WATER, -1, -1, false, false, null, sleep),
				new Performance(33, "Storm", 0, 0, false, true, 3, 0, 5, 0, Performance.DMG_TYPE_WIS, 1, 15, 0, -7, Performance.TRG_ENEMY, ELEMENT_WIND, -1, -1, false, false, null, sleep),
				new Performance(34, "Earthquake", 0, 0, false, true, 3, 0, 5, 0, Performance.DMG_TYPE_WIS, 1, 15, 0, -7, Performance.TRG_ENEMY, ELEMENT_EARTH, -1, -1, false, false, null, sleep),
				new Performance(35, "Eclipse", 0, 0, false, true, 3, 0, 5, 0, Performance.DMG_TYPE_WIS, 1, 13, 0, -7, Performance.TRG_ENEMY, ELEMENT_PSYCHIC, -1, -1, false, false, null, sleep),
				new Performance(36, "Flare", 0, 0, false, true, 5, 0, 12, 0, Performance.DMG_TYPE_WIS, 2, 25, 0, -9, Performance.TRG_ONE, ELEMENT_NONE, -1, -1, false, false, null, sleep),
				new Performance(37, "Light Ray", 0, 0, false, true, 1, 0, 3, 0, Performance.DMG_TYPE_SPI, 1, 11, 0, -5, Performance.TRG_ONE, ELEMENT_LIGHT, -1, -1, false, false, null, sleep),
				new Performance(38, "Light Burst", 0, 0, false, true, 3, 0, 5, 0, Performance.DMG_TYPE_SPI, 1, 15, 0, -7, Performance.TRG_ENEMY, ELEMENT_LIGHT, -1, -1, false, false, null, sleep),
				new Performance(39, "Confusion", 0, 0, false, true, 5, 0, 15, 0, Performance.DMG_TYPE_WIS, 1, 0, 0, 0, Performance.TRG_ONE, ELEMENT_PSYCHIC, -1, -1, false, false, confusion, clarity),
				new Performance(40, "Sleep", 0, 0, false, true, 4, 0, 0, 17, Performance.DMG_TYPE_WIS, 1, 3, 0, 17, Performance.TRG_ONE, ELEMENT_PHYSICAL, -1, -1, false, false, sleep, clarity),
				new Performance(41, "Slash", 0, 0, false, true, 5, 0, 10, 0, Performance.DMG_TYPE_ATK | Performance.DMG_TYPE_AGI, 1, 15, 0, 0, Performance.TRG_ENEMY, ELEMENT_PHYSICAL, 3, 7, false, false, null, confsleep),
				new Performance(42, "Fire Wpn", 0, 0, false, false, 2, 0, 3, 2, Performance.DMG_TYPE_ATK | Performance.DMG_TYPE_WIS, 1, 17, 0, -7, Performance.TRG_ONE, ELEMENT_FIRE, -1, -1, false, false, null, confsleep),
				new Performance(43, "Ice Wpn", 0, 0, false, false, 2, 0, 3, 2, Performance.DMG_TYPE_ATK | Performance.DMG_TYPE_WIS, 1, 17, 0, -7, Performance.TRG_ONE, ELEMENT_WATER, -1, -1, false, false, null, confsleep),
				new Performance(44, "Electric Wpn", 0, 0, false, false, 2, 0, 3, 2, Performance.DMG_TYPE_ATK | Performance.DMG_TYPE_WIS, 1, 17, 0, -7, Performance.TRG_ONE, ELEMENT_WIND, -1, -1, false, false, null, confsleep),
				new Performance(45, "Stone Wpn", 0, 0, false, false, 2, 0, 3, 2, Performance.DMG_TYPE_ATK | Performance.DMG_TYPE_WIS, 1, 17, 0, -7, Performance.TRG_ONE, ELEMENT_EARTH, -1, -1, false, false, null, confsleep),
				new Performance(46, "Dark Wpn", 0, 0, false, false, 2, 0, 3, 2, Performance.DMG_TYPE_ATK | Performance.DMG_TYPE_WIS, 1, 17, 0, -7, Performance.TRG_ONE, ELEMENT_PSYCHIC, -1, -1, false, false, null, confsleep),
				new Performance(47, "Vampiric Wpn", 0, 0, false, false, 5, 0, 10, 0, Performance.DMG_TYPE_ATK | Performance.DMG_TYPE_AGI | Performance.DMG_TYPE_WIS,
						1, 21, 0, -9, Performance.TRG_ONE, ELEMENT_PSYCHIC, 3, 7, true, false, null, confsleep),
				new Performance(48, "Reflect", 0, 0, false, true, 5, 0, 10, 0, Performance.DMG_TYPE_SPI, 1, 0, 0, 0, Performance.TRG_ONE, ELEMENT_PSYCHIC, -1, -1, false, false, reflect, null),
				new Performance(49, "Meteor", 0, 0, false, true, 5, 0, 17, 0, Performance.DMG_TYPE_WIS, 1, 19, 0, -11, Performance.TRG_ENEMY, ELEMENT_PHYSICAL, -1, -1, false, false, null, sleep),
				new Performance(50, "Syphon", 0, 0, false, true, 4, 0, 15, 0, Performance.DMG_TYPE_WIS, 1, 13, 0, -3, Performance.TRG_ENEMY, ELEMENT_PSYCHIC, -1, -1, true, false, null, null),
				new Performance(51, "Dragon Breath", 0, 0, false, false, 4, 0, 13, 7, Performance.DMG_TYPE_ATK | Performance.DMG_TYPE_WIS, 1, 15, 0, -11, Performance.TRG_ONE, ELEMENT_PHYSICAL, -1, -1, false, false, null, confsleep),
				new Performance(52, "Light Wpn", 0, 0, false, false, 2, 0, 3, 2, Performance.DMG_TYPE_ATK | Performance.DMG_TYPE_DEF | Performance.DMG_TYPE_SPI,
						1, 17, 0, -5, Performance.TRG_ONE, ELEMENT_LIGHT, -1, -1, false, false, null, confsleep),
				new Performance(53, "Heal", 0, 0, false, true, 1, 0, 3, 0, Performance.DMG_TYPE_WIS, 1, -25, 0, 0, Performance.TRG_ONE, ELEMENT_LIGHT, -1, -1, false, false, null, null),
				new Performance(54, "Meditate", 0, 0, false, true, 1, 0, 0, Performance.DMG_TYPE_WIS, 2, 1, -3, -7, 0, Performance.TRG_SELF, ELEMENT_LIGHT, -1, -1, false, false, null, dizziness),
				new Performance(55, "Cure", 0, 0, false, true, 3, 0, 7, 0, Performance.DMG_TYPE_WIS, 1, -17, 0, 0, Performance.TRG_ONE, ELEMENT_LIGHT, -1, -1, false, true, null, cure),
				new Performance(56, "Clarity", 0, 0, false, true, 3, 0, 0, 7, Performance.DMG_TYPE_WIS, 1, 0, -3, 0, Performance.TRG_ONE, ELEMENT_LIGHT, -1, -1, false, false, clarity, madness),
				new Performance(57, "Absorb", 0, 0, false, true, 1, 0, 0, 3, Performance.DMG_TYPE_SPI, 1, 0, 7, -3, Performance.TRG_ONE, ELEMENT_LIGHT, -1, -1, true, false, null, null)
		};

		final Performance[] items = new Performance[] {
				new Performance(1, "Potion", 0, 0, false, true, 1, 0, 0, 0, 0, 0, -25, 0, 0, Performance.TRG_ONE, ELEMENT_NONE, -1, -1, false, false, null, null),
				new Performance(2, "Ether", 0, 0, false, true, 1, 0, 0, 0, 0, 0, 0, -10, 0, Performance.TRG_ONE, ELEMENT_NONE, -1, -1, false, false, null, null),
				new Performance(3, "Tonic", 0, 0, false, true, 1, 0, 0, 0, 0, 0, 0, 0, -10, Performance.TRG_ONE, ELEMENT_NONE, -1, -1, false, false, null, null),
				new Performance(4, "Antidote", 0, 0, false, true, 1, 0, 0, 0, 0, 0, 0, 0, 0, Performance.TRG_ONE, ELEMENT_NONE, -1, -1, false, false, null, poison),
				new Performance(9, "Hi-Potion", 0, 0, false, true, 1, 0, 0, 0, 0, 0, -50, 0, 0, Performance.TRG_ONE, ELEMENT_NONE, -1, -1, false, false, null, null),
				new Performance(5, "Hi-Ether", 0, 0, false, true, 1, 0, 0, 0, 0, 0, 0, -25, 0, Performance.TRG_ONE, ELEMENT_NONE, -1, -1, false, false, null, dizziness),
				new Performance(6, "Hi-Tonic", 0, 0, false, true, 1, 0, 0, 0, 0, 0, 0, -20, 0, Performance.TRG_ONE, ELEMENT_NONE, -1, -1, false, false, vigour, weakness),
				new Performance(7, "Panacea", 0, 0, false, true, 1, 0, 0, 0, 0, 0, 0, 0, 0, Performance.TRG_ONE, ELEMENT_NONE, -1, -1, false, false, null, cure),
				new Performance(8, "Elixir", 0, 0, false, true, 1, 0, 0, 0, 0, 0, -100, -100, -100, Performance.TRG_ONE, ELEMENT_NONE, -1, -1, false, true, null, null)
		};

		final Integer itemsNr = new Integer(15);
		final Hashtable itemsMap = new Hashtable(items.length);
		for (int i = 0; i < items.length; i++) {
			itemsMap.put(items[i], itemsNr);
		}

		final Performance[] skills = new Performance[] {skill[0], skill[1]};

		final Costume[] race = this.pcRace = new Costume[] {
				new Costume(1, "Elf", null, 40, 25, 10, 7, 5, 15, 12, 11, 0, false, null, skills, null, null),
				new Costume(2, "Human", null, 47, 15, 13, 9, 11, 9, 11, 10, 0, false, null, skills, null, null),
				new Costume(3, "Half-Orc", null, 55, 7, 13, 17, 12, 5, 7, 9, 0, false, null, skills, null, null),
				new Costume(4, "Gnome", null, 40, 15, 20, 12, 8, 10, 5, 15, 0, false, null, skills, null, null)
		};

		final Costume[] job = this.pcJob = new Costume[] {
				new Costume(15, "Hero", "hero", 1, 1, 1, 0, 0, 0, 0, 0, 0, false, null, new Performance[] {skill[8], skill[9], skill[10], skill[11], skill[23], skill[24], skill[25], skill[26], skill[27], skill[28], skill[29], skill[41], skill[42], skill[43], skill[44], skill[45], skill[30], skill[31], skill[32], skill[33], skill[34], skill[36], skill[51], skill[37], skill[2], skill[3], skill[4], skill[5], skill[15], skill[16], skill[17], skill[18]}, null, null),
				new Costume(1, "Berserker", "berserker", 1, 0, 0, 1, 0, 0, 0, 1, 0, false, null, new Performance[] {skill[8], skill[9], skill[10], skill[11], skill[12], skill[14]}, null, null),
				new Costume(2, "Wizard", "wizard", 1, 0, 0, 0, 1, 0, 1, 0, 0, false, null, new Performance[] {skill[23], skill[24], skill[25], skill[26], skill[27], skill[28], skill[29], skill[30], skill[31], skill[32], skill[33], skill[34], skill[35], skill[38]}, null, null),
				new Costume(3, "Hesychast", "hesychast", 0, 1, 0, 0, 1, 0, 1, 0, 0, false, null, new Performance[] {skill[2], skill[3], skill[4], skill[5], skill[6], skill[7]}, null, null),
				new Costume(4, "Spy", "spy", 0, 0, 1, 1, 0, 0, 0, 1, 0, false, null, new Performance[] {skill[15], skill[16], skill[17], skill[18], skill[19], skill[38]}, null, null),
				new Costume(5, "Alchemist", "alchemist", 0, 1, 0, 0, 0, 1, 0, 1, 0, false, null, new Performance[] {skill[23], skill[24], skill[25], skill[26], skill[27], skill[28], skill[30], skill[31], skill[32], skill[33], skill[15], skill[16], skill[17], skill[18], skill[20]}, null, null),
				new Costume(6, "Dragoon", "dragoon", 1, 0, 0, 1, 0, 1, 0, 0, 0, false, null, new Performance[] {skill[8], skill[9], skill[10], skill[11], skill[23], skill[24], skill[25], skill[26], skill[27], skill[28], skill[30], skill[31], skill[32], skill[33], skill[41], skill[42], skill[43], skill[44], skill[50]}, null, null),
				new Costume(7, "Knight", "knight", 1, 0, 0, 0, 1, 0, 1, 0, 0, false, null, new Performance[] {skill[2], skill[3], skill[4], skill[5], skill[8], skill[9], skill[10], skill[11], skill[13]}, null, null),
				new Costume(8, "Ranger", "ranger", 0, 0, 1, 0, 0, 0, 1, 1, 0, false, null, new Performance[] {skill[2], skill[3], skill[4], skill[5], skill[15], skill[16], skill[17], skill[18], skill[21]}, null, null),
				new Costume(9, "Shaman", "shaman", 0, 1, 0, 0, 0, 1, 1, 0, 0, false, null, new Performance[] {skill[52], skill[53], skill[54], skill[55], skill[23], skill[24], skill[29], skill[34], skill[49]}, null, null),
				new Costume(10, "Corsair", "swashbuckler", 0, 0, 1, 1, 0, 0, 0, 1, 0, false, null, new Performance[] {skill[8], skill[9], skill[10], skill[11], skill[15], skill[16], skill[17], skill[18], skill[22]}, null, null),
				new Costume(11, "Reaver", "reaver", 1, 0, 0, 1, 0, 0, 0, 0, 0, false, null, new Performance[] {skill[8], skill[9], skill[11], skill[23], skill[24], skill[29], skill[15], skill[16], skill[18], skill[45], skill[46]}, null, null),
				new Costume(12, "Ninja", "ninja", 0, 0, 1, 0, 0, 0, 0, 1, 0, false, null, new Performance[] {skill[8], skill[9], skill[11], skill[15], skill[16], skill[17], skill[2], skill[3], skill[5], skill[40]}, null, null),
				new Costume(13, "Crusader", "templar", 1, 0, 0, 0, 0, 0, 1, 0, 0, false, null, new Performance[] {skill[8], skill[9], skill[10], skill[2], skill[3], skill[4], skill[56], skill[36], skill[51], skill[37], skill[47]}, null, null),
				new Costume(14, "Druid", "druid", 0, 1, 0, 0, 0, 1, 0, 0, 0, false, null, new Performance[] {skill[23], skill[25], skill[26], skill[27], skill[28], skill[30], skill[31], skill[32], skill[33], skill[52], skill[53], skill[54], skill[15], skill[16], skill[18], skill[48]}, null, null),
				new Costume(16, "Sorceress", "sorceress", 1, 0, 0, 0, 1, 0, 1, 0, 0, false, null, new Performance[] {skill[23], skill[24], skill[25], skill[26], skill[27], skill[28], skill[29], skill[30], skill[31], skill[32], skill[33], skill[34], skill[35], skill[38]}, null, null),
				new Costume(17, "Valkyrie", "valkyrie", 1, 0, 0, 1, 0, 0, 0, 1, 0, false, null, new Performance[] {skill[8], skill[9], skill[10], skill[11], skill[12], skill[14]}, null, null)
		};

		final Actor[] party = this.Player = new Actor[] {
				new Actor(1, "Cody", race[2], job[0], 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null, itemsMap),
				new Actor(3, "Victoria", race[0], job[16], 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null, itemsMap),
				new Actor(5, "Stephanie", race[3], job[15], 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null, itemsMap),
				new Actor(7, "George", race[1], job[13], 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null, itemsMap)
		};

		final Actor[] enemy = new Actor[] {
				new Actor(2, "Goblin", race[2], job[12], 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null, null),
				new Actor(4, "Lizard", race[0], job[6], 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null, null),
				new Actor(6, "Troll", race[3], job[9], 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null, null),
				new Actor(8, "Ogre", race[1], job[1], 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null, null)
		};

		final Actor enemies[][] = this.Enemy = new Actor[6][4];

		enemies[0][0] = enemy[3];
		enemies[0][1] = enemy[0];
		enemies[0][2] = null;
		enemies[0][3] = null;

		enemies[1][0] = enemy[1];
		enemies[1][1] = enemy[2];
		enemies[1][2] = null;
		enemies[1][3] = null;

		enemies[2][0] = enemy[1];
		enemies[2][1] = enemy[0];
		enemies[2][2] = enemy[3];
		enemies[2][3] = null;

		enemies[3][0] = enemy[1];
		enemies[3][1] = enemy[2];
		enemies[3][2] = enemy[3];
		enemies[3][3] = null;

		enemies[4][0] = enemy[0];
		enemies[4][1] = enemy[2];
		enemies[4][2] = enemy[1];
		enemies[4][3] = enemy[3];

		enemies[5][0] = enemy[0];
		enemies[5][1] = enemy[2];
		enemies[5][2] = enemy[1];
		enemies[5][3] = enemy[3];

		final Spinner[] raceBox = this.raceBox, jobBox = this.jobBox;
		final ArrayAdapter<String> raceList = this.raceList = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item),
            jobList = this.jobList = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item);
		raceBox[0] = (Spinner) findViewById(R.id.RaceBox1);
		raceBox[1] = (Spinner) findViewById(R.id.RaceBox2);
		raceBox[2] = (Spinner) findViewById(R.id.RaceBox3);
		for (int i = 0; i < race.length; i++) {
            raceList.add(race[i].getName());
        }
		for (int i = 0; i < 3; i++) {
            raceBox[i].setAdapter(raceList);
        }
		raceBox[0].setSelection(1);
		raceBox[1].setSelection(0);
		raceBox[2].setSelection(2);
		jobBox[0] = (Spinner) findViewById(R.id.JobBox1);
		jobBox[1] = (Spinner) findViewById(R.id.JobBox2);
		jobBox[2] = (Spinner) findViewById(R.id.JobBox3);
		for (int i = 0; i < job.length; i++) {
            jobList.add(job[i].getName());
        }
		for (int i = 0; i < 3; i++) {
            jobBox[i].setAdapter(jobList);
        }
		jobBox[0].setSelection(0);
		jobBox[1].setSelection(5);
		jobBox[2].setSelection(3);
		final EditText[] name = this.name;
		name[0] = (EditText) findViewById(R.id.NameField1);
		name[1] = (EditText) findViewById(R.id.NameField2);
		name[2] = (EditText) findViewById(R.id.NameField3);
		final Button begin = this.Begin = (Button) findViewById(R.id.StartBt),
		    about = this.About = (Button) findViewById(R.id.InfoBt),
		    exit = this.Exit = (Button) findViewById(R.id.QuitBt);
		final OnClickListener cAction = this.cAction;
		begin.setOnClickListener(cAction);
		about.setOnClickListener(cAction);
		exit.setOnClickListener(cAction);
	}

	private void setPlayer(final int p) {
		String n = this.name[p].getText().toString();
		if (n.equals("")) {
            switch (p) {
                case 0:
                    n = "Cody";
                    break;
                case 1:
                    n = "George";
                    break;
                case 2:
                    n = "Stephen";
                    break;
            }
        }
		final Actor[] players = this.Player;
        final Spinner[] raceBox = this.raceBox, jobBox = this.jobBox;
		players[p].setName(n);
		if (raceBox[p].isEnabled()) {
            players[p].setRace(this.pcRace[raceBox[p].getSelectedItemPosition()]);
        }
		players[p].setJob(this.pcJob[jobBox[p].getSelectedItemPosition()]);
		raceBox[p].setEnabled(false);
	}

	private void beginBt() {
		for (int i = 0; i < 3; i++) {
            this.setPlayer(i);
        }
		final int level = this.level;
		int surprise = 0;
		if (level % 2 == 1) {
            surprise = -1;
        }
		if (level == 2) {
            surprise = 1;
        }
		final Actor[] players = this.Player;
		for (int i = 0; i < players.length; i++) {
            players[i].recover();
        }
		//ArenaAct.PlayDemo(this);
//		ArenaAct.InitiateBattle(this, 0, 0, this.Party, this.Enemy[this.level], new Performance[] { this.Skill[0], this.Skill[1] },
//				this.Item, null, surprise, this.level % 2 == 0, true);
	}

	private OnClickListener cAction = new OnClickListener() {
		@Override
		public void onClick(final View v) {
			switch (v.getId()) {
			case R.id.StartBt:
				StageAct.this.beginBt();
				break;
			case R.id.InfoBt:
				StageAct.this.displayMsg("About", getString(R.string.msg_about));
				break;
			case R.id.QuitBt:
				StageAct.this.finish();
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
					System.arraycopy(extra.getParcelableArray("Party"), 0, this.Player, 0, 4);
					/*this.Player[1] = this.Party[0];
					this.Player[2] = this.Party[1];
					this.Player[3] = this.Party[2];*/
				}
				int level = StageAct.this.level;
				if (extra.getInt("Outcome") > 0 && level < 5)
					StageAct.this.level = level + 1;
			}
		}
	}
    
}