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
use crate::role::*;

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
    pub(crate) base: Box<Costume<'a>>,
    pub(crate) equipment: HashMap<EquipPos, &'a Costume<'a>>,
    pub(crate) skills_cr_qty: Option<HashMap<&'a Ability<'a>, i32>>,
    pub(crate) skills_rg_trn: Option<HashMap<&'a Ability<'a>, i32>>,
    pub(crate) items: Option<Rc<&'a HashMap<&'a Ability<'a>, i32>>>,
    pub(crate) dmg_roles: Option<&'a Vec<&'a Costume<'a>>>,
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

extend_struct!(Actor, Costume);

impl<'a> Actor<'a> {

    #[inline(always)] pub const FLAG_SURVIVES: i32 = 1024;
    #[inline(always)] pub const FLAG_RANDOM_AI: i32 = 2048;
    #[inline(always)] pub const FLAG_AI_PLAYER: i32 = 4096;

    #[inline(always)]
    pub fn survives(&self) -> bool {
        (self.base().base().flags() & Actor::FLAG_SURVIVES) == Actor::FLAG_SURVIVES
    }

    #[inline(always)]
    pub fn random_ai(&self) -> bool {
        (self.base().base().flags() & Actor::FLAG_RANDOM_AI) == Actor::FLAG_RANDOM_AI
    }

    #[inline(always)]
    pub fn ai_player(&self) -> bool {
        (self.base().base().flags() & Actor::FLAG_AI_PLAYER) == Actor::FLAG_AI_PLAYER
    }

    #[inline(always)]
    pub fn items(&self) -> &Option<Rc<&'a HashMap<&'a Ability<'a>, i32>>> {
        &self.items
    }

    #[inline(always)]
    pub fn delay_act(&self) -> &Option<&'a DelayAct> {
        &self.delay_act
    }

    #[inline(always)]
    pub fn dmg_roles(&self) -> &Option<&'a Vec<&'a Costume<'a>>> {
        &self.dmg_roles
    }

    #[inline(always)]
    pub fn drawn_by(&self) -> &Option<&'a Actor<'a>> {
        &self.drawn_by
    }

    #[inline(always)]
    pub(crate) fn old_side(&self) -> i32 {
        self.old_side
    }

    #[inline(always)]
    pub fn delay_trn(&self) -> i32 {
        self.delay_trn()
    }

    #[inline(always)]
    pub fn dmg_chain(&self) -> i32 {
        self.dmg_chain
    }

    #[inline(always)]
    pub fn chain_nr(&self) -> i32 {
        self.chain_nr
    }

    #[inline(always)]
    pub fn actions(&self) -> i32 {
        self.actions
    }

    #[inline(always)]
    pub fn max_exp(&self) -> i32 {
        self.maxp
    }

    #[inline(always)]
    pub fn max_level(&self) -> i32 {
        self.max_lv
    }

    #[inline(always)]
    pub fn level(&self) -> i32 {
        self.cr_lv
    }

    #[inline(always)]
    pub fn init(&self) -> i32 {
        self.init
    }

    #[inline(always)]
    pub fn side(&self) -> i32 {
        self.side
    }

    #[inline(always)]
    pub fn exp(&self) -> i32 {
        self.xp
    }

    #[inline(always)]
    pub fn job(&self) -> &'a Costume {
        self.equipment.get(&EquipPos::Job).unwrap()
    }

    #[inline(always)]
    pub fn race(&self) -> &'a Costume {
        self.equipment.get(&EquipPos::Race).unwrap()
    }

    #[inline]
    pub(crate) fn set_flags(&mut self, val: i32) {
        self.base_mut().base_mut().flags = val;
    }

    #[inline]
    pub fn set_random_ai(&mut self, val: bool) {
        let role = self.base_mut().base_mut();
        let flags = role.flags();
        if val != ((flags & Actor::FLAG_RANDOM_AI) == Actor::FLAG_RANDOM_AI) {
            role.flags = flags ^ Actor::FLAG_RANDOM_AI;
        }
    }

    #[inline]
    pub fn set_ai_player(&mut self, val: bool) {
        let role = self.base_mut().base_mut();
        let flags = role.flags();
        if val != ((flags & Actor::FLAG_AI_PLAYER) == Actor::FLAG_AI_PLAYER) {
            role.flags = flags ^ Actor::FLAG_AI_PLAYER;
        }
    }

    #[inline]
    pub fn set_survives(&mut self, val: bool) {
        let role = self.base_mut().base_mut();
        let flags = role.flags();
        if val != ((flags & Actor::FLAG_SURVIVES) == Actor::FLAG_SURVIVES) {
            role.flags = flags ^ Actor::FLAG_SURVIVES;
        }
    }

    #[inline]
    pub fn set_stunned(&mut self, val: bool) {
        let costume = self.base_mut();
        let flags = costume.flags();
        if val != ((flags & Costume::FLAG_STUN) == Costume::FLAG_STUN) {
            costume.flags = flags ^ Costume::FLAG_STUN;
        }
    }

    #[inline]
    pub fn set_shape_shifted(&mut self, val: bool) {
        let costume = self.base_mut();
        let flags = costume.flags();
        if val != ((flags & Costume::FLAG_SHAPE_SHIFT) == Costume::FLAG_SHAPE_SHIFT) {
            costume.flags = flags ^ Costume::FLAG_SHAPE_SHIFT;
        }
    }
    
    #[inline]
    pub fn set_knocked_out(&mut self, val: bool) {
        let costume = self.base_mut();
        let flags = costume.flags();
        if val != ((flags & Costume::FLAG_KO) == Costume::FLAG_KO) {
            costume.flags = flags ^ Costume::FLAG_KO;
        }
    }

    #[inline]
    pub fn set_invincible(&mut self, val: bool) {
        let costume = self.base_mut();
        let flags = costume.flags();
        if val != ((flags & Costume::FLAG_SHAPE_SHIFT) == Costume::FLAG_SHAPE_SHIFT) {
            costume.flags = flags ^ Costume::FLAG_SHAPE_SHIFT;
        }
    }

    #[inline]
    pub fn set_confused(&mut self, val: bool) {
        let costume = self.base_mut();
        let flags = costume.flags();
        if val != ((flags & Costume::FLAG_CONFUSE) == Costume::FLAG_CONFUSE) {
            costume.flags = flags ^ Costume::FLAG_CONFUSE;
        }
    }

    #[inline]
    pub fn set_enraged(&mut self, val: bool) {
        let costume = self.base_mut();
        let flags = costume.flags();
        if val != ((flags & Costume::FLAG_ENRAGED) == Costume::FLAG_ENRAGED) {
            costume.flags = flags ^ Costume::FLAG_ENRAGED;
        }
    }

    #[inline]
    pub fn set_converted(&mut self, val: bool) {
        let costume = self.base_mut();
        let flags = costume.flags();
        if val != ((flags & Costume::FLAG_CONVERT) == Costume::FLAG_CONVERT) {
            costume.flags = flags ^ Costume::FLAG_CONVERT;
        }
    }

    #[inline]
    pub fn set_drawn(&mut self, val: bool) {
        let costume = self.base_mut();
        let flags = costume.flags();
        if val != ((flags & Costume::FLAG_DRAW) == Costume::FLAG_DRAW) {
            costume.flags = flags ^ Costume::FLAG_DRAW;
        }
    }

    #[inline]
    pub fn set_ranged(&mut self, val: bool) {
        let role = self.base_mut().base_mut();
        let flags = role.flags();
        if val != ((flags & Role::FLAG_RANGE) == Role::FLAG_RANGE) {
            role.flags = flags ^ Role::FLAG_RANGE;
        }
    }

    #[inline]
    pub fn set_revives(&mut self, val: bool) {
        let role = self.base_mut().base_mut();
        let flags = role.flags();
        if val != ((flags & Role::FLAG_REVIVE) == Role::FLAG_REVIVE) {
            role.flags = flags ^ Role::FLAG_REVIVE;
        }
    }

    #[inline(always)]
    pub fn set_atk(&mut self, val: i32) {
        self.base_mut().atk = val;
    }

    #[inline(always)]
    pub fn set_def(&mut self, val: i32) {
        self.base_mut().def = val;
    }

    #[inline(always)]
    pub fn set_spi(&mut self, val: i32) {
        self.base_mut().spi = val;
    }

    #[inline(always)]
    pub fn set_wis(&mut self, val: i32) {
        self.base_mut().wis = val;
    }

    #[inline(always)]
    pub fn set_agi(&mut self, val: i32) {
        self.base_mut().agi = val;
    }

    #[inline]
    pub fn set_agi_scene(&mut self, val: i32, scene: &mut Option<&'a mut dyn Scene>) {
        self.set_agi(val);
        if let Some(s) = scene {
            s.agi_calc();
        }
    }

    #[inline(always)]
    pub fn set_side(&mut self, val: i32) {
        self.side = val;
    }

    pub(crate) fn set_race_scene(&mut self, ret: &mut Option<&'a mut String>, scene: &mut Option<&'a mut dyn Scene>, val: &'a Costume) {

    }

    pub(crate) fn set_job_scene(&mut self, ret: &mut Option<&'a mut String>, scene: &mut Option<&'a mut dyn Scene>, val: &'a Costume) {
        
    }

    #[inline]
    pub fn set_race(&mut self, val: &'a Costume) {

    }

    #[inline]
    pub fn set_job(&mut self, val: &'a Costume) {
        
    }

    #[inline]
    pub fn set_m_actions(&mut self, val: i32) {
        if val > 0 {
            let costume = self.base_mut();            
            costume.m_actions = val;
            if val < self.actions() {
                self.actions = val;
            }
        }
    }

    #[inline]
    pub fn set_m_hp(&mut self, val: i32) {
        if val > 0 {
            let role = self.base_mut().base_mut();
            role.m_hp = val;
            if val < role.hp() {
                role.hp = val;
            }
        }
    }

    #[inline]
    pub fn set_m_mp(&mut self, val: i32) {
        if val > 0 {
            let role = self.base_mut().base_mut();
            role.m_mp = val;
            if val < role.mp() {
                role.mp = val;
            }
        }
    }

    #[inline]
    pub fn set_m_sp(&mut self, val: i32) {
        if val > 0 {
            let role = self.base_mut().base_mut();
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

    #[inline]
    pub fn set_hp(&mut self, val: i32) {
        let role = self.base_mut().base_mut();
        let m_hp = role.m_hp();
        role.hp = if val > m_hp { m_hp } else { val };
    }

    #[inline]
    pub fn set_mp(&mut self, val: i32) {
        let role = self.base_mut().base_mut();
        let m_mp = role.m_mp();
        role.mp = if val > m_mp { m_mp } else { val };
    }

    #[inline]
    pub fn set_sp(&mut self, val: i32) {
        let role = &mut self.base_mut().base_mut();
        let m_sp = role.m_sp();
        role.mp = if val > m_sp { m_sp } else { val };
    }

    #[inline(always)]
    pub fn set_name(&mut self, val: &'static str) {
        self.base_mut().base_mut().name = val;
    }

    #[inline(always)]
    pub fn set_sprite(&mut self, val: &Option<&'static str>) {
        self.base_mut().base_mut().sprite = if let Some(v) = val {
            Some(v)
        } else {
            None
        };
    }

    #[inline(always)]
    pub fn set_delay_act(&mut self, val: &Option<&'a DelayAct>) {
        self.delay_act = if let Some(v) = val {
            Some(*v)
        } else {
            None
        };
    }

    #[inline(always)]
    pub fn set_delay_trn(&mut self, val: i32) {
        self.delay_trn = val;
    }

    #[inline(always)]
    pub fn set_dmg_chain(&mut self, val: i32) {
        self.dmg_chain = val;
    }

    #[inline(always)]
    pub fn set_chain_nr(&mut self, val: i32) {
        self.chain_nr = val;
    }

    #[inline(always)]
    pub fn set_max_level(&mut self, val: i32) {
        self.max_lv = val;
    }

    #[inline]
    pub(crate) fn set_exp_scene(&mut self, val: i32, ret: &mut Option<&'a mut String>, scene: &mut Option<&'a mut dyn Scene>) {

    }

    #[inline]
    pub(crate) fn set_level_scene(&mut self, val: i32, ret: &mut Option<&'a mut String>, scene: &mut Option<&'a mut dyn Scene>) {
        while val > self.level() {
            self.xp = self.maxp;
            self.level_up(ret, scene);
        }
        self.cr_lv = val;
    }

    #[inline(always)]
    pub fn set_level(&mut self, val: i32) {
        self.set_level_scene(val, &mut None, &mut None);
    }

    #[inline(always)]
    pub fn set_max_exp(&mut self, val: i32) {
        self.maxp = if val < 1 {
            1
        } else {
            val
        };
    }

    #[inline(always)]
    pub fn set_exp(&mut self, val: i32) {
        self.xp = val;
        self.level_up(&mut None, &mut None);
    }

    #[inline]
    pub fn check_reg_skill(&mut self, ability: &'a Ability) {
        
    }

    #[inline(always)]
    pub fn unequip_pos(&mut self, pos: EquipPos) -> Option<&'a Costume> {
        self.equip_item(pos, &None)
    }

    #[inline(always)]
    pub fn equip_item(&mut self, pos: EquipPos, item: &'a Option<&'a Costume>) -> Option<&'a Costume> {
        self.equip_item_scene(pos, item, &None)
    }

    pub(crate) fn equip_item_scene(&mut self, pos: EquipPos, item: &'a Option<&'a Costume>, scene: &'a Option<&'a dyn Scene>) -> Option<&'a Costume> {
        None
    }

    pub(crate) fn level_up(&mut self, ret: &mut Option<&'a mut String>, scene: &mut Option<&'a mut dyn Scene>) {
        let maxp = self.max_exp();
        let max_lv = self.max_level();
        let mut lv = self.level();
        let xp = self.exp();
        while maxp <= xp && lv < max_lv {
            self.max_lv = maxp * 2;
            self.m_hp += 3;
            self.m_mp += 2;
            self.m_sp += 2;
            self.atk += 1;
            self.def += 1;
            self.spi += 1;
            self.wis += 1;
            self.set_agi_scene(self.agi + 1, scene);
            lv += 1;
        }
        self.cr_lv = lv;
    }

    pub(crate) fn apply_states(&mut self, ret: &mut Option<&mut String>, scene: &mut Option<&mut dyn Scene>, consume: bool) {
        if consume {
            self.apply_dmg_roles(ret, scene);
        }
        if let Some(states) = self.base().base().state_dur {
            for (state, dur) in states.iter() {
                if (*dur) > State::END_DUR {
                    state.alter(ret, scene, self, consume);
                }
            }
        }
    }

    pub(crate) fn apply_dmg_roles(&mut self, ret: &mut Option<&mut String>, scene: &mut Option<&mut dyn Scene>) {
        if let Some(dmg_roles) = self.dmg_roles {
            for costume in dmg_roles {
                costume.apply(ret, self);
            }
            /*if let Some(scene) = scene {
                if let Some(actor_run) = scene.sprite_run() {
                    if actor_run(scene, &mut Some(self), &None, false, &mut None, &None) {

                    }
                }
            }*/
        }
    }

    pub(crate) fn update_attributes(&mut self, ret: &mut Option<&'a mut String>, scene: &mut Option<&'a mut dyn Scene>,
                                    costume: &'a Costume, remove: bool) {
        let c_role = costume.base();
        let i = if remove { -1 } else { 1 };
        self.set_m_hp(self.m_hp() + (i * c_role.m_hp()));
        self.set_m_mp(self.m_mp() + (i * c_role.m_mp()));
        self.set_m_sp(self.m_sp() + (i * c_role.m_sp()));
        self.set_m_actions(self.m_actions() + (i * costume.m_actions()));
        self.set_agi_scene(i * costume.agi(), scene);
        self.set_atk(i * costume.atk());
        self.set_def(i * costume.def());
        self.set_wis(i * costume.wis());
        self.set_spi(i * costume.spi());
    }

    pub(crate) fn update_resistance(&mut self, ret: &Option<&'a mut String>, scene: &Option<&'a mut dyn Scene>, elm_res: &Option<&'a HashMap<i32, i32>>,
                                    st_res: &Option<&'a HashMap<&'a State, i32>>, remove: bool) {
        if let Some(elm_res) = elm_res {
            if remove {
                if let Some(a_elm_res) = self.res() {
                    for (elm, res) in elm_res.iter() {
                        a_elm_res.insert(*elm, (if a_elm_res.contains_key(elm) { a_elm_res[elm] } else { 0 }) + res);
                    }
                }
            } else {
                if self.res().is_none() {
                    //self.res = Some(&'a mut HashMap<i32, i32>::new());
                }
            }
        }
        if let Some(st_res) = st_res {
            if remove {

            } else {

            }
        }
    }

    pub(crate) fn update_skills(&mut self, ret: &Option<&'a mut String>, scene: &Option<&'a mut dyn Scene>,
                                skills: &'a Vec<&'a Ability>, counters: bool, remove: bool) {
        
    }

    pub(crate) fn update_states(&mut self, ret: &Option<&'a mut String>, scene: &Option<&'a mut dyn Scene>,
                                states: &'a HashMap<&'a State, i32>, with_dur: bool, remove: bool) {
        
    }

    pub(crate) fn refresh_costumes(&mut self, ret: &Option<&'a String>, scene: &Option<&'a mut dyn Scene>) {

    }

    pub(crate) fn switch_costume(&mut self, ret: &Option<&'a String>, scene: &Option<&'a mut dyn Scene>,
                                 old_cost: &Option<&'a Costume>, new_cost: &Option<&'a Costume>) {

    }

    pub(crate) fn recover_scene(&mut self, ret: &Option<&'a String>, scene: &Option<&'a mut dyn Scene>) {
        
    }

    pub fn recover(&mut self, ret:&'a mut String) {
        self.recover_scene(&Some(ret), &None);
    }
    
}