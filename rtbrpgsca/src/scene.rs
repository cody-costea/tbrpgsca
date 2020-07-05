/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
use crate::play::*;
use crate::actor::*;
use crate::ability::*;
use crate::costume::*;
use crate::state::*;

pub type SpriteRun = dyn FnMut(&mut dyn Scene, &mut Option<&mut Actor>, &Option<&Ability>,
                         bool, &mut Option<&mut Actor>, &Option<&Ability>) -> bool;

pub trait Scene<'a>: PlayMut {
    
    fn status(&self) -> i32;
    
    fn current(&self) -> i32;

    fn cr_party(&self) -> i32;

    fn cr_actor(&self) -> &'a Actor<'a>;

    fn last_ability(&self) -> &'a Ability<'a>;

    fn sprite_run(&self) -> &Option<&SpriteRun>;

    fn targets(&self) -> &'a Option<&'a Vec<&'a Actor>>;

    fn set_targets(&mut self, targets: &'a Option<&'a Vec<&'a Actor>>);

    fn play_ai(&mut self, ret: &'a mut String, actor: &'a mut Actor) {

    }

    fn end_turn(&mut self, ret: &'a mut String, actor: &'a mut Actor) {
        
    }

    fn perform(&mut self, ret: &'a mut String, user: &'a mut Actor, ability: &'a Ability, target: &'a mut Actor, item: bool) {

    }

    fn execute(&mut self, ret: &'a String, user: &'a mut Actor, ability: &'a Ability, target: &Option<&'a mut Actor>, apply_costs: bool) {

    }

    fn initialize(&mut self, ret: &'a mut String, parties: Vec<&'a Vec<&'a mut Actor>>, surprise: i32, m_init: i32) {

    }

    fn ai_skill(&mut self, user: &'a Actor, skills: &'a Vec<&'a Ability>, index: i32, n_restore: bool) -> i32 {
        0
    }

    fn guardian(&mut self, user: &'a Actor, ability: &'a Ability, target: &'a Actor<'a>) -> &'a Actor<'a> {
        target
    }

    fn can_target(&mut self, user: &'a Actor, ability: &'a Ability, target: &'a Actor) -> bool {
        false
    }

    fn check_status(&mut self, ret: &'a mut String) {
        
    }

    fn reset_turn(&mut self, actor: &'a mut Actor) {
        
    }

    fn escape(&mut self, ret: &'a mut String) {
        
    }

    fn agi_calc(&mut self) {
        
    }

}