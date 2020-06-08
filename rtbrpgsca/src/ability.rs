/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
use crate::role::*;
use crate::state::*;
use crate::actor::*;

use std::collections::HashMap;

//#[derive(Clone, PartialEq, Eq, Hash)]
pub struct Ability<'a> {
    pub(crate) role: Box<Role<'a>>,
    pub(crate) r_states: Option<&'a HashMap<&'a State<'a>, i32>>,
    pub(crate) sound: Option<&'static str>,
    pub(crate) attr_inc: i32,
    pub(crate) lv_rq: i32,
    pub(crate) m_qty: i32,
    pub(crate) r_qty: i32,
}

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

    pub fn role_mut(&mut self) -> &mut Role<'a> {
        &mut (*self.role)
    }
    
    pub fn role(&self) -> &Role<'a> {
        &(*self.role)
    }

    pub fn lv_rq(&self) -> i32 {
        self.lv_rq
    }

    pub fn attr_inc(&self) -> i32 {
        self.attr_inc
    }

    pub fn only_melee(&mut self) -> bool {
        (self.role().flags() & Ability::FLAG_MELEE) == Ability::FLAG_MELEE
    }

    pub fn steals(&self) -> bool {
        (self.role().flags() & Ability::FLAG_STEAL) == Ability::FLAG_STEAL
    }

    pub fn absorbs(&self) -> bool {
        (self.role().flags() & Ability::FLAG_ABSORB) == Ability::FLAG_ABSORB
    }

    pub fn can_miss(&self) -> bool {
        (self.role().flags() & Ability::FLAG_MISSABLE) == Ability::FLAG_MISSABLE
    }

    pub fn does_critical(&self) -> bool {
        (self.role().flags() & Ability::FLAG_CRITICAL) == Ability::FLAG_CRITICAL
    }

    pub fn r_states(&self) -> &Option<&'a HashMap<&'a State<'a>, i32>> {
        &self.r_states
    }

    pub fn sound(&self) -> Option<&'static str> {
        self.sound
    }

    pub fn m_qty(&self) -> i32 {
        self.m_qty
    }

    pub fn r_qty(&self) -> i32 {
        self.r_qty
    }

    pub fn can_perform(&self, user: &Actor) -> bool {
        true
    }

    pub fn execute(&self, ret: &mut String, user: &mut Actor, target: &mut Actor, apply_costs: bool) -> bool {
        true
    }

    pub fn replenish(&self, user: &mut Actor) -> bool {
        true
    }

    fn new(id: i32, name: &'static str, sprite: Option<&'static str>, sound: Option<&'static str>, steal: bool, range: bool, melee: bool, can_miss: bool,
           critical: bool, lv_rq: i32, m_hp: i32, m_mp: i32, m_sp: i32, dmg_type: i32, attr_inc: i32, hp: i32, mp: i32, sp: i32, trg: i32, elm: i32, m_qty: i32,
           r_qty: i32, absorb: bool, revive: bool, a_states: Option<&'a HashMap<&'a State<'a>, i32>>, r_states: Option<&'a HashMap<&'a State<'a>, i32>>) -> Ability<'a> {
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
            role: role,
            lv_rq: lv_rq,
            attr_inc: attr_inc,
            r_states: r_states,
            sound: sound,
            m_qty: m_qty,
            r_qty: r_qty
        }
    }

}

impl<'a> std::ops::Deref for Ability<'a> {

    type Target = Role<'a>;
    fn deref(&self) -> &Role<'a> {
        self.role()
    }

}

impl<'a> std::ops::DerefMut for Ability<'a> {

    fn deref_mut(&mut self) -> &mut Role<'a> {
        self.role_mut()
    }

}