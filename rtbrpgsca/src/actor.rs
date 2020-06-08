/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
use crate::ability::*;
use crate::costume::*;
use crate::state::*;

use std::rc::Rc;
use std::any::Any;
use std::collections::HashMap;

pub type ActorRun = FnMut(&mut Actor, &Any) -> bool;

#[derive(Hash, Copy, Clone, PartialEq, Eq)]
pub enum EquipPos {
    race, job, arms, chest, weapon, shield, head, legs, feet, belt, ring1,
    ring2, ring3, ring4, ring5, ring6, ring7, ring8, necklace, mantle
}

#[derive(Hash, Copy, Clone, PartialEq, Eq)]
pub enum EventType {
    hp, mp, sp, mHp, mMp, mSp, atk, def, spi, wis, agi, actions, mActions, mDelayTrn,
    dmgType, rflType, items, cvrType, drawn, race, job, exp, maxExp, level, maxLv, side,
    cInit, sprite, name, flags, delayTrn, dmgChain, chainNr
}

//#[derive(Clone, PartialEq, Eq, Hash)]
pub struct Actor<'a> {
    pub(crate) costume: Box<Costume<'a>>,
    pub(crate) equipment: HashMap<EquipPos, &'a Costume<'a>>,
    pub(crate) skills_cr_qty: Option<HashMap<&'a Ability<'a>, i32>>,
    pub(crate) skills_rg_trn: Option<HashMap<&'a Ability<'a>, i32>>,
    pub(crate) events: Option<HashMap<EventType, &'a mut ActorRun>>,
    pub(crate) items: Option<Rc<HashMap<&'a Ability<'a>, i32>>>,
    pub(crate) delay_act: Option<Box<&'a dyn FnMut(bool)>>,
    pub(crate) dmg_roles: Option<Vec<&'a Costume<'a>>>,
    pub(crate) drawn_by: Option<Box<&'a Actor<'a>>>,
    pub(crate) delay_trn: i32,
    pub(crate) dmg_chain: i32,
    pub(crate) chain_nr: i32,
    pub(crate) old_side: i32,
    pub(crate) actions: i32,
    pub(crate) max_lv: i32,
    pub(crate) cr_lv: i32,
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

    pub fn items(&self) -> &Option<Rc<HashMap<&'a Ability<'a>, i32>>> {
        &self.items
    }

    pub fn delay_act(&self) -> &Option<Box<&'a dyn FnMut(bool)>> {
        &self.delay_act
    }

    pub fn dmg_roles(&self) -> &Option<Vec<&'a Costume<'a>>> {
        &self.dmg_roles
    }

    pub fn drawn_by(&self) -> &Option<Box<&'a Actor<'a>>> {
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

    pub fn side(&self) -> i32 {
        self.side
    }

    pub fn exp(&self) -> i32 {
        self.xp
    }

    pub(crate) fn set_flags(&mut self, val: i32) {
        if self.run_event(&EventType::flags, &val) {
            self.flags = val;
        }
    }

    pub(crate) fn run_event(&mut self, eventType: &EventType, newValue: &dyn Any) -> bool {
        if let Some(eventsMap) = &mut self.events {
            if let Some(event) = eventsMap.get(eventType) {
                //return event(&mut self, newValue);
            }
        }
        true
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