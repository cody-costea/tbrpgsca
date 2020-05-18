/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

import Foundation

open class State : Costume {
    
    public static let STATE_END_DUR = -3
    
    internal var _dur: Int, _sRes: Int, _rSkills: [Ability]?
    
    open var dur: Int {
        return self._dur
    }
    
    open var sRes: Int {
        return self._sRes
    }
    
    open var rSkills: [Ability]? {
        return self._rSkills
    }
    
    open func remove(actor: Actor) {
        self.blockSkills(actor: actor, remove: true)
        self.adopt(actor: actor, updStates: false, remove: true)
        if self.converted && !actor.converted {
            actor.side = actor._oldSide
        }
        if self.covered && !actor.covered {
            actor.coveredBy = nil
        }
        if self.drawn && !actor.drawn {
            actor.drawnBy = nil
        }
    }
    
    open func blockSkills(actor: Actor, remove: Bool) {
        if let rSkills = self.rSkills {
            var iSkills: [Ability : Int]! = actor._skillsCrQty
            if remove {
                if iSkills != nil {
                    for skill in rSkills {
                        if skill.mQty > 0 {
                            iSkills[skill] = -1 * (iSkills[skill] ?? 0)
                        } else {
                            iSkills.removeValue(forKey: skill)
                        }
                    }
                }
            } else {
                if iSkills == nil {
                    iSkills = [Ability : Int]()
                    actor._skillsCrQty = iSkills
                }
                for skill in rSkills {
                    iSkills[skill] = skill.mQty > 0 ? -1 * (iSkills[skill] ?? 0) : 0
                }
            }
        }
    }
    
    open func alter(ret: inout String?, actor: Actor, consume: Bool) {
        if var sDur = actor.stateDur {
            let d = sDur[self] ?? State.STATE_END_DUR
            if consume {
                if d > 0 {
                    sDur[self] = d - 1
                }
            } else if dur == 0 {
                sDur[self] = State.STATE_END_DUR
                self.remove(actor: actor)
            }
        }
    }
    
    open func disable(actor: Actor, dur: Int, remove: Bool) -> Bool {
        let d = dur == 0 ? self.dur : dur
        if d > State.STATE_END_DUR {
            if var sDur = actor.stateDur {
                let crDur = sDur[self] ?? State.STATE_END_DUR
                if crDur > -1 || d <= crDur {
                    if d > 0 && crDur > d {
                        sDur[self] = crDur - d
                        return false
                    } else {
                        if remove {
                            sDur.removeValue(forKey: self)
                        } else {
                            sDur[self] = State.STATE_END_DUR
                        }
                        if crDur > State.STATE_END_DUR {
                            self.remove(actor: actor)
                        }
                        return true
                    }
                }
                return crDur <= State.STATE_END_DUR
            } else {
                return true
            }
        } else {
            return false
        }
    }
    
    open func inflict(user: Actor!, target: Actor, dur: Int, always: Bool) {
        var d: Int
        if dur == 0 {
            d = self.dur
            if d == 0 {
                return
            }
        } else {
            d = dur
        }
        if d > State.STATE_END_DUR {
            if !always {
                var stateRes = self.sRes
                if stateRes > -1 {
                    let stRes: [State? : Int]! = target.stRes
                    stateRes = stRes == nil ? 0 : (stRes[self] ?? 0) + (stRes[nil] ?? 0) + stateRes
                    if Int.random(in: 0..<10) < stateRes {
                        return
                    }
                }
            }
            var trgStates: [State : Int]! = target.stateDur
            if trgStates == nil {
                trgStates = [State : Int]()
                target.stateDur = trgStates
            } else if let rStates = self.stateDur {
                for (rState, var rDur) in rStates {
                    if rDur == 0 || rDur <= State.STATE_END_DUR {
                        rDur = rState.dur
                    }
                    for (aState, aDur) in trgStates {
                        if aState == rState {
                            if aDur > State.STATE_END_DUR {
                                if aDur < 0 && rDur > aDur {
                                    return
                                }
                                rState.disable(actor: target, dur: rDur, remove: false)
                                if rDur > 0 && aDur > 0 {
                                    d -= aDur < rDur ? aDur : rDur
                                    if d < 1 {
                                        return
                                    }
                                }
                            }
                        }
                    }
                }
            }
            self.blockSkills(actor: target, remove: false)
            let crDur = trgStates[self] ?? State.STATE_END_DUR
            if crDur == State.STATE_END_DUR {
                self.adopt(actor: target, updStates: false, remove: false)
                trgStates[self] = d
            } else if (crDur > -1 && crDur < d) || (d < 0 && d < crDur) {
                trgStates[self] = d
            }
            if user != nil {
                if self.converted && target.side != user.side {
                    target.side = user.side
                }
                if self.covered {
                    target.coveredBy = user
                }
                if self.drawn {
                    target.drawnBy = user
                }
            }
        }
    }
    
    init(id: Int, name: String, sprite: String?, shapeShift: Bool, dur: Int, sRes: Int, mActions: Int, dmgType: Int,
         rflType: Int, hpDmg: Int, mpDmg: Int, spDmg: Int, mHp: Int, mMp: Int, mSp: Int, atk: Int, def: Int, spi: Int,
         wis: Int, agi: Int, stun: Bool, range: Bool, enrage: Bool, confuse: Bool, convert: Bool, reflect: Bool,
         invincible: Bool, cover: Bool, draw: Bool, ko: Bool, revive: Bool, aSkills: [Ability]?, rSkills: [Ability]?,
         counters: [Ability]?, states: [State: Int]?, stRes: [State: Int]?, res: [Int: Int]?) {
        self._dur = dur
        self._sRes = sRes
        self._rSkills = rSkills
        super.init(id: id, name: name, sprite: sprite, shapeShift: shapeShift, mActions: mActions, dmgType: dmgType, rflType: rflType,
                   hpDmg: hpDmg, mpDmg: mpDmg, spDmg: spDmg, mHp: mHp, mMp: mMp, mSp: mSp, atk: atk, def: def, spi: spi, wis: wis,
                   agi: agi, stun: stun, range: range, enrage: enrage, confuse: confuse, invincible: invincible, ko: ko, revive: revive,
                   skills: aSkills, counters: counters, states: states, stRes: stRes, res: res)
        var flags = self._flags
        if convert {
            flags |= State.FLAG_CONVERT
        }
        if cover {
            flags |= State.FLAG_COVER
        }
        if draw {
            flags |= State.FLAG_DRAW
        }
        self._flags = flags
    }
    
}
