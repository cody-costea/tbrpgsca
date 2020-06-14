/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
use crate::actor::*;
use crate::ability::*;
use crate::costume::*;
use crate::state::*;

pub type SpriteAct<'a> = dyn FnMut(&mut dyn Scene, &Option<&'a Actor>, &Option<&'a Ability>, bool, &Option<&'a Actor>, &Option<&'a Ability>);

pub trait Scene<'a> {
    
    fn status(&self) -> i32;
    
    fn current(&self) -> i32;

    fn cr_party(&self) -> i32;

    fn cr_actor(&self) -> &'a Actor<'a>;

    fn last_ability(&self) -> &'a Ability<'a>;

    fn play_ai(&mut self,ret: &'a String, actor: &'a Actor) {

    }

    fn end_turn(&mut self, ret: &'a String, actor: &'a Actor) {
        
    }

    fn perform(&mut self, ret: &'a String, user: &'a Actor, ability: &'a Ability, target: &'a Actor, item: bool) {

    }

    fn execute(&mut self, ret: &'a String, user: &'a Actor, ability: &'a Ability, target: &Option<&'a Actor>, apply_costs: bool) {

    }

    fn initialize(&mut self, ret: &'a String, parties: Vec<&'a Vec<&'a Actor>>, surprise: i32, m_init: i32) {

    }

    fn ai_skill(&mut self, user: &'a Actor, skills: &'a Vec<&'a Ability>, index: i32, n_restore: bool) -> i32 {
        0
    }

    fn guardian(&mut self, user: &'a Actor, ability: &'a Ability, target: &'a Actor) -> &'a Actor<'a> {
        target
    }

    fn can_target(&mut self, user: &'a Actor, ability: &'a Ability, target: &'a Actor) -> bool {
        false
    }

    fn check_status(&mut self, ret: &'a String) {
        
    }

    fn reset_turn(&mut self, actor: &'a Actor) {
        
    }

    fn escape(&mut self, ret: &'a String) {
        
    }

    fn agi_calc(&mut self) {
        
    }

}