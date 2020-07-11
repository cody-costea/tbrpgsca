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
use crate::scene::*;
use crate::play::*;

use std::collections::HashMap;

#[derive(Clone)]
pub struct Costume<'a> {
    pub(crate) base: Box<Role<'a>>,
    pub(crate) a_skills: Option<Vec<&'a Ability<'a>>>,
    pub(crate) counters: Option<Vec<&'a Ability<'a>>>,
    pub(crate) st_res: Option<HashMap<&'a State<'a>, i32>>,
    pub(crate) res: Option<HashMap<i32, i32>>,
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

    #[inline(always)]
    pub fn enraged(&self) -> bool {
        self.has_flag(Costume::FLAG_ENRAGED)
    }

    #[inline(always)]
    pub fn confused(&self) -> bool {
        self.has_flag(Costume::FLAG_CONFUSE)
    }

    #[inline(always)]
    pub fn shape_shifted(&self) -> bool {
        self.has_flag(Costume::FLAG_SHAPE_SHIFT)
    }

    #[inline(always)]
    pub fn invincible(&self) -> bool {
        self.has_flag(Costume::FLAG_INVINCIBLE)
    }

    #[inline(always)]
    pub fn stunned(&self) -> bool {
        self.has_flag(Costume::FLAG_STUN)
    }

    #[inline(always)]
    pub fn drawn(&self) -> bool {
        self.has_flag(Costume::FLAG_DRAW)
    }

    #[inline(always)]
    pub fn knocked_out(&self) -> bool {
        self.has_flag(Costume::FLAG_KO)
    }

    #[inline(always)]
    pub fn rfl_type(&self) -> i32 {
        self.rfl_type
    }

    #[inline(always)]
    pub fn cvr_type(&self) -> i32 {
        self.cvr_type
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
    pub fn a_skills(&self) -> Option<&'a Vec<&'a Ability>> {
        self.a_skills.as_ref().clone()
    }

    #[inline(always)]
    pub fn a_skills_mut(&mut self) -> Option<&'a mut Vec<&'a Ability>> {
        if let Some(v) = self.a_skills.as_mut() {
            Some(v)
        } else {
            None
        }
    }

    #[inline(always)]
    pub fn counters(&self) -> Option<&'a Vec<&'a Ability>> {
        self.counters.as_ref().clone()
    }

    #[inline(always)]
    pub fn counters_mut(&mut self) -> Option<&'a mut Vec<&'a Ability>> {
        if let Some(v) = self.counters.as_mut() {
            Some(v)
        } else {
            None
        }
    }

    #[inline(always)]
    pub fn res(&self) -> Option<&HashMap<i32, i32>> {
        self.res.as_ref().clone()
    }

    #[inline(always)]
    pub fn res_mut(&mut self) -> Option<&mut HashMap<i32, i32>> {
        if let Some(v) = self.res.as_mut() {
            Some(v)
        } else {
            None
        }
    }

    #[inline(always)]
    pub fn st_res(&self) -> Option<&HashMap<&'a State<'a>, i32>> {
        self.st_res.as_ref().clone()
    }

    #[inline(always)]
    pub fn st_res_mut(&mut self) -> Option<&mut HashMap<&'a State<'a>, i32>> {
        if let Some(v) = self.st_res.as_mut() {
            Some(v)
        } else {
            None
        }
    }

    pub(crate) fn refresh<'b>(&self, ret: &mut Option<&mut String>, scene: &mut Option<&mut dyn Scene>, actor: &'b mut Actor, upd_states: bool, remove: bool) {

    }

    pub(crate) fn adopt<'b>(&self, ret: &mut Option<&mut String>, scene: &mut Option<&mut dyn Scene>, actor: &'b mut Actor, upd_states: bool, remove: bool) {

    }

    pub(crate) fn apply(&self, ret: &mut Option<&mut String>, scene: &Option<&mut dyn Scene>, actor: &mut Actor) {
        
    }
    
}