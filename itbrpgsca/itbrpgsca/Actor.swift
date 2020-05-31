/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

import Foundation

open class Actor : Costume {
    
    public static let FLAG_RANDOM_AI = 2048
    public static let FLAG_AI_PLAYER = 4096
    
    public static var KoTxt = ", %@ falls unconscious"
    public static var RiseTxt = ", but rises again"
    
    public typealias ActorRun = (Actor, Any) -> Bool
    
    public enum EquipPos: Character {
        case race = "\u{0}", job = "\u{1}", arms = "a", chest = "c", weapon = "w", shield = "s", head = "h",
             legs = "l", feet = "f", belt = "b", ring1 = "1", ring2 = "2", ring3 = "3", ring4 = "4", ring5 = "5",
             ring6 = "6", ring7 = "7", ring8 = "8", ring9 = "9", ring10 = "0", necklace = "n", mantle = "m"
    }
    
    public enum EventType {
        case hp, mp, sp, mHp, mMp, mSp, atk, def, spi, wis, agi, actions, mActions, mDelayTrn, dmgType, rflType, items, cover,
             drawn, race, job, exp, maxExp, level, maxLv, side, cInit, sprite, name, flags, delayTrn, dmgChain, chainNr
    }
    
    internal var _lv: Int, _mLv: Int, _xp: Int, _maXp: Int, _init: Int, _side: Int, _oldSide: Int, _actions: Int,
                 _dmgChain: Int, _chainNr: Int, _delayTrn: Int, _delayAct: ((Bool) -> Void)?, _dmgRoles: [Costume]?,
                 _skillsCrQty: [Ability : Int]?, _skillsRgTurn: [Ability: Int]?, _drawn: Actor?, _cover: Actor?,
                 _items: [Ability: Int]?, _equipment: [EquipPos: Costume], _events: [EventType: [ActorRun]]?
    
    internal var flags: Int {
        get {
            return self._flags
        }
        set (val) {
            if self.runEvent(eventType: EventType.flags, newValue: val) {
                self._flags = val
            }
        }
    }
    
    open var coveredBy: Actor? {
        get {
            return self._drawn
        }
        set (val) {
            if self.runEvent(eventType: EventType.cover, newValue: val as Any) {
                self._cover = val
            }
        }
    }
    
    open var drawnBy: Actor? {
        get {
            return self._drawn
        }
        set (val) {
            if self.runEvent(eventType: EventType.flags, newValue: val as Any) {
                self._drawn = val
            }
        }
    }
    
    open var race: Costume {
        get {
            return self._equipment[EquipPos.race]!
        }
        set (val) {
            if self.runEvent(eventType: EventType.race, newValue: val) {
                self.equipItem(pos: EquipPos.race, item: val)
            }
        }
    }
    
    open var job: Costume {
        get {
            return self._equipment[EquipPos.job]!
        }
        set (val) {
            if self.runEvent(eventType: EventType.job, newValue: val) {
                self.equipItem(pos: EquipPos.job, item: val)
                if !self.shapeShifted, let spr = val.sprite {
                    self.sprite = spr
                }
            }
        }
    }
    
    open var level: Int {
        get {
            return self._lv
        }
        set (val) {
            if self.runEvent(eventType: EventType.level, newValue: val) {
                let mLv = self._mLv
                self._lv = val > mLv ? mLv : (val < 2 ? 1 : val)
            }
        }
    }
    
    open var maxLevel: Int {
        get {
            return self._mLv
        }
        set (val) {
            if self.runEvent(eventType: EventType.maxLv, newValue: val) {
                self._mLv = val < 2 ? 1 : val
            }
        }
    }
    
    open var exp: Int {
        get {
            return self._xp
        }
        set (val) {
            if self.runEvent(eventType: EventType.exp, newValue: val) {
                let maXp = self._maXp
                self._xp = val > maXp ? maXp : (val < 1 ? 0 : val)
            }
        }
    }
    
    open var maxExp: Int {
        get {
            return self._maXp
        }
        set (val) {
            if self.runEvent(eventType: EventType.maxExp, newValue: val) {
                self._maXp = val < 2 ? 1 : val
            }
        }
    }
    
    open var side: Int {
        get {
            return self._side
        }
        set (val) {
            if self.runEvent(eventType: EventType.side, newValue: val) {
                self._side = val
            }
        }
    }
    
    open var cInit: Int {
        get {
            return self._init
        }
        set (val) {
            if self.runEvent(eventType: EventType.cInit, newValue: val) {
                self._side = val
            }
        }
    }
    
    open var delayTrn: Int {
        get {
            return self._delayTrn
        }
        set (val) {
            if self.runEvent(eventType: EventType.delayTrn, newValue: val) {
                self._delayTrn = val
            }
        }
    }
    
    open var dmgChain: Int {
        get {
            return self._dmgChain
        }
        set (val) {
            if self.runEvent(eventType: EventType.dmgChain, newValue: val) {
                self._dmgChain = val
            }
        }
    }
    
    open var chainNr: Int {
        get {
            return self._chainNr
        }
        set (val) {
            if self.runEvent(eventType: EventType.chainNr, newValue: val) {
                self._chainNr = val
            }
        }
    }
    
    open var actions: Int {
        get {
            return self._actions
        }
        set (val) {
            if self.runEvent(eventType: EventType.actions, newValue: val) {
                let mActions = self._mActions
                self._actions = val > mActions ? mActions : (val < 1 ? 0 : val)
            }
        }
    }
    
    open var items: [Ability: Int]? {
        get {
            return self._items
        }
        set (val) {
            if self.runEvent(eventType: EventType.items, newValue: val as Any) {
                self._items = val
            }
        }
    }
    
    open var randomAi: Bool {
        get {
            return (self._flags & Actor.FLAG_RANDOM_AI) == Actor.FLAG_RANDOM_AI
        }
        set (val) {
            let flags = self._flags
            if val != ((flags & Actor.FLAG_RANDOM_AI) == Actor.FLAG_RANDOM_AI) {
                self.flags = flags ^ Actor.FLAG_RANDOM_AI
            }
        }
    }
    
    open var aiControl: Bool {
        get {
            return (self._flags & Actor.FLAG_AI_PLAYER) == Actor.FLAG_AI_PLAYER
        }
        set (val) {
            let flags = self._flags
            if val != ((flags & Actor.FLAG_AI_PLAYER) == Actor.FLAG_AI_PLAYER) {
                self.flags = flags ^ Actor.FLAG_AI_PLAYER
            }
        }
    }
    
    override open var revives: Bool {
        get {
            return super.revives
        }
        set (val) {
            let flags = self._flags
            if val != ((flags & Role.FLAG_REVIVE) == Role.FLAG_REVIVE) {
                self.flags = flags ^ Role.FLAG_REVIVE
            }
        }
    }
    
    override open var enraged: Bool {
        get {
            return super.enraged
        }
        set (val) {
            let flags = self._flags
            if val != ((flags & Costume.FLAG_ENRAGED) == Costume.FLAG_ENRAGED) {
                self.flags = flags ^ Costume.FLAG_ENRAGED
            }
        }
    }
    
    override open var ranged: Bool {
        get {
            return super.ranged
        }
        set (val) {
            let flags = self._flags
            if val != ((flags & Role.FLAG_RANGE) == Role.FLAG_RANGE) {
                self.flags = flags ^ Role.FLAG_RANGE
            }
        }
    }
    
    override open var confused: Bool {
        get {
            return super.confused
        }
        set (val) {
            let flags = self._flags
            if val != ((flags & Costume.FLAG_CONFUSE) == Costume.FLAG_CONFUSE) {
                self.flags = flags ^ Costume.FLAG_CONFUSE
            }
        }
    }
    
    override open var converted: Bool {
        get {
            return super.converted
        }
        set (val) {
            let flags = self._flags
            if val != ((flags & Costume.FLAG_CONVERT) == Costume.FLAG_CONVERT) {
                self.flags = flags ^ Costume.FLAG_CONVERT
            }
        }
    }
    
    override open var shapeShifted: Bool {
        get {
            return super.shapeShifted
        }
        set (val) {
            let flags = self._flags
            if val != ((flags & Costume.FLAG_SHAPE_SHIFT) == Costume.FLAG_SHAPE_SHIFT) {
                self.flags = flags ^ Costume.FLAG_SHAPE_SHIFT
            }
        }
    }
    
    override open var stunned: Bool {
        get {
            return super.stunned
        }
        set (val) {
            let flags = self._flags
            if val != ((flags & Costume.FLAG_STUN) == Costume.FLAG_STUN) {
                self.flags = flags ^ Costume.FLAG_STUN
            }
        }
    }
    
    override open var name: String {
        get {
            return self._name
        }
        set (val) {
            if self.runEvent(eventType: EventType.name, newValue: val) {
                self._name = val
            }
        }
    }
    
    override open var sprite: String? {
        get {
            return self._sprite
        }
        set (val) {
            if self.runEvent(eventType: EventType.sprite, newValue: val as Any) {
                self._sprite = val
            }
        }
    }
    
    override open var mDelayTrn: Int {
        get {
            return self._mDelayTrn
        }
        set(val) {
            if self.runEvent(eventType: EventType.mDelayTrn, newValue: val) {
                self._mDelayTrn = val
            }
        }
    }
    
    override open var mHp: Int {
        get {
            return self._mHp
        }
        set(val) {
            if self.runEvent(eventType: EventType.mHp, newValue: val) {
                self._mHp = val < 2 ? 1 : val
            }
        }
    }
    
    override open var mMp: Int {
        get {
            return self._mMp
        }
        set(val) {
            if self.runEvent(eventType: EventType.mMp, newValue: val) {
                self._mMp = val < 2 ? 1 : val
            }
        }
    }
    
    override open var mSp: Int {
        get {
            return self._mSp
        }
        set(val) {
            if self.runEvent(eventType: EventType.mSp, newValue: val) {
                self._mSp = val < 2 ? 1 : val
            }
        }
    }
    
    override open var hp: Int {
        get {
            return self._hp
        }
        set(val) {
            if self.runEvent(eventType: EventType.hp, newValue: val) {
                let mHp = self._mHp
                self._hp = val > mHp ? mHp : (val < 1 ? 0 : val)
            }
        }
    }
    
    override open var mp: Int {
        get {
            return self._mp
        }
        set(val) {
            if self.runEvent(eventType: EventType.mp, newValue: val) {
                let mMp = self._mMp
                self._mp = val > mMp ? mMp : (val < 1 ? 0 : val)
            }
        }
    }
    
    override open var sp: Int {
        get {
            return self._sp
        }
        set(val) {
            if self.runEvent(eventType: EventType.sp, newValue: val) {
                let mSp = self._mSp
                self._sp = val > mSp ? mSp : (val < 1 ? 0 : val)
            }
        }
    }
    
    override open var atk: Int {
        get {
            return self._atk
        }
        set (val) {
            if self.runEvent(eventType: EventType.atk, newValue: val) {
                self._atk = val < 2 ? 1 : val
            }
        }
    }
    
    override open var agi: Int {
        get {
            return self._agi
        }
        set (val) {
            if self.runEvent(eventType: EventType.agi, newValue: val) {
                self._agi = val < 2 ? 1 : val
            }
        }
    }
    
    override open var def: Int {
        get {
            return self._def
        }
        set (val) {
            if self.runEvent(eventType: EventType.def, newValue: val) {
                self._def = val < 2 ? 1 : val
            }
        }
    }
    
    override open var spi: Int {
        get {
            return self._spi
        }
        set (val) {
            if self.runEvent(eventType: EventType.spi, newValue: val) {
                self._spi = val < 2 ? 1 : val
            }
        }
    }
    
    override open var wis: Int {
        get {
            return self._wis
        }
        set (val) {
            if self.runEvent(eventType: EventType.wis, newValue: val) {
                self._wis = val < 2 ? 1 : val
            }
        }
    }
    
    override open var dmgType: Int {
        get {
            return self._dmgType
        }
        set (val) {
            if self.runEvent(eventType: EventType.dmgType, newValue: val) {
                self._dmgType = val
            }
        }
    }
    
    override open var rflType: Int {
        get {
            return self._rflType
        }
        set (val) {
            if self.runEvent(eventType: EventType.rflType, newValue: val) {
                self._rflType = val
            }
        }
    }
    
    override open var mActions: Int {
        get {
            return self._mActions
        }
        set (val) {
            if self.runEvent(eventType: EventType.mActions, newValue: val) {
                self._mActions = val < 2 ? 1 : val
            }
        }
    }
    
    override open var counters: [Ability]? {
        get {
            return self._counters
        }
        set (val) {
            self._counters = val
        }
    }
    
    override open var aSkills: [Ability]? {
        get {
            return self._aSkills
        }
        set (val) {
            self._aSkills = val
        }
    }
    
    override open var stateDur: [State : Int]? {
        get {
            return self._stateDur
        }
        set (val) {
            self._stateDur = val
        }
    }
    
    override open var stRes: [State?: Int]? {
        get {
            return self._stRes
        }
        set (val) {
            self._stRes = val
        }
    }
    
    override open var res: [Int: Int]? {
        get {
            return self._res
        }
        set (val) {
            self._res = val
        }
    }
    
    open func runEvent(eventType: EventType, newValue: Any) -> Bool {
        if let eventsMap = self._events {
            if let eventList = eventsMap[eventType] {
                var ret: Bool = true
                for event in eventList {
                    if !event(self, newValue) {
                        ret = false
                    }
                }
                return ret
            }
        }
        return true
    }
    
    open func levelUp() {
        while self.maxExp <= self.exp && self.level < self.maxLevel {
            self.maxExp *= 2
            self.level += 1
            self.mHp += 3
            self.mMp += 2
            self.mSp += 2
            self.atk += 1
            self.def += 1
            self.spi += 1
            self.wis += 1
            self.agi += 1
        }
    }
    
    open func recover(ret: inout String?) {
        if let stateDur = self.stateDur {
            self.updateStates(remove: true, states: stateDur, withDur: true)
            self.stateDur = nil
        }
        self.refreshCostumes()
        self.actions = self.mActions
        self.hp = self.mHp
        self.mp = self.mMp
        self.sp = 0
        if var res = self.res {
            for (key, val) in res {
                if val == 0 {
                    res.removeValue(forKey: key)
                }
            }
            if res.count == 0 {
                self.res = nil
            }
        }
        if var skillsQty = self._skillsCrQty {
            for (key, _) in skillsQty {
                skillsQty[key] = key.mQty
            }
        }
    }
    
    open func unequipPos(pos: EquipPos) -> Costume? {
        return self.equipItem(pos: pos, item: nil)
    }
    
    open func unequipItem(item: Costume) -> EquipPos? {
        if let pos = self._equipment.first(where: { $0.value == item })?.key {
            return pos
        } else {
            return nil
        }
    }
    
    open func equipItem(pos: EquipPos, item: Costume?) -> Costume? {
        var equipment = self._equipment
        let oldCost = equipment[pos]
        self.switchCostume(oldCost: oldCost, newCost: item)
        equipment[pos] = item
        return oldCost
    }
    
    open func switchCostume(oldCost: Costume?, newCost: Costume?) {
        if let oldCost = oldCost {
            oldCost.adopt(actor: self, updStates: true, remove: true)
        }
        if let newCost = newCost {
            newCost.adopt(actor: self, updStates: true, remove: false)
        }
    }
    
    open func updateSkills(remove: Bool, counters: Bool, skills: [Ability]) {
        var aSkills: [Ability]! = counters ? self.counters : self.aSkills
        if remove {
            if aSkills != nil {
                for ability in skills {
                    aSkills.removeAll(where: { $0 == ability })
                    if ability.rQty > 0, var regTurn = self._skillsRgTurn {
                        regTurn.removeValue(forKey: ability)
                    }
                    if ability.mQty > 0, var crQty = self._skillsCrQty {
                        crQty.removeValue(forKey: ability)
                    }
                }
            }
        } else {
            //TODO:
            if aSkills == nil {
                aSkills = [Ability]()
                if counters {
                    self.counters = aSkills
                } else {
                    self.aSkills = aSkills
                }
            }
            for ability in skills {
                if !aSkills.contains(ability) {
                    aSkills.append(ability)
                    let mQty = ability.mQty
                    if mQty > 0 {
                        var crQty: [Ability: Int]! = self._skillsCrQty
                        if crQty == nil {
                            crQty = [Ability: Int]()
                            self._skillsCrQty = crQty
                        }
                        crQty[ability] = mQty
                        self.checkRegSkill(ability: ability)
                    }
                }
            }
        }
    }
    
    open func updateResistance(remove: Bool, elmRes: [Int: Int]?, stRes: [State?: Int]?) {
        if let elmRes = elmRes {
            var aElmRes: [Int: Int]! = self.res
            if remove {
                if aElmRes != nil {
                    for (key, val) in elmRes {
                        aElmRes[key] = (aElmRes[key] ?? val) - val
                    }
                }
            } else {
                if aElmRes == nil {
                    aElmRes = [Int: Int]()
                    self.res = aElmRes
                }
                for (key, val) in aElmRes {
                    aElmRes![key] = (aElmRes[key] ?? 0) + val
                }
            }
        }
        if let stRes = stRes {
            var aStRes: [State?: Int]! = self.stRes
            if remove {
                if aStRes != nil {
                    for (key, val) in stRes {
                        aStRes[key] = (aStRes[key] ?? val) - val
                    }
                }
            } else {
                if aStRes == nil {
                    aStRes = [State: Int]()
                    self.stRes = aStRes
                }
                for (key, val) in aStRes {
                    aStRes![key] = (aStRes[key] ?? 0) + val
                }
            }
        }
    }
    
    open func updateAttributes(remove: Bool, costume: Costume) {
        let i = remove ? -1 : 1
        self.mHp = self.mHp + (i * costume.mHp)
        self.mMp = self.mMp + (i * costume.mMp)
        self.mSp = self.mSp + (i * costume.mSp)
        self.mDelayTrn = self.mDelayTrn + (i * costume.mDelayTrn)
        self.mActions = self.mActions + (i * costume.mActions)
        self.atk += costume.atk
        self.def += costume.def
        self.spi += costume.spi
        self.wis += costume.wis
        self.agi += costume.agi
    }
    
    open func updateStates(remove: Bool, states: [State: Int], withDur: Bool) {
        if remove {
            if let stateDur = self.stateDur, stateDur.count > 0 {
                for (state, rDur) in states {
                    if withDur || (rDur < 0 && rDur > State.STATE_END_DUR) {
                        state.disable(actor: self, dur: rDur, remove: false)
                    }
                }
            }
        } else {
            for (state, rDur) in states {
                if withDur || (rDur < 0 && rDur > State.STATE_END_DUR) {
                    state.inflict(user: nil, target: self, dur: rDur, always: true)
                }
            }
        }
    }
    
    open func applyDmgRoles(ret: inout String) {
        if let dmgRoles = self._dmgRoles {
            for costume in dmgRoles {
                costume.apply(ret: &ret, actor: self)
            }
        }
    }
    
    open func applyStates(ret: inout String?, consume: Bool) {
        if consume, var ret = ret {
            self.applyDmgRoles(ret: &ret)
        }
        if let stateDur = self.stateDur {
            for (key, _) in stateDur {
                if key.dur > State.STATE_END_DUR {
                    key.alter(ret: &ret, actor: self, consume: consume)
                }
            }
        }
    }
    
    open func checkRegSkill(ability: Ability) {
        if ability.rQty > 0 {
            var regSkills: [Ability : Int]! = self._skillsRgTurn
            if regSkills == nil {
                regSkills = [Ability : Int]()
                self._skillsRgTurn = regSkills
            }
            regSkills[ability] = 0
        }
    }
    
    open func refreshCostumes() {
        for (_, costume) in self._equipment {
            costume.refresh(actor: self, updStates: true, remove: false)
        }
        if let stateDur = self.stateDur {
            for (state, dur) in stateDur {
                if dur > State.STATE_END_DUR {
                    state.refresh(actor: self, updStates: false, remove: false)
                }
            }
        }
    }
    
    open func addEvent(eventType: EventType, actorRun: @escaping ActorRun) {
        var events: [EventType : [ActorRun]]! = self._events
        if events == nil {
            events = [EventType : [ActorRun]]()
        }
        var eventList: [ActorRun]! = events[eventType]
        if eventList == nil {
            eventList = [ActorRun]()
            events[eventType] = eventList
        }
        eventList.append(actorRun)
    }
    
    init(id: Int, name: String, sprite: String?, race: Costume, job: Costume, level: Int, maxLv: Int,
         mActions: Int, mHp: Int, mMp: Int, mSp: Int, atk: Int, def: Int, spi: Int,wis: Int, agi: Int,
         res: [Int: Int]?, stRes: [State: Int]?, items: [Ability: Int]?) {
        self._actions = mActions
        self._delayAct = nil
        self._dmgChain = 0
        self._oldSide = 0
        self._cover = nil
        self._drawn = nil
        self._mLv = maxLv
        self._maXp = 15
        self._init = 0
        self._lv = 1
        self._xp = 0
        self._side = 0
        self._chainNr = 0
        self._delayTrn = 0
        self._events = nil
        self._items = items
        self._dmgRoles = nil
        self._skillsCrQty = nil
        self._skillsRgTurn = nil
        self._equipment = [EquipPos:Costume]()
        super.init(id: id, name: name, sprite: sprite, shapeShift: false, mActions: mActions, mDelayTrn: 0, dmgType: 0, rflType: 0,
                   hpDmg: mHp, mpDmg: mMp, spDmg: mSp, mHp: mHp, mMp: mMp, mSp: mSp, atk: atk, def: def, spi: spi, wis: wis, agi: agi,
                   stun: false, range: false, enrage: false, confuse: false, invincible: false, ko: false, revive: false, skills: nil,
                   counters: nil, states: nil, stRes: stRes, res: res)
        self.race = race
        self.job = job
    }
    
}
