/*
Copyright (C) AD 2018-2019 Claudiu-Stefan Costea

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

open class Ability(id: Int, name: String, range: Boolean = false, steal: Boolean = false, open val lvRq: Int, open val hpC: Int,
                   open val mpC: Int, open val spC: Int, hpDmg: Int, mpDmg: Int, spDmg: Int, open val dmgType: Int, atkI: Int,
                   open val trg: Int, open val elm: Int, open val mQty: Int, open val rQty: Int, absorb: Boolean, restore: Boolean,
                   aStates: Array<State>? = null, open val rStates: Array<State>? = null)
    : Role(id, name, hpDmg, mpDmg, spDmg, atkI, range, aStates) {

    companion object {
        @JvmStatic
        var reflectedTxt = ", reflected by %s"

        @JvmStatic
        var suffersTxt = ", %s suffers"

        @JvmStatic
        var stolenTxt = ", obtaining %s from %s"

        @JvmStatic
        var missesTxt = ", but misses"

        const val FLAG_STEAL: Int = 2
        const val FLAG_ABSORB: Int = 4
        const val FLAG_RESTORE: Int = 8

        const val DmgTypeAtk: Int = 1
        const val DmgTypeDef: Int = 2
        const val DmgTypeSpi: Int = 4
        const val DmgTypeWis: Int = 8
        const val DmgTypeAgi: Int = 16
    }

    open var steal: Boolean
        get() {
            return (this.flags and FLAG_STEAL) == FLAG_STEAL
        }
        set(value) {
            val flags = this.flags
            if (value != (flags and FLAG_STEAL == FLAG_STEAL)) {
                this.flags = flags xor FLAG_STEAL
            }
        }

    open var absorb: Boolean
        get() {
            return (this.flags and FLAG_ABSORB) == FLAG_ABSORB
        }
        set(value) {
            val flags = this.flags
            if (value != (flags and FLAG_ABSORB == FLAG_ABSORB)) {
                this.flags = flags xor FLAG_ABSORB
            }
        }

    open var restore: Boolean
        get() {
            return (this.flags and FLAG_RESTORE) == FLAG_RESTORE
        }
        set(value) {
            val flags = this.flags
            if (value != (flags and FLAG_RESTORE == FLAG_RESTORE)) {
                this.flags = flags xor FLAG_RESTORE
            }
        }

    open fun execute(user: Actor, target: Actor, applyCosts: Boolean): String {
        var s = ""
        val trg: Actor
        var dmg = (Math.random() * 4).toInt()
        val dmgType = this.dmgType
        if (target.reflect && dmgType == DmgTypeWis) {
            s += String.format(reflectedTxt, target.name)
            trg = user
        } else {
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
        dmg = if (i == 0) 0 else (this.mInit + (dmg / i)) / (def / i * res + 1)
        if (canMiss == 0 || trg == user
                || (Math.random() * 13 + user.agi / canMiss).toInt() > 2 + trg.agi / 4) {
            var dmgHp = this.mHp
            dmgHp = if (dmgHp != 0) (if (dmgHp < 0) -1 else 1) * dmg + dmgHp else 0
            var dmgMp = this.mMp
            dmgMp = if (dmgMp != 0) (if (dmgMp < 0) -1 else 1) * dmg + dmgMp else 0
            var dmgSp = this.mSp
            dmgSp = if (dmgSp != 0) (if (dmgSp < 0) -1 else 1) * dmg + dmgSp else 0
            if (res < 0) {
                dmgHp = -dmgHp
                dmgMp = -dmgMp
                dmgSp = -dmgSp
            }
            trg.hp -= dmgHp
            trg.mp -= dmgMp
            trg.sp -= dmgSp
            if (this.absorb) {
                user.hp += dmgHp / 2
                user.mp += dmgMp / 2
                user.sp += dmgSp / 2
            }
            if (dmgHp != 0 || dmgMp != 0 || dmgSp != 0) {
                s += String.format(suffersTxt, trg.name) + Role.getDmgText(dmgHp, dmgMp, dmgSp)
            }
            val aStates = this.aStates
            var r: String
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
                    for (element in rStates) {
                        for (aState in trgStates.keys) {
                            if (aState == element) {
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
                val itemsSize = trgItems.size
                val itemId = (Math.random() * itemsSize).toInt()
                if (itemId < itemsSize) {
                    val stolen = trgItems.keys.elementAt(itemId)
                    var trgItemQty = trgItems[stolen]
                    if (trgItemQty !== null && trgItemQty > 0) {
                        var usrItems = user._items
                        if (usrItems === null) {
                            usrItems = LinkedHashMap()
                            user._items = usrItems
                        }
                        usrItems[stolen] = (usrItems[stolen] ?: 0) + 1
                        if (--trgItemQty == 0) {
                            trgItems.remove(stolen)
                        } else {
                            trgItems[stolen] = trgItemQty
                        }
                        s += String.format(stolenTxt, stolen.name, trg.name)
                    }
                }
            }
            s += trg.checkStatus()
        } else {
            s += missesTxt
        }
        if (applyCosts) {
            user.hp -= this.hpC
            user.mp -= this.mpC
            user.sp -= this.spC
            val mQty = this.mQty
            if (mQty > 0) {
                var usrSkillsQty = user.skillsQty
                if (usrSkillsQty === null) {
                    usrSkillsQty = HashMap()
                    user.skillsQty = usrSkillsQty
                }
                usrSkillsQty[this] = (usrSkillsQty[this] ?: mQty) - 1
            }
        }
        if (ko && trg.hp > 0) {
            trg.applyStates(false)
        }
        return s + user.checkStatus()
    }

    open fun replenish(user: Actor) {
        val mQty = this.mQty
        if (mQty > 0) {
            var usrSkills: MutableMap<Ability, Int>? = user.skillsQty
            if (usrSkills === null) {
                usrSkills = HashMap()
                user.skillsQty = usrSkills
            }
            usrSkills[this] = mQty
        }
    }

    open fun canPerform(actor: Actor): Boolean {
        val skillsQty = actor.skillsQty
        return this.mpC <= actor.mp && this.hpC < actor.hp && this.spC <= actor.sp && actor.level >= this.lvRq
                && (skillsQty === null || (skillsQty[this] ?: 1) > 0)
    }

    init {
        this.steal = steal
        this.absorb = absorb
        this.restore = restore
    }

}