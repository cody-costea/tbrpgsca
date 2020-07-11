/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
use crate::role::*;
use crate::state::*;
use crate::actor::*;
use crate::play::*;

extern crate rand;

use std::collections::HashMap;
use rand::Rng;

#[derive(Clone)]
pub struct Ability<'a> {
    pub(crate) base: Box<Role<'a>>,
    pub(crate) r_states: Option<HashMap<&'a State<'a>, i32>>,
    pub(crate) sound: Option<&'static str>,
    pub(crate) attr_inc: i32,
    pub(crate) lv_rq: i32,
    pub(crate) m_qty: i32,
    pub(crate) r_qty: i32,
}

extend_struct!(Ability, Role);

impl<'a> Ability<'a> {

    pub const MISSES_TXT: &'static str = ", but misses {}";
    pub const SUFFERS_TXT: &'static str = ", {} suffers";
    pub const REFLECT_TXT: &'static str = ", reflected by {}";
    pub const RESIST_TXT: &'static str = ", resisted by {}";
    pub const STOLEN_TXT: &'static str = ", obtaining {0} from {1}";

    pub const FLAG_MELEE: i32 = 4;
    pub const FLAG_STEAL: i32 = 8;
    pub const FLAG_ABSORB: i32 = 16;
    pub const FLAG_MISSABLE: i32 = 32;
    pub const FLAG_CRITICAL: i32 = 64;
    pub const FLAG_TRG_SIDE: i32 = 128;
    pub const FLAG_TRG_SELF: i32 = 256;
    pub const FLAG_TRG_ALL: i32 = 4;
    pub const FLAG_TRG_ONE: i32 = 0;
    pub const DEFAULT_RES: i32 = 3;

    #[inline(always)]
    pub fn lv_rq(&self) -> i32 {
        self.lv_rq
    }

    #[inline(always)]
    pub fn attr_inc(&self) -> i32 {
        self.attr_inc
    }

    #[inline(always)]
    pub fn only_melee(&mut self) -> bool {
        self.has_flag(Ability::FLAG_MELEE)
    }

    #[inline(always)]
    pub fn steals(&self) -> bool {
        self.has_flag(Ability::FLAG_STEAL)
    }

    #[inline(always)]
    pub fn absorbs(&self) -> bool {
        self.has_flag(Ability::FLAG_ABSORB)
    }

    #[inline(always)]
    pub fn can_miss(&self) -> bool {
        self.has_flag(Ability::FLAG_MISSABLE)
    }

    #[inline(always)]
    pub fn does_critical(&self) -> bool {
        self.has_flag(Ability::FLAG_CRITICAL)
    }

    #[inline(always)]
    pub fn r_states(&self) -> Option<&HashMap<&'a State<'a>, i32>> {
        if let Some(v) = self.r_states.as_ref() {
            Some(v)
        } else {
            None
        }
    }

    #[inline(always)]
    pub fn r_states_mut(&mut self) -> Option<&mut HashMap<&'a State<'a>, i32>> {
        if let Some(v) = self.r_states.as_mut() {
            Some(v)
        } else {
            None
        }
    }

    #[inline(always)]
    pub fn sound(&self) -> Option<&'static str> {
        self.sound
    }

    #[inline(always)]
    pub fn m_qty(&self) -> i32 {
        self.m_qty
    }

    #[inline(always)]
    pub fn r_qty(&self) -> i32 {
        self.r_qty
    }

    pub fn can_perform(&self, user: &Actor) -> bool {
        let mut has_qty = true;
        if let Some(skills_qty) = user.skills_cr_qty() {
            if let Some(&qty) = skills_qty.get(self) {
                if qty < 1 {
                    has_qty = false;
                }
            }
        }
        has_qty && self.m_mp() <= user.mp() && self.m_hp < user.hp() && self.m_sp() <= user.sp() && user.level() >= self.lv_rq()
    }

    pub fn execute<'b>(&self, ret: &mut String, user: &'b mut Actor<'b>, target: &'b mut Actor<'b>, apply_costs: bool) {
        let dmg_type = self.dmg_type() | user.dmg_type();
        unsafe {
            let usr = user as *mut Actor;
            let trg = if (dmg_type & target.rfl_type()) == 0 {
                ret.push_str(&*format!(", reflected by {}", target.name()));
                &mut (*usr)
            } else { target }; //as *mut Actor;
            let mut can_miss = if self.can_miss() { 4 } else { 0 };
            let mut trg_agi = trg.agi();
            let mut trg_spi = trg.spi();
            let mut usr_agi = user.agi();
            let mut usr_wis = user.wis();
            let mut dmg = 0;
            let mut def = 0;
            let mut i = 0;
            if (dmg_type & Role::DMG_TYPE_ATK) == Role::DMG_TYPE_ATK {
                dmg += user.atk();
                def += trg.def();
                i += 1;
            }
            if (dmg_type & Role::DMG_TYPE_DEF) == Role::DMG_TYPE_DEF {
                dmg += user.def();
                def += trg.def();
                i += 1;
            }
            if (dmg_type & Role::DMG_TYPE_SPI) == Role::DMG_TYPE_SPI {
                dmg += user.spi();
                def += trg.wis();
                i += 1;
            }
            if (dmg_type & Role::DMG_TYPE_WIS) == Role::DMG_TYPE_WIS {
                dmg += usr_wis;
                def += trg_spi;
                i += 1;
            }
            if (dmg_type & Role::DMG_TYPE_AGI) == Role::DMG_TYPE_AGI {
                dmg += usr_agi;
                def += trg_agi;
                if can_miss > 0 {
                    can_miss = 3;
                }
                i += 1;
            }
            trg_agi = ((trg_agi + trg_spi) / 2) / 3;
            usr_agi = (usr_agi + usr_wis) / 2;
            let mut rng = rand::thread_rng();
            let miss_factor = rng.gen_range(0, usr_agi / 2);
            if can_miss == 0 || usr == (trg as *mut Actor) || (miss_factor + (usr_agi / miss_factor)) > trg_agi - (rng.gen_range(0, trg_agi)) {
                if self.does_critical() && miss_factor > (trg_agi * 2) + rng.gen_range(0, trg_agi) {
                    dmg = (dmg * 2) + (dmg / 2);
                }
                
            }
        }
    }

    pub fn replenish(&'a self, user: &'a mut Actor<'a>) {
        let m_qty = self.m_qty();
        if m_qty > 0 {
            if user.skills_cr_qty().is_none() {
                user.skills_cr_qty = Some(HashMap::new());
            }
            if let Some(skills_cr_qty) = user.skills_cr_qty_mut() {
                skills_cr_qty.insert(self, m_qty);
            }
        }
    }

    fn new(id: i32, name: &'static str, sprite: Option<&'static str>, sound: Option<&'static str>, steal: bool, range: bool, melee: bool, can_miss: bool,
           critical: bool, lv_rq: i32, m_hp: i32, m_mp: i32, m_sp: i32, dmg_type: i32, attr_inc: i32, hp: i32, mp: i32, sp: i32, trg: i32, elm: i32, m_qty: i32,
           r_qty: i32, absorb: bool, revive: bool, a_states: Option<HashMap<&'a State<'a>, i32>>, r_states: Option<HashMap<&'a State<'a>, i32>>) -> Ability<'a> {
        let mut role = Box::new(Role::new(id, name, sprite, m_hp, m_mp, m_sp, hp, mp, sp, dmg_type | elm, range, revive, a_states));
        let mut flags = role.flags();
        if steal {
            flags |= Ability::FLAG_STEAL;
        }
        if melee {
            flags |= Ability::FLAG_MELEE;
        }
        if can_miss {
            flags |= Ability::FLAG_MISSABLE;
        }
        if critical {
            flags |= Ability::FLAG_CRITICAL;
        }
        if absorb {
            flags |= Ability::FLAG_ABSORB;
        }
        (*role).flags = flags;
        Ability {
            base: role,
            lv_rq: lv_rq,
            attr_inc: attr_inc,
            r_states: r_states,
            sound: sound,
            m_qty: m_qty,
            r_qty: r_qty
        }
    }

}