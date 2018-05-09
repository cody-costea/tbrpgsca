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

import java.util.*

open class Ability(val id: Int, open val name: String, open val range: Boolean = false, open val steal: Boolean = false,
                   open val lvRq: Int, open val hpC: Int, open val mpC: Int, open val spC: Int, open val hpDmg: Int,
                   open val mpDmg: Int, open val spDmg: Int, open val dmgType: Int, open val atkI: Int, open val trg : Int,
                   open val elm: Int, open val mQty: Int, open val rQty: Int, open val absorb: Boolean, open val restoreKO: Boolean,
                   open val aStates: Array<State>? = null, open val rStates: Array<State>? = null) {

    open fun execute(user: Actor, target: Actor, applyCosts: Boolean): String {
        var s = ""
        var dmg = (Math.random() * 4).toInt()
        val trg : Actor
        if (target.reflect && this.dmgType == 3) {
            s += ", reflected by " + target.name
            trg = user
        }
        else {
            trg = target
        }
        val trgResMap = trg.res
        var res = if (trgResMap === null) 3 else (trgResMap[this.elm] ?: 3)
        if (res > 6) res = -1
        when (this.dmgType) {
            1 -> dmg += (this.atkI + (user.def + user.atk) / 2) / (trg.def * res + 1)
            2 -> dmg += (this.atkI + user.wis) / (trg.spi * res + 1)
            3 -> dmg += (this.atkI + user.spi) / (trg.wis * res + 1)
            4 -> dmg += (this.atkI + (user.agi + user.atk) / 2) / ((trg.agi + trg.def) / 2 * res + 1)
            5 -> dmg += (this.atkI + (user.wis + user.atk) / 2) / ((trg.spi + trg.def) / 2 * res + 1)
            6 -> dmg += (this.atkI + (user.agi + user.wis + user.atk) / 3) / ((trg.agi + trg.spi) / 2 * res + 1)
            7 -> dmg += (this.atkI + (user.spi + user.atk + user.def) / 3) / ((trg.wis + trg.def) / 2 * res + 1)
            else -> dmg += (this.atkI + user.atk) / (trg.def * res + 1)
        }
        if (this.dmgType == 2
                || this.dmgType == 3
                //|| this.qty > 0 && this.mQty < 1
                || this.dmgType != 4 && (Math.random() * 13 + user.agi / 5).toInt() > 2 + trg.agi / 4
                || this.dmgType == 4 && (Math.random() * 13 + user.agi / 3).toInt() > 2 + trg.agi / 4) {
            var dmghp = if (this.hpDmg != 0) (if (this.hpDmg < 0) -1 else 1) * dmg + this.hpDmg else 0
            var dmgmp = if (this.mpDmg != 0) (if (this.mpDmg < 0) -1 else 1) * dmg + this.mpDmg else 0
            var dmgsp = if (this.spDmg != 0) (if (this.spDmg < 0) -1 else 1) * dmg + this.spDmg else 0
            if (res < 0) {
                dmghp = -dmghp
                dmgmp = -dmgmp
                dmgsp = -dmgsp
            }
            trg.hp -= dmghp
            trg.mp -= dmgmp
            trg.sp -= dmgsp
            if (this.absorb) {
                user.hp += dmghp / 2
                user.mp += dmgmp / 2
                user.sp += dmgsp / 2
            }
            if (applyCosts) {
                user.hp -= this.hpC
                user.mp -= this.mpC
                user.sp -= this.spC
                if (this.mQty > 0) {
                    var usrSkillsQty = user.skillsQty
                    if (usrSkillsQty === null) {
                        usrSkillsQty = HashMap()
                        user.skillsQty = usrSkillsQty
                    }
                    usrSkillsQty[this] = (usrSkillsQty[this] ?: this.mQty) - 1
                }
            }
            var c = false
            if (dmghp != 0 || dmgmp != 0 || dmgsp != 0)
                s += ", " + trg.name + " suffers"
            if (dmghp != 0) {
                s += " "
                if (dmghp < 1) s += "+"
                s += (-dmghp).toString() + " HP"
                c = true
            }
            if (dmgmp != 0) {
                if (c) s += ","
                s += " "
                if (dmgmp < 1) s += "+"
                s += (-dmgmp).toString() + " MP"
                c = true
            }
            if (dmgsp != 0) {
                if (c) s += ","
                s += " "
                if (dmgsp < 1) s += "+"
                s += (-dmgsp).toString() + " RP"
            }
            val aStates = this.aStates
            var r : String
            if (aStates !== null) {
                for (state in aStates) {
                    r = state.inflict(trg, false)
                    if (r.isNotEmpty()) {
                        s += ", " + r
                    }
                }
            }
            val trgStates = trg.states
            if (trgStates !== null) {
                val rStates = this.rStates
                if (rStates !== null) {
                    for (i in 0 until rStates.size) {
                        for (aState in trgStates) {
                            if (aState == rStates[i]) {
                                aState.remove(trg, false, false)
                                break
                            }
                        }
                    }
                }
            }
            val trgItems = trg.items
            if (this.steal
                    && trgItems !== null
                    && trgItems !== user.items
                    && trgItems.isNotEmpty()
                    && (Math.random() * 12 + user.agi / 4).toInt() > 4 + trg.agi / 3) {
                var itemId = (Math.random() * trgItems.size).toInt()
                if (itemId < trgItems.size) {
                    val iterator = trgItems.keys.iterator()
                    if (itemId > 1) {
                        itemId--
                    }
                    for (i in 0..itemId) {
                        iterator.next()
                    }
                    val stolen = iterator.next()
                    val trgItemQty = trgItems[stolen]
                    if (trgItemQty !== null && trgItemQty > 0) {
                        var usrItems = user.items
                        if (usrItems === null) {
                            usrItems = TreeMap()
                            user.items = usrItems
                        }
                        usrItems[stolen] = (usrItems[stolen] ?: 0) + 1
                        trgItems[stolen] = trgItemQty - 1
                        s += ", " + stolen.name + " stolen"
                        if (trgItems[stolen] == 0) trgItems.remove(stolen)
                    }
                }
            }
        }
        else {
            s += ", but misses"
        }
        //s += trg.applyStates(false)
        s += user.checkStatus()
        return s
    }

    open fun replenish(user: Actor) {
        if (this.mQty > 0) {
            var usrSkills: MutableMap<Ability, Int>? = user.skillsQty
            if (usrSkills === null) {
                usrSkills = HashMap()
                user.skillsQty = usrSkills
            }
            usrSkills[this] = this.mQty
        }
    }

    open fun canPerform(actor : Actor) : Boolean {
        val skillsQty = actor.skillsQty
        return this.mpC <= actor.mp && this.hpC <= actor.hp && this.spC <= actor.sp && actor.level >= this.lvRq
                && /*(this.mQty < 1 && skillsQty === null) || */(skillsQty === null || (skillsQty[this] ?: 1) > 0)
    }
}