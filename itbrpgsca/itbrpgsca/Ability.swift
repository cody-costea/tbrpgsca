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
    public static let FLAG_TRG_SIDE = 256
    public static let FLAG_TRG_SELF = 512
    public static let FLAG_TRG_ALL = 4
    
    public static var MissesTxt = ", but misses %@"
    public static var SuffersTxt = ", %@ suffers"
    public static var ReflectTxt = ", reflected by %@"
    public static var ResistTxt = ", resisted by %@"
    public static var StolenTxt = ", obtaining %@ from %@"
    
    internal var _lvRq: Int, _attrInc: Int, _mQty: Int, _rQty: Int, _rStates: [State: Int]?
    
    open var steals: Bool {
        return (self._flags & Ability.FLAG_STEAL) == Ability.FLAG_STEAL
    }
    
    open var melee: Bool {
        return (self._flags & Ability.FLAG_MELEE) == Ability.FLAG_MELEE
    }
    
    open var absorbs: Bool {
        return (self._flags & Ability.FLAG_ABSORB) == Ability.FLAG_ABSORB
    }
    
    open var canMiss: Bool {
        return (self._flags & Ability.FLAG_MISSABLE) == Ability.FLAG_MISSABLE
    }
    
    open var doesCritical: Bool {
        return (self._flags & Ability.FLAG_CRITICAL) == Ability.FLAG_CRITICAL
    }
    
    open var targetsSide: Bool {
        return (self._flags & Ability.FLAG_TRG_SIDE) == Ability.FLAG_TRG_SIDE
    }
    
    open var targetsSelf: Bool {
        return (self._flags & Ability.FLAG_TRG_SELF) == Ability.FLAG_TRG_SELF
    }
    
    open var targetsAll: Bool {
        return (self._flags & Ability.FLAG_TRG_ALL) == Ability.FLAG_TRG_ALL
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
    
    open func replenish(user: Actor) {
        let mQty = self._mQty
        if mQty > 0 {
            var usrSkills: [Ability : Int]! = user._skillsCrQty
            if usrSkills == nil {
                usrSkills = [Ability : Int]()
                user._skillsCrQty = usrSkills
            }
        }
    }
    
    open func canPerform(user: Actor) -> Bool {
        let skillsQty: [Ability : Int]! = user._skillsCrQty
        return (self.mMp <= user.mp && self.mSp <= user.sp && self.mHp < user.hp && user.level >= self._lvRq
                && (skillsQty == nil || skillsQty[self] ?? 1 > 0))
    }
    
    open func execute(ret: inout String, user: Actor, target: Actor, applyCosts: Bool) {
        var trg: Actor
        let dmgType = self.dmgType | user.dmgType, trgRflType = target.rflType
        if dmgType & trgRflType == trgRflType {
            ret.append(String(format: Ability.ReflectTxt, target.name))
            trg = user
        } else {
            trg = target
        }
        var canMiss = self.canMiss ? 4 : 0, def = 0, i = 0, dmg = 0, usrAgi = user.agi,
                      trgAgi = trg.agi, trgSpi = trg.spi, usrWis = user.wis
        if (dmgType & Role.DMG_TYPE_ATK) == Role.DMG_TYPE_ATK {
            def += trg.def
            dmg += user.atk
            i += 1
        }
        if (dmgType & Role.DMG_TYPE_DEF) == Role.DMG_TYPE_DEF {
            def += trg.def
            dmg += user.def
            i += 1
        }
        if (dmgType & Role.DMG_TYPE_SPI) == Role.DMG_TYPE_SPI {
            def += trg.wis
            dmg += user.spi
            i += 1
        }
        if (dmgType & Role.DMG_TYPE_WIS) == Role.DMG_TYPE_WIS {
            def += trg.spi
            dmg += user.wis
            i += 1
        }
        if (dmgType & Role.DMG_TYPE_AGI) == Role.DMG_TYPE_AGI {
            def += trg.agi
            dmg += user.agi
            if canMiss > 0 {
                canMiss = 3
            }
            i += 1
        }
        usrAgi = (usrAgi + usrWis) / 2
        trgAgi = ((trgAgi + trgSpi) / 2) / 3
        if canMiss == 0 {
            canMiss = Int.random(in: 0...(usrAgi / 2)) + (usrAgi / canMiss)
            if canMiss > trgAgi - (Int.random(in: 0...trgAgi)) {
                
            }
        }
    }
    
    init(id: Int, name: String, sprite: String?, steal: Bool, range: Bool, melee: Bool, canMiss: Bool,
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
        var flags = self._flags
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
