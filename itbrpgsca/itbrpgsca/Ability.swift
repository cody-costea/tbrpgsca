/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

import Foundation

class Ability : Role {
    
    public static let FLAG_MELEE = 4
    public static let FLAG_STEAL = 8
    public static let FLAG_ABSORB = 16
    public static let FLAG_MISSABLE = 32
    public static let FLAG_CRITICAL = 64
    public static let FLAG_CHAINING = 128
    public static let FLAG_TRG_ALL = 4
    public static let FLAG_TRG_SIDE = 256
    public static let FLAG_TRG_SELF = 512
    
    public static var MissesTxt = ", but misses %@"
    public static var SuffersTxt = ", %@ suffers"
    public static var ReflectTxt = ", reflected by %@"
    public static var ResistTxt = ", resisted by %@"
    public static var StolenTxt = ", obtaining %@ from %@"
    
    internal var _lvRq: Int, _attrInc: Int, _mQty: Int, _rQty: Int, _rStates: [State: Int]?
    
    open var steals: Bool {
        return (self.flags & Ability.FLAG_STEAL) == Ability.FLAG_STEAL
    }
    
    open var absorbs: Bool {
        return (self.flags & Ability.FLAG_ABSORB) == Ability.FLAG_ABSORB
    }
    
    open var melee: Bool {
        return (self.flags & Ability.FLAG_MELEE) == Ability.FLAG_MELEE
    }
    
    open var targetsSide: Bool {
        return (self.flags & Ability.FLAG_TRG_SIDE) == Ability.FLAG_TRG_SIDE
    }
    
    open var targetsSelf: Bool {
        return (self.flags & Ability.FLAG_TRG_SELF) == Ability.FLAG_TRG_SELF
    }
    
    open var targetsAll: Bool {
        return (self.flags & Ability.FLAG_TRG_ALL) == Ability.FLAG_TRG_ALL
    }
    
    open var rStates: [State: Int]? {
        return self._rStates
    }
    
    open var attrInc: Int {
        return self._attrInc
    }
    
    open var lvRq: Int {
        return self._lvRq
    }
    
    open var mQty: Int {
        return self._mQty
    }
    
    open var rQty: Int {
        return self._rQty
    }
    
    open func replenish(user: Actor) -> Ability {
        let mQty = self._mQty
        if mQty > 0 {
            
        }
        return self
    }
    
    open func canPerform(user: Actor) -> Ability {
        return self
    }
    
    open func execute(ret: inout String, scene: Scene?, user: Actor,
                      target: Actor, applyCosts: Bool) -> Ability {
        return self
    }
    
    init(id: Int, name: String, sprite: String, steal: Bool, range: Bool, melee: Bool, canMiss: Bool,
        doesCritical: Bool, canChain: Bool, lvRq: Int, hp: Int, mp: Int, sp: Int, attrInc: Int, dmgType: Int,
        mHp: Int, mMp: Int, mSp: Int, trg: Int, elm: Int, rQty: Int, mQty: Int, absorb: Bool, revive: Bool,
        aStates: [State : Int]?, rStates: [State : Int]?) {
        self._attrInc = attrInc
        self._mQty = mQty
        self._rQty = rQty
        self._lvRq = lvRq
        self._rStates = rStates
        super.init(id: id, name: name, sprite: sprite, hp: hp, mp: mp, sp: sp, mHp: mHp, mMp: mMp,
                   mSp: mSp, dmgType: dmgType, range: range, revive: revive, stateDur: aStates)
        var flags = self.flags
        if doesCritical {
            flags |= Ability.FLAG_CRITICAL
        }
        if canChain {
            flags |= Ability.FLAG_CHAINING
        }
        if canMiss {
            flags |= Ability.FLAG_MISSABLE
        }
        if absorb {
            flags |= Ability.FLAG_ABSORB
        }
        if steal {
            flags |= Ability.FLAG_STEAL
        }
        if melee {
            flags |= Ability.FLAG_MELEE
        }
        flags |= trg
    }
    
}