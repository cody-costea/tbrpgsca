/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
use crate::actor::*;
use crate::ability::*;
use crate::costume::*;
use crate::scene::*;

//#[derive(Clone, PartialEq, Eq, Hash)]
pub struct State<'a> {
    pub(crate) costume: Box<Costume<'a>>,
    pub(crate) r_skills: Option<&'a Vec<&'a Ability<'a>>>,
    pub(crate) s_res: i32,
    pub(crate) dur: i32,
}

extend_struct!(State, Costume);

impl<'a> State<'a> {

    pub const END_DUR: i32 = 3;

    pub fn base(&self) -> &Costume<'a> {
        &(*self.costume)
    }

    pub fn base_mut(&mut self) -> &mut Costume<'a> {
        &mut (*self.costume)
    }

    pub fn r_skills(&self) -> &Option<&'a Vec<&'a Ability>> {
        &self.r_skills
    }

    pub fn s_res(&self) -> i32 {
        self.s_res
    }

    pub fn dur(&self) -> i32 {
        self.dur
    }

    pub fn remove(&self, actor: &Actor) {

    }

    pub fn block_skills(&self, actor: &Actor, remove: bool) {

    }

    pub fn alter<'b>(&self, ret: &'b mut Option<&'a mut String>, scene: &'b mut Option<&'a mut dyn Scene>, actor: &'b mut Actor, consume: bool) {

    }

    pub fn disable(&self, actor: &Actor, dur: i32, remove: bool) {

    }

    pub fn inflict(&self, user: &Actor, dur: i32, always: bool) {

    }

}