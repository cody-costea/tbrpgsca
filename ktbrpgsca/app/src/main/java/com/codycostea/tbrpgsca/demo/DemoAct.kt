/*
Copyright (C) AD 2018 Claudiu-Stefan Costea

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
package com.codycostea.tbrpgsca.demo

import android.app.AlertDialog
import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.text.SpannableString
import android.text.util.Linkify
import android.view.View
import android.view.View.OnClickListener
import android.widget.ArrayAdapter
import android.widget.Button
import android.widget.EditText
import android.widget.Spinner
import com.codycostea.tbrpgsca.R
import com.codycostea.tbrpgsca.library.*
import java.util.*
import kotlin.collections.LinkedHashMap

class DemoAct : AppCompatActivity(), ArenaStager {

    private lateinit var fragment: View
    private lateinit var partyLayout: View
    private lateinit var party: Array<Actor>
    private lateinit var arena: Arena
    private var level = 0

    private val states: Array<State>
        get() {
            return arrayOf(
                    AdState(1, this.getString(R.string.skill_regen), null, false, false, false, false, -1, 0, -10, 0, 0,
                            0, 0, 0, 0, 2, 0, 0, 0, 0, false, null, null, null),
                    AdState(2, this.getString(R.string.skill_poison), null, false, false, false, false, 10, 0, 7, 0, 2,
                            0, 0, 0, 0, -2, 0, 0, 0, 0, false, null, null, null),
                    AdState(3, this.getString(R.string.skill_clarity), null, false, false, false, false, -1, 0, 0, -7, 0,
                            0, 0, 0, 0, 0, 1, 1, 0, 0, false, null, null, null),
                    AdState(4, this.getString(R.string.state_dizziness), null, false, false, false, false, 3, 0, 0, 7, 0,
                            0, 0, 0, 0, 0, -1, -1, 0, 0, false, null, null, null),
                    AdState(5, this.getString(R.string.state_vigour), null, false, false, false, false, -1, 0, 0, 0, -7,
                            0, 0, 0, 1, 0, 0, 0, 1, 0, false, null, null, null),
                    AdState(6, this.getString(R.string.state_weakness), null, false, false, false, false, 5, 0, 0, 0, 7,
                            0, 0, 0, -1, 0, 0, 0, -1, 0, false, null, null, null),
                    AdState(7, this.getString(R.string.skill_berserk), null, false, true, false, false, 7, 0, 0, 0, 0,
                            0, 0, 0, 5, -3, 0, 0, 3, 0, false, null, null, null),
                    AdState(8, this.getString(R.string.skill_confusion), null, false, false, true, false, 3, 2, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null),
                    AdState(9, this.getString(R.string.skill_sleep), null, true, false, false, false, 5, 1, 0, 0, 0,
                            0, 0, 0, 0, -3, 0, 0, -3, 0, false, null, null, null),
                    AdState(10, this.getString(R.string.state_stun), null, true, false, false, false, 1, 0, 0, 0, 0,
                            0, 0, 0, 0, -1, 0, 0, -1, 0, false, null, null, null),
                    AdState(11, this.getString(R.string.skill_reflect), null, false, false, false, true, 7, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, false, null, null, null)
            )
        }

    private fun getRaces(skill: Array<Ability>): Array<Costume> {
        val skills: Array<Ability> = arrayOf(skill[0], skill[1])

        return arrayOf(
                AdCostume(1, this.getString(R.string.race_elf), "", 40, 25, 10, 7, 5, 15, 12, 11, 0, false, mutableMapOf(1 to -1, 2 to 1, 3 to 1, 4 to 1, 5 to 1), skills, null, null),
                AdCostume(2, this.getString(R.string.race_human), "", 47, 15, 13, 9, 11, 9, 11, 10, 0, false, mutableMapOf(6 to -1, 7 to 1), skills, null, null),
                AdCostume(3, this.getString(R.string.race_halforc), "", 55, 7, 13, 17, 12, 5, 7, 9, 0, false, mutableMapOf(1 to 1, 2 to -1, 3 to -1, 4 to -1, 5 to -1), skills, null, null),
                AdCostume(4, this.getString(R.string.race_gnome), "", 40, 15, 20, 12, 8, 10, 5, 15, 0, false, mutableMapOf(6 to 1, 7 to -1), skills, null, null)
        )
    }

    private fun getJobs(skill: Array<Ability>): Array<Costume> {
        return arrayOf(
                AdCostume(15, this.getString(R.string.job_hero), "hero", 1, 1, 1, 0, 0, 0, 0, 0, 0, false, null, arrayOf(skill[8], skill[9], skill[10], skill[11], skill[23], skill[24], skill[25], skill[26], skill[27], skill[28], skill[29], skill[41], skill[42], skill[43], skill[44], skill[45], skill[30], skill[31], skill[32], skill[33], skill[34], skill[36], skill[51], skill[37], skill[2], skill[3], skill[4], skill[5], skill[15], skill[16], skill[17], skill[18]), null, null),
                AdCostume(1, this.getString(R.string.job_berserker), "berserker", 1, 0, 0, 1, 0, 0, 0, 1, 0, false, mutableMapOf(1 to 1, 2 to -1, 3 to -1, 4 to -1, 5 to -1), arrayOf(skill[8], skill[9], skill[10], skill[11], skill[12], skill[14]), null, null),
                AdCostume(2, this.getString(R.string.job_wizard), "wizard", 1, 0, 0, 0, 1, 0, 1, 0, 0, false, mutableMapOf(1 to -1, 2 to 1, 3 to 1, 4 to 1, 5 to 1, 6 to -1), arrayOf(skill[23], skill[24], skill[25], skill[26], skill[27], skill[28], skill[29], skill[30], skill[31], skill[32], skill[33], skill[34], skill[35], skill[38]), null, null),
                AdCostume(3, this.getString(R.string.job_hesychast), "hesychast", 0, 1, 0, 0, 1, 0, 1, 0, 0, false, mutableMapOf(6 to -7, 7 to 7), arrayOf(skill[2], skill[3], skill[4], skill[5], skill[6], skill[7]), null, null),
                AdCostume(4, this.getString(R.string.job_spy), "spy", 0, 0, 1, 1, 0, 0, 0, 1, 0, false, null, arrayOf(skill[15], skill[16], skill[17], skill[18], skill[19], skill[38]), null, null),
                AdCostume(5, this.getString(R.string.job_alchemist), "alchemist", 0, 1, 0, 0, 0, 1, 0, 1, 0, false, null, arrayOf(skill[23], skill[24], skill[25], skill[26], skill[27], skill[28], skill[30], skill[31], skill[32], skill[33], skill[15], skill[16], skill[17], skill[18], skill[20]), null, null),
                AdCostume(6, this.getString(R.string.job_dragoon), "dragoon", 1, 0, 0, 1, 0, 1, 0, 0, 0, false, mutableMapOf(2 to 1, 3 to 1, 4 to 1, 5 to 1, 7 to -1), arrayOf(skill[8], skill[9], skill[10], skill[11], skill[23], skill[24], skill[25], skill[26], skill[27], skill[28], skill[30], skill[31], skill[32], skill[33], skill[41], skill[42], skill[43], skill[44], skill[50]), null, null),
                AdCostume(7, this.getString(R.string.job_knight), "knight", 1, 0, 0, 0, 1, 0, 1, 0, 0, false, mutableMapOf(1 to 1, 2 to -1, 3 to -1, 4 to -1, 5 to -1, 6 to -7, 7 to 7), arrayOf(skill[2], skill[3], skill[4], skill[5], skill[8], skill[9], skill[10], skill[11], skill[13]), null, null),
                AdCostume(8, this.getString(R.string.job_ranger), "ranger", 0, 0, 1, 0, 0, 0, 1, 1, 0, false, mutableMapOf(6 to -2, 7 to 2), arrayOf(skill[2], skill[3], skill[4], skill[5], skill[15], skill[16], skill[17], skill[18], skill[21]), null, null),
                AdCostume(9, this.getString(R.string.job_shaman), "shaman", 0, 1, 0, 0, 0, 1, 1, 0, 0, false, mutableMapOf(6 to 7, 7 to -7), arrayOf(skill[52], skill[53], skill[54], skill[55], skill[23], skill[24], skill[29], skill[34], skill[49]), null, null),
                AdCostume(10, this.getString(R.string.job_corsair), "corsair", 0, 0, 1, 1, 0, 0, 0, 1, 0, false, null, arrayOf(skill[8], skill[9], skill[10], skill[11], skill[15], skill[16], skill[17], skill[18], skill[22]), null, null),
                AdCostume(11, this.getString(R.string.job_reaver), "reaver", 1, 0, 0, 1, 0, 0, 0, 0, 0, false, mutableMapOf(6 to 7, 7 to -7), arrayOf(skill[8], skill[9], skill[11], skill[23], skill[24], skill[29], skill[15], skill[16], skill[18], skill[45], skill[46]), null, null),
                AdCostume(12, this.getString(R.string.job_ninja), "ninja", 0, 0, 1, 0, 0, 0, 0, 1, 0, false, null, arrayOf(skill[8], skill[9], skill[11], skill[15], skill[16], skill[17], skill[2], skill[3], skill[5], skill[40]), null, null),
                AdCostume(13, this.getString(R.string.job_crusader), "crusader", 1, 0, 0, 0, 0, 0, 1, 0, 0, false, mutableMapOf(6 to -7, 7 to 7), arrayOf(skill[8], skill[9], skill[10], skill[2], skill[3], skill[4], skill[56], skill[36], skill[51], skill[37], skill[47]), null, null),
                AdCostume(14, this.getString(R.string.job_druid), "druid", 0, 1, 0, 0, 0, 1, 0, 0, 0, false, mutableMapOf(2 to 1, 3 to 1, 4 to 1, 5 to 1, 6 to -1, 7 to -1), arrayOf(skill[23], skill[25], skill[26], skill[27], skill[28], skill[30], skill[31], skill[32], skill[33], skill[52], skill[53], skill[54], skill[15], skill[16], skill[18], skill[48]), null, null)
        )
    }

    private fun getCharacters(state: Array<State>, skill: Array<Ability>, race: Array<Costume>, item: Array<Ability>): Array<Actor> {
        val maxlv = 5

        val ogreJob = AdCostume(1, this.getString(R.string.race_ogre), "ogre", 1, 0, 0, 1, 0, 0, 0, 1, 0, false, mutableMapOf(1 to 1, 2 to -1, 3 to -1, 4 to -1, 5 to -1), arrayOf(skill[11]), null, null)
        val lizardJob = AdCostume(2, this.getString(R.string.race_lizard), "lizard", 1, 0, 0, 0, 1, 0, 1, 0, 0, false, mutableMapOf(1 to -1, 2 to 1, 3 to 1, 4 to 1, 5 to 1, 6 to -1), arrayOf(skill[30]), null, null)
        val trollJob = AdCostume(3, this.getString(R.string.race_troll), "troll", 0, 1, 0, 0, 1, 0, 1, 0, 0, false, mutableMapOf(6 to -7, 7 to 7), arrayOf(skill[2]), null, null)
        val goblinJob = AdCostume(4, this.getString(R.string.race_goblin), "goblin", 0, 0, 1, 1, 0, 0, 0, 1, 0, false, null, arrayOf(skill[15]), null, null)

        val characters = arrayOf<Actor>(
                AdActor(1, this, this.getString(R.string.name_cody), null, race[2], goblinJob, mInit = 7, maxLv = maxlv),
                AdActor(2, this, this.getString(R.string.name_george), null, race[0], lizardJob, mInit = 6, maxLv = maxlv),
                AdActor(3, this, this.getString(R.string.name_stephen), null, race[1], trollJob, mInit = 7, maxLv = maxlv),
                AdActor(4, this, this.getString(R.string.race_ogre), null, race[2], ogreJob, level = 3, maxLv = 3, mInit = 10, states = arrayOf(state[10])),
                AdActor(5, this, this.getString(R.string.race_lizard), null, race[0], lizardJob, level = 3, maxLv = 3, mInit = 7, mRes = mutableMapOf(2 to 7, 3 to 1)),
                AdActor(6, this, this.getString(R.string.race_goblin), null, race[3], goblinJob, mInit = 5, level = 3, maxLv = 3),
                AdActor(7, this, this.getString(R.string.race_troll), null, race[1], trollJob, level = 3, maxLv = 3, mInit = 8, states = arrayOf(state[0]))
        )
        characters[3].items = (mutableMapOf(item[0] to 2, item[2] to 2)) as LinkedHashMap<Ability, Int>
        characters[4].items = (mutableMapOf(item[1] to 2, item[5] to 1)) as LinkedHashMap<Ability, Int>
        characters[5].items = (mutableMapOf(item[3] to 2, item[7] to 1)) as LinkedHashMap<Ability, Int>
        characters[6].items = (mutableMapOf(item[4] to 1, item[8] to 1)) as LinkedHashMap<Ability, Int>

        return characters
    }

    private fun getSkills(state: Array<State>): Array<Ability> {
        val confsleep = arrayOf<State>(state[7], state[8])
        val cure = arrayOf<State>(state[1], state[3], state[5], state[6], state[7], state[8], state[9])
        val dizziness = arrayOf<State>(state[3])
        val poison = arrayOf<State>(state[1])
        val regen = arrayOf<State>(state[0])
        val madness = arrayOf<State>(state[3], state[6], state[7], state[8], state[9])
        val clarity = arrayOf<State>(state[2])
        val weakness = arrayOf<State>(state[5])
        val vigour = arrayOf<State>(state[4])
        val tregen = arrayOf<State>(state[0], state[2], state[4])
        val berserk = arrayOf<State>(state[6])
        val stun = arrayOf<State>(state[9])
        val reflect = arrayOf<State>(state[10])
        val confusion = arrayOf<State>(state[7])
        val confclarity = arrayOf<State>(state[2], state[7], state[8])
        val conftregen = arrayOf<State>(state[0], state[2], state[4], state[7], state[8])
        val sleep = arrayOf<State>(state[8])
        val tpoison = arrayOf<State>(state[1], state[3], state[5])
        val dizzystun = arrayOf<State>(state[3], state[9])

        return arrayOf(
                AdAbility(1, this.getString(R.string.skill_attack), 0, 0, false, false, 0, 0, 0, 0, Ability.DmgTypeAtk, 1, 10, 0, -3, 0, 1, -1, -1, false, false, null, confsleep),
                AdAbility(2, this.getString(R.string.skill_defend), 0, 0, false, true, 0, 0, 0, 0, Ability.DmgTypeDef, 1, 0, -1, -2, -1, 0, -1, -1, false, false, null, null),
                AdAbility(3, this.getString(R.string.skill_heal), 0, 0, false, true, 1, 0, 3, 0, Ability.DmgTypeSpi, 1, -25, 0, 0, 0, 6, -1, -1, false, false, null, null),
                AdAbility(4, this.getString(R.string.skill_mediatate), 0, 0, false, true, 1, 0, 0, 0, Ability.DmgTypeSpi, 1, -1, -7, 10, -1, 6, -1, -1, false, false, null, dizziness),
                AdAbility(5, this.getString(R.string.skill_cure), 0, 0, false, true, 3, 0, 7, 0, Ability.DmgTypeSpi, 1, -17, 0, 0, 0, 7, -1, -1, false, true, null, cure),
                AdAbility(6, this.getString(R.string.skill_clarity), 0, 0, false, true, 3, 0, 0, 7, Ability.DmgTypeSpi, 1, 0, -3, 0, 0, 6, -1, -1, false, false, clarity, madness),
                AdAbility(7, this.getString(R.string.skill_regen), 0, 0, false, true, 4, 0, 10, 0, Ability.DmgTypeSpi, 1, -37, 0, 0, 0, 6, -1, -1, false, true, regen, poison),
                AdAbility(8, this.getString(R.string.skill_prayer), 0, 0, false, true, 5, 0, 7, 0, Ability.DmgTypeSpi, 1, -23, 0, 0, 1, 0, -1, -1, false, true, null, null),
                AdAbility(9, this.getString(R.string.skill_smite), 0, 0, false, false, 1, 1, 0, 2, Ability.DmgTypeAtk or Ability.DmgTypeDef, 1, 4, 3, -4, 0, 1, -1, -1, false, false, null, confclarity),
                AdAbility(10, this.getString(R.string.skill_hit), 0, 0, false, false, 1, 3, 0, 1, Ability.DmgTypeAtk, 1, 12, 0, -4, 0, 1, -1, -1, false, false, null, confsleep),
                AdAbility(11, this.getString(R.string.skill_bash), 0, 0, false, false, 3, 3, 0, 5, Ability.DmgTypeAtk or Ability.DmgTypeDef, 1, 7, 5, -5, 0, 1, -1, -1, false, false, dizziness, confclarity),
                AdAbility(12, this.getString(R.string.skill_smash), 0, 0, false, false, 3, 5, 0, 3, Ability.DmgTypeAtk, 1, 18, 0, -7, 0, 1, -1, -1, false, false, null, confsleep),
                AdAbility(13, this.getString(R.string.skill_berserk), 0, 0, false, false, 4, 7, 0, 4, Ability.DmgTypeDef, 1, 0, 0, 0, -1, 0, -1, -1, false, false, berserk, weakness),
                AdAbility(14, this.getString(R.string.skill_shock), 0, 0, false, false, 4, 4, 0, 7, Ability.DmgTypeAtk or Ability.DmgTypeDef, 1, 10, 5, -9, 0, 7, -1, -1, false, false, dizzystun, confclarity),
                AdAbility(15, this.getString(R.string.skill_crush), 0, 0, false, false, 5, 7, 4, 0, Ability.DmgTypeAtk, 1, 25, 0, -11, 0, 1, -1, -1, false, false, stun, confsleep),
                AdAbility(16, this.getString(R.string.skill_strike), 0, 0, false, true, 1, 0, 0, 0, Ability.DmgTypeAgi, 1, 13, 0, -3, 0, 1, 5, 3, false, false, null, confsleep),
                AdAbility(17, this.getString(R.string.skill_steal), 0, 0, true, true, 1, 0, 0, 0, Ability.DmgTypeDef, 1, 0, 0, 0, 0, 1, 5, 3, false, false, null, null),
                AdAbility(18, this.getString(R.string.skill_dash), 0, 0, false, true, 3, 0, 0, 0, Ability.DmgTypeAgi, 1, 15, 0, -5, 0, 1, 3, 7, false, false, null, confsleep),
                AdAbility(19, this.getString(R.string.skill_poison), 0, 0, false, true, 3, 0, 0, 0, Ability.DmgTypeAgi, 2, 5, 0, 2, 0, 1, 3, 7, false, false, poison, regen),
                AdAbility(20, this.getString(R.string.skill_mug), 0, 0, true, true, 4, 0, 0, 0, Ability.DmgTypeAgi or Ability.DmgTypeAtk, 2, 15, 0, -7, 0, 1, 1, 5, false, false, null, confsleep),
                AdAbility(21, this.getString(R.string.skill_toxic_gas), 0, 0, false, true, 4, 0, 10, 0, Ability.DmgTypeAgi or Ability.DmgTypeWis, 3, 1, 1, 1, 1, 1, 3, 7, false, false, tpoison, tregen),
                AdAbility(22, this.getString(R.string.skill_cheer), 0, 0, false, true, 4, 0, 10, 0, Ability.DmgTypeSpi, -1, 0, 0, -5, -2, 0, 3, 7, false, false, vigour, cure),
                AdAbility(23, this.getString(R.string.skill_venom_blade), 0, 0, false, false, 5, 0, 0, 10, Ability.DmgTypeAgi or Ability.DmgTypeAtk, 1, 17, 0, -9, 0, 1, 3, 7, false, false, poison, conftregen),
                AdAbility(24, this.getString(R.string.skill_absorb), 0, 0, false, true, 1, 0, 0, 3, Ability.DmgTypeWis, 1, 0, 7, -3, 0, 6, -1, -1, true, false, null, null),
                AdAbility(25, this.getString(R.string.skill_drain), 0, 0, false, true, 3, 0, 10, 0, Ability.DmgTypeWis, 1, 15, 0, -3, 0, 6, -1, -1, true, false, null, null),
                AdAbility(26, this.getString(R.string.skill_fireball), 0, 0, false, true, 1, 0, 3, 0, Ability.DmgTypeWis, 1, 11, 0, -5, 0, 2, -1, -1, false, false, null, sleep),
                AdAbility(27, this.getString(R.string.skill_iceshard), 0, 0, false, true, 1, 0, 3, 0, Ability.DmgTypeWis, 1, 11, 0, -5, 0, 3, -1, -1, false, false, null, sleep),
                AdAbility(28, this.getString(R.string.skill_lighting), 0, 0, false, true, 1, 0, 3, 0, Ability.DmgTypeWis, 1, 11, 0, -5, 0, 4, -1, -1, false, false, null, sleep),
                AdAbility(29, this.getString(R.string.skill_rock), 0, 0, false, true, 1, 0, 3, 0, Ability.DmgTypeWis, 1, 11, 0, -5, 0, 5, -1, -1, false, false, null, sleep),
                AdAbility(30, this.getString(R.string.skill_darkness), 0, 0, false, true, 1, 0, 3, 0, Ability.DmgTypeWis, 1, 11, 0, -5, 0, 6, -1, -1, false, false, null, sleep),
                AdAbility(31, this.getString(R.string.skill_flame), 0, 0, false, true, 3, 0, 5, 0, Ability.DmgTypeWis, 1, 15, 0, -7, 1, 2, -1, -1, false, false, null, sleep),
                AdAbility(32, this.getString(R.string.skill_blizzard), 0, 0, false, true, 3, 0, 5, 0, Ability.DmgTypeWis, 1, 15, 0, -7, 1, 3, -1, -1, false, false, null, sleep),
                AdAbility(33, this.getString(R.string.skill_storm), 0, 0, false, true, 3, 0, 5, 0, Ability.DmgTypeWis, 1, 15, 0, -7, 1, 4, -1, -1, false, false, null, sleep),
                AdAbility(34, this.getString(R.string.skill_earthquake), 0, 0, false, true, 3, 0, 5, 0, Ability.DmgTypeWis, 1, 15, 0, -7, 1, 5, -1, -1, false, false, null, sleep),
                AdAbility(35, this.getString(R.string.skill_eclipse), 0, 0, false, true, 3, 0, 5, 0, Ability.DmgTypeWis, 1, 13, 0, -7, 1, 6, -1, -1, false, false, null, sleep),
                AdAbility(36, this.getString(R.string.skill_flare), 0, 0, false, true, 5, 0, 12, 0, Ability.DmgTypeWis, 2, 25, 0, -9, 0, 1, -1, -1, false, false, null, sleep),
                AdAbility(37, this.getString(R.string.skill_light_ray), 0, 0, false, true, 1, 0, 3, 0, Ability.DmgTypeSpi or Ability.DmgTypeWis, 1, 11, 0, -5, 0, 7, -1, -1, false, false, null, sleep),
                AdAbility(38, this.getString(R.string.skill_light_burst), 0, 0, false, true, 3, 0, 5, 0, Ability.DmgTypeSpi or Ability.DmgTypeWis, 1, 15, 0, -7, 1, 7, -1, -1, false, false, null, sleep),
                AdAbility(39, this.getString(R.string.skill_confusion), 0, 0, false, true, 5, 0, 15, 0, Ability.DmgTypeWis, 1, 0, 0, 0, 0, 6, -1, -1, false, false, confusion, clarity),
                AdAbility(40, this.getString(R.string.skill_sleep), 0, 0, false, true, 4, 0, 0, 17, Ability.DmgTypeWis, 1, 3, 0, 17, 0, 1, -1, -1, false, false, sleep, clarity),
                AdAbility(41, this.getString(R.string.skill_slash), 0, 0, false, true, 5, 0, 10, 0, Ability.DmgTypeAtk or Ability.DmgTypeAgi, 1, 15, 0, 0, 1, 1, 3, 7, false, false, null, confsleep),
                AdAbility(42, this.getString(R.string.skill_fire_wpn), 0, 0, false, false, 2, 0, 3, 2, Ability.DmgTypeAtk or Ability.DmgTypeWis, 1, 17, 0, -7, 0, 2, -1, -1, false, false, null, confsleep),
                AdAbility(43, this.getString(R.string.skill_ice_wpn), 0, 0, false, false, 2, 0, 3, 2, Ability.DmgTypeAtk or Ability.DmgTypeWis, 1, 17, 0, -7, 0, 3, -1, -1, false, false, null, confsleep),
                AdAbility(44, this.getString(R.string.skill_electric_wpn), 0, 0, false, false, 2, 0, 3, 2, Ability.DmgTypeAtk or Ability.DmgTypeWis, 1, 17, 0, -7, 0, 4, -1, -1, false, false, null, confsleep),
                AdAbility(45, this.getString(R.string.skill_stone_wpn), 0, 0, false, false, 2, 0, 3, 2, Ability.DmgTypeAtk or Ability.DmgTypeWis, 1, 17, 0, -7, 0, 5, -1, -1, false, false, null, confsleep),
                AdAbility(46, this.getString(R.string.skill_dark_wpn), 0, 0, false, false, 2, 0, 3, 2, Ability.DmgTypeAtk or Ability.DmgTypeWis, 1, 17, 0, -7, 0, 6, -1, -1, false, false, null, confsleep),
                AdAbility(47, this.getString(R.string.skill_vampiric_wpn), 0, 0, false, false, 5, 0, 10, 0, Ability.DmgTypeAtk or Ability.DmgTypeAgi or Ability.DmgTypeWis, 1, 21, 0, -9, 0, 6, 3, 7, true, false, null, confsleep),
                AdAbility(48, this.getString(R.string.skill_reflect), 0, 0, false, true, 5, 0, 10, 0, Ability.DmgTypeSpi, 1, 0, 0, 0, 0, 0, -1, -1, false, false, reflect, null),
                AdAbility(49, this.getString(R.string.skill_meteor), 0, 0, false, true, 5, 0, 17, 0, Ability.DmgTypeWis, 1, 19, 0, -11, 1, 1, -1, -1, false, false, null, sleep),
                AdAbility(50, this.getString(R.string.skill_syphon), 0, 0, false, true, 4, 0, 15, 0, Ability.DmgTypeWis, 1, 13, 0, -3, 1, 6, -1, -1, true, false, null, null),
                AdAbility(51, this.getString(R.string.skill_dragon_breath), 0, 0, false, false, 4, 0, 13, 7, Ability.DmgTypeWis or Ability.DmgTypeAtk, 1, 15, 0, -11, 1, 1, -1, -1, false, false, null, confsleep),
                AdAbility(52, this.getString(R.string.skill_light_wpn), 0, 0, false, false, 2, 0, 3, Ability.DmgTypeWis, Ability.DmgTypeSpi or Ability.DmgTypeWis or Ability.DmgTypeAtk, 1, 17, 0, -5, 0, 7, -1, -1, false, false, null, confsleep),
                AdAbility(53, this.getString(R.string.skill_heal), 0, 0, false, true, 1, 0, 3, 0, Ability.DmgTypeWis, 1, -25, 0, 0, 0, 7, -1, -1, false, false, null, null),
                AdAbility(54, this.getString(R.string.skill_mediatate), 0, 0, false, true, 1, 0, 0, 2, Ability.DmgTypeWis, 1, -3, -7, 0, -1, 7, -1, -1, false, false, null, dizziness),
                AdAbility(55, this.getString(R.string.skill_cure), 0, 0, false, true, 3, 0, 7, 0, Ability.DmgTypeWis, 1, -17, 0, 0, 0, 7, -1, -1, false, true, null, cure),
                AdAbility(56, this.getString(R.string.skill_clarity), 0, 0, false, true, 3, 0, 0, 7, Ability.DmgTypeWis, 1, 0, -3, 0, 0, 7, -1, -1, false, false, clarity, madness),
                AdAbility(57, this.getString(R.string.skill_absorb), 0, 0, false, true, 1, 0, 0, 3, Ability.DmgTypeSpi, 1, 0, 7, -3, 0, 7, -1, -1, true, false, null, null)
        )
    }

    private fun getItems(state: Array<State>): Array<Ability> {
        return arrayOf(
                AdAbility(1, this.getString(R.string.item_potion), 0, 0, false, true, 1, 0, 0, 0, 0, 0, -25, 0, 0, 0, 0, -1, -1, false, false, null, null),
                AdAbility(2, this.getString(R.string.item_ether), 0, 0, false, true, 1, 0, 0, 0, 0, 0, 0, -10, 0, 0, 0, -1, -1, false, false, null, null),
                AdAbility(3, this.getString(R.string.item_tonic), 0, 0, false, true, 1, 0, 0, 0, 0, 0, 0, 0, -10, 0, 0, -1, -1, false, false, null, null),
                AdAbility(4, this.getString(R.string.item_antidote), 0, 0, false, true, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, false, false, null, arrayOf<State>(state[1])),
                AdAbility(9, this.getString(R.string.item_hipotion), 0, 0, false, true, 1, 0, 0, 0, 0, 0, -50, 0, 0, 0, 0, -1, -1, false, false, null, null),
                AdAbility(5, this.getString(R.string.item_hiether), 0, 0, false, true, 1, 0, 0, 0, 0, 0, 0, -25, 0, 0, 0, -1, -1, false, false, null, arrayOf<State>(state[3])),
                AdAbility(6, this.getString(R.string.item_hitonic), 0, 0, false, true, 1, 0, 0, 0, 0, 0, 0, -20, 0, 0, 0, -1, -1, false, false, arrayOf<State>(state[4]), arrayOf<State>(state[5])),
                AdAbility(7, this.getString(R.string.item_panacea), 0, 0, false, true, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, false, false, null, arrayOf<State>(state[1], state[3], state[5], state[6], state[7], state[8], state[9])),
                AdAbility(8, this.getString(R.string.item_elixir), 0, 0, false, true, 1, 0, 0, 0, 0, 0, -100, -100, -100, 0, 0, -1, -1, false, true, null, null)
        )
    }

    private fun getEnemies(actors: Array<Actor>): Array<Array<Actor>> {
        return arrayOf(
                arrayOf(actors[3], actors[5]),
                arrayOf(actors[4], actors[6]),
                arrayOf(actors[3], actors[4], actors[5]),
                arrayOf(actors[4], actors[6], actors[3]),
                arrayOf(actors[3], actors[6], actors[4], actors[5]),
                arrayOf(actors[5], actors[4], actors[6], actors[3])
        )
    }

    private fun displayMsg(t: String, s: String, r: Runnable?) {
        val text = SpannableString(s)
        Linkify.addLinks(text, Linkify.WEB_URLS)
        val msg = AlertDialog.Builder(this)
        msg.setCancelable(false)
        msg.setMessage(text)
        msg.setTitle(t)
        msg.setPositiveButton(this.getString(R.string.ok)) { _, _ -> r?.run() }
        val dialog = msg.create()
        dialog.show()
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        this.setContentView(R.layout.activity_demo)

        this.fragment = this.findViewById(R.id.Fragment)
        this.partyLayout = this.findViewById(R.id.PartyLayout)

        val state = this.states
        val skill = this.getSkills(state)
        val item = this.getItems(state)
        val pcRace = this.getRaces(skill)
        val pcJob = this.getJobs(skill)
        val player = this.getCharacters(state, skill, pcRace, item)
        val enemy = this.getEnemies(player)

        val raceList = ArrayAdapter<String>(this, android.R.layout.simple_spinner_item)
        val jobList = ArrayAdapter<String>(this, android.R.layout.simple_spinner_item)
        val raceBox = arrayOf<Spinner>(
                this.findViewById(R.id.RaceBox1),
                this.findViewById(R.id.RaceBox2),
                this.findViewById(R.id.RaceBox3)
        )
        for (i in pcRace.indices) {
            raceList.add(pcRace[i].name)
        }
        for (i in 0..2) {
            raceBox[i].adapter = raceList
        }
        raceBox[0].setSelection(1)
        raceBox[1].setSelection(0)
        raceBox[2].setSelection(2)
        val jobBox = arrayOf<Spinner>(
                this.findViewById(R.id.JobBox1),
                this.findViewById(R.id.JobBox2),
                this.findViewById(R.id.JobBox3)
        )
        for (i in pcJob.indices) {
            jobList.add(pcJob[i].name)
        }
        for (i in 0..2) {
            jobBox[i].adapter = jobList
        }
        jobBox[0].setSelection(0)
        jobBox[1].setSelection(5)
        jobBox[2].setSelection(3)
        val nameEdit = arrayOf<EditText>(
                this.findViewById(R.id.NameField1),
                this.findViewById(R.id.NameField2),
                this.findViewById(R.id.NameField3)
        )

        val cAction = OnClickListener { v ->
            when (v.id) {
                R.id.StartBt -> {
                    for (p in 0..2) {
                        var n = nameEdit[p].text.toString()
                        if (n == "")
                            when (p) {
                                0 -> n = this@DemoAct.getString(R.string.name_cody)
                                1 -> n = this@DemoAct.getString(R.string.name_george)
                                2 -> n = this@DemoAct.getString(R.string.name_stephen)
                            }
                        player[p].name = n
                        if (raceBox[p].isEnabled) {
                            player[p].race = pcRace[raceBox[p].selectedItemPosition]
                        }
                        player[p].job = pcJob[jobBox[p].selectedItemPosition]
                        raceBox[p].isEnabled = false
                    }
                    val surprise = when {
                        this.level % 2 == 1 -> -1
                        this.level == 2 -> 1
                        else -> 0
                    }
                    for (i in 0 until player.size) {
                        player[i].recover()
                    }
                    this.arena = this.stage(this, R.id.Fragment, 0, 0, this.party, enemy[this.level], surprise, this.level % 2 == 0, null)
                    this.partyLayout.visibility = View.GONE
                    this.fragment.visibility = View.VISIBLE
                }
                R.id.InfoBt -> this@DemoAct.displayMsg(this@DemoAct.getString(R.string.about), this@DemoAct.getString(R.string.msg_about), null)
                R.id.QuitBt -> this@DemoAct.finish()
            }
        }

        (this.findViewById(R.id.StartBt) as Button).setOnClickListener(cAction)
        (this.findViewById(R.id.InfoBt) as Button).setOnClickListener(cAction)
        (this.findViewById(R.id.QuitBt) as Button).setOnClickListener(cAction)
        val items = LinkedHashMap<Ability, Int>()
        for (i in item.indices) {
            items[item[i]] = 5
            items[item[i]] = 5
            items[item[i]] = 5
        }
        this.party = arrayOf(player[0], player[2], player[1])
        this.party[0].items = items
        this.party[1].items = items
        this.party[2].items = items
        if (Locale.getDefault().language != "en") {
            Scene.victoryTxt = this.getString(R.string.scene_victory)
            Scene.fallenTxt = this.getString(R.string.scene_defeat)
            Scene.escapeTxt = this.getString(R.string.scene_escape)
            Scene.performsTxt = this.getString(R.string.scene_performs)
            Scene.failTxt = this.getString(R.string.scene_fail)
            Actor.koTxt = this.getString(R.string.actor_ko)
            Ability.reflectedTxt = this.getString(R.string.ability_reflected)
            Ability.suffersTxt = this.getString(R.string.ability_suffers)
            Ability.stolenTxt = this.getString(R.string.ability_stolen)
            Ability.missesTxt = this.getString(R.string.ability_misses)
            State.causesTxt = this.getString(R.string.state_causes)
            Role.hpText = this.getString(R.string.costume_hp)
            Role.mpText = this.getString(R.string.costume_mp)
            Role.spText = this.getString(R.string.costume_sp)
        }
    }

    override fun onArenaConclusion(outcome: Int) {
        this.fragment.visibility = View.GONE
        this.partyLayout.visibility = View.VISIBLE
        this.supportFragmentManager.beginTransaction().remove(this.arena).commit()
        if (outcome > 0 && this@DemoAct.level < 5) this@DemoAct.level++
    }

    override fun onBackPressed() {
        if (this.fragment.visibility == View.GONE) {
            super.onBackPressed()
        } else {
            this.arena.renounce()
        }
    }

}