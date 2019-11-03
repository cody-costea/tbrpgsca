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

typealias SceneFun = (Scene, String?) -> Boolean

interface Scene {

    companion object {
        var surprisedTxt = "Surprised"
        var performsTxt = "%s performs %s"
        var countersTxt = " and counters"
        var victoryTxt = "The party has won!"
        var fallenTxt = "The party has fallen!"
        var escapeTxt = "The party has escaped!"
        var failTxt = "The party attempted to escape, but failed."
    }

    var enIdx: Int
    var status: Int
    var current: Int
    var ordIndex: Int
    var lastAbility: Ability?
    var players: Array<Actor>
    var crItems: MutableMap<Int, MutableList<Ability>?>?
    var ordered: Array<Actor>?
    var onStop: SceneFun?
    var onStart: SceneFun?
    var onBeforeAct: SceneFun?
    var onAfterAct: SceneFun?
    var onNewTurn: SceneFun?
    var fTarget: Int
    var lTarget: Int
    var surprise: Int

    val aiTurn: Boolean
        get() {
            return this.players[this.current].automatic != 0
        }

    fun setNextCurrent(): String {
        var ret = ""
        var initInc: Int
        val useInit: Boolean
        val players = this.players
        val ordered = this.ordered
        var ordIndex = this.ordIndex
        var crActor: Actor = players[this.current]
        val oldActor = crActor
        if (ordered === null) {
            useInit = ordIndex < -1
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
                                        iPlayer.actions = iPlayer.mActions
                                        iPlayer.applyStates(false)
                                        if (iPlayer.actions > 0) {
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
                                if (iPlayer.actions > 0) {
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
        } else {
            useInit = false
            val pSize = players.size
            this.ordIndex = -1
            do {
                if (++ordIndex == pSize) {
                    ordIndex = 0
                }
                crActor = ordered[ordIndex]
                if (crActor.hp > 0) { //TODO: analyze if ok
                    crActor.actions = crActor.mActions
                    if (crActor !== oldActor) {
                        crActor.applyStates(false)
                        if (crActor.actions < 1) {
                            if (ret.isNotEmpty()) {
                                ret += "\n"
                            }//TODO: actor sprites should be played when some states are applied
                            this.ordIndex = 0
                            ret += crActor.applyStates(true)
                            this.ordIndex = -1
                            continue
                        } else {
                            this.current = players.indexOf(crActor)
                        }
                    }
                    this.ordIndex = ordIndex
                    break
                }
            } while (true)
        }
        if (oldActor === crActor) {
            if (useInit) {
                crActor.actions = crActor.mActions
            }
            crActor.applyStates(false)
            if (crActor.actions < 1) {
                ret += crActor.applyStates(true)
                return ret + this.setNextCurrent() //TODO: analyze if ok
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
        val onNewTurn = this.onNewTurn
        return if (onNewTurn !== null && onNewTurn(this, ret) && crActor.automatic != 0) {
            this.executeAI("")
        } else {
            ret
        }
    }

    fun endTurn(txt: String): String {
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
        return if (status != 0) {
            this.status = status
            val onStop = this.onStop
            if (onStop === null || onStop(this, ret)) ret else this.setNextCurrent()
        } else {
            ret
        }
    }

    fun getGuardian(target: Int, skill: Ability): Int {
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

    fun executeAbility(skill: Ability, defTarget: Int, txt: String): String {
        var ret: String = txt
        val beforeAct = this.onBeforeAct
        if (beforeAct === null || beforeAct(this, ret)) {
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
            val healing = skill.mHp < 0
            for (i in fTarget..lTarget) {
                val iPlayer = players[i]
                if ((healing && skill.restore) || iPlayer.hp > 0) {
                    ret += skill.execute(crActor, iPlayer, applyCosts)
                    if (!healing) {
                        val counter = iPlayer.counter
                        if (counter !== null) {
                            val dmgType = counter.dmgType
                            if ((skill.dmgType and dmgType) == dmgType) {
                                ret += countersTxt + counter.execute(iPlayer, crActor, false)
                            }
                        }
                    }
                    applyCosts = false
                }
            }
            ret += "."
            this.lastAbility = skill
            val afterAct = this.onAfterAct
            if (afterAct === null || afterAct(this, ret)) {
                crActor.exp++
                crActor.levelUp()
            }
        }
        return ret
    }

    fun executeAI(ret: String): String {
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

    fun getAIskill(defSkill: Int, nRestore: Boolean): Int {
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

    fun performSkill(index: Int, target: Int, txt: String): String {
        return this.executeAbility(this.players[this.current].availableSkills[index], target, txt)
    }

    fun useItem(index: Int, target: Int, ret: String): String {
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

    fun escape(): String {
        val beforeAct = this.onBeforeAct
        if (beforeAct === null || beforeAct(this, null)) {
            val enIdx = this.enIdx
            val players = this.players
            val afterAct = this.onAfterAct
            val pAgiSum = players.filterIndexed { i, _ -> i < enIdx }.sumBy { it.agi } / enIdx
            val eAgiSum = players.filterIndexed { i, _ -> i >= enIdx }.sumBy { it.agi } / (players.size - this.enIdx)
            if (this.surprise > 0 || (Math.random() * 7) + pAgiSum > eAgiSum) {
                this.status = -1
                if (afterAct === null || afterAct(this, null)) {
                    val ret = Scene.escapeTxt
                    val onStop = this.onStop
                    if (onStop === null || onStop(this, ret)) {
                        return ret
                    }
                }
                this.status = 0
            } else if (afterAct !== null && !afterAct(this, null)) {
                this.status = -1
                val ret = Scene.escapeTxt
                val onStop = this.onStop
                if (onStop === null || onStop(this, ret)) {
                    return ret
                }
            }
        }
        return Scene.failTxt
    }

    fun prepare(party: Array<Actor>, enemy: Array<Actor>, surprise: Int, reorder: Boolean): String {
        val players = party + enemy
        this.surprise = surprise
        val pSize = players.size
        val enIdx = party.size
        this.players = players
        var useInit = false
        this.enIdx = enIdx
        val surprised = if (surprise == 0) null else State(0, surprisedTxt, null, true, false,
                false, false, false, null,2, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0,false, null, null, null)
        for (i in 0 until pSize) {
            val iPlayer = players[i]
            var iInit = iPlayer.mInit
            if (!useInit && iInit != 0) {
                useInit = true
            }
            if (iInit < 1) {
                iInit = pSize
            } //TODO: also set actions to 0 for surprised actors
            if ((surprise < 0 && i < enIdx) || (surprise > 0 && i >= enIdx)) {
                surprised!!.inflict(iPlayer, true, false)
            }
            iPlayer.init = iInit
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
        this.status = 0
        this.fTarget = enIdx
        this.lTarget = enIdx
        this.lastAbility = null
        val current = if (this.surprise < 0) this.enIdx else 0
        val crActor = players[current]
        val crItems = crActor._items
        this.current = current
        if (crItems !== null) {
            this.crItems!![current] = crItems.keys.toMutableList()
        }
        if (useInit) {
            this.ordIndex = -2
        } else if (reorder) {
            val ordered = players.copyOf()
            ordered.sortByDescending { it.agi }
            this.ordered = ordered
            this.ordIndex = pSize - 1
            Actor.onTurnReorder = {
                if (this.ordIndex > -1) {
                    ordered.sortByDescending { it.agi }
                    this.ordIndex = ordered.indexOf(players[this.current])
                }
            }
        }
        val ret = this.setNextCurrent()
        val onStart = this.onStart
        if (onStart === null || onStart(this, ret)) {
            val onNewTurn = this.onNewTurn
            if (onNewTurn !== null && onNewTurn(this, ret) && players[this.current].automatic != 0) {
                return this.executeAI(ret)
            }
        }
        return ret
    }

}