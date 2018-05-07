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

open class Scene(party : Array<Actor>, enemy : Array<Actor>, val surprise : Int) {

    open var status : Int = 0
        protected set

    val enIdx : Int = party.size

    open val Players : Array<Actor> = party + enemy

    open var current : Int = if (this.surprise < 0) this.enIdx else 0
        protected set
    open var crItems : MutableMap<Int, Array<Ability>?>? = null
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

    val aiTurn : Boolean
        get() {
            return this.Players[this.current].automatic != 0
        }

    protected open fun setCurrentActive(activate : Boolean) : Boolean {
        val oldCr = this.current
        for (i in 0 until this.Players.size) {
            if (activate && this.Players[i].hp > 0) {
                this.Players[i].actions = this.Players[i].mActions
            }

            if (this.current != i && (this.Players[i].actions > 0 && (this.Players[this.current].actions < 1
                            || this.Players[i].agi > this.Players[this.current].agi))) {
                this.current = i
            }
        }
        if (oldCr != this.current) {
            this.Players[this.current].applyStates(false)
        }
        if (this.Players[this.current].actions > 0) {
            if (this.Players[this.current].automatic == 0) {
                val crItems = this.Players[this.current].items
                if (crItems !== null) {
                    this.crItems!![this.current] = crItems.keys.toTypedArray()
                }
            }
            val recoverableSkills = this.Players[this.current].skillsQtyRgTurn
            if (recoverableSkills !== null) {
                for (skill in recoverableSkills.keys) {
                    val skillsQty = this.Players[this.current].skillsQty
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
                this.Players[this.current].actions--
                if (this.Players[this.current].actions < 1) {
                    ret += this.Players[this.current].applyStates(true)
                    if (!this.setCurrentActive(false)) {
                        this.setCurrentActive(true)
                    }
                    //ret += this.Players[this.current].applyStates(true)
                }

                var i = 0
                var noParty = true
                var noEnemy = true
                while (i < this.Players.size) {
                    if (this.Players[i].hp > 0) {
                        if (i < this.enIdx) {
                            noParty = false
                            i = this.enIdx - 1
                        } else {
                            noEnemy = false
                            break
                        }
                    }
                    i++
                }

                if (noEnemy) {
                    this.status = 1
                    ret += "The party has won!"
                }

                if (noParty) {
                    this.status = -2
                    ret += "The party has fallen!"
                }
            } while (this.status == 0 && this.Players[this.current].actions < 1)
        }

        return ret
    }

    protected open fun getGuardian(target : Int, skill : Ability) : Int {
        if (skill.range) {
            return target
        }
        val f : Int
        val l : Int
        if (this.current < this.enIdx) {
            if (target <= this.enIdx || target == this.Players.size - 1) {
                return target
            }
            f = this.enIdx
            l = this.Players.size - 1
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
            if (this.Players[i].hp > 0 && this.Players[i].guards) {
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
                if (this.Players[i].hp > 0 && this.Players[i].guards) {
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
                     this.lTarget = this.Players.size - 1
                 }
             }
            2 -> {
                this.fTarget = 0
                this.lTarget = this.Players.size - 1
            }
            -2 -> {
                if (this.current < this.enIdx) {
                    this.fTarget = 0
                    this.lTarget = this.enIdx - 1
                }
                else  {
                    this.fTarget = this.enIdx
                    this.lTarget = this.Players.size - 1
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

        for (i in this.fTarget..this.lTarget) {
            if (skill.hpDmg < 0 || this.Players[i].hp > 0) {
                ret += skill.execute(this.Players[this.current], this.Players[i], (i == this.fTarget))
            }
        }

        this.Players[this.current].exp++
        this.Players[this.current].levelUp()

        return ret
    }

    protected open fun executeAI(ret : String) : String {
        return this.endTurn(this.setAItarget(this.Players[this.current].availableSkills[this.getAIskill(this.checkAIheal(-1))], ret))
    }

    protected open fun checkAIheal(def : Int) : Int {
        var ret = def
        var nHeal = false
        val f : Int
        val l : Int
        if (this.current < this.enIdx) {
            f = 0
            l = this.enIdx
        }
        else {
            f = this.enIdx
            l = this.Players.size
        }
        for (i in f until l) {
            if (this.Players[i].hp < (this.Players[i].mHp / 3)) {
                nHeal = true
                break
            }
        }
        if (nHeal) {
            for (i in 0 until this.Players[this.current].availableSkills.size) {
                val s = this.Players[this.current].availableSkills[i]
                if (s.hpDmg < 0 && s.canPerform(this.Players[this.current])) {
                    ret = i
                    break
                }
            }
        }
        else {
            ret = 0
        }
        return ret
    }

    protected open fun getAIskill(defSkill : Int) : Int {
        val healSkill = if (defSkill < 0) 0 else defSkill
        var ret = healSkill
        var s = this.Players[this.current].availableSkills[healSkill]
        for (i in healSkill + 1 until this.Players[this.current].availableSkills.size) {
            val a = this.Players[this.current].availableSkills[i]
            if (a.mpC <= this.Players[this.current].mp
            && a.hpC < this.Players[this.current].hp
            && a.spC <= this.Players[this.current].sp
            && this.Players[this.current].level >= a.lvRq) {
                if (healSkill > 0) {
                    if (a.hpDmg < s.hpDmg && a.hpDmg < 0) {
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

    protected open fun setAItarget(ability : Ability, ret : String) : String {
        val f : Int
        val l : Int
        if ((this.current < this.enIdx && ability.hpDmg >= 0)
        || (this.current >= this.enIdx && ability.hpDmg < 0)) {
            f = this.enIdx
            l = this.Players.size
        }
        else {
            f = 0
            l = this.enIdx
        }
        var target = f

        while ((this.Players[target].hp < 1) && (ability.hpDmg > 1) && target < l) target++

        for (i in target until l) {
            if (this.Players[i].hp < this.Players[target].hp
            && ((this.Players[i].hp > 0 && ability.hpDmg >= 0) || ability.hpDmg < 0)) {
                target = i
            }
        }
        return this.executeAbility(ability, target, ret)
    }

    open fun performSkill(index : Int, target : Int, txt : String) : String {
        return this.endTurn(this.executeAbility(this.Players[this.current].availableSkills[index], target, txt))
    }

    open fun useItem(index : Int, target : Int, ret : String) : String {
        val crItems = this.crItems!![this.current]
        if (crItems !== null) {
            val item = crItems[index]
            return this.endTurn(this.executeAbility(item, target, ret))
        }
        else {
            return ret
        }
    }

}