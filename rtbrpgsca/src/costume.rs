/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
use crate::role::*;
use crate::actor::*;
use crate::ability::*;
use crate::state::*;

use std::collections::HashMap;

//#[derive(Clone, PartialEq, Eq, Hash)]
pub struct Costume<'a> {
    pub(crate) role: Box<Role<'a>>,
    pub(crate) a_skills: Option<&'a Vec<&'a Ability<'a>>>,
    pub(crate) counters: Option<&'a Vec<&'a Ability<'a>>>,
    pub(crate) st_res: Option<&'a HashMap<&'a State<'a>, i32>>,
    pub(crate) res: Option<&'a HashMap<i32, i32>>,
    pub(crate) m_actions: i32,
    pub(crate) cvr_type: i32,
    pub(crate) rfl_type: i32,
    pub(crate) atk: i32,
    pub(crate) def: i32,
    pub(crate) spi: i32,
    pub(crate) wis: i32,
    pub(crate) agi: i32,
}

extend_struct!(Costume, Role);

impl<'a> Costume<'a> {
    
    pub const CAUSES_TXT: &'static str = ", {0} is affected by {1}";

    pub const FLAG_ENRAGED: i32 = 4;
    pub const FLAG_CONFUSE: i32 = 8;
    pub const FLAG_CONVERT: i32 = 16;
    pub const FLAG_SHAPE_SHIFT: i32 = 32;
    pub const FLAG_INVINCIBLE: i32 = 64;
    pub const FLAG_DRAW: i32 = 128;
    pub const FLAG_STUN: i32 = 256;
    pub const FLAG_KO: i32 = 512;

    pub fn base_mut(&mut self) -> &mut Role<'a> {
        &mut (*self.role)
    }

    pub fn base(&self) -> &Role<'a> {
        &(*self.role)
    }

    pub fn enraged(&self) -> bool {
        (self.base().flags() & Costume::FLAG_ENRAGED) == Costume::FLAG_ENRAGED
    }

    pub fn confused(&self) -> bool {
        (self.base().flags() & Costume::FLAG_CONFUSE) == Costume::FLAG_CONFUSE
    }

    pub fn shape_shifted(&self) -> bool {
        (self.base().flags() & Costume::FLAG_SHAPE_SHIFT) == Costume::FLAG_SHAPE_SHIFT
    }

    pub fn invincible(&self) -> bool {
        (self.base().flags() & Costume::FLAG_INVINCIBLE) == Costume::FLAG_INVINCIBLE
    }

    pub fn stunned(&self) -> bool {
        (self.base().flags() & Costume::FLAG_STUN) == Costume::FLAG_STUN
    }

    pub fn drawn(&self) -> bool {
        (self.base().flags() & Costume::FLAG_DRAW) == Costume::FLAG_DRAW
    }

    pub fn knocked_out(&self) -> bool {
        (self.base().flags() & Costume::FLAG_KO) == Costume::FLAG_KO
    }

    pub fn atk(&self) -> i32 {
        self.atk
    }

    pub fn def(&self) -> i32 {
        self.def
    }

    pub fn wis(&self) -> i32 {
        self.wis
    }

    pub fn spi(&self) -> i32 {
        self.spi
    }

    pub fn agi(&self) -> i32 {
        self.agi
    }

    pub fn m_actions(&self) -> i32 {
        self.m_actions
    }

    pub fn a_skills(&self) -> &Option<&'a Vec<&'a Ability>> {
        &self.a_skills
    }

    pub fn counters(&self) -> &Option<&'a Vec<&'a Ability>> {
        &self.counters
    }

    pub fn res(&self) -> &Option<&'a HashMap<i32, i32>> {
        &self.res
    }

    pub fn st_res(&self) -> &Option<&'a HashMap<&'a State<'a>, i32>> {
        &self.st_res
    }

    pub fn refresh(&self, actor: Actor, upd_states: bool, remove: bool) {

    }

    pub fn adopt(&self, actor: Actor, upd_states: bool, remove: bool) {

    }

    pub fn apply(&self, ret: Option<&mut String>, actor: Actor) {
        
    }
    
}