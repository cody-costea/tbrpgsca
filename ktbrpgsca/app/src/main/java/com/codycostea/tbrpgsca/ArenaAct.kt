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

import android.content.Context
import android.graphics.drawable.AnimationDrawable
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.widget.ArrayAdapter
import android.widget.Button
import android.widget.ImageView
import android.widget.Spinner

class AdActor(id : Int, private val context : Context, name: String, sprites : Array<Array<AnimationDrawable>>? = null, race: Costume, job: Costume,
              level : Int, maxLv: Int, mActions : Int = 1, mHp: Int, mMp: Int, mSp: Int, mAtk: Int, mDef: Int, mSpi: Int, mWis: Int, mAgi: Int,
              mRes: MutableMap<Int, Int>? = null, skills: Array<Ability>? = null, states: Array<State>?, mStRes: MutableMap<State, Int>?)
    : Actor(id, name, race, job, level, maxLv, mActions, mHp, mMp, mSp, mAtk, mDef, mSpi,
            mWis, mAgi, mRes, skills, states, mStRes) {

    override var job : Costume = job
        set(value) {
            super.job = value
            this.sprites = this.getSprites(value.name.toLowerCase())
        }

    internal var sprites : Array<Array<AnimationDrawable>> = sprites ?: this.getSprites(job.name.toLowerCase())

    private fun getSprites(sprName: String) : Array<Array<AnimationDrawable>> {
        return arrayOf(
                arrayOf(this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_l_idle", "drawable", this.context.packageName))
                                as AnimationDrawable,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_l_ko", "drawable", this.context.packageName))
                                as AnimationDrawable,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_l_hit", "drawable", this.context.packageName))
                                as AnimationDrawable,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_l_fallen", "drawable", this.context.packageName))
                                as AnimationDrawable,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_l_restored", "drawable", this.context.packageName))
                                as AnimationDrawable,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_l_act", "drawable", this.context.packageName))
                                as AnimationDrawable,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_l_cast", "drawable", this.context.packageName))
                                as AnimationDrawable),
                arrayOf(this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_r_idle", "drawable", this.context.packageName))
                                as AnimationDrawable,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_r_ko", "drawable", this.context.packageName))
                                as AnimationDrawable,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_r_hit", "drawable", this.context.packageName))
                                as AnimationDrawable,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_r_fallen", "drawable", this.context.packageName))
                                as AnimationDrawable,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_r_restored", "drawable", this.context.packageName))
                                as AnimationDrawable,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_r_act", "drawable", this.context.packageName))
                                as AnimationDrawable,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_r_cast", "drawable", this.context.packageName))
                                as AnimationDrawable)
        )
    }

    override fun equals(other: Any?): Boolean {
        return super.equals(other) || (other is Actor && other.id == this.id)
    }

    override fun hashCode(): Int {
        return javaClass.hashCode()
    }
}

class AdAbility(id: Int, name: String, val sprId : Int, val sndId : Int, range: Boolean = false,
                steal: Boolean = false, lvRq: Int, hpC: Int, mpC: Int, spC: Int, hpDmg: Int, mpDmg: Int,
                spDmg: Int, dmgType: Int, atkI: Int, trg : Int, elm: Int, mQty: Int, rQty: Int, absorb: Boolean,
                restoreKO: Boolean, aStates: Array<State>? = null, rStates: Array<State>? = null)
    : Ability(id, name, range, steal, lvRq, hpC, mpC, spC, hpDmg, mpDmg, spDmg, dmgType, atkI, trg, elm, mQty,
              rQty, absorb, restoreKO, aStates, rStates) {

    override fun equals(other: Any?): Boolean {
        return super.equals(other) || (other is Ability && other.id == this.id)
    }

    override fun hashCode(): Int {
        return javaClass.hashCode()
    }
}

class AdState(id : Int, name : String, inactivate : Boolean, automate : Boolean, confuse : Boolean, reflect : Boolean,
              dur : Int = 3, sRes : Int = 0, mHp : Int, mMp : Int, mSp : Int, mAtk : Int, mDef: Int, mSpi: Int, mWis : Int,
              mAgi : Int, mRes : MutableMap<Int, Int>? = null, skills : Array<Ability>? = null, rSkills : Array<Ability>? = null,
              rStates : Array<State>? = null,mStRes : MutableMap<State, Int>? = null)
    : State(id, name, inactivate, automate, confuse, reflect, dur, sRes, mHp, mMp, mSp, mAtk, mDef, mSpi, mWis, mAgi, mRes,
            skills, rSkills, rStates, mStRes) {

    override fun equals(other: Any?): Boolean {
        return super.equals(other) || (other is State && other.id == this.id)
    }

    override fun hashCode(): Int {
        return javaClass.hashCode()
    }
}

class ArenaAct : AppCompatActivity() {

    lateinit var scenePlay : Scene

    lateinit var imgActor : Array<ImageView>
    lateinit var autoBtn : Button
    lateinit var runBtn : Button
    lateinit var skillActBtn : Button
    lateinit var itemUseBtn : Button
    lateinit var skillsSpn : Spinner
    lateinit var itemsSpn : Spinner


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        this.setContentView(R.layout.activity_arena)

        val humanRace = Costume(1, "Human")
        val heroJob = Costume(1, "Hero")

        val skills : Array<Ability> = arrayOf(
                AdAbility(1, "Attack", 0, 0, false, false, 1, 0, 0, 1, 10, 0, 0,
                        0, 0, 0, 0, 0, 0, false, false, null, null),
                AdAbility(2, "Defend", 0, 0, false, false, 1, 0, 0, 0, 0, -2, -3,
                        1, 0, -1, 0, 0, 0, false, false, null, null),
                AdAbility(3, "Heal", 0, 0, true, false, 1, 0, 3, 0, -15, 0, 0,
                        3, 0, 0, 0, 0, 0, false, true, null, null)
        )

        val party : Array<Actor> = arrayOf(
                AdActor(1, this, "Cody", null, humanRace, heroJob, 1, 9, 1, 50, 25, 25, 7, 7,
                        7, 7, 7, null, skills, null, null),
                AdActor(2, this, "Victoria", null, humanRace, heroJob, 1, 9, 1, 50, 25, 25, 7, 7,
                        7, 7, 7, null, skills, null, null),
                AdActor(3, this, "Stephanie", null, humanRace, heroJob, 1, 9, 1, 50, 25, 25, 7, 7,
                        7, 7, 7, null, skills, null, null),
                AdActor(4, this, "George", null, humanRace, heroJob, 1, 9, 1, 50, 25, 25, 7, 7,
                        7, 7, 7, null, skills, null, null)
        )

        val enemy : Array<Actor> = arrayOf(
                AdActor(8, this, "Goblin", null, humanRace, heroJob, 1, 9, 1, 50, 25, 25, 7, 7,
                        7, 7, 7, null, skills, null, null),
                AdActor(7, this, "Troll", null, humanRace, heroJob, 1, 9, 1, 50, 25, 25, 7, 7,
                        7, 7, 7, null, skills, null, null),
                AdActor(6, this, "Lizard", null, humanRace, heroJob, 1, 9, 1, 50, 25, 25, 7, 7,
                        7, 7, 7, null, skills, null, null),
                AdActor(5, this, "Ogre", null, humanRace, heroJob, 1, 9, 1, 50, 25, 25, 7, 7,
                        7, 7, 7, null, skills, null, null)
        )

        this.scenePlay = Scene(party, enemy, 0)

        this.runBtn = this.findViewById(R.id.RunBt)
        this.autoBtn = this.findViewById(R.id.AutoBt)
        this.skillActBtn = this.findViewById(R.id.ActBt)
        this.itemUseBtn = this.findViewById(R.id.UseBt)
        this.skillsSpn = this.findViewById(R.id.SkillBox)
        this.itemsSpn = this.findViewById(R.id.ItemBox)

        val imgViews = ArrayList<ImageView>(party.size + enemy.size)
        imgViews.add(this.findViewById(R.id.ImgPlayer1))
        if (party.size > 1) {
            imgViews.add(this.findViewById(R.id.ImgPlayer2))
        }
        if (party.size > 2) {
            imgViews.add(this.findViewById(R.id.ImgPlayer3))
        }
        if (party.size > 3) {
            imgViews.add(this.findViewById(R.id.ImgPlayer4))
        }
        imgViews.add(this.findViewById(R.id.ImgEnemy1))
        if (enemy.size > 1) {
            imgViews.add(this.findViewById(R.id.ImgEnemy2))
        }
        if (enemy.size > 2) {
            imgViews.add(this.findViewById(R.id.ImgEnemy3))
        }
        if (enemy.size > 3) {
            imgViews.add(this.findViewById(R.id.ImgEnemy4))
        }
        this.imgActor = imgViews.toTypedArray()

        /*for (i in 0 until this.scenePlay.enIdx) {
            this.imgActor[i].setBackgroundDrawable((this.scenePlay.players[i] as AdActor).sprites[0]
                    [if (this.scenePlay.players[i].hp > 0) 0 else 1])
        }

        for (i in this.scenePlay.enIdx until this.scenePlay.players.size) {
            this.imgActor[i].setBackgroundDrawable((this.scenePlay.players[i] as AdActor).sprites[1]
                    [if (this.scenePlay.players[i].hp > 0) 0 else 1])
        }*/

        /*this.skillsSpn.adapter = ArrayAdapter<Ability>(this, android.R.layout.simple_spinner_dropdown_item,
                this.scenePlay.players[this.scenePlay.current].availableSkills)*/

    }
}