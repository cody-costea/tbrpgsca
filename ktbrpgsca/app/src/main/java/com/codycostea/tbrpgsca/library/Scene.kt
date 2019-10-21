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

open class Scene(party: Array<Actor>, enemy: Array<Actor>, private val surprise: Int) {

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

    open var status: Int = 0
        internal set

    open val enIdx: Int = party.size
    open val players: Array<Actor> = party + enemy

    open var current: Int = if (this.surprise < 0) this.enIdx else 0
        protected set
    open var crItems: MutableMap<Int, MutableList<Ability>?>? = null
        get() {
            if (field === null) {
                field = HashMap()
            }
            return field
        }
        protected set

    open var fTarget: Int = this.enIdx
        protected set
    open var lTarget: Int = this.enIdx
        protected set

    open var lastAbility: Ability? = null

    open val aiTurn: Boolean
        get() {
            return this.players[this.current].automatic != 0
        }

    private val useInit: Boolean

    init {
        var useInit = false
        val players = this.players
        val surprise = this.surprise
        val pSize = players.size
        val enIdx = this.enIdx
        for (i in 0 until pSize) {
            val iPlayer = players[i]
            var iInit = iPlayer.mInit
            if (!useInit && iInit != 0) {
                useInit = true
            }
            if (iInit < 1) {
                iInit = pSize
            }
            iPlayer.init = (if ((surprise < 0 && i < enIdx)
                    || (surprise > 0 && i >= enIdx)) 0 else iInit)
            for (j in 0 until pSize) {
                if (j == i) {
                    continue
                } else {
                    val jPlayer = players[j]
                    var jInit = jPlayer.mInit
                    if (jInit < 1) {
                        jInit = pSize
                    }
                    if (iInit < jInit /*|| (iInit == jInit && this.players[i].agi > this.players[j].agi)*/) {
                        jPlayer.init -= (jInit - iInit) //+ 1
                    }
                }
            }
        }
        val current = this.current
        val crActor = players[current]
        val crItems = crActor._items
        if (crItems !== null) {
            this.crItems!![current] = crItems.keys.toMutableList()
        }
        this.useInit = useInit
        this.setNextCurrent()
    }

    protected open fun setNextCurrent(): String {
        var ret = ""
        var initInc: Int
        val useInit = this.useInit
        val players = this.players
        var crActor = players[this.current]
        val oldActor = crActor
        var minInit = 1
        do {
            initInc = minInit
            for (i in players.indices) {
                val iPlayer = players[i]
                if (iPlayer.hp > 0) {
                    if (useInit) {
                        var nInit = iPlayer.init + initInc
                        iPlayer.init = nInit
                        var mInit = iPlayer.mInit
                        if (mInit < 1) {
                            mInit = players.size
                        }
                        if (nInit > mInit) {
                            nInit -= mInit
                            if (initInc == 1) minInit = -1
                            if (crActor !== iPlayer) {
                                val cInit = (crActor.init - (if (crActor.mInit < 1) players.size else crActor.mInit))
                                if (cInit < nInit || (cInit == nInit && iPlayer.agi > crActor.agi)) {
                                    val iActions = iPlayer.mActions
                                    iPlayer.actions = iActions
                                    iPlayer.applyStates(false)
                                    if (iActions > 0) {
                                        crActor = iPlayer
                                        this.current = i
                                    } else {
                                        iPlayer.init = 0
                                        if (ret.isNotEmpty()) {
                                            ret += "\n"
                                        }
                                        ret += iPlayer.applyStates(true)
                                    }
                                }
                            }
                        }
                        if (minInit > 0 && minInit > mInit) {
                            minInit = mInit
                        }
                    } else {
                        val iActions: Int
                        if (minInit != 1) {
                            iActions = iPlayer.mActions
                            iPlayer.actions = iActions
                        } else {
                            iActions = iPlayer.actions
                        }
                        if (crActor !== iPlayer && iActions > 0
                                && (crActor.actions < 1 || iPlayer.agi > crActor.agi)) {
                            iPlayer.applyStates(false)
                            if (iActions > 0) {
                                if (initInc > 0) initInc = 0
                                crActor = iPlayer
                                this.current = i
                            } else {
                                if (ret.isNotEmpty()) {
                                    ret += "\n"
                                }
                                ret += iPlayer.applyStates(true)
                            }
                        }
                    }
                }
            }
            if (minInit != 0 && !useInit) {
                minInit = 0
            }
        } while (initInc == 1 && minInit > -1)
        if (oldActor === crActor) {
            val cActions: Int
            if (useInit) {
                cActions = crActor.mActions
                crActor.actions = cActions
            } else {
                cActions = crActor.actions
            }
            crActor.applyStates(false)
            if (cActions < 1) {
                ret += crActor.applyStates(true)
                return ret + this.setNextCurrent()
            }
        } else {
            if (crActor.automatic == 0) {
                val crItems = crActor._items
                if (crItems !== null && oldActor._items !== crItems) {//TODO: analyze if ok
                    this.crItems!![this.current] = crItems.keys.toMutableList()
                }
            }
        }
        //TODO: analyze if recoverableSkills should be moved before oldCrr == this.current if;
        val recoverableSkills = crActor.skillsQtyRgTurn
        if (recoverableSkills !== null) {
            for (skill in recoverableSkills.keys) {
                val skillsQty = crActor.skillsQty
                if (skillsQty !== null && (skillsQty[skill] ?: skill.mQty) < skill.mQty) {
                    if (recoverableSkills[skill] == skill.rQty) {
                        skillsQty[skill] = (skillsQty[skill] ?: 0) + 1
                        recoverableSkills[skill] = 0
                    } else {
                        recoverableSkills[skill] = (recoverableSkills[skill] ?: 0) + 1
                    }
                }
            }
        }
        return ret
    }

    open fun endTurn(txt: String): String {
        var ret = txt
        var current = this.current
        val players = this.players
        var status = this.status
        val enIdx = this.enIdx
        if (status == 0) {
            var crActor = players[current]
            var cActions = crActor.actions
            do {
                if (--cActions < 1) {
                    crActor.init = 0
                    crActor.actions = 0
                    ret += crActor.applyStates(true)
                    ret += this.setNextCurrent()
                    current = this.current
                    crActor = players[current]
                    cActions = crActor.actions
                }
                var i = 0
                var noParty = true
                var noEnemy = true
                while (i < players.size) {
                    if (players[i].hp > 0) {
                        if (i < enIdx) {
                            noParty = false
                            i = enIdx - 1
                        } else {
                            noEnemy = false
                            break
                        }
                    }
                    i++
                }
                if (noParty) {
                    status = -2
                    ret += "\n$fallenTxt"
                } else {
                    if (noEnemy) {
                        status = 1
                        ret += "\n$victoryTxt"
                    }
                }
            } while (status == 0 && cActions < 1)
            crActor.actions = cActions
        }
        if (status != 0) {
            this.status = status
        }
        return ret
    }

    open fun getGuardian(target: Int, skill: Ability): Int {
        val players = this.players
        val current = this.current
        if (skill.range || (/*skill.range === null &&*/ players[current].range)) {
            return target
        }
        val f: Int
        val l: Int
        val enIdx = this.enIdx
        if (current < enIdx) {
            val pSize = players.size - 1
            if (target <= enIdx || target == pSize) {
                return target
            }
            f = enIdx
            l = pSize
        } else {
            if (target >= enIdx - 1 || target == 0) {
                return target
            }
            f = 0
            l = enIdx - 1
        }
        var difF = 0
        var difL = 0
        var guardF = target
        var guardL = target
        for (i in f until target) {
            val iPlayer = players[i]
            if (iPlayer.hp > 0 && iPlayer.guards) {
                if (guardF == target) {
                    guardF = i
                }
                difF++
            }
        }
        return if (difF == 0) {
            target
        } else {
            for (i in l downTo target + 1) {
                val iPlayer = players[i]
                if (iPlayer.hp > 0 && iPlayer.guards) {
                    if (guardL == target) {
                        guardL = i
                    }
                    difL++
                }
            }
            if (difL == 0) target else (if (difF < difL) guardF else guardL)
        }
    }

    protected open fun executeAbility(skill: Ability, defTarget: Int, txt: String): String {
        var ret: String = txt
        var target: Int = defTarget
        val current = this.current
        val players = this.players
        val enIdx = this.enIdx
        val fTarget: Int
        val lTarget: Int
        when (skill.trg) {
            1 -> {
                if (target < enIdx) {
                    fTarget = 0
                    lTarget = enIdx - 1
                } else {
                    fTarget = enIdx
                    lTarget = players.size - 1
                }
            }
            2 -> {
                fTarget = 0
                lTarget = players.size - 1
            }
            -2 -> {
                if (current < enIdx) {
                    fTarget = 0
                    lTarget = enIdx - 1
                } else {
                    fTarget = enIdx
                    lTarget = players.size - 1
                }
            }
            -1 -> {
                fTarget = current
                lTarget = current
            }
            else -> {
                target = this.getGuardian(target, skill)
                fTarget = target
                lTarget = target
            }
        }
        var applyCosts = true
        this.fTarget = fTarget
        this.lTarget = lTarget
        val crActor = players[current]
        ret += String.format("\n$performsTxt", crActor.name, skill.name)
        for (i in fTarget..lTarget) {
            val iPlayer = players[i]
            if ((skill.mHp < 0 && skill.restore) || iPlayer.hp > 0) {
                ret += skill.execute(crActor, iPlayer, applyCosts)
                applyCosts = false
            }
        }
        ret += "."
        crActor.exp++
        crActor.levelUp()
        this.lastAbility = skill
        return ret
    }

    open fun executeAI(ret: String): String {
        val players = this.players
        val current = this.current
        val enIdx = this.enIdx
        var skillIndex = 0
        var nHeal = false
        var nRestore = false
        var f: Int
        var l: Int
        if (current < enIdx) {
            f = 0
            l = enIdx
        } else {
            f = enIdx
            l = players.size
        }
        for (i in f until l) {
            val iPlayer = players[i]
            if (iPlayer.hp < 1) {
                nRestore = true
            } else if (iPlayer.hp < (iPlayer.mHp / 3)) {
                nHeal = true
            }
        }
        val crActor = players[current]
        val availableSkills = crActor.availableSkills
        if (nRestore || nHeal) {
            for (i in 0 until availableSkills.size) {
                val s = availableSkills[i]
                if ((s.restore || (nHeal && s.mHp < 0)) && s.canPerform(crActor)) {
                    skillIndex = i
                    break
                }
            }
        }
        val ability = availableSkills[this.getAIskill(skillIndex, nRestore)]
        val atkSkill = ability.mHp > -1
        if (atkSkill) {
            if (current < enIdx) {
                f = enIdx
                l = players.size
            } else {
                f = 0
                l = enIdx
            }
        }
        var target = f
        var trgPlayer = players[target]
        while (trgPlayer.hp < 1 && (atkSkill || !ability.restore) && target < l) trgPlayer = players[++target]
        for (i in target + 1 until l) {
            val iPlayer = players[i]
            if (iPlayer.hp < trgPlayer.hp && (iPlayer.hp > 0 || ability.restore)) {
                trgPlayer = iPlayer
                target = i
            }
        }
        return this.executeAbility(ability, target, ret)
    }

    open fun getAIskill(defSkill: Int, nRestore: Boolean): Int {
        var ret = defSkill
        val current = this.current
        val players = this.players
        val crActor = players[current]
        val availableSkills = crActor.availableSkills
        var s = crActor.availableSkills[defSkill]
        for (i in defSkill + 1 until availableSkills.size) {
            val a = availableSkills[i]
            if (a.canPerform(crActor)) {
                if (defSkill > 0) {
                    if (a.mHp < s.mHp && (a.restore || !nRestore)) {
                        s = a
                        ret = i
                    }
                } else if (a.mHp > s.mHp) {
                    s = a
                    ret = i
                }
            }
        }
        return ret
    }

    open fun performSkill(index: Int, target: Int, txt: String): String {
        return this.executeAbility(this.players[this.current].availableSkills[index], target, txt)
    }

    open fun useItem(index: Int, target: Int, ret: String): String {
        val current = this.current
        val crItems = this.crItems!![current]
        if (crItems !== null) {
            val item = crItems[index]
            val crItemsMap = this.players[current]._items
            if (crItemsMap !== null) {
                val itemQty = (crItemsMap[item] ?: 1) - 1
                if (itemQty > 0) {
                    crItemsMap[item] = itemQty
                } else {
                    crItems.remove(item)
                    crItemsMap.remove(item)
                }
            }
            return this.executeAbility(item, target, ret)
        } else {
            return ret
        }
    }

    open fun escape(): String {
        val enIdx = this.enIdx
        val players = this.players
        val pAgiSum = players.filterIndexed { i, _ -> i < enIdx }.sumBy { it.agi } / enIdx
        val eAgiSum = players.filterIndexed { i, _ -> i >= enIdx }.sumBy { it.agi } / (players.size - this.enIdx)
        return if (this.surprise > 0 || (Math.random() * 7) + pAgiSum > eAgiSum) {
            this.status = -1
            Scene.escapeTxt
        } else Scene.failTxt
    }

}