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

import java.util.*
import kotlin.collections.LinkedHashMap

open class Ability(val id: Int, open val name: String, open val range: Boolean? = null, open val steal: Boolean = false,
                   open val lvRq: Int, open val hpC: Int, open val mpC: Int, open val spC: Int, open val hpDmg: Int,
                   open val mpDmg: Int, open val spDmg: Int, open val dmgType: Int, open val atkI: Int, open val trg : Int,
                   open val elm: Int, open val mQty: Int, open val rQty: Int, open val absorb: Boolean, open val restoreKO: Boolean,
                   open val aStates: Array<State>? = null, open val rStates: Array<State>? = null) {

    companion object {
        @JvmStatic
        var reflectedTxt = ", reflected by %s"
        @JvmStatic
        var suffersTxt = ", %s suffers"
        @JvmStatic
        var stolenTxt = ", obtaining %s from %s"
        @JvmStatic
        var missesTxt = ", but misses"
        @JvmStatic
        val DmgTypeAtk : Int = 1
        @JvmStatic
        val DmgTypeDef : Int = 2
        @JvmStatic
        val DmgTypeSpi : Int = 4
        @JvmStatic
        val DmgTypeWis : Int = 8
        @JvmStatic
        val DmgTypeAgi : Int = 16
    }

    open fun execute(user: Actor, target: Actor, applyCosts: Boolean): String {
        var s = ""
        var dmg = (Math.random() * 4).toInt()
        val trg : Actor
        val dmgType = this.dmgType
        if (target.reflect && dmgType == DmgTypeWis) {
            s += String.format(reflectedTxt, target.name)
            trg = user
        }
        else {
            trg = target
        }
        val ko = trg.hp < 1
        val trgResMap = trg.res
        var res = if (trgResMap === null) 3 else (trgResMap[this.elm] ?: 3)
        if (res > 6) res = -1
        var canMiss = 0
        var def = 0
        var i = 0
        when {
            dmgType and DmgTypeAtk == DmgTypeAtk -> {
                dmg += user.atk
                def += target.def
                canMiss = 2
                i++
            }
            dmgType and DmgTypeDef == DmgTypeDef -> {
                dmg += user.def
                def += target.def
                i++
            }
            dmgType and DmgTypeSpi == DmgTypeSpi -> {
                dmg += user.spi
                def += target.wis
                i++
            }
            dmgType and DmgTypeWis == DmgTypeWis -> {
                dmg += user.wis
                def += target.spi
                i++
            }
            dmgType and DmgTypeAgi == DmgTypeAgi -> {
                dmg += user.agi
                def += target.agi
                canMiss = -canMiss + 4
                i++
            }
        }
        dmg = if (i == 0) 0 else (this.atkI + (dmg / i)) / (def / i * res + 1)
        if (canMiss == 0 || trg == user
                || (Math.random() * 13 + user.agi / canMiss).toInt() > 2 + trg.agi / 4) {
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
            if (dmghp != 0 || dmgmp != 0 || dmgsp != 0) {
                s += String.format(suffersTxt, trg.name) + Costume.getDmgText(dmghp, dmgmp, dmgsp)
            }
            val aStates = this.aStates
            var r : String
            if (aStates !== null) {
                for (state in aStates) {
                    r = state.inflict(trg, false, false)
                    if (r.isNotEmpty()) {
                        s += ",$r"
                    }
                }
            }
            val trgStates = trg.stateDur
            if (trgStates !== null) {
                val rStates = this.rStates
                if (rStates !== null) {
                    for (i in 0 until rStates.size) {
                        for (aState in trgStates.keys) {
                            if (aState == rStates[i]) {
                                aState.remove(trg, false, false)
                                break
                            }
                        }
                    }
                }
            }
            val trgItems = trg._items
            if (this.steal
                    && trgItems !== null
                    && trgItems !== user._items
                    && trgItems.isNotEmpty()
                    && (Math.random() * 12 + user.agi / 4).toInt() > 4 + trg.agi / 3) {
                val itemId = (Math.random() * trgItems.size).toInt()
                if (itemId < trgItems.size) {
                    /*val iterator = trgItems.keys.iterator()
                    if (itemId > 1) {
                        itemId--
                    }
                    for (i in 0..itemId) {
                        iterator.next()
                    }*/
                    val stolen = trgItems.keys.elementAt(itemId)//if (iterator.hasNext()) iterator.next() else trgItems.keys.first()
                    val trgItemQty = trgItems[stolen]
                    if (trgItemQty !== null && trgItemQty > 0) {
                        var usrItems = user._items
                        if (usrItems === null) {
                            usrItems = LinkedHashMap()
                            user._items = usrItems
                        }
                        usrItems[stolen] = (usrItems[stolen] ?: 0) + 1
                        trgItems[stolen] = trgItemQty - 1
                        s += String.format(stolenTxt, stolen.name, trg.name)
                        if (trgItems[stolen] == 0) trgItems.remove(stolen)
                    }
                }
            }
            s += trg.checkStatus()
        }
        else {
            s += missesTxt
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
        if (ko && trg.hp > 0) {
            trg.applyStates(false)
        }
        return s + user.checkStatus()
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
        return this.mpC <= actor.mp && this.hpC < actor.hp && this.spC <= actor.sp && actor.level >= this.lvRq
                && /*(this.mQty < 1 && skillsQty === null) || */(skillsQty === null || (skillsQty[this] ?: 1) > 0)
    }

}