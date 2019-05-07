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

open class Scene(party : Array<Actor>, enemy : Array<Actor>, private val surprise : Int) {

    companion object {
        @JvmStatic
        var performsTxt = "%s performs %s"
        @JvmStatic
        var victoryTxt = "The party has won!"
        @JvmStatic
        var fallenTxt = "The party has fallen!"
        @JvmStatic
        var escapeTxt = "The party has escaped!"
        @JvmStatic
        var failTxt = "The party attempted to escape, but failed."
    }

    open var status : Int = 0
        internal set

    open val enIdx : Int = party.size
    open val players : Array<Actor> = party + enemy

    open var current : Int = if (this.surprise < 0) this.enIdx else 0
        protected set
    open var crItems : MutableMap<Int, MutableList<Ability>?>? = null
        get() {
            if (field === null) {
                field = HashMap()
            }
            return field
        }
        protected set

    open var fTarget : Int = this.enIdx
        protected set
    open var lTarget : Int = this.enIdx
        protected set

    open var lastAbility : Ability? = null

    open val aiTurn : Boolean
        get() {
            return this.players[this.current].automatic != 0
        }

    private val useInit : Boolean

    init {
        var useInit = false
        val players = this.players
        for (i in 0 until players.size) {
            if (!useInit && players[i].mInit != 0) {
                useInit = true
            }
            val iInit = if (players[i].mInit > 0) players[i].mInit else players.size
            players[i].init = (if ((this.surprise < 0 && i < this.enIdx)
                    || (this.surprise > 0 && i >= this.enIdx)) 0 else iInit)
            for (j in 0 until players.size) {
                if (j == i) {
                    continue
                }
                else {
                    val jInit = if (players[j].mInit > 0) players[j].mInit else players.size
                    if (iInit < jInit /*|| (iInit == jInit && this.players[i].agi > this.players[j].agi)*/) {
                        players[j].init -= (jInit - iInit) //+ 1
                    }
                }
            }
        }
        this.useInit = useInit
        this.setNextCurrent()
    }

    protected open fun setNextCurrent() : String {
        var ret = ""
        var initInc: Int
        var current = this.current
        val oldCr = current
        val players = this.players
        var minInit = 1
        do {
            initInc = minInit
            for (i in 0 until players.size) {
                if (players[i].hp > 0) {
                    if (this.useInit) {
                        players[i].init += initInc
                        var nInit = players[i].init
                        val mInit = if (players[i].mInit < 1) players.size else players[i].mInit
                        if (nInit > mInit) {
                            nInit -= mInit
                            if (initInc == 1) minInit = -1
                            if (current != i) {
                                val cInit = (players[current].init
                                        - (if (players[current].mInit < 1) players.size else players[current].mInit))
                                if (cInit < nInit || (cInit == nInit && players[i].agi > players[current].agi)) {
                                    players[i].actions = players[i].mActions
                                    players[i].applyStates(false)
                                    if (players[i].actions > 0) {
                                        this.current = i
                                        current = i
                                    }
                                    else {
                                        players[i].init = 0
                                        if (ret.isNotEmpty()) {
                                            ret += "\n"
                                        }
                                        ret += players[i].applyStates(true)
                                    }
                                }
                            }
                        }
                        if (minInit > 0 && minInit > mInit) {
                            minInit = mInit
                        }
                    }
                    else {
                        if (minInit != 1) {
                            players[i].actions = players[i].mActions
                        }
                        if (current != i && players[i].actions > 0
                                && (players[current].actions < 1
                                        || players[i].agi > players[current].agi)) {
                            players[i].applyStates(false)
                            if (players[i].actions > 0) {
                                if (initInc > 0) initInc = 0
                                this.current = i
                                current = i
                            }
                            else {
                                if (ret.isNotEmpty()) {
                                    ret += "\n"
                                }
                                ret += players[i].applyStates(true)
                            }
                        }
                    }
                }
            }
            if (minInit != 0 && !this.useInit) {
                minInit = 0
            }
        } while (initInc == 1 && minInit > -1)
        if (oldCr == current) {
            if (this.useInit) {
                players[oldCr].actions = players[oldCr].mActions
            }
            players[oldCr].applyStates(false)
            if (players[current].actions < 1) {
                ret += players[current].applyStates(true)
                return ret + this.setNextCurrent()
            }
        }
        else {
            if (players[current].automatic == 0) {
                val crItems = players[current]._items
                if (crItems !== null) {
                    this.crItems!![current] = crItems.keys.toMutableList()
                }
            }
        }
        //TODO: analyze if recoverableSkills should be moved before oldCrr == this.current if;
        val recoverableSkills = players[current].skillsQtyRgTurn
        if (recoverableSkills !== null) {
            for (skill in recoverableSkills.keys) {
                val skillsQty = players[current].skillsQty
                if (skillsQty !== null && (skillsQty[skill] ?: skill.mQty) < skill.mQty) {
                    if (recoverableSkills[skill] == skill.rQty) {
                        skillsQty[skill] = (skillsQty[skill] ?: 0) + 1
                        recoverableSkills[skill] = 0
                    }
                    else {
                        recoverableSkills[skill] = (recoverableSkills[skill] ?: 0) + 1
                    }
                }
            }
        }
        return ret
    }

    open fun endTurn(txt : String) : String {
        var ret = txt
        var current = this.current
        val players = this.players
        if (this.status == 0) {
            do {
                players[current].actions--
                if (players[current].actions < 1) {
                    players[current].init = 0
                    ret += players[current].applyStates(true)
                    ret += this.setNextCurrent()
                    current = this.current
                }
                var i = 0
                var noParty = true
                var noEnemy = true
                while (i < players.size) {
                    if (players[i].hp > 0) {
                        if (i < this.enIdx) {
                            noParty = false
                            i = this.enIdx - 1
                        }
                        else {
                            noEnemy = false
                            break
                        }
                    }
                    i++
                }
                if (noParty) {
                    this.status = -2
                    ret += "\n$fallenTxt"
                }
                else {
                    if (noEnemy) {
                        this.status = 1
                        ret += "\n$victoryTxt"
                    }
                }
            } while (this.status == 0 && players[current].actions < 1)
        }
        return ret
    }

    open fun getGuardian(target : Int, skill : Ability) : Int {
        val players = this.players
        if (skill.range == true || (skill.range === null && players[this.current].range)) {
            return target
        }
        val f : Int
        val l : Int
        if (this.current < this.enIdx) {
            if (target <= this.enIdx || target == players.size - 1) {
                return target
            }
            f = this.enIdx
            l = players.size - 1
        }
        else {
            if (target >= this.enIdx - 1 || target == 0) {
                return target
            }
            f = 0
            l = this.enIdx - 1
        }
        var difF = 0
        var difL = 0
        var guardF = target
        var guardL = target
        for (i in f until target) {
            if (players[i].hp > 0 && players[i].guards) {
                if (guardF == target) {
                    guardF = i
                }
                difF++
            }
        }
        return if (difF == 0) {
            target
        }
        else {
            for (i in l downTo target + 1) {
                if (players[i].hp > 0 && players[i].guards) {
                    if (guardL == target) {
                        guardL = i
                    }
                    difL++
                }
            }
            if (difL == 0) target else (if (difF < difL) guardF else guardL)
        }
    }

    protected open fun executeAbility(skill : Ability, defTarget : Int, txt : String) : String {
        var ret : String = txt
        var target : Int = defTarget
        val current = this.current
        val players = this.players
        when (skill.trg) {
             1 -> {
                 if (target < this.enIdx) {
                     this.fTarget = 0
                     this.lTarget = this.enIdx - 1
                 }
                 else {
                     this.fTarget = this.enIdx
                     this.lTarget = players.size - 1
                 }
             }
            2 -> {
                this.fTarget = 0
                this.lTarget = players.size - 1
            }
            -2 -> {
                if (current < this.enIdx) {
                    this.fTarget = 0
                    this.lTarget = this.enIdx - 1
                }
                else  {
                    this.fTarget = this.enIdx
                    this.lTarget = players.size - 1
                }
            }
            -1 -> {
                this.fTarget = current
                this.lTarget = current
            }
            else -> {
                target = this.getGuardian(target, skill)
                this.fTarget = target
                this.lTarget = target
            }
        }
        var applyCosts = true
        ret += String.format("\n$performsTxt", players[current].name, skill.name)
        for (i in this.fTarget..this.lTarget) {
            if ((skill.hpDmg < 0 && skill.restoreKO) || players[i].hp > 0) {
                ret += skill.execute(players[current], players[i], applyCosts)
                applyCosts = false
            }
        }
        ret += "."
        players[current].exp++
        players[current].levelUp()
        this.lastAbility = skill
        return ret
    }

    open fun executeAI(ret : String) : String {
        val players = this.players
        val current = this.current
        var skillIndex = 0
        var nHeal = false
        var nRestore = false
        var f : Int
        var l : Int
        if (current < this.enIdx) {
            f = 0
            l = this.enIdx
        }
        else {
            f = this.enIdx
            l = players.size
        }
        for (i in f until l) {
            if (players[i].hp < 1) {
                nRestore = true
            }
            else if (players[i].hp < (players[i].mHp / 3)) {
                nHeal = true
            }
        }
        if (nRestore || nHeal) {
            for (i in 0 until players[current].availableSkills.size) {
                val s = players[current].availableSkills[i]
                if ((s.restoreKO || (nHeal && s.hpDmg < 0)) && s.canPerform(players[current])) {
                    skillIndex = i
                    break
                }
            }
        }
        val ability = players[current].availableSkills[this.getAIskill(skillIndex, nRestore)]
        val atkSkill = ability.hpDmg > -1
        if (atkSkill) {
            if (current < this.enIdx) {
                f = this.enIdx
                l = players.size
            }
            else {
                f = 0
                l = this.enIdx
            }
        }
        var target = f
        while (players[target].hp < 1 && (atkSkill || !ability.restoreKO) && target < l) target++
        for (i in target until l) {
            if (players[i].hp < players[target].hp && (players[i].hp > 0 || ability.restoreKO)) {
                target = i
            }
        }
        return this.executeAbility(ability, target, ret)
    }

    open fun getAIskill(defSkill : Int, nRestore : Boolean) : Int {
        var ret = defSkill
        val current = this.current
        val players = this.players
        var s = players[current].availableSkills[defSkill]
        for (i in defSkill + 1 until players[current].availableSkills.size) {
            val a = players[current].availableSkills[i]
            if (a.canPerform(players[current])) {
                if (defSkill > 0) {
                    if (a.hpDmg < s.hpDmg && (a.restoreKO || !nRestore)) {
                        s = a
                        ret = i
                    }
                }
                else if (a.hpDmg > s.hpDmg) {
                    s = a
                    ret = i
                }
            }
        }
        return ret
    }

    open fun performSkill(index : Int, target : Int, txt : String) : String {
        return this.executeAbility(this.players[this.current].availableSkills[index], target, txt)
    }

    open fun useItem(index : Int, target : Int, ret : String) : String {
        val crItems = this.crItems!![this.current]
        if (crItems !== null) {
            val item = crItems[index]
            val crItemsMap = this.players[this.current]._items
            if (crItemsMap !== null) {
                val itemQty = (crItemsMap[item] ?: 1) - 1
                if (itemQty > 0) {
                    crItemsMap[item] = itemQty
                }
                else {
                    crItems.remove(item)
                    crItemsMap.remove(item)
                }
            }
            return this.executeAbility(item, target, ret)
        }
        else {
            return ret
        }
    }

    open fun escape() : String {
        val players = this.players
        val pAgiSum = players.filterIndexed { i, _ ->  i < this.enIdx}.sumBy { it.agi } / this.enIdx
        val eAgiSum = players.filterIndexed { i, _ ->  i >= this.enIdx}.sumBy { it.agi } / (players.size - this.enIdx)
        return if (this.surprise > 0 || (Math.random() * 7) + pAgiSum > eAgiSum) {
            this.status = -1
            Scene.escapeTxt
        }
        else Scene.failTxt
    }

}