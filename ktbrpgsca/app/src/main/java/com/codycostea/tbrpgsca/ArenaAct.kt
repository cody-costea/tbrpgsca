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

class AdActor(id : Int, val context : Context, name: String, sprites : Array<Array<AnimationDrawable>>? = null, race: Costume, job: Costume,
              level : Int, maxLv: Int, mActions : Int = 1, mHp: Int, mMp: Int, mSp: Int, mAtk: Int, mDef: Int, mSpi: Int, mWis: Int, mAgi: Int,
              mRes: MutableMap<Int, Int>? = null, skills: Array<Ability>? = null, states: Array<State>, mStRes: MutableMap<State, Int>)
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
                arrayOf(context.resources.getDrawable(
                        context.resources.getIdentifier("spr_bt_" + sprName + "_l_idle", "drawable", context.packageName))
                        as AnimationDrawable,
                        context.resources.getDrawable(
                                context.resources.getIdentifier("spr_bt_" + sprName + "_l_ko", "drawable", context.packageName))
                                as AnimationDrawable,
                        context.resources.getDrawable(
                                context.resources.getIdentifier("spr_bt_" + sprName + "_l_hit", "drawable", context.packageName))
                                as AnimationDrawable,
                        context.resources.getDrawable(
                                context.resources.getIdentifier("spr_bt_" + sprName + "_l_fallen", "drawable", context.packageName))
                                as AnimationDrawable,
                        context.resources.getDrawable(
                                context.resources.getIdentifier("spr_bt_" + sprName + "_l_restored", "drawable", context.packageName))
                                as AnimationDrawable,
                        context.resources.getDrawable(
                                context.resources.getIdentifier("spr_bt_" + sprName + "_l_act", "drawable", context.packageName))
                                as AnimationDrawable,
                        context.resources.getDrawable(
                                context.resources.getIdentifier("spr_bt_" + sprName + "_l_cast", "drawable", context.packageName))
                                as AnimationDrawable),
                arrayOf(context.resources.getDrawable(
                        context.resources.getIdentifier("spr_bt_" + sprName + "_r_idle", "drawable", context.packageName))
                        as AnimationDrawable,
                        context.resources.getDrawable(
                                context.resources.getIdentifier("spr_bt_" + sprName + "_r_ko", "drawable", context.packageName))
                                as AnimationDrawable,
                        context.resources.getDrawable(
                                context.resources.getIdentifier("spr_bt_" + sprName + "_r_hit", "drawable", context.packageName))
                                as AnimationDrawable,
                        context.resources.getDrawable(
                                context.resources.getIdentifier("spr_bt_" + sprName + "_r_fallen", "drawable", context.packageName))
                                as AnimationDrawable,
                        context.resources.getDrawable(
                                context.resources.getIdentifier("spr_bt_" + sprName + "_r_restored", "drawable", context.packageName))
                                as AnimationDrawable,
                        context.resources.getDrawable(
                                context.resources.getIdentifier("spr_bt_" + sprName + "_r_act", "drawable", context.packageName))
                                as AnimationDrawable,
                        context.resources.getDrawable(
                                context.resources.getIdentifier("spr_bt_" + sprName + "_r_cast", "drawable", context.packageName))
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

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_arena)
    }
}