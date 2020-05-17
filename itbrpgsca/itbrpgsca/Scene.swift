/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

import Foundation

public let STATUS_DEFEAT = -2
public let STATUS_RETREAT = -1
public let STATUS_ONGOING = 0
public let STATUS_VICTORY = 1
public let EVENT_BEGIN_SCENE = 0
public let EVENT_BEFORE_ACT = 1
public let EVENT_AFTER_ACT = 2
public let EVENT_NEW_TURN = 3
public let EVENT_END_SCENE = 4
public let MIN_ROUND = Int.min

protocol Scene {
    
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
    
    func playAi(ret: inout String, player: Actor)
    
    func endTurn(ret: inout String, actor: Actor)
    
    func getGuardian(user: Actor, target: Actor, skill: Ability) -> Actor
    
    func getAiSkill(user: Actor, skills: [Ability], index: Int, nRestore: Bool) -> Int
    
    func perform(ret: inout String, user: Actor, target: Actor, ability: Ability, item: Bool)
    
    func canTarget(user: Actor, ability: Ability, target: Actor) -> Bool
    
    func checkStatus(ret: inout String)
    
    func escape(ret: inout String)
    
}

extension Scene {
    
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
    
    func perform(ret: inout String, user: Actor, target: Actor, ability: Ability, item: Bool) {
        
    }
    
    func canTarget(user: Actor, ability: Ability, target: Actor) -> Bool {
        return false
    }
    
    func checkStatus(ret: inout String) {
        
    }
    
    func escape(ret: inout String) {
        
    }
    
    
}
