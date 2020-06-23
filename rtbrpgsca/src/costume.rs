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
    pub(crate) base: Box<Role<'a>>,
    pub(crate) a_skills: Option<&'a mut Vec<&'a Ability<'a>>>,
    pub(crate) counters: Option<&'a mut Vec<&'a Ability<'a>>>,
    pub(crate) st_res: Option<&'a mut HashMap<&'a State<'a>, i32>>,
    pub(crate) res: Option<&'a mut HashMap<i32, i32>>,
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
    
    #[inline(always)] pub const CAUSES_TXT: &'static str = ", {0} is affected by {1}";

    #[inline(always)] pub const FLAG_ENRAGED: i32 = 4;
    #[inline(always)] pub const FLAG_CONFUSE: i32 = 8;
    #[inline(always)] pub const FLAG_CONVERT: i32 = 16;
    #[inline(always)] pub const FLAG_SHAPE_SHIFT: i32 = 32;
    #[inline(always)] pub const FLAG_INVINCIBLE: i32 = 64;
    #[inline(always)] pub const FLAG_DRAW: i32 = 128;
    #[inline(always)] pub const FLAG_STUN: i32 = 256;
    #[inline(always)] pub const FLAG_KO: i32 = 512;

    #[inline(always)]
    pub fn enraged(&self) -> bool {
        (self.base().flags() & Costume::FLAG_ENRAGED) == Costume::FLAG_ENRAGED
    }

    #[inline(always)]
    pub fn confused(&self) -> bool {
        (self.base().flags() & Costume::FLAG_CONFUSE) == Costume::FLAG_CONFUSE
    }

    #[inline(always)]
    pub fn shape_shifted(&self) -> bool {
        (self.base().flags() & Costume::FLAG_SHAPE_SHIFT) == Costume::FLAG_SHAPE_SHIFT
    }

    #[inline(always)]
    pub fn invincible(&self) -> bool {
        (self.base().flags() & Costume::FLAG_INVINCIBLE) == Costume::FLAG_INVINCIBLE
    }

    #[inline(always)]
    pub fn stunned(&self) -> bool {
        (self.base().flags() & Costume::FLAG_STUN) == Costume::FLAG_STUN
    }

    #[inline(always)]
    pub fn drawn(&self) -> bool {
        (self.base().flags() & Costume::FLAG_DRAW) == Costume::FLAG_DRAW
    }

    #[inline(always)]
    pub fn knocked_out(&self) -> bool {
        (self.base().flags() & Costume::FLAG_KO) == Costume::FLAG_KO
    }

    #[inline(always)]
    pub fn atk(&self) -> i32 {
        self.atk
    }

    #[inline(always)]
    pub fn def(&self) -> i32 {
        self.def
    }

    #[inline(always)]
    pub fn wis(&self) -> i32 {
        self.wis
    }

    #[inline(always)]
    pub fn spi(&self) -> i32 {
        self.spi
    }

    #[inline(always)]
    pub fn agi(&self) -> i32 {
        self.agi
    }

    #[inline(always)]
    pub fn m_actions(&self) -> i32 {
        self.m_actions
    }

    #[inline(always)]
    pub fn a_skills(&mut self) -> &mut Option<&'a mut Vec<&'a Ability>> {
        &mut self.a_skills
    }

    #[inline(always)]
    pub fn counters(&mut self) -> &mut Option<&'a mut Vec<&'a Ability>> {
        &mut self.counters
    }

    #[inline(always)]
    pub fn res(&mut self) -> &mut Option<&'a mut HashMap<i32, i32>> {
        &mut self.res
    }

    #[inline(always)]
    pub fn st_res(&mut self) -> &mut Option<&'a mut HashMap<&'a State<'a>, i32>> {
        &mut self.st_res
    }

    pub fn refresh<'b>(&self, actor: &'b mut Actor, upd_states: bool, remove: bool) {

    }

    pub fn adopt<'b>(&self, actor: &'b mut Actor, upd_states: bool, remove: bool) {

    }

    pub fn apply(&self, ret: &mut Option<&mut String>, actor: &mut Actor) {
        
    }
    
}