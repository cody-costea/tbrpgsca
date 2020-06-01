/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

import Foundation

public let MIN_ROUND = Int.min

public let FLAG_USE_GUARDIANS = 1

public var PreparesTxt = "%@ prepares %@"
public var PerformsTxt = "%@ performs %@"
public var VictoryTxt = "The party has won!"
public var FallenTxt = "The party has fallen!"
public var EscapeTxt = "The party has escaped!"
public var FailTxt = "The party attempted to escape, but failed."

public enum SceneEvent {
    case begin, beforeAct, afterAct, newTurn, end
}

public enum SceneStatus: Int {
    case defeat = -2, retreat = -1, ongoing = 0, victory = 1
}

public protocol Scene : class {
    
    typealias SceneRun = (Scene, inout String) -> Bool
    
    typealias SpriteRun = (Scene, Actor?, Ability?, Bool, Actor?, Ability?) -> Bool
    
    var status: SceneStatus {
        get
        set
    }
    
    var previous: Int { //oldCurrent
        get
        set
    }
    
    var current: Int {
        get
        set
    }
    
    var actions: Int {
        get
        set
    }
    
    var players: [Actor]? {
        get
        set
    }
    
    var targets: [Actor]? {
        get
        set
    }
    
    var parties: [[Actor]] {
        get
        set
    }
    
    var lastAbility: Ability? {
        get
        set
    }
    
    var crActor: Actor? {
        get
        set
    }
    
    var surprise: Int {
        get
        set
    }
    
    var fTarget: Int {
        get
        set
    }
    
    var lTartet: Int {
        get
        set
    }
    
    var flags: Int {
        get
        set
    }
    
    var mInit: Int {
        get
        set
    }
    
    var message: String? {
        get
        set
    }
    
    var useGuardians: Bool {
        get
        set
    }
    
    var spriteRun: SpriteRun? {
        get
        set
    }
    
    var events: [SceneEvent : SceneRun]? {
        get
        set
    }
    
    func playAi(ret: inout String, player: Actor)
    
    func endTurn(ret: inout String, actor: Actor)
    
    func getGuardian(user: Actor, target: Actor, skill: Ability) -> Actor
    
    func getAiSkill(user: Actor, skills: [Ability], defSkill: Int, restore: Bool) -> Int
    
    func execute(ret: inout String, user: Actor, target: Actor?, ability: Ability, applyCosts: Bool)
    
    func perform(ret: inout String, user: Actor, target: Actor, ability: Ability, item: Bool)
    
    func initialize(ret: inout String, parties: [[Actor]], useGuards: Bool, spriteRun: SpriteRun?,
                    events: [SceneEvent : [SceneRun]]?, surprise: Int, mInit: Int)
    
    func canTarget(user: Actor, ability: Ability, target: Actor) -> Bool
    
    func checkStatus(ret: inout String)
    
    func escape(ret: inout String)
    
    func resetTurn(actor: Actor)
    
    func agiCalc()
    
}

public extension Scene where Self: AnyObject {
    
    var useGuardians: Bool {
        get {
            return (self.flags & FLAG_USE_GUARDIANS) == FLAG_USE_GUARDIANS
        }
        set (val) {
            let flags = self.flags
            if val != ((flags & FLAG_USE_GUARDIANS) == FLAG_USE_GUARDIANS) {
                self.flags = flags ^ FLAG_USE_GUARDIANS
            }
        }
    }
    
    func playAi(ret: inout String, player: Actor) {
        let parties = self.parties, skills: [Ability]! = player.aSkills
        var side: Int, sSize: Int = 0, skillIndex = 0, heal = -1, pSize = parties.count
        var party: [Actor]!
        if player.confused {
            party = nil
            side = -1
        } else {
            side = player.side
            party = parties[side]
            sSize = party.count
            if !player.enraged {
                for i in 0..<sSize {
                    let iPlayer = party[i], iHp = iPlayer.hp
                    if iHp < 1 {
                        heal = 1
                    } else if iHp < iPlayer.hp / 3 {
                        heal = 0
                    }
                }
            }
            if heal > -1 {
                for (i, s) in skills.enumerated() {
                    if s.canPerform(user: player) && (s.hp < 0 && (heal == 0 || s.revives)) {
                        skillIndex = i
                        break
                    }
                }
            }
        }
        var target: Actor! = nil
        var (_, ability) = self.getAiSkill(user: player, skills: skills, defSkill: skillIndex, restore: heal == 1)
        if ability.hp > -1 {
            if party == nil || player.randomAi {
                var trgSide: Int
                if side > -1 && pSize == 2 {
                    trgSide = side == 0 ? 1 : 0
                } else {
                    trgSide = Int.random(in: 0..<pSize)
                    if side == trgSide {
                        trgSide += 1
                        if trgSide == pSize {
                            trgSide = 0
                        }
                    }
                }
                party = parties[trgSide]
                sSize = party.count
                var trg = Int.random(in: 0..<sSize)
                target = party[trg]
                while target.knockedOut {
                    trg += 1
                    if trg == sSize {
                        trg = 0
                    }
                    target = party[trg]
                }
            } else {
                for j in 0..<pSize {
                    if j == side {
                        continue
                    }
                    var trg = 0
                    let players = parties[j]
                    sSize = players.count
                    if target === nil {
                        repeat {
                            target = players[trg]
                            trg += 1
                        } while (trg < sSize) && (target.knockedOut || target.side == side)
                    }
                    for i in trg + 1..<sSize {
                        let iPlayer = players[i]
                        if iPlayer.side != side && (!iPlayer.knockedOut) && iPlayer.hp < target.hp {
                            target = iPlayer
                        }
                    }
                }
            }
        } else {
            if party == nil {
                party = parties[Int.random(in: 0..<pSize)]
                sSize = party.count
            }
            target = party[0]
            let restore = ability.revives
            for i in 1..<sSize {
                let iPlayer = party[i], iHp = iPlayer.hp
                if iHp < target.hp && (restore || iHp > 0) {
                    target = iPlayer
                }
            }
        }
        if target === nil {
            target = player
            if heal < 0 {
                (_, ability) = self.getAiSkill(user: player, skills: skills, defSkill: 1, restore: false)
            }
        }
        self.perform(ret: &ret, user: player, target: target, ability: ability, item: false)
    }
    
    func endTurn(ret: inout String) {
        if var crActor = self.crActor {
            var current = self.current
            var cActions = crActor.actions - 1
            crActor.actions = cActions
            var retOption: String? = ret
            while cActions < 1 {
                if crActor.hp > 0 && !(crActor.invincible && crActor.knockedOut && crActor.stunned) {
                    crActor.applyStates(ret: &retOption, consume: true)
                }
                var mInit = self.mInit
                if mInit > 0 {
                    var cInit = crActor.cInit - mInit
                    crActor.cInit = cInit
                    repeat {
                        /*if let ordered = self.players {
                            for (i, iPlayer) in ordered.enumerated() {
                                if iPlayer.hp > 0 {
                                    let iInit = iPlayer.cInit + iPlayer.agi
                                    if iInit > cInit {
                                        cInit = iInit
                                        crActor = iPlayer
                                        current = i
                                    }
                                }
                            }
                        } else {*/
                            let parties = self.parties
                            for party in parties {
                                for (i, iPlayer) in party.enumerated() {
                                    if iPlayer.hp > 0 {
                                        iPlayer.doDelayedAct()
                                        let iInit = iPlayer.cInit + iPlayer.agi
                                        if iInit > cInit {
                                            cInit = iInit
                                            crActor = iPlayer
                                            current = i
                                        }
                                    }
                                }
                            }
                        //}
                    } while cInit < mInit || crActor.delayTrn > -1
                } else {
                    current = self.previous
                    let players: [Actor]! = self.players, pSize = players.count
                    repeat {
                        if self.actions > 0 {
                            for i in 0..<pSize {
                                let iPlayer = players[i]
                                if iPlayer.hp > 0 {
                                    iPlayer.doDelayedAct()
                                }
                            }
                        }
                        var nInit = mInit - 1
                        if nInit == MIN_ROUND {
                            mInit = 0
                            self.mInit = 0
                            for i in 0..<pSize {
                                players[i].cInit = 0
                            }
                            nInit = -1
                        }
                        crActor.cInit = nInit
                        repeat {
                            current += 1
                            if current == pSize {
                                self.mInit = nInit
                                mInit = nInit
                            }
                            crActor = players[current]
                        } while crActor.hp < 1 || crActor.cInit < mInit
                    } while crActor.delayTrn > -1
                }
                if var regSkills = crActor._skillsRgTurn {
                    var skillsQty: [Ability : Int]! = crActor._skillsCrQty
                    if skillsQty == nil {
                        skillsQty = [Ability : Int]()
                        crActor._skillsCrQty = skillsQty
                    }
                    for (skill, rgTurn) in regSkills {
                        let skillMaxQty = skill.mQty, skillCrQty = skillsQty[skill] ?? skillMaxQty
                        if skillCrQty < skillMaxQty {
                            if rgTurn == skill.rQty {
                                skillsQty[skill] = skillCrQty + 1
                                regSkills[skill] = 0
                            } else {
                                regSkills[skill] = rgTurn + 1
                            }
                        }
                    }
                }
                let shapeShifted = crActor.shapeShifted
                crActor.applyStates(ret: &retOption, consume: false)
                if shapeShifted && (!crActor.shapeShifted), let actorEvent = self.spriteRun {
                    actorEvent(self, crActor, nil, true, nil, nil)
                }
                cActions = crActor.stunned ? 0 : crActor.mActions
                crActor.actions = cActions
                ret = retOption!
            }
            self.crActor = crActor
            self.current = current
            self.previous = current
            if let events = self.events, let event = events[SceneEvent.newTurn],
                event(self, &ret) && (crActor.aiControl || crActor.enraged || crActor.confused) {
                self.playAi(ret: &ret, player: crActor)
            }
            ret.append(".")
        }
    }
    
    func getGuardian(user: Actor, target: Actor, skill: Ability) -> Actor {
        let side = target._oldSide
        if user._oldSide == side {
            return target
        } else {
            var covered: Actor = target
            if self.useGuardians && ((!skill.ranged) && ((!user.ranged) || skill.melee)) {
                let party = self.parties[side], pSize = party.count
                var fGuard: Actor! = nil, lGuard: Actor! = nil
                var first = true
                var pos = -1
                for i in 0..<pSize {
                    let guardian = party[i]
                    if guardian === target {
                        if fGuard === nil || i == pSize - 1 {
                            break
                        } else {
                            pos = i
                            first = false
                            continue
                        }
                    } else if (fGuard === nil || pos != -1) && guardian.hp > 0 && !(guardian.stunned || guardian.confused) {
                        if first {
                            fGuard = guardian
                        } else {
                            lGuard = guardian
                        }
                    }
                }
                if fGuard !== nil && lGuard !== nil {
                    covered = pos < (pSize / 2) ? fGuard : lGuard
                }
            }
            if let covering = covered.coveredBy {
                return covering
            } else {
                return covered
            }
        }
    }
    
    func getAiSkill(user: Actor, skills: [Ability], defSkill: Int, restore: Bool) -> (Int, Ability) {
        var ret = defSkill, sSize = skills.count, s = skills[defSkill]
        for i in (defSkill + 1)..<sSize {
            let a = skills[i]
            if a.canPerform(user: user) && ((defSkill > 0 && (a.hp < s.hp)
                && (a.revives || !restore)) || (a.hp > s.hp)) {
                ret = i
                s = a
            }
        }
        return (ret, s)
    }
    
    func execute(ret: inout String, user: Actor, target: Actor, ability: Ability, applyCosts: Bool) {
        let healing = ability.hp < 0
        let ko = target.hp < 1
        if (healing && ability.revives) || !ko {
            var cntSkill: Ability! = nil
            ability.execute(ret: &ret, user: user, target: target, applyCosts: applyCosts)
            if !healing {
                if let counters = target.counters, (counters.count > 0 && (!target.stunned)
                    && (target.side != user.side || target.confused)) {
                    let usrDmgType = ability.dmgType
                    for counter in counters {
                        let cntDmgType = counter.dmgType
                        if (usrDmgType & cntDmgType) == cntDmgType && (cntSkill === nil || counter.hp > cntSkill.hp) {
                            cntSkill = counter
                        }
                    }
                    if cntSkill !== nil {
                        cntSkill.execute(ret: &ret, user: target, target: user, applyCosts: false)
                    }
                }
            }
            let actorEvent: SpriteRun! = self.spriteRun
            if actorEvent == nil || actorEvent(self, applyCosts ? user : nil, ability, ko && target.hp > 0,
                                               target, target === user ? ability : cntSkill) {
                var targets: [Actor]! = self.targets
                if targets == nil {
                    targets = [Actor]()
                    self.targets = targets
                }
                targets.append(target)
            }
        }
    }
    
    func perform(ret: inout String, user: Actor, target: Actor, ability: Ability, item: Bool) {
        self.message = ret
        let performance = { (endTurn: Bool) -> Void in
            if var ret = self.message {
                ret.append(String(format: PerformsTxt, user.name, ability.name))
                if var targets = self.targets {
                    targets.removeAll(keepingCapacity: true)
                }
                if let event = self.events?[SceneEvent.beforeAct], !event(self, &ret) {
                    return
                }
                if ability.ranged && ability.targetsAll {
                    let noSelfTarget = !ability.targetsSelf
                    let sideTarget = ability.targetsSide
                    let usrSide = user.side
                    var applyCosts = true
                    for (j, party) in self.parties.enumerated() {
                        if sideTarget && noSelfTarget && j == usrSide {
                            continue
                        }
                        for trg in party {
                            if noSelfTarget && trg === user {
                                continue
                            } else {
                                self.execute(ret: &ret, user: user, target: target, ability: ability, applyCosts: applyCosts)
                            }
                            applyCosts = false
                        }
                    }
                } else if ability.targetsSide {
                    let side = ability.targetsSelf ? user.side : target._oldSide
                    for (i, trg) in self.parties[side].enumerated() {
                        self.execute(ret: &ret, user: user, target: trg, ability: ability, applyCosts: i == 0)
                    }
                } else {
                    self.execute(ret: &ret, user: user, target: user === target || ability.targetsSelf ? user
                        : self.getGuardian(user: user, target: target, skill: ability), ability: ability, applyCosts: true)
                }
                if item, var items = user.items {
                    items[ability] = (items[ability] ?? 1) - 1
                }
                self.lastAbility = ability
                user.exp += 1
                if endTurn {
                    if let event = self.events?[SceneEvent.afterAct], event(self, &ret) {
                        self.endTurn(ret: &ret, actor: user)
                    }
                } else if self.mInit < 0 {
                    self.actions -= 1
                }
            }
        }
        let aDelayTrn = ability.mDelayTrn
        if aDelayTrn < 0 {
            performance(true)
        } else {
            let delayTrn = aDelayTrn - user.mDelayTrn
            if delayTrn > 0 {
                if self.mInit < 0 {
                    self.actions += 1
                }
                self.lastAbility = nil
                user.delayTrn = delayTrn
                user._delayAct = performance
                ret.append(String(format: PreparesTxt, user.name, ability.name))
                if let event = self.events?[SceneEvent.afterAct], event(self, &ret) {
                    self.endTurn(ret: &ret, actor: user)
                }
            } else {
                performance(true)
            }
        }
    }
    
    func checkStatus(ret: inout String) {
        if self.status == SceneStatus.ongoing {
            let parties = self.parties
            var party = parties[0]
            for actor in party {
                if !actor.knockedOut {
                    let pSize = parties.count
                    for i in 1..<pSize {
                        party = parties[i]
                        for player in party {
                            if !player.knockedOut {
                                return
                            }
                        }
                    }
                    ret.append(VictoryTxt)
                    self.status = SceneStatus.victory
                    return
                }
            }
            ret.append(FallenTxt)
            self.status = SceneStatus.defeat
        }
    }
    
    func escape(ret: inout String) {
        
    }
    
    func resetTurn(actor: Actor) {
        let mInit = self.mInit + 1
        if mInit < 2 {
            if actor.cInit > mInit {
                actor.cInit = mInit
            } else if mInit == 1 && actor.cInit < -1 {
                actor.cInit = 0
            }
            self.previous = -1
        }
    }
    
    func agiCalc() {
        if self.mInit < 1 {
            self.players?.sort(by: { $0.agi > $1.agi })
            self.previous = -1
        }
    }
    
    func canTarget(user: Actor, ability: Ability, target: Actor) -> Bool {
        return (ability.canPerform(user: user) && (ability.targetsSelf || ((user.drawnBy === nil
            || user.drawnBy === target) && (target.hp > 0 || ability.revives))))
    }
    
    func initialize(ret: inout String, parties: [[Actor]], useGuards: Bool, spriteRun: SpriteRun?,
                    events: [SceneEvent : SceneRun]?, surprise: Int, mInit: Int) {
        assert(parties.count > 1)
        self.useGuardians = useGuards
        var players: [Actor]?
        let useInit: Bool
        if mInit > 0 {
            self.mInit = mInit
            useInit = true
            players = nil
        } else {
            players = [Actor]()
            useInit = false
            self.mInit = 0
        }
        var current = 0
        var crActor: Actor! = nil
        self.events = events
        self.parties = parties
        self.players = players
        self.spriteRun = spriteRun
        self.targets = nil
        self.surprise = 0
        for (i, party) in parties.enumerated() {
            let aiPlayer = i > 0
            let surprised = surprise == i
            if crActor === nil {
                crActor = party[0]
            }
            for (j, player) in party.enumerated() {
                player.actions = 0
                if surprised {
                    player.cInit = useInit ? -(mInit + 1) : -1
                } else {
                    player.cInit = 0
                    if player.agi > crActor.agi || crActor.cInit < 0 {
                        crActor = player
                        if useInit {
                            current = j
                        }
                    }
                }
                if aiPlayer {
                    player.aiControl = true
                    //player.randomAi = true
                    player.addEvent(eventType: Actor.EventType.agi, actorRun: { (actor: Actor, val: Any) -> Bool in
                        if (val as! Int) != actor.agi {
                            self.agiCalc()
                        }
                        return true
                    })
                    player.addEvent(eventType: Actor.EventType.hp, actorRun: { (actor: Actor, val: Any) -> Bool in
                        let hp = val as! Int
                        var ret: String! = self.message
                        if ret != nil {
                            ret = ""
                        }
                        if hp < 1 {
                            if actor.hp != 0 {
                                if actor.survives || actor.invincible {
                                    actor._hp = 1
                                } else {
                                    actor.sp = 0
                                    ret.append(String.init(format: Actor.KoTxt, actor.name))
                                    if actor.cInit > 0 {
                                        actor.cInit = 0
                                    }
                                    actor.delayTrn = -1
                                    actor.drawnBy = nil
                                    actor.coveredBy = nil
                                    actor._delayAct = nil
                                    let revives = actor.revives
                                    if revives {
                                        if ret != nil {
                                            ret.append(Actor.RiseTxt)
                                        }
                                        actor._hp = actor.mHp
                                        if actor.shapeShifted, let actorEvent = self.spriteRun {
                                            actorEvent(self, crActor, nil, true, nil, nil)
                                        }
                                        if let stateDur = actor.stateDur {
                                            actor.updateStates(remove: true, states: stateDur, withDur: true)
                                        }
                                    } else {
                                        actor._hp = 0
                                        if let stateDur = actor.stateDur {
                                            actor.updateStates(remove: true, states: stateDur, withDur: true)
                                        }
                                        actor.stunned = true
                                        actor.knockedOut = true
                                        self.checkStatus(ret: &ret)
                                    }
                                }
                            }
                        } else {
                            let oHp = actor.hp, mHp = actor.mHp
                            actor._hp = hp > mHp ? mHp : hp
                            if oHp < 1 {
                                actor.stunned = false
                                actor.knockedOut = false
                                actor.refreshCostumes()
                                self.resetTurn(actor: actor)
                            }
                        }
                        return false
                    })
                }
                player._delayTrn = -1
                player._oldSide = i
                player.side = i
            }
            players?.append(contentsOf: party)
        }
        self.crActor = crActor
        self.agiCalc()
        if useInit {
            crActor.cInit = mInit
        } else {
            current = players!.firstIndex(of: crActor)!
        }
        self.current = current
        self.previous = current
        if let event = events?[SceneEvent.begin], event(self, &ret) {
            self.endTurn(ret: &ret, actor: crActor)
        }
    }
    
}
