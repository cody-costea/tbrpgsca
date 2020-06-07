/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
use crate::actor::*;
use crate::ability::*;
use crate::costume::*;

pub struct State<'a> {
    pub(crate) costume: Box<Costume<'a>>,
    pub(crate) r_skills: Option<Vec<Ability<'a>>>,
    pub(crate) s_res: i32,
    pub(crate) dur: i32,
}

impl<'a> State<'a> {

    pub fn costume(&self) -> &Costume<'a> {
        &(*self.costume)
    }

    pub fn costume_mut(&mut self) -> &mut Costume<'a> {
        &mut (*self.costume)
    }

    pub fn r_skills(&self) -> &Option<Vec<Ability>> {
        &self.r_skills
    }

    pub fn s_res(&self) -> i32 {
        self.s_res()
    }

    pub fn dur(&self) -> i32 {
        self.dur
    }

    pub fn remove(&self, actor: Actor) {

    }

    pub fn block_skills(&self, actor: &Actor, remove: bool) {

    }

    pub fn alter(&self, ret: &String, actor: &Actor, consume: bool) {

    }

    pub fn disable(&self, actor: &Actor, dur: i32, remove: bool) {

    }

    pub fn inflict(&self, user: &Actor, dur: i32, always: bool) {

    }

}

impl<'a> std::ops::Deref for State<'a> {

    type Target = Costume<'a>;
    fn deref(&self) -> &Costume<'a> {
        self.costume()
    }

}

impl<'a> std::ops::DerefMut for State<'a> {

    fn deref_mut(&mut self) -> &mut Costume<'a> {
        self.costume_mut()
    }

}