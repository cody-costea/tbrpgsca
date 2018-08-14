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
package com.codycostea.tbrpgsca

import java.util.ArrayList

import android.app.Activity
import android.app.AlertDialog
import android.content.Intent
import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.text.SpannableString
import android.text.util.Linkify
import android.view.View.OnClickListener
import android.widget.ArrayAdapter
import android.widget.Button
import android.widget.EditText
import android.widget.Spinner

class DemoAct : AppCompatActivity() {

    private lateinit var Skill: Array<Ability>
    private lateinit var Item: ArrayList<Ability>
    private lateinit var pcRace: Array<Costume>
    private lateinit var pcJob: Array<Costume>
    private lateinit var Player: Array<Actor>
    private lateinit var Party: Array<Actor>
    private lateinit var Enemy: Array<Array<Actor>>

    private lateinit var name: Array<EditText>
    private lateinit var jobBox: Array<Spinner>
    private lateinit var raceBox: Array<Spinner>
    private lateinit var jobList: ArrayAdapter<String>
    private lateinit var raceList: ArrayAdapter<String>
    private lateinit var Begin: Button
    private lateinit var About: Button
    private lateinit var Exit: Button
    private var level = 0

    private val cAction = OnClickListener { v ->
        when (v.id) {
            R.id.StartBt -> this@DemoAct.beginBt()
            R.id.InfoBt -> this@DemoAct.displayMsg("About", this@DemoAct.getString(R.string.msg_about))
            R.id.QuitBt -> this@DemoAct.finish()
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        this.setContentView(R.layout.activity_demo)

        val state = this.AddStates()
        val skill = AddSkills(state)
        val item = AddItems(state)
        val pcRace = AddRaces()
        val pcJob = AddJobs(skill)
        val player = AddCharacters(state, skill, pcRace)
        val enemy = AddEnemies(player)

        this.Skill = skill
        this.Item = item
        this.pcRace = pcRace
        this.pcJob = pcJob
        this.Player = player
        this.Enemy = enemy

        this.raceList = ArrayAdapter(this, android.R.layout.simple_spinner_item)
        this.jobList = ArrayAdapter(this, android.R.layout.simple_spinner_item)
        this.raceBox = arrayOf(
            this.findViewById(R.id.RaceBox1),
            this.findViewById(R.id.RaceBox2),
            this.findViewById(R.id.RaceBox3)
        )
        for (i in pcRace.indices) {
            this.raceList.add(pcRace[i].name)
        }
        for (i in 0..2) {
            this.raceBox[i].setAdapter(this.raceList)
        }
        this.raceBox[0].setSelection(1)
        this.raceBox[1].setSelection(0)
        this.raceBox[2].setSelection(2)
        this.jobBox = arrayOf(
            this.findViewById(R.id.JobBox1),
            this.findViewById(R.id.JobBox2),
            this.findViewById(R.id.JobBox3)
        )
        for (i in pcJob.indices) {
            this.jobList.add(pcJob[i].name)
        }
        for (i in 0..2) {
            this.jobBox[i].setAdapter(this.jobList)
        }
        this.jobBox[0].setSelection(0)
        this.jobBox[1].setSelection(5)
        this.jobBox[2].setSelection(3)
        this.name = arrayOf(
            this.findViewById(R.id.NameField1) as EditText,
            this.findViewById(R.id.NameField2) as EditText,
            this.findViewById(R.id.NameField3) as EditText
        )
        this.Begin = this.findViewById(R.id.StartBt) as Button
        this.About = this.findViewById(R.id.InfoBt) as Button
        this.Exit = this.findViewById(R.id.QuitBt) as Button
        this.Begin.setOnClickListener(this.cAction)
        this.About.setOnClickListener(this.cAction)
        this.Exit.setOnClickListener(this.cAction)
        this.Party = arrayOf(this.Player[0], this.Player[2], this.Player[1])
        for (i in this.Item.indices) {
            this.Party[0].items[this.Item[i]] = 5
            this.Party[1].items[this.Item[i]] = 5
            this.Party[2].items[this.Item[i]] = 5
        }
    }

    private fun setPlayer(p: Int) {
        var n = this.name[p].getText().toString()
        if (n == "")
            when (p) {
                1 -> n = "Cody"
                2 -> n = "George"
                3 -> n = "Stephen"
            }
        this.Player[p].name = n
        if (this.raceBox[p].isEnabled()) {
            this.Player[p].race = this.pcRace[this.raceBox[p].getSelectedItemPosition()]
        }
        this.Player[p].job = this.pcJob[this.jobBox[p].getSelectedItemPosition()]
        this.raceBox[p].setEnabled(false)
    }

    private fun beginBt() {
        for (i in 0..2) {
            this.setPlayer(i)
        }
        val surprise = when {
            this.level % 2 == 1 -> -1
            this.level == 2 -> 1
            else -> 0
        }
        for (i in 0 until this.Player.size) {
            this.Player[i].recover()
        }
        ArenaAct.Begin(this, 0, 0, this.Party, this.Enemy[this.level], surprise, this.level % 2 == 0, null)
    }

    private fun displayMsg(t: String, s: String) {
        val text = SpannableString(s)
        Linkify.addLinks(text, Linkify.WEB_URLS)
        val msg = AlertDialog.Builder(this)
        msg.setCancelable(false)
        msg.setMessage(text)
        msg.setTitle(t)
        msg.setPositiveButton("Ok") { arg0, arg1 -> }
        val dialog = msg.create()
        dialog.show()
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent) {
        if (requestCode >= 0) {
            if (resultCode == Activity.RESULT_OK) {
                val extra = data.extras
                /*if (extra.containsKey("Party")) {
                    System.arraycopy(extra.getParcelableArray("Party"), 0, this.Party, 0, 4)
                    this.Player[1] = this.Party[0]
                    this.Player[2] = this.Party[1]
                    this.Player[3] = this.Party[2]
                }
                if (extra.containsKey("Item"))
                    this.Item = extra.getParcelableArrayList<AdAbility>("Item")*/
                if (extra.getInt("Outcome") > 0 && this@DemoAct.level < 5) this@DemoAct.level++
            }
        }
    }

    private fun AddStates(): Array<State> {
        return arrayOf(
            AdState(1, "Regen", false, false, false, false,-1, 0, 10, 0, 0, 0,
                    2, 0, 0, 0, 0, false, null, null, null),
            AdState(2, "Poison", false, false, false, false, 10, 0, -7, 0, -2,
                    0, -2, 0, 0, 0, 0, false, null, null, null),
            AdState(3, "Clarity", false, false, false, false,-1, 0, 0, 7, 0, 0,
                    0, 1, 1, 0, 0, false, null, null, null),
            AdState(4, "Dizziness", false, false, false, false, 3, 0, 0, -7, 0,
                    0, 0, -1, -1, 0, 0, false, null, null, null),
            AdState(5, "Vigour", false, false, false, false, -1, 0, 0, 0, 7, 1,
                    0, 0, 0, 1, 0, false, null, null, null),
            AdState(6, "Weakness", false, false, false, false, 5, 0, 0, 0, -7,
                    -1, 0, 0, 0, -1, 0, false, null, null, null),
            AdState(7, "Berserk", false, true, false, false, 7, 0, 0, 0, 0, 5,
                    -3, 0, 0, 3, 0, false, null, null, null),
            AdState(8, "Confusion", false, false, true, false, 3, 2, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, false, null, null, null),
            AdState(9, "Sleep", true, false, false, false, 5, 1, 0, 0, 0, 0, -3,
                    0, 0, -3, 0, false, null, null, null),
            AdState(10, "Stun", true, false, false, false, 1, 0, 0, 0, 0, 0, -1,
                    0, 0, -1, 0, false, null, null, null),
            AdState(11, "Reflect", false, false, false, true, 7, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, false, null, null, null)
        )
    }

    private fun AddRaces(): Array<Costume> {

        val skills: Array<Ability> = arrayOf(
                AdAbility(1, "Attack", 0, 0, false, null, 1, 0, 0, 0, 0, 1, 10,
                        0, 0, 0, 0, 0, 0, false, false, null, null),
                AdAbility(2, "Defend", 0, 0, false, false, 1, 0, 0, 0, 1, 1,
                        0, -2, -3, 0, 0, 0, 0, false, false, null)
        )

        return arrayOf(
            AdCostume(1, "Elf", "", 40, 25, 10, 7, 5, 15, 12, 11, 0, false, mutableMapOf(1 to -1, 2 to 1, 3 to 1, 4 to 1, 5 to 1), skills, null, null),
            AdCostume(2, "Human", "", 47, 15, 13, 9, 11, 9, 11, 10, 0, false, mutableMapOf(6 to -1, 7 to 1), skills, null, null),
            AdCostume(3, "Half-Orc", "", 55, 7, 13, 17, 12, 5, 7, 9, 0, false, mutableMapOf(1 to 1, 2 to -1, 3 to -1, 4 to -1, 5 to -1), skills, null, null),
            AdCostume(4, "Gnome", "", 40, 15, 20, 12, 8, 10, 5, 15, 0, false, mutableMapOf(6 to 1, 7 to -1), skills, null, null)
        )

    }

    private fun AddJobs(skill : Array<Ability>): Array<Costume> {

        return arrayOf(
                AdCostume(15, "Hero", "hero", 1, 1, 1, 0, 0, 0, 0, 0, 0, false, null, arrayOf(skill[8], skill[9], skill[10], skill[11], skill[23], skill[24], skill[25], skill[26], skill[27], skill[28], skill[29], skill[41], skill[42], skill[43], skill[44], skill[45], skill[30], skill[31], skill[32], skill[33], skill[34], skill[36], skill[51], skill[37], skill[2], skill[3], skill[4], skill[5], skill[15], skill[16], skill[17], skill[18]), null, null),
                AdCostume(1, "Berserker", "berserker", 1, 0, 0, 1, 0, 0, 0, 1, 0, false, mutableMapOf(1 to 1, 2 to -1, 3 to -1, 4 to -1, 5 to -1), arrayOf(skill[8], skill[9], skill[10], skill[11], skill[12], skill[14]), null, null),
                AdCostume(2, "Sorcerer", "wizard", 1, 0, 0, 0, 1, 0, 1, 0, 0, false,  mutableMapOf(1 to -1, 2 to 1, 3 to 1, 4 to 1, 5 to 1, 6 to -1), arrayOf(skill[23], skill[24], skill[25], skill[26], skill[27], skill[28], skill[29], skill[30], skill[31], skill[32], skill[33], skill[34], skill[35], skill[38]), null, null),
                AdCostume(3, "Hesychast", "hesychast", 0, 1, 0, 0, 1, 0, 1, 0, 0, false, mutableMapOf(6 to -7, 7 to 7), arrayOf(skill[2], skill[3], skill[4], skill[5], skill[6], skill[7]), null, null),
                AdCostume(4, "Spy", "spy", 0, 0, 1, 1, 0, 0, 0, 1, 0, false,  null, arrayOf(skill[15], skill[16], skill[17], skill[18], skill[19], skill[38]), null, null),
                AdCostume(5, "Alchemist", "alchemist", 0, 1, 0, 0, 0, 1, 0, 1, 0, false, null, arrayOf(skill[23], skill[24], skill[25], skill[26], skill[27], skill[28], skill[30], skill[31], skill[32], skill[33], skill[15], skill[16], skill[17], skill[18], skill[20]), null, null),
                AdCostume(6, "Dragoon", "dragoon", 1, 0, 0, 1, 0, 1, 0, 0, 0, false, mutableMapOf(2 to 1, 3 to 1, 4 to 1, 5 to 1, 7 to -1), arrayOf(skill[8], skill[9], skill[10], skill[11], skill[23], skill[24], skill[25], skill[26], skill[27], skill[28], skill[30], skill[31], skill[32], skill[33], skill[41], skill[42], skill[43], skill[44], skill[50]), null, null),
                AdCostume(7, "Knight", "knight", 1, 0, 0, 0, 1, 0, 1, 0, 0, false, mutableMapOf(1 to 1, 2 to -1, 3 to -1, 4 to -1, 5 to -1, 6 to -7, 7 to 7), arrayOf(skill[2], skill[3], skill[4], skill[5], skill[8], skill[9], skill[10], skill[11], skill[13]), null, null),
                AdCostume(8, "Warden", "ranger", 0, 0, 1, 0, 0, 0, 1, 1, 0, false, mutableMapOf(6 to -2, 7 to 2), arrayOf(skill[2], skill[3], skill[4], skill[5], skill[15], skill[16], skill[17], skill[18], skill[21]), null, null),
                AdCostume(9, "Shaman", "shaman", 0, 1, 0, 0, 0, 1, 1, 0, 0, false, mutableMapOf(6 to 7, 7 to -7), arrayOf(skill[52], skill[53], skill[54], skill[55], skill[23], skill[24], skill[29], skill[34], skill[49]), null, null),
                AdCostume(10, "Corsair", "corsair", 0, 0, 1, 1, 0, 0, 0, 1, 0, false, null, arrayOf(skill[8], skill[9], skill[10], skill[11], skill[15], skill[16], skill[17], skill[18], skill[22]), null, null),
                AdCostume(11, "Reaver", "reaver", 1, 0, 0, 1, 0, 0, 0, 0, 0, false, mutableMapOf(6 to 7, 7 to -7), arrayOf(skill[8], skill[9], skill[11], skill[23], skill[24], skill[29], skill[15], skill[16], skill[18], skill[45], skill[46]), null, null),
                AdCostume(12, "Ninja", "ninja", 0, 0, 1, 0, 0, 0, 0, 1, 0, false, null, arrayOf(skill[8], skill[9], skill[11], skill[15], skill[16], skill[17], skill[2], skill[3], skill[5], skill[40]), null, null),
                AdCostume(13, "Crusader", "crusader", 1, 0, 0, 0, 0, 0, 1, 0, 0, false, mutableMapOf(6 to -7, 7 to 7), arrayOf(skill[8], skill[9], skill[10], skill[2], skill[3], skill[4], skill[56], skill[36], skill[51], skill[37], skill[47]), null, null),
                AdCostume(14, "Druid", "druid", 0, 1, 0, 0, 0, 1, 0, 0, 0, false, mutableMapOf(2 to 1, 3 to 1, 4 to 1, 5 to 1, 6 to -1, 7 to -1), arrayOf(skill[23], skill[25], skill[26], skill[27], skill[28], skill[30], skill[31], skill[32], skill[33], skill[52], skill[53], skill[54], skill[15], skill[16], skill[18], skill[48]), null, null)
        )
    }

    private fun AddCharacters(state: Array<State>, skill : Array<Ability>, race : Array<Costume>): Array<Actor> {

        val maxlv = 5

        val ogreJob = AdCostume(1, "Ogre", "ogre", 1, 0, 0, 1, 0, 0, 0, 1, 0, false, mutableMapOf(1 to 1, 2 to -1, 3 to -1, 4 to -1, 5 to -1), arrayOf(skill[8], skill[9], skill[10], skill[11], skill[12], skill[14]), null, null)
        val lizardJob = AdCostume(2, "Lizard", "lizard", 1, 0, 0, 0, 1, 0, 1, 0, 0, false,  mutableMapOf(1 to -1, 2 to 1, 3 to 1, 4 to 1, 5 to 1, 6 to -1), arrayOf(skill[23], skill[24], skill[25], skill[26], skill[27], skill[28], skill[29], skill[30], skill[31], skill[32], skill[33], skill[34], skill[35], skill[38]), null, null)
        val trollJob = AdCostume(3, "Troll", "troll", 0, 1, 0, 0, 1, 0, 1, 0, 0, false, mutableMapOf(6 to -7, 7 to 7), arrayOf(skill[2], skill[3], skill[4], skill[5], skill[6], skill[7]), null, null)
        val goblinJob = AdCostume(4, "Goblin", "goblin", 0, 0, 1, 1, 0, 0, 0, 1, 0, false,  null, arrayOf(skill[15], skill[16], skill[17], skill[18], skill[19], skill[38]), null, null)

        return arrayOf(
                AdActor(1, this, "Cody", null, race[2], goblinJob, maxLv = maxlv),
                AdActor(2, this, "George", null, race[0], lizardJob, maxLv = maxlv),
                AdActor(3, this, "Stephen", null, race[1], trollJob, maxLv = maxlv),
                AdActor(4, this, "Ogre", null, race[2], ogreJob, maxLv = maxlv),
                AdActor(5, this, "Lizard", null, race[0], lizardJob, maxLv = maxlv),
                AdActor(6, this, "Goblin", null, race[3], goblinJob, maxLv = maxlv),
                AdActor(7, this, "Troll", null, race[1], trollJob, maxLv = maxlv)
        )
    }

    private fun AddSkills(state: Array<State>): Array<Ability> {

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
            AdAbility(1, "Attack", 0, 0, false, false, 0, 0, 0, 0, 0, 1, 10, 0, -3, 0, 1, -1, -1, false, false, null, confsleep),
            AdAbility(2, "Defend", 0, 0, false, true, 0, 0, 0, 0, 3, 1, 0, -1, -2, -1, 0, -1, -1, false, false, null, null),
            AdAbility(3, "Heal", 0, 0, false, true, 1, 0, 3, 0, 3, 1, -25, 0, 0, 0, 6, -1, -1, false, true, null, null),
            AdAbility(4, "Meditate", 0, 0, false, true, 1, 0, 0, 0, 3, 1, -1, -7, 10, -1, 6, -1, -1, false, false, null, dizziness),
            AdAbility(5, "Cure", 0, 0, false, true, 3, 0, 7, 0, 3, 1, -17, 0, 0, 0, 7, -1, -1, false, true, null, cure),
            AdAbility(6, "Clarity", 0, 0, false, true, 3, 0, 0, 7, 3, 1, 0, -3, 0, 0, 6, -1, -1, false, false, clarity, madness),
            AdAbility(7, "Regen", 0, 0, false, true, 4, 0, 10, 0, 3, 1, -37, 0, 0, 0, 6, -1, -1, false, true, regen, poison),
            AdAbility(8, "Prayer", 0, 0, false, true, 5, 0, 7, 0, 3, 1, -23, 0, 0, 1, 0, -1, -1, false, true, null, null),
            AdAbility(9, "Smite", 0, 0, false, false, 1, 1, 0, 2, 1, 1, 4, 3, -4, 0, 1, -1, -1, false, false, null, confclarity),
            AdAbility(10, "Hit", 0, 0, false, false, 1, 3, 0, 1, 0, 1, 12, 0, -4, 0, 1, -1, -1, false, false, null, confsleep),
            AdAbility(11, "Bash", 0, 0, false, false, 3, 3, 0, 5, 1, 1, 7, 5, -5, 0, 1, -1, -1, false, false, dizziness, confclarity),
            AdAbility(12, "Smash", 0, 0, false, false, 3, 5, 0, 3, 0, 1, 18, 0, -7, 0, 1, -1, -1, false, false, null, confsleep),
            AdAbility(13, "Berserk", 0, 0, false, false, 4, 7, 0, 4, 3, 1, 0, 0, 0, -1, 0, -1, -1, false, false, berserk, weakness),
            AdAbility(14, "Shock", 0, 0, false, false, 4, 4, 0, 7, 1, 1, 10, 5, -9, 0, 7, -1, -1, false, false, dizzystun, confclarity),
            AdAbility(15, "Crush", 0, 0, false, false, 5, 7, 4, 0, 0, 1, 25, 0, -11, 0, 1, -1, -1, false, false, stun, confsleep),
            AdAbility(16, "Strike", 0, 0, false, true, 1, 0, 0, 0, 4, 1, 13, 0, -3, 0, 1, 5, 3, false, false, null, confsleep),
            AdAbility(17, "Steal", 0, 0, true, true, 1, 0, 0, 0, 3, 1, 0, 0, 0, 0, 1, 5, 3, false, false, null, null),
            AdAbility(18, "Dash", 0, 0, false, true, 3, 0, 0, 0, 4, 1, 15, 0, -5, 0, 1, 3, 7, false, false, null, confsleep),
            AdAbility(19, "Poison", 0, 0, false, true, 3, 0, 0, 0, 4, 2, 5, 0, 2, 0, 1, 3, 7, false, false, poison, regen),
            AdAbility(20, "Mug", 0, 0, true, true, 4, 0, 0, 0, 4, 2, 15, 0, -7, 0, 1, 1, 5, false, false, null, confsleep),
            AdAbility(21, "Toxic Gas", 0, 0, false, true, 4, 0, 10, 0, 6, 3, 1, 1, 1, 1, 1, 3, 7, false, false, tpoison, tregen),
            AdAbility(22, "Cheer", 0, 0, false, true, 4, 0, 10, 0, 3, -1, 0, 0, -5, -2, 0, 3, 7, false, false, vigour, cure),
            AdAbility(23, "Venom Blade", 0, 0, false, false, 5, 0, 0, 10, 4, 1, 17, 0, -9, 0, 1, 3, 7, false, false, poison, conftregen),
            AdAbility(24, "Absorb", 0, 0, false, true, 1, 0, 0, 3, 2, 1, 0, 7, -3, 0, 6, -1, -1, true, false, null, null),
            AdAbility(25, "Drain", 0, 0, false, true, 3, 0, 10, 0, 2, 1, 15, 0, -3, 0, 6, -1, -1, true, false, null, null),
            AdAbility(26, "Fireball", 0, 0, false, true, 1, 0, 3, 0, 2, 1, 11, 0, -5, 0, 2, -1, -1, false, false, null, sleep),
            AdAbility(27, "Iceshard", 0, 0, false, true, 1, 0, 3, 0, 2, 1, 11, 0, -5, 0, 3, -1, -1, false, false, null, sleep),
            AdAbility(28, "Lighting", 0, 0, false, true, 1, 0, 3, 0, 2, 1, 11, 0, -5, 0, 4, -1, -1, false, false, null, sleep),
            AdAbility(29, "Rock", 0, 0, false, true, 1, 0, 3, 0, 2, 1, 11, 0, -5, 0, 5, -1, -1, false, false, null, sleep),
            AdAbility(30, "Darkness", 0, 0, false, true, 1, 0, 3, 0, 2, 1, 11, 0, -5, 0, 6, -1, -1, false, false, null, sleep),
            AdAbility(31, "Flame", 0, 0, false, true, 3, 0, 5, 0, 2, 1, 15, 0, -7, 1, 2, -1, -1, false, false, null, sleep),
            AdAbility(32, "Blizzard", 0, 0, false, true, 3, 0, 5, 0, 2, 1, 15, 0, -7, 1, 3, -1, -1, false, false, null, sleep),
            AdAbility(33, "Storm", 0, 0, false, true, 3, 0, 5, 0, 2, 1, 15, 0, -7, 1, 4, -1, -1, false, false, null, sleep),
            AdAbility(34, "Earthquake", 0, 0, false, true, 3, 0, 5, 0, 2, 1, 15, 0, -7, 1, 5, -1, -1, false, false, null, sleep),
            AdAbility(35, "Eclipse", 0, 0, false, true, 3, 0, 5, 0, 2, 1, 13, 0, -7, 1, 6, -1, -1, false, false, null, sleep),
            AdAbility(36, "Flare", 0, 0, false, true, 5, 0, 12, 0, 2, 2, 25, 0, -9, 0, 1, -1, -1, false, false, null, sleep),
            AdAbility(37, "Light Ray", 0, 0, false, true, 1, 0, 3, 0, 3, 1, 11, 0, -5, 0, 7, -1, -1, false, false, null, sleep),
            AdAbility(38, "Light Burst", 0, 0, false, true, 3, 0, 5, 0, 3, 1, 15, 0, -7, 1, 7, -1, -1, false, false, null, sleep),
            AdAbility(39, "Confusion", 0, 0, false, true, 5, 0, 15, 0, 2, 1, 0, 0, 0, 0, 6, -1, -1, false, false, confusion, clarity),
            AdAbility(40, "Sleep", 0, 0, false, true, 4, 0, 0, 17, 4, 1, 3, 0, 17, 0, 1, -1, -1, false, false, sleep, clarity),
            AdAbility(41, "Slash", 0, 0, false, true, 5, 0, 10, 0, 4, 1, 15, 0, 0, 1, 1, 3, 7, false, false, null, confsleep),
            AdAbility(42, "Fire Wpn", 0, 0, false, false, 2, 0, 3, 2, 5, 1, 17, 0, -7, 0, 2, -1, -1, false, false, null, confsleep),
            AdAbility(43, "Ice Wpn", 0, 0, false, false, 2, 0, 3, 2, 5, 1, 17, 0, -7, 0, 3, -1, -1, false, false, null, confsleep),
            AdAbility(44, "Electric Wpn", 0, 0, false, false, 2, 0, 3, 2, 5, 1, 17, 0, -7, 0, 4, -1, -1, false, false, null, confsleep),
            AdAbility(45, "Stone Wpn", 0, 0, false, false, 2, 0, 3, 2, 5, 1, 17, 0, -7, 0, 5, -1, -1, false, false, null, confsleep),
            AdAbility(46, "Dark Wpn", 0, 0, false, false, 2, 0, 3, 2, 5, 1, 17, 0, -7, 0, 6, -1, -1, false, false, null, confsleep),
            AdAbility(47, "Vampiric Wpn", 0, 0, false, false, 5, 0, 10, 0, 5, 1, 21, 0, -9, 0, 6, 3, 7, true, false, null, confsleep),
            AdAbility(48, "Reflect", 0, 0, false, true, 5, 0, 10, 0, 3, 1, 0, 0, 0, 0, 0, -1, -1, false, false, reflect, null),
            AdAbility(49, "Meteor", 0, 0, false, true, 5, 0, 17, 0, 2, 1, 19, 0, -11, 1, 1, -1, -1, false, false, null, sleep),
            AdAbility(50, "Syphon", 0, 0, false, true, 4, 0, 15, 0, 2, 1, 13, 0, -3, 1, 6, -1, -1, true, false, null, null),
            AdAbility(51, "Dragon Breath", 0, 0, false, false, 4, 0, 13, 7, 5, 1, 15, 0, -11, 1, 1, -1, -1, false, false, null, confsleep),
            AdAbility(52, "Light Wpn", 0, 0, false, false, 2, 0, 3, 2, 7, 1, 17, 0, -5, 0, 7, -1, -1, false, false, null, confsleep),
            AdAbility(53, "Heal", 0, 0, false, true, 1, 0, 3, 0, 2, 1, -25, 0, 0, 0, 7, -1, -1, false, true, null, null),
            AdAbility(54, "Meditate", 0, 0, false, true, 1, 0, 0, 2, 2, 1, -3, -7, 0, -1, 7, -1, -1, false, false, null, dizziness),
            AdAbility(55, "Cure", 0, 0, false, true, 3, 0, 7, 0, 2, 1, -17, 0, 0, 0, 7, -1, -1, false, true, null, cure),
            AdAbility(56, "Clarity", 0, 0, false, true, 3, 0, 0, 7, 2, 1, 0, -3, 0, 0, 7, -1, -1, false, false, clarity, madness),
            AdAbility(57, "Absorb", 0, 0, false, true, 1, 0, 0, 3, 3, 1, 0, 7, -3, 0, 7, -1, -1, true, false, null, null)
        )
    }

    private fun AddItems(state: Array<State>): ArrayList<Ability> {

        return arrayListOf(
            AdAbility(1, "Potion", 0, 0, false, true, 1, 0, 0, 0, 0, 0, -25, 0, 0, 0, 0, -1, -1, false, false, null, null),
            AdAbility(2, "Ether", 0, 0, false, true, 1, 0, 0, 0, 0, 0, 0, -10, 0, 0, 0, -1, -1, false, false, null, null),
            AdAbility(3, "Tonic", 0, 0, false, true, 1, 0, 0, 0, 0, 0, 0, 0, -10, 0, 0, -1, -1, false, false, null, null),
            AdAbility(4, "Antidote", 0, 0, false, true, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, false, false, null, arrayOf<State>(state[1])),
            AdAbility(5, "Hi-Ether", 0, 0, false, true, 1, 0, 0, 0, 0, 0, 0, -25, 0, 0, 0, -1, -1, false, false, null, arrayOf<State>(state[3])),
            AdAbility(6, "Hi-Tonic", 0, 0, false, true, 1, 0, 0, 0, 0, 0, 0, -20, 0, 0, 0, -1, -1, false, false, arrayOf<State>(state[4]), arrayOf<State>(state[5])),
            AdAbility(7, "Panacea", 0, 0, false, true, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, false, false, null, arrayOf<State>(state[1], state[3], state[5], state[6], state[7], state[8], state[9])),
            AdAbility(8, "Elixir", 0, 0, false, true, 1, 0, 0, 0, 0, 0, -100, -100, -100, 0, 0, -1, -1, false, false, null, null),
            AdAbility(9, "Hi-Potion", 0, 0, false, true, 1, 0, 0, 0, 0, 0, -50, 0, 0, 0, 0, -1, -1, false, false, null, null)
        )
    }

    private fun AddEnemies(actors: Array<Actor>): Array<Array<Actor>> {

        return arrayOf(
                arrayOf(actors[3], actors[5]),
                arrayOf(actors[4], actors[6]),
                arrayOf(actors[3], actors[4], actors[5]),
                arrayOf(actors[4], actors[6], actors[3]),
                arrayOf(actors[3], actors[6], actors[4], actors[5]),
                arrayOf(actors[5], actors[4], actors[6], actors[3])
        )
    }

}