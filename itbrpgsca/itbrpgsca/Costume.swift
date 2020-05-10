/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

import Foundation

class Costume : Role {
    
    public static let FLAG_ENRAGED = 4
    public static let FLAG_CONFUSE = 8
    public static let FLAG_CONVERT = 16
    public static let FLAG_REFLECT = 32
    public static let FLAG_SHAPE_SHIFT = 64
    public static let FLAG_INVINCIBLE = 128
    public static let FLAG_STUN = 256
    public static let FLAG_KO = 512
    
    public static var CausesTxt = ", %@ is affected by %@"
    
    internal var _atk: Int, _def: Int, _spi: Int, _wis: Int, _agi: Int, _mActions: Int,
                 _aSkills: [Ability]?, _counters: [Ability]?, _stRes: [State: Int]?, _res: [Int: Int]?
    
    open var enraged: Bool {
        return (self._flags & Costume.FLAG_ENRAGED) == Costume.FLAG_ENRAGED
    }
    
    open var confused: Bool {
        return (self._flags & Costume.FLAG_CONFUSE) == Costume.FLAG_CONFUSE
    }
    
    open var converted: Bool {
        return (self._flags & Costume.FLAG_CONVERT) == Costume.FLAG_CONVERT
    }
    
    open var reflects: Bool {
        return (self._flags & Costume.FLAG_REFLECT) == Costume.FLAG_REFLECT
    }
    
    open var invincible: Bool {
        return (self._flags & Costume.FLAG_INVINCIBLE) == Costume.FLAG_INVINCIBLE
    }
    
    open var shapeShifted: Bool {
        return (self._flags & Costume.FLAG_SHAPE_SHIFT) == Costume.FLAG_SHAPE_SHIFT
    }
    
    open var stunned: Bool {
        return (self._flags & Costume.FLAG_STUN) == Costume.FLAG_STUN
    }
    
    open var knockedOut: Bool {
        return (self._flags & Costume.FLAG_KO) == Costume.FLAG_KO
    }
    
    open var counters: [Ability]? {
        return self._counters
    }
    
    open var stRes: [State: Int]? {
        return self._stRes
    }
    
    open var aSkills: [Ability]? {
        return self._aSkills
    }
    
    open var res: [Int: Int]? {
        return self._res
    }
    
    open var atk: Int {
        return self._atk
    }
    
    open var def: Int {
        return self._def
    }
    
    open var spi: Int {
        return self._spi
    }
    
    open var wis: Int {
        return self._wis
    }
    
    open var agi: Int {
        return self._agi
    }
    
    open var mActions: Int {
        return self._mActions
    }
    
    open func refresh(actor: Actor, updStates: Bool, remove: Bool) {
        if let skills = self.aSkills {
            actor.updateSkills(remove: remove, counters: false, skills: skills)
        }
        if let counters = self.counters {
            actor.updateSkills(remove: remove, counters: true, skills: counters)
        }
        if updStates, let cStates = self.stateDur {
            actor.updateStates(remove: true, states: cStates, withDur: false)
        }
        if remove {
            let roleFlags = self._flags
            let actorFlags = actor.flags
            if (actorFlags & roleFlags) == roleFlags {
                actor.flags = actorFlags ^ roleFlags
            }
            if self.shapeShifted, self.sprite != nil {
                actor.sprite = actor.job.sprite
            }
            if self.hp == 0 && self.mp == 0 && self.sp == 0 {
                let roleElm = self.dmgType
                let actorElm = self.dmgType
                if (actorElm & roleElm) == roleElm {
                    actor.dmgType = actorElm ^ roleElm
                }
            } else if var dmgRoles = actor._dmgRoles {
                dmgRoles.removeAll(where: { $0 == self })
            }
            actor.refreshCostumes()
        } else {
            actor.flags |= self._flags
            if self.hp == 0 && self.mp == 0 && self.sp == 0 {
                actor.dmgType |= self.dmgType
            }
            if self.shapeShifted, let spr = self.sprite {
                actor.sprite = spr
            }
        }
    }
    
    open func adopt(actor: Actor, updStates: Bool) {
        self.adopt(actor: actor, updStates: updStates, remove: false)
    }
    
    open func adopt(actor: Actor, updStates: Bool, remove: Bool) {
        actor.updateAttributes(remove: remove, costume: self)
        actor.updateResistance(remove: remove, elmRes: self.res, stRes: self.stRes)
        if remove {
            self.refresh(actor: actor, updStates: updStates, remove: true)
        } else {
            if updStates {
                if let cStates = self.stateDur {
                    actor.updateStates(remove: false, states: cStates, withDur: true)
                }
            }
            if self.hp != 0 || self.mp != 0 || self.sp != 0 {
                var dmgRoles: [Costume]! = actor._dmgRoles
                if dmgRoles == nil {
                    dmgRoles = [Costume]()
                    actor._dmgRoles = dmgRoles
                }
                dmgRoles.append(self)
            }
            self.refresh(actor: actor, updStates: false, remove: false)
        }
    }
    
    open func apply(ret: inout String, actor: Actor) {
        //TODO: append ret string
        self.damage(ret: &ret, absorber: nil, actor: actor, dmg: Int.random(in: 0...4), percent: true)
    }
    
    open func abandon(actor: Actor, delStates: Bool) {
        actor.updateAttributes(remove: true, costume: self)
        actor.updateResistance(remove: true, elmRes: self.res, stRes: self.stRes)
        self.refresh(actor: actor, updStates: delStates, remove: true)
    }
    
    init(id: Int, name: String, sprite: String?, shapeShift: Bool, mActions: Int, elm: Int, hpDmg: Int,
         mpDmg: Int, spDmg: Int, mHp: Int, mMp: Int, mSp: Int, atk: Int, def: Int, spi: Int, wis: Int,
         agi: Int, stun: Bool, range: Bool, enrage: Bool, confuse: Bool, reflect: Bool, invincible: Bool,
         ko: Bool, revive: Bool, skills: [Ability]?, counters: [Ability]?, states: [State: Int]?,
         stRes: [State: Int]?, res: [Int: Int]?) {
        self._counters = counters
        self._aSkills = skills
        self._stRes = stRes
        self._res = res
        self._atk = atk
        self._def = def
        self._spi = spi
        self._wis = wis
        self._agi = agi
        self._mActions = mActions
        super.init(id: id, name: name, sprite: sprite, hp: hpDmg, mp: mpDmg, sp: spDmg, mHp: mHp,
                   mMp: mMp, mSp: mSp, dmgType: elm, range: range, revive: revive, stateDur: states)
        var flags = self._flags
        if shapeShift {
            flags |= Costume.FLAG_SHAPE_SHIFT
        }
        if invincible {
            flags |= Costume.FLAG_INVINCIBLE
        }
        if enrage {
            flags |= Costume.FLAG_ENRAGED
        }
        if confuse {
            flags |= Costume.FLAG_CONFUSE
        }
        if reflect {
            flags |= Costume.FLAG_REFLECT
        }
        if stun {
            flags |= Costume.FLAG_STUN
        }
        if ko {
            flags |= Costume.FLAG_KO
        }
        self._flags = flags
    }
    
}
