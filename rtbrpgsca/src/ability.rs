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

use std::collections::HashMap;

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

    pub fn execute(&self, ret: &mut String, user: &mut Actor, target: &mut Actor, apply_costs: bool) -> bool {
        true
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