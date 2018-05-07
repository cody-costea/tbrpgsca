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
import kotlin.collections.ArrayList
import kotlin.collections.HashMap


open class Actor(id : Int, name: String, race: Costume, job: Costume, level : Int, open var maxLv: Int, var mActions : Int = 1,
                 mHp: Int, mMp: Int, mSp: Int, mAtk: Int, mDef: Int, mSpi: Int, mWis: Int, mAgi: Int, mRes: MutableMap<Int, Int>? = null,
                 skills: Array<Ability>? = null, states: Array<State>, mStRes: MutableMap<State, Int>)
    : Costume(id, name, mHp + race.mHp + job.mHp, mMp + race.mMp + job.mMp, mSp + race.mSp + job.mSp,
        mAtk + race.atk + job.atk, mDef + race.def + job.def, mSpi + race.spi + job.spi,
        mWis + race.wis + job.wis, mAgi + race.agi + job.agi, mRes, skills, states, mStRes) {

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

    private var _lv : Int = level
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
            if (value > this.mHp) {
                field = this.mHp
            }
            if (value < -this.mHp) {
                field = -this.mHp
            }
            else {
                field = value
            }
        }
    open var mp : Int = this.mMp
        set(value) {
            if (value > this.mMp) {
                field = this.mMp
            }
            if (value < 0) {
                field = 0
            }
            else {
                field = value
            }
        }
    open var sp : Int = this.mSp
        set(value) {
            if (value > this.mSp) {
                field = this.mSp
            }
            if (value < 0) {
                field = 0
            }
            else {
                field = value
            }
        }

    open var activeStates : MutableMap<State, Int>? = null
        get() {
            if (field == null) {
                this.updateStates(false, this.race.states)
                this.updateStates(false, this.job.states)
                this.updateStates(false, this.states)
                val e = this.equipment
                if (e != null) {
                    for (k in e.keys) {
                        this.updateStates(false, e[k]?.states)
                    }
                }
            }
            return field
        }
        internal set
    open var stateDur : MutableMap<State, Int>? = null
        internal set

    open val availableSkills : ArrayList<Ability> = ArrayList()
        get() {
            if (field.size == 0) {
                val raceSkills = this.race.skills
                val jobSkills = this.job.skills
                val skills = this.skills
                val s : Int = (raceSkills?.size ?: 0)
                            + (jobSkills?.size ?: 0)
                            + (skills?.size ?: 0)
                field.ensureCapacity(s)
                if (raceSkills != null) {
                    for (a in raceSkills) {
                        this.checkRegSkill(a)
                        field.add(a)
                    }
                }
                if (jobSkills != null) {
                    for (a in jobSkills) {
                        this.checkRegSkill(a)
                        field.add(a)
                    }
                }
                if (skills != null) {
                    for (a in skills) {
                        this.checkRegSkill(a)
                        field.add(a)
                    }
                }
                var l : Array<Ability>?
                val e = this.equipment
                if (e != null) {
                    for (k in e.keys) {
                        l = e[k]?.skills
                        if (l != null) {
                            this.updateSkills(false, l)
                        }
                    }
                }
                /*val t = this.activeStates
                if (t != null) {
                    for (k in t) {
                        l = k.skills
                        if (l != null) {
                            this.updateSkills(false, l)
                        }
                        /*l = k.rSkills
                        if (l != null) {
                            this.updateSkills(true, l)
                        }*/
                    }
                }*/
            }
            return field
        }

    internal var skillsQty : MutableMap<Ability, Int>? = null
    internal var skillsQtyRgTurn : MutableMap<Ability, Int>? = null
    //internal var inactiveSkills : ArrayList<Ability>? = null

    open var items : SortedMap<Ability, Int>? = null

    private var equipment : MutableMap<Char, Costume>? = null

    open val equippedItems : Map<Char, Costume>?
        get() {
            if (this.equipment == null) {
                return null
            }
            else {
                return HashMap<Char, Costume>(this.equipment)
            }
        }

    open fun equipItem(pos: Char, item : Costume) : Costume? {
        val r : Costume? = this.unequipPos(pos)
        var e = this.equipment
        if (e == null) {
            e = HashMap()
            this.equipment = e
        }
        e.put(pos, item)
        this.switchCostume(null, item)
        return r
    }

    open  fun unequipPos(pos : Char) : Costume? {
        var e = this.equipment
        if (e == null) {
            return null
        }
        else {
            val r: Costume? = e.get(pos)
            this.switchCostume(r, null)
            return r
        }
    }

    open fun unequipItem(item : Costume) : Char? {
        val e = this.equipment
        if (e != null) {
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
        if (states == null) {
            return
        }
        val aStates = this.activeStates
        if (remove) {
            if (aStates == null) {
                return
            }
            for (k in states) {
                k.remove(this, true, true)
            }
        }
        else {
            for (k in states) {
                k.inflict(this, true)
            }
        }
    }

    internal fun updateSkills(remove : Boolean, abilities : Array<Ability>?) {
        if (abilities == null) {
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
        if (oldRole != null) {
            this.updateSkills(true, oldRole.skills)
            this.updateAttributes(true, oldRole)
            this.updateResistance(true, oldRole.res, oldRole.stRes)
            this.updateStates(true, oldRole.states)
        }
        if (newRole != null) {
            this.updateStates(false, newRole.states)
            this.updateResistance(false, newRole.res, newRole.stRes)
            this.updateAttributes(false, newRole)
            this.updateSkills(false, newRole.skills)
        }
    }

    internal fun updateAttributes(remove : Boolean, role : Costume) {
        this.mHp += (if (remove) -1 else 1) * role.mHp
        this.mMp += (if (remove) -1 else 1) * role.mMp
        this.mSp += (if (remove) -1 else 1) * role.mSp
        this.atk += (if (remove) -1 else 1) * role.atk
        this.def += (if (remove) -1 else 1) * role.def
        this.mSp += (if (remove) -1 else 1) * role.spi
        this.wis += (if (remove) -1 else 1) * role.wis
        this.agi += (if (remove) -1 else 1) * role.agi
    }

    internal fun updateResistance(remove : Boolean, resMap : Map<Int, Int>?, stResMap : Map<State, Int>?) {
        if (resMap != null) {
            var r = this.res
            if (r == null) {
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
        if (stResMap != null) {
            var rs = this.stRes
            if (rs == null) {
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
        if (this.automatic < 2 && this.automatic > -2) {
            this.automatic = 0
        }
        else {
            this.automatic = 2
        }
        if (this.hp > 0) {
            if (consume) this.actions = this.mActions
            this.guards = true
        }
        this.reflect = false
        var c = false
        val sDur = this.stateDur
        if (sDur != null) {
            for (state in sDur) {
                if (state.value != 0 && this.hp > 0) {
                    val r = state.key.apply(this, consume)
                    if (r.isNotEmpty()) {
                        if (c) s += ", "
                        if (consume && !c) {
                            s += "\n"
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
            s += " (and falls unconcious)"
            this.actions = 0
            this.guards = false
            this.sp = 0
            val sDur = this.stateDur
            if (sDur != null) {
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
        if (sDur != null) {
            for (state in sDur.keys) {
                state.remove(this, true, false)
            }
            if (sDur.isEmpty()) {
                this.stateDur = null
            }
        }
        this.applyStates(false)
        val res = this.res
        if (res != null) {
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
        if (stRes != null) {
            for (r in stRes) {
                if (r.value == 0) {
                    stRes.remove(r.key)
                }
            }
            if (stRes.isEmpty()) {
                this.stRes = null
            }
        }
    }

}