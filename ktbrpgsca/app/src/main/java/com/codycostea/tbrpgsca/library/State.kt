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
package com.codycostea.tbrpgsca.library

open class State(id: Int, name: String, sprite: String?, inactivate: Boolean, automate: Boolean, confuse: Boolean, reflect: Boolean,
                 revive: Boolean, open val dur: Int = 3, open val sRes: Int = 0, open val dmgHp: Int = 0, open val dmgMp: Int = 0,
                 open val dmgSp: Int = 0, mHp: Int, mMp: Int, mSp: Int, mAtk: Int, mDef: Int, mSpi: Int, mWis: Int, mAgi: Int,
                 mActions: Int, mInit: Int = 0, range: Boolean, mRes: MutableMap<Int, Int>? = null, skills: Array<Ability>? = null,
                 open val rSkills: Array<Ability>? = null, rStates: Array<State>? = null, mStRes: MutableMap<State, Int>? = null)
    : Costume(id, name, sprite, mHp, mMp, mSp, mAtk, mDef, mSpi, mWis, mAgi, mActions, mInit, range, mRes, skills, rStates, mStRes) {

    companion object {
        @JvmStatic
        var causesTxt: String = " %s causes %s"

        const val FLAG_AUTOMATE: Int = 2
        const val FLAG_CONFUSE: Int = 4
        const val FLAG_INACTIVATE: Int = 8
        const val FLAG_REFLECT: Int = 16
        const val FLAG_REVIVE: Int = 32
    }

    open var inactivate: Boolean
        get() {
            return (this.flags and FLAG_INACTIVATE) == FLAG_INACTIVATE
        }
        set(value) {
            val flags = this.flags
            if (value != (flags and FLAG_INACTIVATE == FLAG_INACTIVATE)) {
                this.flags = flags xor FLAG_INACTIVATE
            }
        }

    open var automate: Boolean
        get() {
            return (this.flags and FLAG_AUTOMATE) == FLAG_AUTOMATE
        }
        set(value) {
            val flags = this.flags
            if (value != (flags and FLAG_AUTOMATE == FLAG_AUTOMATE)) {
                this.flags = flags xor FLAG_AUTOMATE
            }
        }

    open var confuse: Boolean
        get() {
            return (this.flags and FLAG_CONFUSE) == FLAG_CONFUSE
        }
        set(value) {
            val flags = this.flags
            if (value != (flags and FLAG_CONFUSE == FLAG_CONFUSE)) {
                this.flags = flags xor FLAG_CONFUSE
            }
        }

    open var reflect: Boolean
        get() {
            return (this.flags and FLAG_REFLECT) == FLAG_REFLECT
        }
        set(value) {
            val flags = this.flags
            if (value != (flags and FLAG_REFLECT == FLAG_REFLECT)) {
                this.flags = flags xor FLAG_REFLECT
            }
        }

    open var revive: Boolean
        get() {
            return (this.flags and FLAG_REVIVE) == FLAG_REVIVE
        }
        set(value) {
            val flags = this.flags
            if (value != (flags and FLAG_REVIVE == FLAG_REVIVE)) {
                this.flags = flags xor FLAG_REVIVE
            }
        }

    open fun inflict(actor: Actor, always: Boolean, indefinite: Boolean): String {
        val trgStRes = actor.stRes
        if (always || (Math.random() * 10).toInt() > (if (trgStRes === null) 0 else trgStRes[this]
                        ?: 0) + this.sRes) {
            var trgStates = actor.stateDur
            if (trgStates === null) {
                trgStates = HashMap(1)
                actor.stateDur = trgStates
            }
            val crDur = (trgStates[this] ?: 0)
            if (crDur < this.dur || (crDur > -1 && this.dur < 0)) {
                trgStates[this] = if (indefinite) -2 else this.dur
            }
            actor.updateAttributes(false, this)
            actor.updateResistance(false, this.res, this.stRes)
            actor.updateStates(false, this.aStates)
            actor.updateSkills(false, this.aSkills)
            this.disableSkills(actor, false)
            return this.apply(actor, false)
        } else return ""
    }

    private fun disableSkills(actor: Actor, remove: Boolean) {
        val rSkills = this.rSkills ?: return
        var iSkills = actor.skillsQty
        if (remove) {
            if (iSkills === null) {
                return
            } else {
                for (k in rSkills) {
                    if (k.mQty > 0) {
                        iSkills[k] = -1 * (iSkills[k] ?: 0)
                    } else {
                        iSkills.remove(k)
                    }
                }
            }
        } else {
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
                } else if (dur > -3) {
                    if (consume) {
                        val rnd = (Math.random() * 3).toInt()
                        val dmgHp = (actor.mHp + rnd) * this.dmgHp / 100
                        val dmgMp = (actor.mMp + rnd) * this.dmgMp / 100
                        val dmgSp = (actor.mSp + rnd) * this.dmgSp / 100
                        val actorHp = actor.hp
                        actor.hp = if (actorHp > dmgHp) actorHp - dmgHp else 1
                        actor.mp -= dmgMp
                        actor.sp -= dmgSp
                        if (dmgHp != 0 || dmgMp != 0 || dmgSp != 0) {
                            s += String.format(causesTxt, this.name, actor.name) + Role.getDmgText(dmgHp, dmgMp, dmgSp)
                        }
                        if (dur > 0) {
                            sDur[this] = dur - 1
                        }
                    } else {
                        val sprite = this.sprite
                        if (sprite !== null && sprite.isNotEmpty()) {
                            actor.shapeShift = true
                            actor.sprite = sprite
                        }
                        if (this.inactivate) {
                            if (dur > 0 && dur == this.dur && actor.actions > 0) {
                                sDur[this] = dur - 1
                            }
                            actor.actions = 0
                            actor.guards = false
                        }
                        if (this.revive) {
                            actor.revives = true
                        }
                        if (this.reflect) {
                            actor.reflect = true
                        }
                        if (this.confuse) {
                            val actorAuto = actor.automatic
                            if (actorAuto > -1) {
                                actor.automatic = if (actorAuto < 2) -1 else -2
                            }
                        } else if (this.automate) {
                            val actorAuto = actor.automatic
                            if (actorAuto < 2 && actorAuto > -2) {
                                actor.automatic = 1
                            }
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
        actor.updateStates(true, this.aStates)
        actor.updateSkills(true, this.aSkills)
        this.disableSkills(actor, true)
        if (this.reflect) {
            actor.applyStates(false)
        }
    }

    open fun remove(actor: Actor, delete: Boolean, always: Boolean): Boolean {
        val sDur = actor.stateDur
        return if (sDur !== null && (always || (sDur[this] ?: -2) != -2)) {
            if (sDur[this] ?: -3 > -3) {
                this.disable(actor)
            }
            if (delete) {
                sDur.remove(this)
            } else {
                sDur[this] = -3
            }
            true
        } else {
            false
        }
    }

    init {
        this.inactivate = inactivate
        this.automate = automate
        this.confuse = confuse
        this.reflect = reflect
        this.revive = revive
    }

}