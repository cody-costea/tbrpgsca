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
use std::cell::*;

//#[derive(Clone, PartialEq, Eq, Hash)]
pub struct State<'a> {
    pub(crate) base: Box<Costume<'a>>,
    pub(crate) r_skills: Option<&'a Vec<&'a Ability<'a>>>,
    pub(crate) s_res: i32,
    pub(crate) dur: i32,
}

extend_struct!(State, Costume);

impl<'a> State<'a> {

    #[inline(always)] pub const END_DUR: i32 = 3;

    #[inline(always)]
    pub fn r_skills(&self) -> &Option<&'a Vec<&'a Ability>> {
        &self.r_skills
    }

    #[inline(always)]
    pub fn s_res(&self) -> i32 {
        self.s_res
    }

    #[inline(always)]
    pub fn dur(&self) -> i32 {
        self.dur
    }

    pub(crate) fn remove<'b>(&self, actor: &'b mut Actor) {

    }

    pub(crate) fn block_skills<'b>(&self, actor: &'b mut Actor, remove: bool) {

    }

    pub(crate) fn alter<'b>(&self, ret: &mut Option<&mut String>, scene: &mut Option<&mut dyn Scene>, actor: &mut Actor, consume: bool) {
        
    }

    pub(crate) fn disable<'b>(&self, actor: &'b mut Actor, dur: i32, remove: bool) {

    }

    pub fn inflict<'b>(&self, user: &'b mut Actor, dur: i32, always: bool) {

    }

}