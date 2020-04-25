/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

import Foundation

class Actor : Costume {
    
    public static var KoTxt = ", %@ falls unconscious"
    public static var RiseTxt = ", but rises again"
    
    public static let EVENT_HP = 0
    public static let EVENT_MP = 1
    public static let EVENT_SP = 2
    
    internal var _lv: Int, _mLv: Int, _xp: Int, _maXp: Int, _init: Int, _side: Int, _oldSide: Int, _actions: Int,
                 _dmgChain: Int, _chainNr: Int, _delayTrn: Int, _delayAct: (() -> Bool)?, _dmgRoles: [Costume]?,
                 _skillsCrQty: [Ability : Int]?, _skillsRgTurn: [Ability: Int]?, _items: [Ability: Int]?,
                 _events: [Int: [(Actor) -> Bool]?]?
    
    override var revives: Bool {
        get {
            return super.revives
        }
        set (val) {
            let flags = self.flags
            if val != ((flags & Role.FLAG_REVIVE) == Role.FLAG_REVIVE) {
                self.flags = flags ^ Role.FLAG_REVIVE
            }
        }
    }
    
    override var enraged: Bool {
        get {
            return super.enraged
        }
        set (val) {
            let flags = self.flags
            if val != ((flags & Costume.FLAG_ENRAGED) == Costume.FLAG_ENRAGED) {
                self.flags = flags ^ Costume.FLAG_ENRAGED
            }
        }
    }
    
    override var ranged: Bool {
        get {
            return super.ranged
        }
        set (val) {
            let flags = self.flags
            if val != ((flags & Role.FLAG_RANGE) == Role.FLAG_RANGE) {
                self.flags = flags ^ Role.FLAG_RANGE
            }
        }
    }
    
    override var confused: Bool {
        get {
            return super.confused
        }
        set (val) {
            let flags = self.flags
            if val != ((flags & Costume.FLAG_CONFUSE) == Costume.FLAG_CONFUSE) {
                self.flags = flags ^ Costume.FLAG_CONFUSE
            }
        }
    }
    
    override var converted: Bool {
        get {
            return super.converted
        }
        set (val) {
            let flags = self.flags
            if val != ((flags & Costume.FLAG_CONVERT) == Costume.FLAG_CONVERT) {
                self.flags = flags ^ Costume.FLAG_CONVERT
            }
        }
    }
    
    override var shapeShifted: Bool {
        get {
            return super.shapeShifted
        }
        set (val) {
            let flags = self.flags
            if val != ((flags & Costume.FLAG_SHAPE_SHIFT) == Costume.FLAG_SHAPE_SHIFT) {
                self.flags = flags ^ Costume.FLAG_SHAPE_SHIFT
            }
        }
    }
    
    override var reflects: Bool {
        get {
            return super.reflects
        }
        set (val) {
            let flags = self.flags
            if val != ((flags & Costume.FLAG_REFLECT) == Costume.FLAG_REFLECT) {
                self.flags = flags ^ Costume.FLAG_REFLECT
            }
        }
    }
    
    override var stunned: Bool {
        get {
            return super.stunned
        }
        set (val) {
            let flags = self.flags
            if val != ((flags & Costume.FLAG_STUN) == Costume.FLAG_STUN) {
                self.flags = flags ^ Costume.FLAG_STUN
            }
        }
    }
    
    override var name: String {
        get {
            return self._name
        }
        set (val) {
            self._name = val
        }
    }
    
    override var sprite: String {
        get {
            return self._sprite
        }
        set (val) {
            self._sprite = val
        }
    }
    
    override var mHp: Int {
        get {
            return self._mHp
        }
        set(val) {
            self._mHp = val < 1 ? 1 : val
        }
    }
    
    override var mMp: Int {
        get {
            return self._mMp
        }
        set(val) {
            self._mMp = val < 1 ? 1 : val
        }
    }
    
    override var mSp: Int {
        get {
            return self._mSp
        }
        set(val) {
            self._mSp = val < 1 ? 1 : val
        }
    }
    
    override var hp: Int {
        get {
            return self._hp
        }
        set(val) {
            let mHp = self._mHp
            self._hp = val > mHp ? mHp : (val < 0 ? 0 : val)
        }
    }
    
    override var mp: Int {
        get {
            return self._mp
        }
        set(val) {
            let mMp = self._mMp
            self._mp = val > mMp ? mMp : (val < 0 ? 0 : val)
        }
    }
    
    override var sp: Int {
        get {
            return self._sp
        }
        set(val) {
            let mSp = self._mSp
            self._sp = val > mSp ? mSp : (val < 0 ? 0 : val)
        }
    }
    
    override var atk: Int {
        get {
            return self._atk
        }
        set (val) {
            self._atk = val < 1 ? 1 : val
        }
    }
    
    override var agi: Int {
        get {
            return self._agi
        }
        set (val) {
            self._agi = val < 1 ? 1 : val
        }
    }
    
    override var def: Int {
        get {
            return self._def
        }
        set (val) {
            self._def = val < 1 ? 1 : val
        }
    }
    
    override var spi: Int {
        get {
            return self._spi
        }
        set (val) {
            self._spi = val < 1 ? 1 : val
        }
    }
    
    override var wis: Int {
        get {
            return self._atk
        }
        set (val) {
            self._atk = val < 1 ? 1 : val
        }
    }
    
    override var dmgType: Int {
        get {
            return self._dmgType
        }
        set (val) {
            self._dmgType = val
        }
    }
    
    override var mActions: Int {
        get {
            return self._mActions
        }
        set (val) {
            self._mActions = val < 1 ? 1 : val
        }
    }
    
    override var counters: [Ability]? {
        get {
            return self._counters
        }
        set (val) {
            self._counters = val
        }
    }
    
    override var aSkills: [Ability]? {
        get {
            return self._aSkills
        }
        set (val) {
            self._aSkills = val
        }
    }
    
    override var stateDur: [State : Int]? {
        get {
            return self._stateDur
        }
        set (val) {
            self._stateDur = val
        }
    }
    
    override var stRes: [State: Int]? {
        get {
            return self._stRes
        }
        set (val) {
            self._stRes = val
        }
    }
    
    override var res: [Int: Int]? {
        get {
            return self._res
        }
        set (val) {
            self._res = val
        }
    }
    
    internal func setLv(lv: Int, scene: Scene?) -> Actor {
        return self
    }
    
    internal func setXp(xp: Int, scene: Scene?) -> Actor {
        return self
    }
    
    internal func setJob(job: Costume, scene: Scene?) -> Actor {
        return self
    }
    
    internal func setRace(race: Costume, scene: Scene?) -> Actor {
        return self
    }
    
    internal func setHp(hp: Int, ret: inout String?, scene: Scene?, survive: Bool) -> Actor {
        self.hp = hp
        return self
    }
    
    internal func setAgi(agi: Int, scene: Scene) -> Actor {
        self.agi = agi
        return self
    }
    
    init(id: Int, name: String, sprite: String, race: Costume, job: Costume, level: Int, maxLv: Int,
         mActions: Int, mHp: Int, mMp: Int, mSp: Int, atk: Int, def: Int, spi: Int,wis: Int, agi: Int,
         res: [Int: Int]?, stRes: [State: Int]?, items: [Ability: Int]) {
        self._lv = 1
        self._xp = 0
        self._maXp = 15
        self._mLv = maxLv
        self._init = 0
        self._side = 0
        self._oldSide = 0
        self._dmgChain = 0
        self._chainNr = 0
        self._delayTrn = 0
        self._delayAct = nil
        self._actions = mActions
        self._items = items
        self._dmgRoles = nil
        self._skillsCrQty = nil
        self._skillsRgTurn = nil
        self._events = nil
        super.init(id: id, name: name, sprite: sprite, shapeShift: false, mActions: mActions,
                   elm: 0, hpDmg: mHp, mpDmg: mMp, spDmg: mSp, mHp: mHp, mMp: mMp, mSp: mSp,
                   atk: atk, def: def, spi: spi, wis: wis, agi: agi, stun: false, range: false,
                   enrage: false, confuse: false, reflect: false, invincible: false, ko: false,
                   revive: false, skills: nil, counters: nil, states: nil, stRes: stRes, res: res)
        //self.race = race
    }
    
}
