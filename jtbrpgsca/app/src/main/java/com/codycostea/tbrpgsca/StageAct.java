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
import java.util.Locale;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Intent;
import android.os.Bundle;
import android.os.Parcelable;
import android.text.SpannableString;
import android.text.util.Linkify;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;

public class StageAct extends Activity {

	private Costume[] pcRace;
	private Costume[] pcJob;
	private Interpreter[] Party;
	private Interpreter[][] Enemy;

	private EditText[] name = new EditText[3];
	private Spinner[] jobBox = new Spinner[3], raceBox = new Spinner[3];
	private int level = 0;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		this.setContentView(R.layout.activity_demo);

		if (!Locale.getDefault().getLanguage().equals("en")) {
			ScenePlay.victoryTxt = this.getString(R.string.scene_victory);
			ScenePlay.fallenTxt = this.getString(R.string.scene_defeat);
			ScenePlay.escapeTxt = this.getString(R.string.scene_escape);
			ScenePlay.performsTxt = this.getString(R.string.scene_performs);
			ScenePlay.failTxt = this.getString(R.string.scene_fail);
			Interpreter.koTxt = this.getString(R.string.actor_ko);
			Performance.reflectedTxt = this.getString(R.string.ability_reflected);
			Performance.suffersTxt = this.getString(R.string.ability_suffers);
			Performance.stolenTxt = this.getString(R.string.ability_stolen);
			Performance.missesTxt = this.getString(R.string.ability_misses);
			StateMask.causesTxt = this.getString(R.string.state_causes);
			RoleSheet.hpText = this.getString(R.string.costume_hp);
			RoleSheet.mpText = this.getString(R.string.costume_mp);
			RoleSheet.spText = this.getString(R.string.costume_sp);
		}

		final StateMask[] state = new StateMask[] {
				new StateMask(1, this.getString(R.string.skill_regen), null, false, false, false, false, -1, 0, -10, 0, 0,
						0, 0, 0, 0, 2, 0, 0, 0, 0, false, null, null, null, null, null),
				new StateMask(2, this.getString(R.string.skill_poison), null, false, false, false, false, 10, 0, 7, 0, 2,
						0, 0, 0, 0, -2, 0, 0, 0, 0, false, null, null, null, null, null),
				new StateMask(3, this.getString(R.string.skill_clarity), null, false, false, false, false, -1, 0, 0, -7, 0,
						0, 0, 0, 0, 0, 1, 1, 0, 0, false, null, null, null, null, null),
				new StateMask(4, this.getString(R.string.state_dizziness), null, false, false, false, false, 3, 0, 0, 7, 0,
						0, 0, 0, 0, 0, -1, -1, 0, 0, false, null, null, null, null, null),
				new StateMask(5, this.getString(R.string.state_vigour), null, false, false, false, false, -1, 0, 0, 0, -7,
						0, 0, 0, 1, 0, 0, 0, 1, 0, false, null, null, null, null, null),
				new StateMask(6, this.getString(R.string.state_weakness), null, false, false, false, false, 5, 0, 0, 0, 7,
						0, 0, 0, -1, 0, 0, 0, -1, 0, false, null, null, null, null, null),
				new StateMask(7, this.getString(R.string.skill_berserk), null, false, true, false, false, 7, 0, 0, 0, 0,
						0, 0, 0, 5, -3, 0, 0, 3, 0, false, null, null, null, null, null),
				new StateMask(8, this.getString(R.string.skill_confusion), null, false, false, true, false, 3, 2, 0, 0, 0,
						0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null, null),
				new StateMask(9, this.getString(R.string.skill_sleep), null, true, false, false, false, 5, 1, 0, 0, 0,
						0, 0, 0, 0, -3, 0, 0, -3, 0, false, null, null, null, null, null),
				new StateMask(10, this.getString(R.string.state_stun), null, true, false, false, false, 1, 0, 0, 0, 0,
						0, 0, 0, 0, -1, 0, 0, -1, 0, false, null, null, null, null, null),
				new StateMask(11, this.getString(R.string.skill_reflect), null, false, false, false, true, 7, 0, 0, 0, 0,
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

		final Performance[] skill = new Performance[] {
				new Performance(1, this.getString(R.string.skill_attack), 0, 0, false, false, 0, 0, 0, 0, Performance.DMG_TYPE_ATK, 1, 10, 0, -3, Performance.TRG_ONE, ELEMENT_PHYSICAL, -1, -1, false, false, null, confsleep),
				new Performance(2, this.getString(R.string.skill_defend), 0, 0, false, true, 0, 0, 0, 0, Performance.DMG_TYPE_SPI, 1, 0, -1, -2, Performance.TRG_SELF, ELEMENT_NONE, -1, -1, false, false, null, null),
				new Performance(3, this.getString(R.string.skill_heal), 0, 0, false, true, 1, 0, 3, 0, Performance.DMG_TYPE_SPI, 1, -25, 0, 0, Performance.TRG_ONE, ELEMENT_PSYCHIC, -1, -1, false, true, null, null),
				new Performance(4, this.getString(R.string.skill_mediatate), 0, 0, false, true, 1, 0, 0, 0, Performance.DMG_TYPE_SPI, 1, -1, -7, 10, Performance.TRG_SELF, ELEMENT_PSYCHIC, -1, -1, false, false, null, dizziness),
				new Performance(5, this.getString(R.string.skill_cure), 0, 0, false, true, 3, 0, 7, 0, Performance.DMG_TYPE_SPI, 1, -17, 0, 0, Performance.TRG_ONE, ELEMENT_PSYCHIC, -1, -1, false, true, null, cure),
				new Performance(6, this.getString(R.string.skill_clarity), 0, 0, false, true, 3, 0, 0, 7, Performance.DMG_TYPE_SPI, 1, 0, -3, 0, Performance.TRG_ONE, ELEMENT_PSYCHIC, -1, -1, false, false, clarity, madness),
				new Performance(7, this.getString(R.string.skill_regen), 0, 0, false, true, 4, 0, 10, 0, Performance.DMG_TYPE_SPI, 1, -37, 0, 0, Performance.TRG_ONE, ELEMENT_PSYCHIC, -1, -1, false, true, regen, poison),
				new Performance(8, this.getString(R.string.skill_prayer), 0, 0, false, true, 5, 0, 7, 0, Performance.DMG_TYPE_SPI, 1, -23, 0, 0, Performance.TRG_ENEMY, ELEMENT_PSYCHIC, -1, -1, false, true, null, null),
				new Performance(9, this.getString(R.string.skill_smite), 0, 0, false, false, 1, 1, 0, 2, Performance.DMG_TYPE_ATK | Performance.DMG_TYPE_DEF, 1, 4, 3, -4, Performance.TRG_ONE, ELEMENT_PHYSICAL, -1, -1, false, false, null, confclarity),
				new Performance(10, this.getString(R.string.skill_hit), 0, 0, false, false, 1, 3, 0, 1, Performance.DMG_TYPE_ATK, 1, 12, 0, -4, Performance.TRG_ONE, ELEMENT_PHYSICAL, -1, -1, false, false, null, confsleep),
				new Performance(11, this.getString(R.string.skill_bash), 0, 0, false, false, 3, 3, 0, 5, Performance.DMG_TYPE_ATK | Performance.DMG_TYPE_DEF, 1, 7, 5, -5, Performance.TRG_ONE, ELEMENT_PHYSICAL, -1, -1, false, false, dizziness, confclarity),
				new Performance(12, this.getString(R.string.skill_smash), 0, 0, false, false, 3, 5, 0, 3, Performance.DMG_TYPE_ATK, 1, 18, 0, -7, Performance.TRG_ONE, ELEMENT_PHYSICAL, -1, -1, false, false, null, confsleep),
				new Performance(13, this.getString(R.string.skill_berserk), 0, 0, false, false, 4, 7, 0, 4, Performance.DMG_TYPE_SPI, 1, 0, 0, 0, Performance.TRG_SELF, ELEMENT_PSYCHIC, -1, -1, false, false, berserk, weakness),
				new Performance(14, this.getString(R.string.skill_shock), 0, 0, false, false, 4, 4, 0, 7, Performance.DMG_TYPE_ATK | Performance.DMG_TYPE_DEF, 1, 10, 5, -9, Performance.TRG_ONE, ELEMENT_PHYSICAL, -1, -1, false, false, dizzystun, confclarity),
				new Performance(15, this.getString(R.string.skill_crush), 0, 0, false, false, 5, 7, 4, 0, Performance.DMG_TYPE_ATK, 1, 25, 0, -11, Performance.TRG_ONE, ELEMENT_PHYSICAL, -1, -1, false, false, stun, confsleep),
				new Performance(16, this.getString(R.string.skill_strike), 0, 0, false, true, 1, 0, 0, 0, Performance.DMG_TYPE_ATK | Performance.DMG_TYPE_AGI, 1, 13, 0, -3, Performance.TRG_ONE, ELEMENT_PHYSICAL, 5, 3, false, false, null, confsleep),
				new Performance(17, this.getString(R.string.skill_steal), 0, 0, true, true, 1, 0, 0, 0, Performance.DMG_TYPE_WIS, 1, 0, 0, 0, Performance.TRG_ONE, ELEMENT_PHYSICAL, 5, 3, false, false, null, null),
				new Performance(18, this.getString(R.string.skill_dash), 0, 0, false, true, 3, 0, 0, 0, Performance.DMG_TYPE_ATK | Performance.DMG_TYPE_AGI, 1, 15, 0, -5, Performance.TRG_ONE, ELEMENT_PHYSICAL, 3, 7, false, false, null, confsleep),
				new Performance(19, this.getString(R.string.skill_poison), 0, 0, false, true, 3, 0, 0, 0, Performance.DMG_TYPE_WIS, 2, 5, 0, 2, Performance.TRG_ONE, ELEMENT_PHYSICAL, 3, 7, false, false, poison, regen),
				new Performance(20, this.getString(R.string.skill_mug), 0, 0, true, true, 4, 0, 0, 0, Performance.DMG_TYPE_ATK | Performance.DMG_TYPE_AGI, 2, 15, 0, -7, Performance.TRG_ONE, ELEMENT_PHYSICAL, 1, 5, false, false, null, confsleep),
				new Performance(21, this.getString(R.string.skill_toxic_gas), 0, 0, false, true, 4, 0, 10, 0, Performance.DMG_TYPE_WIS, 3, 1, 1, 1, Performance.TRG_ENEMY, ELEMENT_PHYSICAL, 3, 7, false, false, tpoison, tregen),
				new Performance(22, this.getString(R.string.skill_cheer), 0, 0, false, true, 4, 0, 10, 0, Performance.DMG_TYPE_SPI, -1, 0, 0, -5, Performance.TRG_PARTY, ELEMENT_PSYCHIC, 3, 7, false, false, vigour, cure),
				new Performance(23, this.getString(R.string.skill_venom_blade), 0, 0, false, false, 5, 0, 0, 10, Performance.DMG_TYPE_ATK | Performance.DMG_TYPE_AGI, 1, 17, 0, -9, Performance.TRG_ONE, ELEMENT_PHYSICAL, 3, 7, false, false, poison, conftregen),
				new Performance(24, this.getString(R.string.skill_absorb), 0, 0, false, true, 1, 0, 0, 3, Performance.DMG_TYPE_WIS, 1, 0, 7, -3, Performance.TRG_ONE, ELEMENT_PSYCHIC, -1, -1, true, false, null, null),
				new Performance(25, this.getString(R.string.skill_drain), 0, 0, false, true, 3, 0, 10, 0, Performance.DMG_TYPE_WIS, 1, 15, 0, -3, Performance.TRG_ONE, ELEMENT_PSYCHIC, -1, -1, true, false, null, null),
				new Performance(26, this.getString(R.string.skill_fireball), 0, 0, false, true, 1, 0, 3, 0, Performance.DMG_TYPE_WIS, 1, 11, 0, -5, Performance.TRG_ONE, ELEMENT_FIRE, -1, -1, false, false, null, sleep),
				new Performance(27, this.getString(R.string.skill_iceshard), 0, 0, false, true, 1, 0, 3, 0, Performance.DMG_TYPE_WIS, 1, 11, 0, -5, Performance.TRG_ONE, ELEMENT_WATER, -1, -1, false, false, null, sleep),
				new Performance(28, this.getString(R.string.skill_lighting), 0, 0, false, true, 1, 0, 3, 0, Performance.DMG_TYPE_WIS, 1, 11, 0, -5, Performance.TRG_ONE, ELEMENT_WIND, -1, -1, false, false, null, sleep),
				new Performance(29, this.getString(R.string.skill_rock), 0, 0, false, true, 1, 0, 3, 0, Performance.DMG_TYPE_WIS, 1, 11, 0, -5, Performance.TRG_ONE, ELEMENT_EARTH, -1, -1, false, false, null, sleep),
				new Performance(30, this.getString(R.string.skill_darkness), 0, 0, false, true, 1, 0, 3, 0, Performance.DMG_TYPE_WIS, 1, 11, 0, -5, Performance.TRG_ONE, ELEMENT_PSYCHIC, -1, -1, false, false, null, sleep),
				new Performance(31, this.getString(R.string.skill_flame), 0, 0, false, true, 3, 0, 5, 0, Performance.DMG_TYPE_WIS, 1, 15, 0, -7, Performance.TRG_ENEMY, ELEMENT_FIRE, -1, -1, false, false, null, sleep),
				new Performance(32, this.getString(R.string.skill_blizzard), 0, 0, false, true, 3, 0, 5, 0, Performance.DMG_TYPE_WIS, 1, 15, 0, -7, Performance.TRG_ENEMY, ELEMENT_WATER, -1, -1, false, false, null, sleep),
				new Performance(33, this.getString(R.string.skill_storm), 0, 0, false, true, 3, 0, 5, 0, Performance.DMG_TYPE_WIS, 1, 15, 0, -7, Performance.TRG_ENEMY, ELEMENT_WIND, -1, -1, false, false, null, sleep),
				new Performance(34, this.getString(R.string.skill_earthquake), 0, 0, false, true, 3, 0, 5, 0, Performance.DMG_TYPE_WIS, 1, 15, 0, -7, Performance.TRG_ENEMY, ELEMENT_EARTH, -1, -1, false, false, null, sleep),
				new Performance(35, this.getString(R.string.skill_eclipse), 0, 0, false, true, 3, 0, 5, 0, Performance.DMG_TYPE_WIS, 1, 13, 0, -7, Performance.TRG_ENEMY, ELEMENT_PSYCHIC, -1, -1, false, false, null, sleep),
				new Performance(36, this.getString(R.string.skill_flare), 0, 0, false, true, 5, 0, 12, 0, Performance.DMG_TYPE_WIS, 2, 25, 0, -9, Performance.TRG_ONE, ELEMENT_NONE, -1, -1, false, false, null, sleep),
				new Performance(37, this.getString(R.string.skill_light_ray), 0, 0, false, true, 1, 0, 3, 0, Performance.DMG_TYPE_SPI, 1, 11, 0, -5, Performance.TRG_ONE, ELEMENT_LIGHT, -1, -1, false, false, null, sleep),
				new Performance(38, this.getString(R.string.skill_light_burst), 0, 0, false, true, 3, 0, 5, 0, Performance.DMG_TYPE_SPI, 1, 15, 0, -7, Performance.TRG_ENEMY, ELEMENT_LIGHT, -1, -1, false, false, null, sleep),
				new Performance(39, this.getString(R.string.skill_confusion), 0, 0, false, true, 5, 0, 15, 0, Performance.DMG_TYPE_WIS, 1, 0, 0, 0, Performance.TRG_ONE, ELEMENT_PSYCHIC, -1, -1, false, false, confusion, clarity),
				new Performance(40, this.getString(R.string.skill_sleep), 0, 0, false, true, 4, 0, 0, 17, Performance.DMG_TYPE_WIS, 1, 3, 0, 17, Performance.TRG_ONE, ELEMENT_PHYSICAL, -1, -1, false, false, sleep, clarity),
				new Performance(41, this.getString(R.string.skill_slash), 0, 0, false, true, 5, 0, 10, 0, Performance.DMG_TYPE_ATK | Performance.DMG_TYPE_AGI, 1, 15, 0, 0, Performance.TRG_ENEMY, ELEMENT_PHYSICAL, 3, 7, false, false, null, confsleep),
				new Performance(42, this.getString(R.string.skill_fire_wpn), 0, 0, false, false, 2, 0, 3, 2, Performance.DMG_TYPE_ATK | Performance.DMG_TYPE_WIS, 1, 17, 0, -7, Performance.TRG_ONE, ELEMENT_FIRE, -1, -1, false, false, null, confsleep),
				new Performance(43, this.getString(R.string.skill_ice_wpn), 0, 0, false, false, 2, 0, 3, 2, Performance.DMG_TYPE_ATK | Performance.DMG_TYPE_WIS, 1, 17, 0, -7, Performance.TRG_ONE, ELEMENT_WATER, -1, -1, false, false, null, confsleep),
				new Performance(44, this.getString(R.string.skill_electric_wpn), 0, 0, false, false, 2, 0, 3, 2, Performance.DMG_TYPE_ATK | Performance.DMG_TYPE_WIS, 1, 17, 0, -7, Performance.TRG_ONE, ELEMENT_WIND, -1, -1, false, false, null, confsleep),
				new Performance(45, this.getString(R.string.skill_stone_wpn), 0, 0, false, false, 2, 0, 3, 2, Performance.DMG_TYPE_ATK | Performance.DMG_TYPE_WIS, 1, 17, 0, -7, Performance.TRG_ONE, ELEMENT_EARTH, -1, -1, false, false, null, confsleep),
				new Performance(46, this.getString(R.string.skill_dark_wpn), 0, 0, false, false, 2, 0, 3, 2, Performance.DMG_TYPE_ATK | Performance.DMG_TYPE_WIS, 1, 17, 0, -7, Performance.TRG_ONE, ELEMENT_PSYCHIC, -1, -1, false, false, null, confsleep),
				new Performance(47, this.getString(R.string.skill_vampiric_wpn), 0, 0, false, false, 5, 0, 10, 0, Performance.DMG_TYPE_ATK | Performance.DMG_TYPE_AGI | Performance.DMG_TYPE_WIS,
						1, 21, 0, -9, Performance.TRG_ONE, ELEMENT_PSYCHIC, 3, 7, true, false, null, confsleep),
				new Performance(48, this.getString(R.string.skill_reflect), 0, 0, false, true, 5, 0, 10, 0, Performance.DMG_TYPE_SPI, 1, 0, 0, 0, Performance.TRG_ONE, ELEMENT_PSYCHIC, -1, -1, false, false, reflect, null),
				new Performance(49, this.getString(R.string.skill_meteor), 0, 0, false, true, 5, 0, 17, 0, Performance.DMG_TYPE_WIS, 1, 19, 0, -11, Performance.TRG_ENEMY, ELEMENT_PHYSICAL, -1, -1, false, false, null, sleep),
				new Performance(50, this.getString(R.string.skill_syphon), 0, 0, false, true, 4, 0, 15, 0, Performance.DMG_TYPE_WIS, 1, 13, 0, -3, Performance.TRG_ENEMY, ELEMENT_PSYCHIC, -1, -1, true, false, null, null),
				new Performance(51, this.getString(R.string.skill_dragon_breath), 0, 0, false, false, 4, 0, 13, 7, Performance.DMG_TYPE_ATK | Performance.DMG_TYPE_WIS, 1, 15, 0, -11, Performance.TRG_ONE, ELEMENT_PHYSICAL, -1, -1, false, false, null, confsleep),
				new Performance(52, this.getString(R.string.skill_light_wpn), 0, 0, false, false, 2, 0, 3, 2, Performance.DMG_TYPE_ATK | Performance.DMG_TYPE_DEF | Performance.DMG_TYPE_SPI,
						1, 17, 0, -5, Performance.TRG_ONE, ELEMENT_LIGHT, -1, -1, false, false, null, confsleep),
				new Performance(53, this.getString(R.string.skill_heal), 0, 0, false, true, 1, 0, 3, 0, Performance.DMG_TYPE_WIS, 1, -25, 0, 0, Performance.TRG_ONE, ELEMENT_LIGHT, -1, -1, false, false, null, null),
				new Performance(54, this.getString(R.string.skill_mediatate), 0, 0, false, true, 1, 0, 0, Performance.DMG_TYPE_WIS, 2, 1, -3, -7, 0, Performance.TRG_SELF, ELEMENT_LIGHT, -1, -1, false, false, null, dizziness),
				new Performance(55, this.getString(R.string.skill_cure), 0, 0, false, true, 3, 0, 7, 0, Performance.DMG_TYPE_WIS, 1, -17, 0, 0, Performance.TRG_ONE, ELEMENT_LIGHT, -1, -1, false, true, null, cure),
				new Performance(56, this.getString(R.string.skill_clarity), 0, 0, false, true, 3, 0, 0, 7, Performance.DMG_TYPE_WIS, 1, 0, -3, 0, Performance.TRG_ONE, ELEMENT_LIGHT, -1, -1, false, false, clarity, madness),
				new Performance(57, this.getString(R.string.skill_absorb), 0, 0, false, true, 1, 0, 0, 3, Performance.DMG_TYPE_SPI, 1, 0, 7, -3, Performance.TRG_ONE, ELEMENT_LIGHT, -1, -1, true, false, null, null)
		};

		final Performance[] items = new Performance[] {
				new Performance(1, this.getString(R.string.item_potion), 0, 0, false, true, 1, 0, 0, 0, 0, 0, -25, 0, 0, Performance.TRG_ONE, ELEMENT_NONE, -1, -1, false, false, null, null),
				new Performance(2, this.getString(R.string.item_ether), 0, 0, false, true, 1, 0, 0, 0, 0, 0, 0, -10, 0, Performance.TRG_ONE, ELEMENT_NONE, -1, -1, false, false, null, null),
				new Performance(3, this.getString(R.string.item_tonic), 0, 0, false, true, 1, 0, 0, 0, 0, 0, 0, 0, -10, Performance.TRG_ONE, ELEMENT_NONE, -1, -1, false, false, null, null),
				new Performance(4, this.getString(R.string.item_antidote), 0, 0, false, true, 1, 0, 0, 0, 0, 0, 0, 0, 0, Performance.TRG_ONE, ELEMENT_NONE, -1, -1, false, false, null, poison),
				new Performance(9, this.getString(R.string.item_hipotion), 0, 0, false, true, 1, 0, 0, 0, 0, 0, -50, 0, 0, Performance.TRG_ONE, ELEMENT_NONE, -1, -1, false, false, null, null),
				new Performance(5, this.getString(R.string.item_hiether), 0, 0, false, true, 1, 0, 0, 0, 0, 0, 0, -25, 0, Performance.TRG_ONE, ELEMENT_NONE, -1, -1, false, false, null, dizziness),
				new Performance(6, this.getString(R.string.item_hitonic), 0, 0, false, true, 1, 0, 0, 0, 0, 0, 0, -20, 0, Performance.TRG_ONE, ELEMENT_NONE, -1, -1, false, false, vigour, weakness),
				new Performance(7, this.getString(R.string.item_panacea), 0, 0, false, true, 1, 0, 0, 0, 0, 0, 0, 0, 0, Performance.TRG_ONE, ELEMENT_NONE, -1, -1, false, false, null, cure),
				new Performance(8, this.getString(R.string.item_elixir), 0, 0, false, true, 1, 0, 0, 0, 0, 0, -100, -100, -100, Performance.TRG_ONE, ELEMENT_NONE, -1, -1, false, true, null, null)
		};

		final Integer itemsNr = new Integer(15);
		final Hashtable itemsMap = new Hashtable(items.length);
		for (int i = 0; i < items.length; i++) {
			itemsMap.put(items[i], itemsNr);
		}

		final Performance[] skills = new Performance[] {skill[0], skill[1]};

		final Costume[] race = this.pcRace = new Costume[] {
				new Costume(1, this.getString(R.string.race_elf), null, 40, 25, 10, 7, 5, 15, 12, 11, 0, false, null, skills, null, null),
				new Costume(2, this.getString(R.string.race_human), null, 47, 15, 13, 9, 11, 9, 11, 10, 0, false, null, skills, null, null),
				new Costume(3, this.getString(R.string.race_halforc), null, 55, 7, 13, 17, 12, 5, 7, 9, 0, false, null, skills, null, null),
				new Costume(4, this.getString(R.string.race_gnome), null, 40, 15, 20, 12, 8, 10, 5, 15, 0, false, null, skills, null, null)
		};

		final Costume[] job = this.pcJob = new Costume[] {
				new Costume(15, this.getString(R.string.job_hero), "hero", 1, 1, 1, 0, 0, 0, 0, 0, 0, false, null, new Performance[] {skill[8], skill[9], skill[10], skill[11], skill[23], skill[24], skill[25], skill[26], skill[27], skill[28], skill[29], skill[41], skill[42], skill[43], skill[44], skill[45], skill[30], skill[31], skill[32], skill[33], skill[34], skill[36], skill[51], skill[37], skill[2], skill[3], skill[4], skill[5], skill[15], skill[16], skill[17], skill[18]}, null, null),
				new Costume(1, this.getString(R.string.job_berserker), "berserker", 1, 0, 0, 1, 0, 0, 0, 1, 0, false, null, new Performance[] {skill[8], skill[9], skill[10], skill[11], skill[12], skill[14]}, null, null),
				new Costume(2, this.getString(R.string.job_wizard), "wizard", 1, 0, 0, 0, 1, 0, 1, 0, 0, false, null, new Performance[] {skill[23], skill[24], skill[25], skill[26], skill[27], skill[28], skill[29], skill[30], skill[31], skill[32], skill[33], skill[34], skill[35], skill[38]}, null, null),
				new Costume(3, this.getString(R.string.job_hesychast), "hesychast", 0, 1, 0, 0, 1, 0, 1, 0, 0, false, null, new Performance[] {skill[2], skill[3], skill[4], skill[5], skill[6], skill[7]}, null, null),
				new Costume(4, this.getString(R.string.job_spy), "spy", 0, 0, 1, 1, 0, 0, 0, 1, 0, false, null, new Performance[] {skill[15], skill[16], skill[17], skill[18], skill[19], skill[38]}, null, null),
				new Costume(5, this.getString(R.string.job_alchemist), "alchemist", 0, 1, 0, 0, 0, 1, 0, 1, 0, false, null, new Performance[] {skill[23], skill[24], skill[25], skill[26], skill[27], skill[28], skill[30], skill[31], skill[32], skill[33], skill[15], skill[16], skill[17], skill[18], skill[20]}, null, null),
				new Costume(6, this.getString(R.string.job_dragoon), "dragoon", 1, 0, 0, 1, 0, 1, 0, 0, 0, false, null, new Performance[] {skill[8], skill[9], skill[10], skill[11], skill[23], skill[24], skill[25], skill[26], skill[27], skill[28], skill[30], skill[31], skill[32], skill[33], skill[41], skill[42], skill[43], skill[44], skill[50]}, null, null),
				new Costume(7, this.getString(R.string.job_knight), "knight", 1, 0, 0, 0, 1, 0, 1, 0, 0, false, null, new Performance[] {skill[2], skill[3], skill[4], skill[5], skill[8], skill[9], skill[10], skill[11], skill[13]}, null, null),
				new Costume(8, this.getString(R.string.job_ranger), "ranger", 0, 0, 1, 0, 0, 0, 1, 1, 0, false, null, new Performance[] {skill[2], skill[3], skill[4], skill[5], skill[15], skill[16], skill[17], skill[18], skill[21]}, null, null),
				new Costume(9, this.getString(R.string.job_shaman), "shaman", 0, 1, 0, 0, 0, 1, 1, 0, 0, false, null, new Performance[] {skill[52], skill[53], skill[54], skill[55], skill[23], skill[24], skill[29], skill[34], skill[49]}, null, null),
				new Costume(10, this.getString(R.string.job_corsair), "corsair", 0, 0, 1, 1, 0, 0, 0, 1, 0, false, null, new Performance[] {skill[8], skill[9], skill[10], skill[11], skill[15], skill[16], skill[17], skill[18], skill[22]}, null, null),
				new Costume(11, this.getString(R.string.job_reaver), "reaver", 1, 0, 0, 1, 0, 0, 0, 0, 0, false, null, new Performance[] {skill[8], skill[9], skill[11], skill[23], skill[24], skill[29], skill[15], skill[16], skill[18], skill[45], skill[46]}, null, null),
				new Costume(12, this.getString(R.string.job_ninja), "ninja", 0, 0, 1, 0, 0, 0, 0, 1, 0, false, null, new Performance[] {skill[8], skill[9], skill[11], skill[15], skill[16], skill[17], skill[2], skill[3], skill[5], skill[40]}, null, null),
				new Costume(13, this.getString(R.string.job_crusader), "crusader", 1, 0, 0, 0, 0, 0, 1, 0, 0, false, null, new Performance[] {skill[8], skill[9], skill[10], skill[2], skill[3], skill[4], skill[56], skill[36], skill[51], skill[37], skill[47]}, null, null),
				new Costume(14, this.getString(R.string.job_druid), "druid", 0, 1, 0, 0, 0, 1, 0, 0, 0, false, null, new Performance[] {skill[23], skill[25], skill[26], skill[27], skill[28], skill[30], skill[31], skill[32], skill[33], skill[52], skill[53], skill[54], skill[15], skill[16], skill[18], skill[48]}, null, null),
				new Costume(16, "Sorceress", "sorceress", 1, 0, 0, 0, 1, 0, 1, 0, 0, false, null, new Performance[] {skill[23], skill[24], skill[25], skill[26], skill[27], skill[28], skill[29], skill[30], skill[31], skill[32], skill[33], skill[34], skill[35], skill[38]}, null, null),
				new Costume(17, "Valkyrie", "valkyrie", 1, 0, 0, 1, 0, 0, 0, 1, 0, false, null, new Performance[] {skill[8], skill[9], skill[10], skill[11], skill[12], skill[14]}, null, null),
				new Costume(18, this.getString(R.string.race_ogre), "ogre", 1, 0, 0, 1, 0, 0, 0, 1, 0, false, null, new Performance[] {skill[8], skill[9], skill[10], skill[11], skill[12], skill[14]}, null, null),
				new Costume(19, this.getString(R.string.race_goblin), "goblin", 0, 0, 1, 0, 0, 0, 0, 1, 0, false, null, new Performance[] {skill[8], skill[9], skill[11], skill[15], skill[16], skill[17], skill[2], skill[3], skill[5], skill[40]}, null, null),
				new Costume(20, this.getString(R.string.race_troll), "troll", 1, 0, 0, 0, 1, 0, 1, 0, 0, false, null, new Performance[] {skill[2], skill[3], skill[4], skill[5], skill[8], skill[9], skill[10], skill[11], skill[13]}, null, null),
				new Costume(21, this.getString(R.string.race_lizard), "lizard", 1, 0, 0, 0, 1, 0, 1, 0, 0, false, null, new Performance[] {skill[23], skill[24], skill[25], skill[26], skill[27], skill[28], skill[29], skill[30], skill[31], skill[32], skill[33], skill[34], skill[35], skill[38]}, null, null)
		};

		this.Party = new Interpreter[] {
				new Interpreter(1, "Cody", race[2], job[0], 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null, itemsMap),
				new Interpreter(3, "Victoria", race[0], job[16], 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null, itemsMap),
				new Interpreter(5, "Stephanie", race[3], job[15], 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null, itemsMap),
				//new Actor(7, "George", race[1], job[13], 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null, itemsMap)
		};

		final Interpreter[] enemy = new Interpreter[] {
				new Interpreter(2, this.getString(R.string.race_goblin), race[2], job[18], 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null, null),
				new Interpreter(4, this.getString(R.string.race_lizard), race[0], job[20], 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null, null),
				new Interpreter(6, this.getString(R.string.race_troll), race[3], job[19], 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null, null),
				new Interpreter(8, this.getString(R.string.race_ogre), race[1], job[17], 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null, null, null)
		};

		final Interpreter[][] enemies = this.Enemy = new Interpreter[6][];
		enemies[0] = new Interpreter[2];
		enemies[0][0] = enemy[3];
		enemies[0][1] = enemy[0];
		enemies[1] = new Interpreter[2];
		enemies[1][0] = enemy[1];
		enemies[1][1] = enemy[2];
		enemies[2] = new Interpreter[3];
		enemies[2][0] = enemy[1];
		enemies[2][1] = enemy[0];
		enemies[2][2] = enemy[3];
		enemies[3] = new Interpreter[3];
		enemies[3][0] = enemy[1];
		enemies[3][1] = enemy[2];
		enemies[3][2] = enemy[3];
		enemies[4] = new Interpreter[4];
		enemies[4][0] = enemy[0];
		enemies[4][1] = enemy[2];
		enemies[4][2] = enemy[1];
		enemies[4][3] = enemy[3];
		enemies[5] = new Interpreter[4];
		enemies[5][0] = enemy[0];
		enemies[5][1] = enemy[2];
		enemies[5][2] = enemy[1];
		enemies[5][3] = enemy[3];

		final Spinner[] raceBox = this.raceBox, jobBox = this.jobBox;
		final ArrayAdapter<String> raceList = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item),
            jobList = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item);
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
		jobBox[0] = (Spinner)this.findViewById(R.id.JobBox1);
		jobBox[1] = (Spinner)this.findViewById(R.id.JobBox2);
		jobBox[2] = (Spinner)this.findViewById(R.id.JobBox3);
		for (int i = 0; i < job.length && i < 15; i++) {
            jobList.add(job[i].getName());
        }
		for (int i = 0; i < 3; i++) {
            jobBox[i].setAdapter(jobList);
        }
		jobBox[0].setSelection(0);
		jobBox[1].setSelection(5);
		jobBox[2].setSelection(3);
		final EditText[] name = this.name;
		name[0] = (EditText)this.findViewById(R.id.NameField1);
		name[1] = (EditText)this.findViewById(R.id.NameField2);
		name[2] = (EditText)this.findViewById(R.id.NameField3);
		final Button begin = (Button)this.findViewById(R.id.StartBt),
		    about = (Button)this.findViewById(R.id.InfoBt),
		    exit = (Button)this.findViewById(R.id.QuitBt);
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
                    n = this.getString(R.string.name_cody);
                    break;
                case 1:
                    n = this.getString(R.string.name_george);
                    break;
                case 2:
                    n = this.getString(R.string.name_stephen);
                    break;
            }
        }
		final Interpreter[] players = this.Party;
        final Spinner[] raceBox = this.raceBox, jobBox = this.jobBox;
		players[p].setName(n);
		if (raceBox[p].isEnabled()) {
            players[p].setRace(this.pcRace[raceBox[p].getSelectedItemPosition()]);
			raceBox[p].setEnabled(false);
        }
		players[p].setJob(this.pcJob[jobBox[p].getSelectedItemPosition()]);
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
		final Interpreter[] players = this.Party;
		for (int i = 0; i < players.length; i++) {
            players[i].recover();
        }
		final Interpreter[] enemies = this.Enemy[level];
        for (int i = 0; i < enemies.length; i++) {
            enemies[i].recover();
        }
		//ArenaAct.PlayDemo(this);
		ArenaAct.Stage(this, 0, 0, players, this.Enemy[level],
				null, surprise, level % 2 == 0, true);
	}

	private OnClickListener cAction = new OnClickListener() {
		@Override
		public void onClick(final View v) {
			switch (v.getId()) {
			case R.id.StartBt:
				StageAct.this.beginBt();
				break;
			case R.id.InfoBt:
				StageAct.this.displayMsg(StageAct.this.getString(R.string.about), getString(R.string.msg_about));
				break;
			case R.id.QuitBt:
				StageAct.this.finish();
				break;
			}
		}
	};

	private void displayMsg(final String t, final String s) {
		SpannableString text = new SpannableString(s);
		Linkify.addLinks(text, Linkify.WEB_URLS);
		AlertDialog.Builder msg = new AlertDialog.Builder(this);
		msg.setCancelable(false);
		msg.setMessage(text);
		msg.setTitle(t);
		msg.setPositiveButton(this.getString(R.string.ok), null);
		AlertDialog dialog = msg.create();
		dialog.show();
	}

	@Override
	protected void onActivityResult(final int requestCode, final int resultCode, final Intent data) {
		if (requestCode >= 0) {
			if (resultCode == RESULT_OK) {
				Bundle extra = data.getExtras();
				if (extra != null && extra.containsKey("party")) {
				    final Interpreter[] players = this.Party;
				    final Parcelable[] party = extra.getParcelableArray("party");
					System.arraycopy(party, 0, players, 0, Math.min(party.length, players.length));
				}
				int level = StageAct.this.level;
				if (extra.getInt("outcome", 0) > 0 && level < 5) {
                    StageAct.this.level = level + 1;
                }
			}
		}
	}
    
}