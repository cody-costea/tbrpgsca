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

open class Scene(party : Array<Actor>, enemy : Array<Actor>, private val surprise : Int) {

    companion object {
        var performsTxt = "%s performs %s"
        var victoryTxt = "The party has won!"
        var fallenTxt = "The party has fallen!"
        var escapeTxt = "The party has escaped!"
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

    init {
        if (this.surprise != 0) {
            val l: Int
            val f: Int
            if (this.surprise < 0) {
                f = 0
                l = this.enIdx - 1
            }
            else {
                f = this.enIdx
                l = this.players.size - 1
            }
            for (i in f..l) {
                this.players[i].actions = 0
            }
        }
        this.setNextCurrent(false)
    }

    protected open fun setNextCurrent(activate : Boolean) : Boolean {
        val oldCr = this.current
        for (i in 0 until this.players.size) {
            if (activate && this.players[i].hp > 0) {
                this.players[i].actions = this.players[i].mActions
            }

            if (this.current != i && (this.players[i].actions > 0 && (this.players[this.current].actions < 1
                            || this.players[i].agi > this.players[this.current].agi))) {
                this.current = i
            }
        }
        if (activate || oldCr != this.current) {
            this.players[this.current].applyStates(false)
        }
        if (this.players[this.current].actions > 0) {
            if (this.players[this.current].automatic == 0) {
                val crItems = this.players[this.current]._items
                if (crItems !== null) {
                    this.crItems!![this.current] = crItems.keys.toMutableList()
                }
            }
            val recoverableSkills = this.players[this.current].skillsQtyRgTurn
            if (recoverableSkills !== null) {
                for (skill in recoverableSkills.keys) {
                    val skillsQty = this.players[this.current].skillsQty
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
            return true
        }
        else {
            return false
        }
    }

    open fun endTurn(txt : String) : String {
        var ret = txt
        if (this.status == 0) {
            do {
                this.players[this.current].actions--
                if (this.players[this.current].actions < 1) {
                    ret += this.players[this.current].applyStates(true)
                    if (!this.setNextCurrent(false)) {
                        this.setNextCurrent(true)
                    }
                    //ret += this.players[this.current].applyStates(true)
                }

                var i = 0
                var noParty = true
                var noEnemy = true
                while (i < this.players.size) {
                    if (this.players[i].hp > 0) {
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
                    ret += "\n${Scene.fallenTxt}"
                }
                else {
                    if (noEnemy) {
                        this.status = 1
                        ret += "\n${Scene.victoryTxt}"
                    }
                }
            } while (this.status == 0 && this.players[this.current].actions < 1)
        }

        return ret
    }

    open fun getGuardian(target : Int, skill : Ability) : Int {
        if (skill.range == true || (skill.range === null && this.players[this.current].range)) {
            return target
        }
        val f : Int
        val l : Int
        if (this.current < this.enIdx) {
            if (target <= this.enIdx || target == this.players.size - 1) {
                return target
            }
            f = this.enIdx
            l = this.players.size - 1
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
            if (this.players[i].hp > 0 && this.players[i].guards) {
                if (guardF == target) {
                    guardF = i
                }
                difF++
            }
        }
        if (difF == 0) {
            return target
        }
        else {
            for (i in l downTo target + 1) {
                if (this.players[i].hp > 0 && this.players[i].guards) {
                    if (guardL == target) {
                        guardL = i
                    }
                    difL++
                }
            }
            return if (difL == 0) target else (if (difF < difL) guardF else guardL)
        }
    }

    protected open fun executeAbility(skill : Ability, defTarget : Int, txt : String) : String {
        var ret : String = txt
        var target : Int = defTarget
        when (skill.trg) {
             1 -> {
                 if (target < this.enIdx) {
                     this.fTarget = 0
                     this.lTarget = this.enIdx - 1
                 }
                 else {
                     this.fTarget = this.enIdx
                     this.lTarget = this.players.size - 1
                 }
             }
            2 -> {
                this.fTarget = 0
                this.lTarget = this.players.size - 1
            }
            -2 -> {
                if (this.current < this.enIdx) {
                    this.fTarget = 0
                    this.lTarget = this.enIdx - 1
                }
                else  {
                    this.fTarget = this.enIdx
                    this.lTarget = this.players.size - 1
                }
            }
            -1 -> {
                this.fTarget = this.current
                this.lTarget = this.current
            }
            else -> {
                target = this.getGuardian(target, skill)
                this.fTarget = target
                this.lTarget = target
            }

        }
        var applyCosts = true
        ret += String.format("\n${Scene.performsTxt}", this.players[this.current].name, skill.name)
        for (i in this.fTarget..this.lTarget) {
            if ((skill.hpDmg < 0 && skill.restoreKO) || this.players[i].hp > 0) {
                ret += skill.execute(this.players[this.current], this.players[i], applyCosts)
                applyCosts = false
            }
        }

        this.players[this.current].exp++
        this.players[this.current].levelUp()

        this.lastAbility = skill

        return ret
    }

    open fun executeAI(ret : String) : String {
        var skillIndex = 0
        var nHeal = false
        var nRestore = false
        var f : Int
        var l : Int
        if (this.current < this.enIdx) {
            f = 0
            l = this.enIdx
        }
        else {
            f = this.enIdx
            l = this.players.size
        }
        for (i in f until l) {
            if (this.players[i].hp < 1) {
                nRestore = true
            }
            else if (this.players[i].hp < (this.players[i].mHp / 3)) {
                nHeal = true
            }
        }
        if (nRestore || nHeal) {
            for (i in 0 until this.players[this.current].availableSkills.size) {
                val s = this.players[this.current].availableSkills[i]
                if ((s.restoreKO || (nHeal && s.hpDmg < 0)) && s.canPerform(this.players[this.current])) {
                    skillIndex = i
                    break
                }
            }
        }
        val ability = this.players[this.current].availableSkills[this.getAIskill(skillIndex, nRestore)]
        //if ((this.current < this.enIdx && ability.hpDmg >= 0)
                //|| (this.current >= this.enIdx && ability.hpDmg < 0)) {
        if (ability.hpDmg > -1) {
            if (this.current < this.enIdx) {
                f = this.enIdx
                l = this.players.size
            }
            else {
                f = 0
                l = this.enIdx
            }
        }
        var target = f
        while (this.players[target].hp < 1 && (ability.hpDmg > 1 || !ability.restoreKO) && target < l) target++
        for (i in target until l) {
            if (this.players[i].hp < this.players[target].hp && (this.players[i].hp > 0 || ability.restoreKO)
            /*&& ((this.players[i].hp > 0 && ability.hpDmg >= 0) || ability.hpDmg < 0)*/) {
                target = i
            }
        }
        return this.executeAbility(ability, target, ret)
    }

    open fun getAIskill(defSkill : Int, nRestore : Boolean) : Int {
        var ret = defSkill
        var s = this.players[this.current].availableSkills[defSkill]
        for (i in defSkill + 1 until this.players[this.current].availableSkills.size) {
            val a = this.players[this.current].availableSkills[i]
            if (a.canPerform(this.players[this.current])) {
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
        val pAgiSum = this.players.filterIndexed { i, _ ->  i < this.enIdx}.sumBy { it.agi } / this.enIdx
        val eAgiSum = this.players.filterIndexed { i, _ ->  i >= this.enIdx}.sumBy { it.agi } / (this.players.size - this.enIdx)
        return if (Math.random() * 10 + pAgiSum > Math.random() * 10 + eAgiSum) {
            this.status = -1
            Scene.escapeTxt
        }
        else {
            Scene.failTxt
        }
    }
}