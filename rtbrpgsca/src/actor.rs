/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
use crate::scene::*;
use crate::ability::*;
use crate::costume::*;
use crate::state::*;

use std::rc::Rc;
use std::any::Any;
use std::collections::HashMap;

pub type DelayAct = dyn FnMut(&mut Actor, bool);

#[derive(Hash, Copy, Clone, PartialEq, Eq)]
pub enum EquipPos {
    Race, Job, Arms, Chest, Weapon, Shield, Head, Legs, Feet, Belt, Ring1,
    Ring2, Ring3, Ring4, Ring5, Ring6, Ring7, Ring8, Necklace, Mantle
}

//#[derive(Clone, PartialEq, Eq, Hash)]
pub struct Actor<'a> {
    pub(crate) costume: Box<Costume<'a>>,
    pub(crate) equipment: HashMap<EquipPos, &'a Costume<'a>>,
    pub(crate) skills_cr_qty: Option<HashMap<&'a Ability<'a>, i32>>,
    pub(crate) skills_rg_trn: Option<HashMap<&'a Ability<'a>, i32>>,
    pub(crate) items: Option<Rc<&'a HashMap<&'a Ability<'a>, i32>>>,
    pub(crate) dmg_roles: Option<Vec<&'a Costume<'a>>>,
    pub(crate) delay_act: Option<&'a DelayAct>,
    pub(crate) drawn_by: Option<&'a Actor<'a>>,
    pub(crate) m_delay_trn: i32,
    pub(crate) delay_trn: i32,
    pub(crate) dmg_chain: i32,
    pub(crate) chain_nr: i32,
    pub(crate) old_side: i32,
    pub(crate) actions: i32,
    pub(crate) max_lv: i32,
    pub(crate) cr_lv: i32,
    pub(crate) init: i32,
    pub(crate) side: i32,
    pub(crate) maxp: i32,
    pub(crate) xp: i32,
}

impl<'a> Actor<'a> {

    pub const FLAG_SURVIVES: i32 = 1024;
    pub const FLAG_RANDOM_AI: i32 = 2048;
    pub const FLAG_AI_PLAYER: i32 = 4096;

    pub fn costume(&self) -> &Costume<'a> {
        &(*self.costume)
    }

    pub fn costume_mut(&mut self) -> &mut Costume<'a> {
        &mut (*self.costume)
    }

    pub fn survives(&self) -> bool {
        (self.costume().role().flags() & Actor::FLAG_SURVIVES) == Actor::FLAG_SURVIVES
    }

    pub fn random_ai(&self) -> bool {
        (self.costume().role().flags() & Actor::FLAG_RANDOM_AI) == Actor::FLAG_RANDOM_AI
    }

    pub fn ai_player(&self) -> bool {
        (self.costume().role().flags() & Actor::FLAG_AI_PLAYER) == Actor::FLAG_AI_PLAYER
    }

    pub fn items(&self) -> &Option<Rc<&'a HashMap<&'a Ability<'a>, i32>>> {
        &self.items
    }

    pub fn delay_act(&self) -> &Option<&'a DelayAct> {
        &self.delay_act
    }

    pub fn dmg_roles(&self) -> &Option<Vec<&'a Costume<'a>>> {
        &self.dmg_roles
    }

    pub fn drawn_by(&self) -> &Option<&'a Actor<'a>> {
        &self.drawn_by
    }

    pub(crate) fn old_side(&self) -> i32 {
        self.old_side
    }

    pub fn delay_trn(&self) -> i32 {
        self.delay_trn()
    }

    pub fn dmg_chain(&self) -> i32 {
        self.dmg_chain
    }

    pub fn chain_nr(&self) -> i32 {
        self.chain_nr
    }

    pub fn actions(&self) -> i32 {
        self.actions
    }

    pub fn max_exp(&self) -> i32 {
        self.maxp
    }

    pub fn max_level(&self) -> i32 {
        self.max_lv
    }

    pub fn level(&self) -> i32 {
        self.cr_lv
    }

    pub fn init(&self) -> i32 {
        self.init
    }

    pub fn side(&self) -> i32 {
        self.side
    }

    pub fn exp(&self) -> i32 {
        self.xp
    }

    pub fn job(&self) -> &Costume {
        self.equipment.get(&EquipPos::Race).unwrap()
    }

    pub fn race(&self) -> &Costume {
        self.equipment.get(&EquipPos::Race).unwrap()
    }

    pub(crate) fn set_flags(&mut self, val: i32) {
        self.costume_mut().role_mut().flags = val;
    }

    pub fn set_atk(&mut self, val: i32) {
        self.costume_mut().atk = val;
    }

    pub fn set_def(&mut self, val: i32) {
        self.costume_mut().def = val;
    }

    pub fn set_spi(&mut self, val: i32) {
        self.costume_mut().spi = val;
    }

    pub fn set_wis(&mut self, val: i32) {
        self.costume_mut().wis = val;
    }

    pub fn set_agi(&mut self, val: i32) {
        self.costume_mut().agi = val;
    }

    pub fn set_side(&mut self, val: i32) {
        self.side = val;
    }

    pub fn set_race(&mut self, val: &'a Costume) {

    }

    pub fn set_job(&mut self, val: &'a Costume) {
        
    }

    pub fn set_m_actions(&mut self, val: i32) {
        if val > 0 {
            let costume = &mut self.costume_mut();            
            costume.m_actions = val;
            if val < self.actions() {
                self.actions = val;
            }
        }
    }

    pub fn set_m_hp(&mut self, val: i32) {
        if val > 0 {
            let role = &mut self.costume_mut().role_mut();
            role.m_hp = val;
            if val < role.hp() {
                role.hp = val;
            }
        }
    }

    pub fn set_m_mp(&mut self, val: i32) {
        if val > 0 {
            let role = &mut self.costume_mut().role_mut();
            role.m_mp = val;
            if val < role.mp() {
                role.mp = val;
            }
        }
    }

    pub fn set_m_sp(&mut self, val: i32) {
        if val > 0 {
            let role = &mut self.costume_mut().role_mut();
            role.m_sp = val;
            if val < role.sp() {
                role.sp = val;
            }
        }
    }

    pub(crate) fn set_hp_scene(&mut self, val: i32, ret: &'a mut String, scene: &'a mut dyn Scene) {
        if val < 1 {
            if self.hp != 0 {
                /*if self.survives() || self.invincible() {

                }*/
            }
        } else {

        }
    }

    pub fn set_hp(&mut self, val: i32) {
        let role = &mut self.costume_mut().role_mut();
        let m_hp = role.m_hp();
        role.hp = if val > m_hp { m_hp } else { val };
    }

    pub fn set_mp(&mut self, val: i32) {
        let role = &mut self.costume_mut().role_mut();
        let m_mp = role.m_mp();
        role.mp = if val > m_mp { m_mp } else { val };
    }

    pub fn set_sp(&mut self, val: i32) {
        let role = &mut self.costume_mut().role_mut();
        let m_sp = role.m_sp();
        role.mp = if val > m_sp { m_sp } else { val };
    }

    pub fn set_name(&mut self, val: &'static str) {
        self.costume_mut().role_mut().name = val;
    }

    pub fn set_sprite(&mut self, val: &Option<&'static str>) {
        self.costume_mut().role_mut().sprite = if let Some(v) = val {
            Some(v)
        } else {
            None
        };
    }

    pub fn set_delay_act(&mut self, val: &Option<&'a DelayAct>) {
        self.delay_act = if let v = val.unwrap() {
            Some(v)
        } else {
            None
        };
    }

    pub fn set_delay_trn(&mut self, val: i32) {
        self.delay_trn = val;
    }

    pub fn set_dmg_chain(&mut self, val: i32) {
        self.dmg_chain = val;
    }

    pub fn set_chain_nr(&mut self, val: i32) {
        self.chain_nr = val;
    }

    pub fn set_max_level(&mut self, val: i32) {
        self.max_lv = val;
    }

    pub(crate) fn set_level_scene(&mut self, val: i32, scene: &Option<&'a mut dyn Scene>) {
        while val > self.level() {
            self.xp = self.maxp;
            self.level_up(&scene)
        }
        self.cr_lv = val;
    }

    pub fn set_level(&mut self, val: i32) {
        self.set_level_scene(val, &None);
    }

    pub fn set_max_exp(&mut self, val: i32) {
        
    }    

    pub fn set_exp(&mut self, val: i32) {
        
    }

    pub fn level_up(&mut self, scene: &Option<&'a mut dyn Scene>) {

    }
    
}

impl<'a> std::ops::Deref for Actor<'a> {

    type Target = Costume<'a>;
    fn deref(&self) -> &Costume<'a> {
        self.costume()
    }

}

impl<'a> std::ops::DerefMut for Actor<'a> {

    fn deref_mut(&mut self) -> &mut Costume<'a> {
        self.costume_mut()
    }

}