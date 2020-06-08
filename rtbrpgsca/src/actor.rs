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
use std::collections::HashMap;

pub enum EquipPos {

}

pub enum EventType {

}

pub struct Actor<'a> {
    pub(crate) costume: Box<Costume<'a>>,
    pub(crate) equipment: HashMap<EquipPos, &'a Costume<'a>>,
    pub(crate) skills_cr_qty: Option<HashMap<&'a Ability<'a>, i32>>,
    pub(crate) skills_rg_trn: Option<HashMap<&'a Ability<'a>, i32>>,
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

    pub fn costume(&self) -> &Costume<'a> {
        &(*self.costume)
    }

    pub fn costume_mut(&mut self) -> &mut Costume<'a> {
        &mut (*self.costume)
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