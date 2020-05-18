/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

import Foundation

public let MIN_ROUND = Int.min

public enum SceneEvent {
    case begin, beforeAct, afterAct, newTurn, end
}

public enum SceneStatus: Int {
    case defeat = -2, retreat = -1, ongoing = 0, victory = 1
}

public protocol Scene : class {
    
    typealias SceneRun = (Scene, inout String) -> Bool
    
    typealias SpriteRun = (Scene, Actor?, Ability?, Bool, Actor?, Ability?) -> Bool
    
    var status: Int {
        get
        set
    }
    
    var current: Int {
        get
        set
    }
    
    var previous: Int { //oldCurrent
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
    
    var mInit: Int {
        get
        set
    }
    
    var spriteRun: SpriteRun? {
        get
        set
    }
    
    var events: [SceneEvent : [SceneRun]]? {
        get
        set
    }
    
    func playAi(ret: inout String, player: Actor)
    
    func endTurn(ret: inout String, actor: Actor)
    
    func getGuardian(user: Actor, target: Actor, skill: Ability) -> Actor
    
    func getAiSkill(user: Actor, skills: [Ability], index: Int, nRestore: Bool) -> Int
    
    func execute(ret: inout String, user: Actor, target: Actor?, ability: Ability, applyCosts: Bool)
    
    func perform(ret: inout String, user: Actor, target: Actor, ability: Ability, item: Bool)
    
    func initialize(ret: inout String, parties: [[Actor]], spriteRun: SpriteRun?,
                    events: [SceneEvent : [SceneRun]]?, surprise: Int, mInit: Int)
    
    func canTarget(user: Actor, ability: Ability, target: Actor) -> Bool
    
    func checkStatus(ret: inout String)
    
    func escape(ret: inout String)
    
    func resetTurn(actor: Actor)
    
    func agiCalc()
    
}

public extension Scene where Self: AnyObject {
    
    func playAi(ret: inout String, player: Actor) {
        
    }
    
    func endTurn(ret: inout String, actor: Actor) {
        
    }
    
    func getGuardian(user: Actor, target: Actor, skill: Ability) -> Actor {
        return target
    }
    
    func getAiSkill(user: Actor, skills: [Ability], index: Int, nRestore: Bool) -> Int {
        return 0
    }
    
    func execute(ret: inout String, user: Actor, target: Actor?, ability: Ability, applyCosts: Bool) {
        
    }
    
    func perform(ret: inout String, user: Actor, target: Actor, ability: Ability, item: Bool) {
        
    }
    
    func initialize(ret: inout String, parties: [[Actor]], spriteRun: SpriteRun?,
                    events: [SceneEvent : [SceneRun]]?, surprise: Int, mInit: Int) {
        assert(parties.count > 1)
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
            if crActor == nil {
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
                }
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
        if let events = events?[SceneEvent.begin] {
            var defAction = false
            for event in events {
                if event(self, &ret) {
                    defAction = true
                }
            }
            if defAction {
                self.endTurn(ret: &ret, actor: crActor)
            }
        }
    }
    
    func canTarget(user: Actor, ability: Ability, target: Actor) -> Bool {
        return false
    }
    
    func checkStatus(ret: inout String) {
        
    }
    
    func escape(ret: inout String) {
        
    }
    
    func resetTurn(actor: Actor) {
        
    }
    
    func agiCalc() {
        if self.mInit < 1 {
            self.players?.sort(by: { $0.agi > $1.agi })
            self.previous = -1
        }
    }
    
}
