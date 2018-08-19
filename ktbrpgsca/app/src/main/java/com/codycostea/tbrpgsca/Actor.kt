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

import kotlin.collections.ArrayList
import kotlin.collections.HashMap
import kotlin.collections.LinkedHashMap

open class Actor(id : Int, name: String, race: Costume, job: Costume, level : Int = 1, open var maxLv: Int = 9,
                 mActions : Int = 1, mHp: Int = 30, mMp: Int = 10, mSp: Int = 10, mAtk: Int = 7, mDef: Int = 7,
                 mSpi: Int = 7, mWis: Int = 7, mAgi: Int = 7, range : Boolean = false, mRes: MutableMap<Int, Int>? = null,
                 skills: Array<Ability>? = null, states: Array<State>? = null, mStRes: MutableMap<State, Int>? = null)
    : Costume(id, name, mHp + race.mHp + job.mHp, mMp + race.mMp + job.mMp, mSp + race.mSp + job.mSp,
        mAtk + race.atk + job.atk, mDef + race.def + job.def, mSpi + race.spi + job.spi,
        mWis + race.wis + job.wis, mAgi + race.agi + job.agi, mActions, range, mRes, skills, states, mStRes) {

    companion object {
        var koTxt = ", %s falls unconscious"
    }

    open var race : Costume = race
        set(value) {
            this.switchCostume(field, value)
            field = value
        }

    open var job : Costume = job
        set(value) {
            this.switchCostume(field, value)
            field = value
        }

    private var _lv : Int = 1
    open var level : Int
        get() = this._lv
        set(value) {
            var v = value
            if (value > this.maxLv) {
                v = this.maxLv
            }
            while (this._lv < v) {
                this.exp = this.mExp
                this.levelUp()
            }
        }

    open var exp : Int = (level - 1) * 15
        set(value) {
            field = value
            this.levelUp()
        }
    open var mExp : Int = level * 15

    open var actions : Int = this.mActions
    open var guards : Boolean = true
    open var reflect : Boolean = false
    open var automatic : Int = 0

    open var hp : Int = this.mHp
        set(value) {
            field = when {
                value > this.mHp -> this.mHp
                value < 0 -> 0
                else -> value
            }
        }
    open var mp : Int = this.mMp
        set(value) {
            field = when {
                value > this.mMp -> this.mMp
                value < 0 -> 0
                else -> value
            }
        }
    open var sp : Int = this.mSp
        set(value) {
            field = when {
                value > this.mSp -> this.mSp
                value < 0 -> 0
                else -> value
            }
        }

    open var stateDur : MutableMap<State, Int>? = null
        internal set

    open val availableSkills : ArrayList<Ability> = ArrayList()

    private var ranged: Boolean? = null
    override var range: Boolean
        get() {
            var ranged = this.ranged
            if (ranged === null) {
                val equipment = this.equipment?.values
                val states = this.stateDur
                ranged = super.range || this.job.range || this.race.range
                        || (equipment !== null && equipment.any { it.range }
                        || (states !== null && states.any { it.value != 0 && it.key.range }))
                this.ranged = ranged
            }
            return ranged
        }
        set(value) {
            super.range = value
            if (value) {
                this.ranged = true
            }
            else {
                this.ranged = null
            }
        }

    internal var skillsQty : MutableMap<Ability, Int>? = null
    internal var skillsQtyRgTurn : MutableMap<Ability, Int>? = null

    open internal var _items : LinkedHashMap<Ability, Int>? = null
    val items : LinkedHashMap<Ability, Int>
        get() {
            var items = this._items
            if (items === null) {
                items = LinkedHashMap()
                this._items = items
            }
            return items
        }

    private var equipment : MutableMap<Char, Costume>? = null

    open val equippedItems : Map<Char, Costume>?
        get() {
            return if (this.equipment === null) {
                null
            }
            else {
                HashMap<Char, Costume>(this.equipment)
            }
        }

    open fun equipItem(pos: Char, item : Costume) : Costume? {
        val r : Costume? = this.unequipPos(pos)
        var e = this.equipment
        if (e === null) {
            e = HashMap()
            this.equipment = e
        }
        e[pos] = item
        this.switchCostume(null, item)
        return r
    }

    open fun unequipPos(pos : Char) : Costume? {
        val e = this.equipment
        return if (e === null) {
            null
        }
        else {
            val r: Costume? = e.get(pos)
            this.switchCostume(r, null)
            r
        }
    }

    open fun unequipItem(item : Costume) : Char? {
        val e = this.equipment
        if (e !== null) {
            for (k in e.keys) {
                if (this.unequipPos(k) == item) {
                    return k
                }
            }
        }
        return null
    }

    private fun checkRegSkill(skill : Ability) {
        if (skill.rQty > 0) {
            var regSkills = this.skillsQtyRgTurn
            if (regSkills === null) {
                regSkills = HashMap()
                this.skillsQtyRgTurn = regSkills
            }
            regSkills[skill] = 0
        }
    }

    internal fun updateStates(remove : Boolean, states : Array<State>?) {
        if (states === null) {
            return
        }
        val aStates = this.stateDur//this.activeStates
        if (remove) {
            if (aStates === null) {
                return
            }
            for (k in states) {
                k.remove(this, true, true)
            }
        }
        else {
            for (k in states) {
                k.inflict(this, true, true)
            }
        }
    }

    internal fun updateSkills(remove : Boolean, abilities : Array<Ability>?) {
        if (abilities === null) {
            return
        }
        if (remove) {
            for (k in abilities) {
                this.availableSkills.remove(k)
                if (k.rQty > 0) {
                    val regSkills = this.skillsQtyRgTurn
                    if (regSkills !== null) {
                        regSkills.remove(k)
                    }
                }
            }
        }
        else {
            this.availableSkills.ensureCapacity(this.availableSkills.size + abilities.size)
            for (k in abilities) {
                this.availableSkills.add(k)
                this.checkRegSkill(k)
            }
        }
    }

    protected open fun switchCostume(oldRole : Costume?, newRole : Costume?) {
        if (oldRole !== null) {
            this.updateSkills(true, oldRole.skills)
            this.updateAttributes(true, oldRole)
            this.updateResistance(true, oldRole.res, oldRole.stRes)
            this.updateStates(true, oldRole.states)
        }
        if (newRole !== null) {
            this.updateStates(false, newRole.states)
            this.updateResistance(false, newRole.res, newRole.stRes)
            this.updateAttributes(false, newRole)
            this.updateSkills(false, newRole.skills)
        }
    }

    internal open fun updateAttributes(remove : Boolean, role : Costume) {
        val i: Int
        if (remove) {
            i = -1
            if (role.range) {
                this.ranged = null
            }
        }
        else {
            if (role.range) {
                this.ranged = true
            }
            i = 1
        }
        this.mHp += i * role.mHp
        this.mMp += i * role.mMp
        this.mSp += i * role.mSp
        this.atk += i * role.atk
        this.def += i * role.def
        this.mSp += i * role.spi
        this.wis += i * role.wis
        this.agi += i * role.agi
        this.mActions += i * role.mActions
    }

    internal fun updateResistance(remove : Boolean, resMap : Map<Int, Int>?, stResMap : Map<State, Int>?) {
        if (resMap !== null) {
            var r = this.res
            if (r === null) {
                if (remove) {
                    return
                }
                else {
                    r = HashMap(resMap.size)
                    this.res = r
                }
            }
            for (i in resMap.keys) {
                for (k in r.keys) {
                    if (!remove || i == k) {
                        r[k] = (r[k] ?: 3) + ((if (remove) -1 else 1) * (resMap[i] ?: 0))
                    }
                }
            }
        }
        if (stResMap !== null) {
            var rs = this.stRes
            if (rs === null) {
                if (remove) {
                    return
                }
                else {
                    rs = HashMap()
                    this.stRes = rs
                }
            }
            for (i in stResMap.keys) {
                for (k in rs.keys) {
                    if (!remove || i == k) {
                        rs[k] = (rs[k] ?: 3) + ((if (remove) -1 else 1) * (stResMap[i] ?: 0))
                    }
                }
            }
        }
    }

    fun levelUp() {
        while (this.mExp <= this.exp && this._lv < this.maxLv) {
            this.mExp *= 2
            this._lv++
            this.mHp += 3
            this.mMp += 2
            this.mSp += 2
            this.atk++
            this.def++
            this.wis++
            this.spi++
            this.agi++
        }
    }

    fun applyStates(consume: Boolean): String {
        var s = ""
        if (!consume) {
            if (this.automatic < 2 && this.automatic > -2) {
                this.automatic = 0
            }
            else {
                this.automatic = 2
            }
            if (this.hp > 0) {
                //if (consume) this.actions = this.mActions
                this.guards = true
            }
            this.reflect = false
        }
        var c = false
        val sDur = this.stateDur
        if (sDur !== null) {
            for (state in sDur) {
                if (state.value > -3 && this.hp > 0) {
                    val r = state.key.apply(this, consume)
                    if (r.isNotEmpty()) {
                        if (c) s += ", "
                        if (consume && !c) {
                            //s += "\n"
                            c = true
                        }
                        s += r
                    }
                }
            }
        }
        s += checkStatus()
        if (c && consume) s += "."
        return s
    }

    fun checkStatus(): String {
        var s = ""
        if (this.hp < 1) {
            s += String.format(Actor.koTxt, this.name)
            this.actions = 0
            this.guards = false
            this.sp = 0
            val sDur = this.stateDur
            if (sDur !== null) {
                for (state in sDur.keys) {
                    state.remove(this, false, false)
                }
            }
        }
        return s
    }

    fun recover() {
        this.hp = this.mHp
        this.mp = this.mMp
        this.sp = 0
        this.actions = this.mActions
        val sDur = this.stateDur
        if (sDur !== null) {
            for (state in sDur.keys) {
                state.remove(this, true, false)
            }
            if (sDur.isEmpty()) {
                this.stateDur = null
            }
        }
        this.applyStates(false)
        val res = this.res
        if (res !== null) {
            for (r in res) {
                if (r.value == 3) {
                    res.remove(r.key)
                }
            }
            if (res.isEmpty()) {
                this.res = null
            }
        }
        val stRes = this.stRes
        if (stRes !== null) {
            for (r in stRes) {
                if (r.value == 0) {
                    stRes.remove(r.key)
                }
            }
            if (stRes.isEmpty()) {
                this.stRes = null
            }
        }
        val skillQty = this.skillsQty
        if (skillQty !== null) {
            for (ability in skillQty.keys) {
                skillQty[ability] = ability.mQty
            }
        }
    }

    init {
        val raceSkills = race.skills
        val jobSkills = job.skills
        val s : Int = (raceSkills?.size ?: 0)
        + (jobSkills?.size ?: 0)
        + (skills?.size ?: 0)
        this.availableSkills.ensureCapacity(s)
        if (raceSkills !== null) {
            for (a in raceSkills) {
                this.checkRegSkill(a)
                this.availableSkills.add(a)
            }
        }
        if (jobSkills !== null) {
            for (a in jobSkills) {
                this.checkRegSkill(a)
                this.availableSkills.add(a)
            }
        }
        if (skills !== null) {
            for (a in skills) {
                this.checkRegSkill(a)
                this.availableSkills.add(a)
            }
        }
        this.updateStates(false, race.states)
        this.updateStates(false, job.states)
        this.updateStates(false, states)
        /*val e = this.equipment
        if (e != null) {
            for (k in e.keys) {
                this.updateStates(false, e[k]?.states)
            }
        }*/
        if (level > 1) {
            this.level = level
        }
    }
}