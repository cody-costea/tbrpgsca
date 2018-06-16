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

open class State(id : Int, name : String, open var inactivate : Boolean, open var automate : Boolean, open var confuse : Boolean,
                 open var reflect : Boolean, open val dur : Int = 3, open val sRes : Int = 0, mHp : Int, mMp : Int, mSp : Int, mAtk : Int,
                 mDef: Int, mSpi: Int, mWis : Int, mAgi : Int, mActions : Int, range: Boolean, mRes : MutableMap<Int, Int>? = null,
                 skills : Array<Ability>? = null,open val rSkills : Array<Ability>? = null, rStates : Array<State>? = null, mStRes : MutableMap<State, Int>? = null)
    : Costume(id, name, mHp, mMp, mSp, mAtk, mDef, mSpi, mWis, mAgi, mActions, range, mRes, skills, rStates, mStRes) {

    open fun inflict(actor: Actor, always: Boolean): String {
        val trgStRes = actor.stRes
        if (always || (Math.random() * 10).toInt() > (if (trgStRes === null) 0 else trgStRes[this] ?: 0) + this.sRes) {
            var trgStates = actor.stateDur
            if (trgStates === null) {
                trgStates = HashMap(1)
                actor.stateDur = trgStates
            }
            if ((trgStates[this] ?: this.dur) < this.dur) {
                trgStates[this] = this.dur
            }
            //actor.switchCostume(null, this)
            actor.updateAttributes(false, this)
            actor.updateResistance(false, this.res, this.stRes)
            actor.updateStates(false, this.states)
            actor.updateSkills(false, this.skills)
            this.disableSkills(actor, false)
            return this.apply(actor, false)
        }
        else return ""
    }

    private fun disableSkills(actor : Actor, remove : Boolean) {
        val rSkills = this.rSkills ?: return
        var iSkills = actor.skillsQty

        if (remove) {
            if (iSkills === null) {
                return
            }
            else {
                for (k in rSkills) {
                    if (k.mQty > 0) {
                        iSkills[k] = -1 * (iSkills[k] ?: 0)
                    }
                    else {
                        iSkills.remove(k)
                    }
                }
            }
        }
        else {
            if (iSkills === null) {
                iSkills = HashMap()
                actor.skillsQty = iSkills
            }
            for (k in rSkills) {
                iSkills[k] = if (k.mQty > 0) -1 * (iSkills[k] ?: 0) else 0
            }
        }
    }

    internal fun apply(actor: Actor, consume: Boolean): String {
        var s = ""
        val sDur = actor.stateDur
        if (sDur !== null) {
            val dur = sDur[this]
            if (dur !== null && actor.hp > 0) {
                if (dur == 0) {
                    this.disable(actor)
                    sDur[this] = -3
                }
                else if (dur > -3) {
                    if (consume) {
                        val rnd = (Math.random() * 3).toInt()
                        val dmghp = (actor.mHp + rnd) * this.mHp / 100
                        val dmgmp = (actor.mMp + rnd) * this.mMp / 100
                        val dmgsp = (actor.mSp + rnd) * this.mSp / 100
                        actor.hp += dmghp
                        actor.mp += dmgmp
                        actor.sp += dmgsp
                        var c = false
                        if (dmghp != 0 || dmgmp != 0 || dmgsp != 0) {
                            s += this.name + " causes " + actor.name
                        }
                        if (dmghp != 0) {
                            s += " "
                            if (dmghp >= 0) {
                                s += "+"
                            }
                            s += dmghp.toString() + " HP"
                            c = true
                        }
                        if (dmgmp != 0) {
                            if (c) {
                                s += ","
                            }
                            s += " "
                            if (dmgmp >= 0) {
                                s += "+"
                            }
                            s += dmgmp.toString() + " MP"
                            c = true
                        }
                        if (dmgsp != 0) {
                            if (c) {
                                s += ","
                            }
                            s += " "
                            if (dmgsp >= 0) {
                                s += "+"
                            }
                            s += dmgsp.toString() + " RP"
                        }
                        if (dur > 0) {
                            sDur[this] = dur - 1
                        }
                    }
                    /*else if (actor.actions > 0 && dur > 0 && dur == this.dur
                            && (this.inactivate || this.automate || this.confuse)) {
                        sDur[this] = dur - 1
                    }*/
                    else {
                        if (this.inactivate) {
                            if (dur > 0 && actor.actions > 0) {
                                sDur[this] = dur - 1
                            }
                            actor.actions = 0
                            actor.guards = false
                        }
                        if (this.reflect) {
                            actor.reflect = true
                        }
                        if (this.automate && actor.automatic < 2) {
                            actor.automatic = 1
                        }
                        if (this.confuse) {
                            actor.automatic = if (actor.automatic < 2) -1 else -2
                        }
                    }
                }
            }
        }
        return s
    }

    internal fun disable(actor: Actor) {
        actor.updateAttributes(true, this)
        actor.updateResistance(true, this.res, this.stRes)
        actor.updateStates(true, this.states)
        actor.updateSkills(true, this.skills)
        this.disableSkills(actor, true)
        if (this.reflect) {
            actor.applyStates(false)
        }
    }

    open fun remove(actor: Actor, delete: Boolean, always: Boolean): Boolean {
        val sDur = actor.stateDur
        if (sDur !== null && (always || (sDur[this] ?: -2) != -2)) {
            if (delete) {
                sDur.remove(this)
            }
            else {
                sDur[this] = -3
            }
            this.disable(actor)
            return true
        }
        else {
            return false
        }
    }
}