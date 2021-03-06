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

import kotlin.collections.ArrayList
import kotlin.collections.HashMap
import kotlin.collections.LinkedHashMap
import kotlin.math.abs

typealias ActorFun = (Actor) -> Unit

open class Actor(id: Int, name: String, race: Costume, job: Costume, level: Int = 1, open var maxLv: Int = 9, mActions: Int = 1,
                 mInit: Int = 0, mHp: Int = 30, mMp: Int = 10, mSp: Int = 10, mAtk: Int = 7, mDef: Int = 7, mSpi: Int = 7, mWis: Int = 7,
                 mAgi: Int = 7, range: Boolean = false, mRes: MutableMap<Int, Int>? = null, skills: Array<Ability>? = null,
                 states: Array<State>? = null, mStRes: MutableMap<State, Int>? = null)
    : Costume(id, name, job.sprite, mHp + race.mHp + job.mHp, mMp + race.mMp + job.mMp, mSp + race.mSp + job.mSp,
        mAtk + race.atk + job.atk, mDef + race.def + job.def, mSpi + race.spi + job.spi, mWis + race.wis + job.wis,
        mAgi + race.agi + job.agi, mActions, mInit, range, mRes, skills, states, mStRes) {

    companion object {
        @JvmStatic
        var koTxt = ", %s falls unconscious"
        var riseTxt = ", but rises again"

        @JvmStatic
        internal var onTurnReorder: ActorFun? = null

        const val FLAG_GUARDS: Int = 2
        const val FLAG_REFLECTS: Int = 4
        const val FLAG_SHAPE_SHIFT: Int = 8
        const val FLAG_AI_PLAYER: Int = 16
        const val FLAG_AUTOMATED: Int = 32
        const val FLAG_CONFUSED: Int = 64
        const val FLAG_REVIVES: Int = 128

        const val AUTO_NONE: Int = 0
        const val AUTO_CONFUSED: Int = -1
        const val AUTO_ENRAGED: Int = 1
        const val AUTO_ENEMY: Int = -2
        const val AUTO_ALLY: Int = 2
    }

    open val active: Boolean
        get() {
            return this.actions > 0
        }

    open var reflect: Boolean
        get() {
            return (this.flags and FLAG_REFLECTS) == FLAG_REFLECTS
        }
        set(value) {
            val flags = this.flags
            if (value != (flags and FLAG_REFLECTS == FLAG_REFLECTS)) {
                this.flags = flags xor FLAG_REFLECTS
            }
        }

    open var guards: Boolean
        get() {
            return (this.flags and FLAG_GUARDS) == FLAG_GUARDS
        }
        set(value) {
            val flags = this.flags
            if (value != (flags and FLAG_GUARDS == FLAG_GUARDS)) {
                this.flags = flags xor FLAG_GUARDS
            }
        }

    open val counters: Boolean
        get() {
            return this.counter !== null
        }

    open var revives: Boolean
        get() {
            return (this.flags and FLAG_REVIVES) == FLAG_REVIVES
        }
        set(value) {
            val flags = this.flags
            if (value != (flags and FLAG_REVIVES == FLAG_REVIVES)) {
                this.flags = flags xor FLAG_REVIVES
            }
        }

    open var shapeShift: Boolean
        get() {
            return (this.flags and FLAG_SHAPE_SHIFT) == FLAG_SHAPE_SHIFT
        }
        set(value) {
            val flags = this.flags
            if (value != (flags and FLAG_SHAPE_SHIFT == FLAG_SHAPE_SHIFT)) {
                this.flags = flags xor FLAG_SHAPE_SHIFT
            }
        }

    override var agi: Int
        get() = super.agi
        set(value) {
            val onReorder = onTurnReorder
            if (onReorder !== null) {
                onReorder(this)
            }
            super.agi = value
        }

    open var race: Costume = race
        set(value) {
            this.switchCostume(field, value)
            field = value
        }

    open var job: Costume = job
        set(value) {
            this.switchCostume(field, value)
            if (!this.shapeShift) {
                this.sprite = value.sprite
            }
            field = value
        }

    private var _lv: Int = 1
    open var level: Int
        get() = this._lv
        set(value) {
            var v = value
            val maxLv = this.maxLv
            if (value > maxLv) {
                v = maxLv
            }
            while (this._lv < v) {
                this.exp = this.mExp
                this.levelUp()
            }
        }

    open var exp: Int = (level - 1) * 15
        set(value) {
            field = value
            this.levelUp()
        }
    open var mExp: Int = level * 15

    open var init: Int = 0

    open var actions: Int = this.mActions

    open var automatic: Int
        get() {
            val flags = this.flags
            return if ((flags and FLAG_AI_PLAYER) == FLAG_AI_PLAYER) {
                if ((flags and FLAG_CONFUSED) == FLAG_CONFUSED) AUTO_ENEMY else AUTO_ALLY
            } else if ((flags and FLAG_AUTOMATED) == FLAG_AUTOMATED) {
                if ((flags and FLAG_CONFUSED) == FLAG_CONFUSED) AUTO_CONFUSED else AUTO_ENRAGED
            } else AUTO_NONE
        }
        set(value) {
            var flags = this.flags
            if (value < 0) {
                flags = (flags or FLAG_CONFUSED)
            } else if ((flags and FLAG_CONFUSED) == FLAG_CONFUSED) {
                flags = flags xor FLAG_CONFUSED
            }
            val absValue = abs(value)
            if (absValue > 0) {
                flags = (flags or FLAG_AUTOMATED)
                if (absValue > 1) {
                    flags = (flags or FLAG_AI_PLAYER)
                } else {
                    if ((flags and FLAG_AI_PLAYER) == FLAG_AI_PLAYER) {
                        flags = flags xor FLAG_AI_PLAYER
                    }
                }
            } else {
                if ((flags and FLAG_AUTOMATED) == FLAG_AUTOMATED) {
                    flags = flags xor FLAG_AUTOMATED
                }
                if ((flags and FLAG_AI_PLAYER) == FLAG_AI_PLAYER) {
                    flags = flags xor FLAG_AI_PLAYER
                }
            }
            this.flags = flags
        }

    open var hp: Int = this.mHp
        set(value) {
            val mHp = this.mHp
            field = when {
                value > mHp -> mHp
                value < 0 -> 0
                else -> value
            }
        }
    open var mp: Int = this.mMp
        set(value) {
            field = when {
                value > this.mMp -> this.mMp
                value < 0 -> 0
                else -> value
            }
        }
    open var sp: Int = this.mSp
        set(value) {
            field = when {
                value > this.mSp -> this.mSp
                value < 0 -> 0
                else -> value
            }
        }

    open var stateDur: MutableMap<State, Int>? = null
        internal set

    open val availableSkills: ArrayList<Ability> = ArrayList()

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
            } else {
                this.ranged = null
            }
        }

    internal var counter: Ability? = null

    internal var skillsQty: MutableMap<Ability, Int>? = null
    internal var skillsQtyRgTurn: MutableMap<Ability, Int>? = null

    internal open var _items: LinkedHashMap<Ability, Int>? = null
    var items: LinkedHashMap<Ability, Int>
        get() {
            var items = this._items
            if (items === null) {
                items = LinkedHashMap()
                this._items = items
            }
            return items
        }
        set(value) {
            this._items = value
        }

    private var equipment: MutableMap<Char, Costume>? = null

    open val equippedItems: Map<Char, Costume>?
        get() {
            val e = this.equipment
            return if (e === null) {
                null
            } else {
                HashMap<Char, Costume>(e)
            }
        }

    open fun equipItem(pos: Char, item: Costume): Costume? {
        val r: Costume? = this.unequipPos(pos)
        var e = this.equipment
        if (e === null) {
            e = HashMap()
            this.equipment = e
        }
        e[pos] = item
        this.switchCostume(null, item)
        return r
    }

    open fun unequipPos(pos: Char): Costume? {
        val e = this.equipment
        return if (e === null) {
            null
        } else {
            val r: Costume? = e.get(pos)
            this.switchCostume(r, null)
            r
        }
    }

    open fun unequipItem(item: Costume): Char? {
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

    private fun checkRegSkill(skill: Ability) {
        if (skill.rQty > 0) {
            var regSkills = this.skillsQtyRgTurn
            if (regSkills === null) {
                regSkills = HashMap()
                this.skillsQtyRgTurn = regSkills
            }
            regSkills[skill] = 0
        }
    }

    internal fun updateStates(remove: Boolean, states: Array<State>?) {
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
        } else {
            for (k in states) {
                k.inflict(this, true, true)
            }
        }
    }

    internal fun updateSkills(remove: Boolean, abilities: Array<Ability>?) {
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
        } else {
            val availableSkills = this.availableSkills
            availableSkills.ensureCapacity(availableSkills.size + abilities.size)
            for (k in abilities) {
                availableSkills.add(k)
                if (k.mQty > 0) {
                    var skillsQty = this.skillsQty;
                    if (skillsQty == null) {
                        skillsQty = HashMap()
                        this.skillsQty = skillsQty
                    }
                    skillsQty[k] = k.mQty
                    this.checkRegSkill(k)
                }
            }
        }
    }

    protected open fun switchCostume(oldRole: Costume?, newRole: Costume?) {
        if (oldRole !== null) {
            this.updateSkills(true, oldRole.aSkills)
            this.updateAttributes(true, oldRole)
            this.updateResistance(true, oldRole.res, oldRole.stRes)
            this.updateStates(true, oldRole.aStates)
        }
        if (newRole !== null) {
            this.updateStates(false, newRole.aStates)
            this.updateResistance(false, newRole.res, newRole.stRes)
            this.updateAttributes(false, newRole)
            this.updateSkills(false, newRole.aSkills)
        }
    }

    internal open fun updateAttributes(remove: Boolean, role: Costume) {
        val i: Int
        if (remove) {
            i = -1
            if (role.range) {
                this.ranged = null
            }
        } else {
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
        this.mInit += i * role.mInit
    }

    internal fun updateResistance(remove: Boolean, resMap: Map<Int, Int>?, stResMap: Map<State, Int>?) {
        if (resMap !== null) {
            var r = this.res
            if (r === null) {
                if (remove) {
                    return
                } else {
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
                } else {
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
        val mExp = this.mExp
        while (mExp <= this.exp && this._lv < this.maxLv) {
            this.mExp = mExp * 2
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
        var oldSprite: String? = null
        if (!consume) {
            if (this.shapeShift) {
                oldSprite = this.job.sprite
                this.sprite = oldSprite
            }
            val automatic = this.automatic
            if (automatic < 2 && automatic > -2) {
                this.automatic = 0
            } else {
                this.automatic = 2
            }
            if (this.hp > 0) {
                this.guards = true
            }
            this.reflect = false
            this.revives = false
            this.counter = null
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
                            c = true
                        }
                        s += r
                    }
                }
            }
        }
        if (oldSprite !== null && oldSprite == this.sprite) {
            this.shapeShift = false
        }
        s += checkStatus()
        if (c && consume) s += "."
        return s
    }

    fun checkStatus(): String {
        var s = ""
        if (this.hp < 1) {
            val revives = this.revives
            s += String.format(koTxt, this.name)
            if (this.shapeShift) {
                this.shapeShift = false
                this.sprite = this.job.sprite
            }
            this.sp = 0
            this.init = 0
            this.actions = 0
            val sDur = this.stateDur
            if (sDur !== null) {
                for (state in sDur.keys) {
                    state.remove(this, false, false)
                }
            }
            if (revives) {
                s += riseTxt
                this.hp = this.mHp
            } else {
                this.guards = false
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
            for (state in sDur.keys) { //TODO: prevent "ConcurrentModificationException"
                state.remove(this, false, false)
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
        this.guards = true
        val raceSkills = race.aSkills
        val jobSkills = job.aSkills
        val s: Int = (raceSkills?.size ?: 0)
        +(jobSkills?.size ?: 0)
        +(skills?.size ?: 0)
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
        this.updateStates(false, race.aStates)
        this.updateStates(false, job.aStates)
        this.updateStates(false, states)
        if (level > 1) {
            this.level = level
        }
    }

}