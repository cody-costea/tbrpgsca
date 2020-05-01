/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

import Foundation

class Role : Hashable {
    
    public static let FLAG_REVIVE: Int = 1
    public static let FLAG_RANGE: Int = 2
    public static let DMG_TYPE_ATK: Int = 1
    public static let DMG_TYPE_DEF: Int = 2
    public static let DMG_TYPE_SPI: Int = 4
    public static let DMG_TYPE_WIS: Int = 8
    public static let DMG_TYPE_AGI: Int = 16
    public static let ELEMENT_FIRE: Int = 32
    public static let ELEMENT_WATER: Int = 64
    public static let ELEMENT_THUNDER: Int = 128
    public static let ELEMENT_EARTH: Int = 256
    public static let ELEMENT_PSYCHIC: Int = 512
    public static let ELEMENT_LIGHT: Int = 1024
    public static let DEFAULT_RES = 3
    
    public static var HpTxt: String = "HP"
    public static var MpTxt: String = "MP"
    public static var SpTxt: String = "RP"
    
    internal var _id: Int, _name: String, _sprite: String?, _hp: Int, _mp: Int,_sp: Int, _mHp: Int,
                 _mMp: Int, _mSp: Int, _dmgType: Int, _flags: Int, _stateDur: [State: Int]?
    
    public static func == (lhs: Role, rhs: Role) -> Bool {
        return lhs._id == rhs._id
    }
    
    public static func != (lhs: Role, rhs: Role) -> Bool {
        return lhs._id != rhs._id
    }
    
    public static func <= (lhs: Role, rhs: Role) -> Bool {
        return lhs._id <= rhs._id
    }
    
    public static func >= (lhs: Role, rhs: Role) -> Bool {
        return lhs._id >= rhs._id
    }
    
    public static func < (lhs: Role, rhs: Role) -> Bool {
        return lhs._id < rhs._id
    }
    
    public static func > (lhs: Role, rhs: Role) -> Bool {
        return lhs._id > rhs._id
    }
    
    open func hash(into hasher: inout Hasher) {
        hasher.combine(self._id)
    }
                
    open var hashValue: Int {
        return self._id
    }
    
    open var id: Int {
        return self._id
    }
    
    open var revives: Bool {
        return (self._flags & Role.FLAG_REVIVE) == Role.FLAG_REVIVE
    }
    
    open var ranged: Bool {
        return (self._flags & Role.FLAG_RANGE) == Role.FLAG_RANGE
    }
    
    open var name: String {
        return self._name
    }
    
    open var sprite: String? {
        return self._sprite
    }
    
    open var dmgType: Int {
        return self._dmgType
    }
    
    open var stateDur: [State: Int]? {
        return self._stateDur
    }
    
    open var mHp: Int {
        return self._mHp
    }
    
    open var mMp: Int {
        return self._mMp
    }
    
    open var mSp: Int {
        return self._mSp
    }
    
    open var hp: Int {
        return self._hp
    }
    
    open var mp: Int {
        return self._mp
    }
    
    open var sp: Int {
        return self._sp
    }
    
    open func damage(ret: inout String, scene: Scene?, absorber: Actor?,
                actor: Actor, dmg: Int, percent: Bool) -> Role {
        var dmg: Int = dmg
        if !actor.invincible {
            var dmgHp: Int, dmgMp: Int, dmgSp: Int
            if (percent)
            {
                dmgHp = (actor.mHp + dmg) * self.hp / 100
                dmgMp = (actor.mMp + dmg) * self.mp / 100
                dmgSp = (actor.mSp + dmg) * self.sp / 100
            }
            else
            {
                dmgHp = self.hp
                dmgMp = self.mp
                dmgSp = self.sp
                if (dmgHp != 0)
                {
                    dmgHp += dmgHp < 0 ? (-1 * dmg) : dmg
                }
                if (dmgMp != 0)
                {
                    dmgMp += dmgMp < 0 ? (-1 * dmg) : dmg
                }
                if (dmgSp != 0)
                {
                    dmgSp += dmgSp < 0 ? (-1 * dmg) : dmg
                }
            }
            var c: Bool = false
            if (dmgSp != 0)
            {
                c = true
                ret += String(format: Ability.SuffersTxt, actor.name)
                ret += " "
                if (dmgSp < 0)
                {
                    dmg =  -1 * dmg + dmgSp
                    ret += "+"
                }
                else
                {
                    dmg += dmgSp
                }
                actor.sp = (actor._sp - dmgSp)
                ret += String(format: "%d %@", -dmgSp, Role.SpTxt)
            }
            if (dmgMp != 0)
            {
                if (c)
                {
                    ret += ", "
                }
                else
                {
                    ret += String(format: Ability.SuffersTxt, actor.name)
                    ret += " "
                    c = true
                }
                if (dmgMp < 0)
                {
                    dmg =  -1 * dmg + dmgMp
                    ret += "+"
                }
                else
                {
                    dmg += dmgMp
                }
                actor.mp = (actor._mp - dmgMp)
                ret += String(format: "%d %@", -dmgMp, Role.MpTxt)
            }
            if (dmgHp != 0)
            {
                if (c)
                {
                    ret += ", "
                }
                else
                {
                    ret += String(format: Ability.SuffersTxt, actor.name)
                    ret += " "
                    c = true
                }
                if (dmgHp < 0)
                {
                    dmg =  -1 * dmg + dmgSp
                    ret += "+"
                }
                else
                {
                    dmg += dmgHp
                }
                actor.hp = (actor._hp - dmgHp)
                ret += String(format: "%d %@", -dmgHp, Role.HpTxt)
                if (actor.hp < 1)
                {
                    ret += String(format: Actor.KoTxt, actor.name)
                }
            }
            if c, let absorber = absorber {
                absorber.sp = (absorber._sp + dmgSp / 2)
                absorber.mp = (absorber._mp + dmgMp / 2)
                absorber.hp = (absorber._hp + dmgHp / 2)
            }
        }
        return self
    }
    
    init(id: Int, name: String, sprite: String?, hp: Int, mp: Int, sp: Int, mHp: Int, mMp: Int,
         mSp: Int, dmgType: Int, range: Bool, revive: Bool, stateDur: Dictionary<State, Int>?) {
        self._id = id
        self._name = name
        self._dmgType = dmgType
        self._stateDur = stateDur
        self._sprite = sprite
        self._mHp = mHp
        self._mMp = mMp
        self._mMp = mMp
        self._mSp = mSp
        self._hp = hp
        self._mp = mp
        self._sp = sp
        var flags: Int = 0
        if range {
            flags |= Role.FLAG_RANGE
        }
        if revive {
            flags |= Role.FLAG_REVIVE
        }
        self._flags = flags
    }
    
}
