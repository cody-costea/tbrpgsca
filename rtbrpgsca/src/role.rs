/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
use crate::state::*;
use crate::actor::*;

use std::collections::HashMap;
use std::str;

//#[derive(Clone, PartialEq, Eq, Hash)]
pub struct Role<'a> {
    pub(crate) id: i32,
    pub(crate) flags: i32,
    pub(crate) name: &'static str,
    pub(crate) sprite: Option<&'static str>,
    pub(crate) state_dur: Option<&'a HashMap<&'a State<'a>, i32>>,
    pub(crate) dmg_type: i32,
    pub(crate) m_hp: i32,
    pub(crate) m_mp: i32,
    pub(crate) m_sp: i32,
    pub(crate) hp: i32,
    pub(crate) mp: i32,
    pub(crate) sp: i32,
}

impl<'a> Role<'a> {

    #[inline(always)] pub const HP_TXT: &'static str = "HP";
    #[inline(always)] pub const MP_TXT: &'static str = "MP";
    #[inline(always)] pub const RP_TXT: &'static str = "RP";

    #[inline(always)] pub const FLAG_RANGE: i32 = 2;
    #[inline(always)] pub const FLAG_REVIVE: i32 = 1;
    #[inline(always)] pub const DMG_TYPE_ATK: i32 = 1;
    #[inline(always)] pub const DMG_TYPE_DEF: i32 = 2;
    #[inline(always)] pub const DMG_TYPE_SPI: i32 = 4;
    #[inline(always)] pub const DMG_TYPE_WIS: i32 = 8;
    #[inline(always)] pub const DMG_TYPE_AGI: i32 = 16;
    #[inline(always)] pub const ELEMENT_FIRE: i32 = 32;
    #[inline(always)] pub const ELEMENT_WATER: i32 = 64;
    #[inline(always)] pub const ELEMENT_THUNDER: i32 = 128;
    #[inline(always)] pub const ELEMENT_EARTH: i32 = 256;
    #[inline(always)] pub const ELEMENT_PSYCHIC: i32 = 512;
    #[inline(always)] pub const ELEMENT_LIGHT: i32 = 1024;

    #[inline(always)]
    pub fn id(&self) -> i32 {
        self.id
    }
    
    #[inline(always)]
    pub fn flags(&self) -> i32 {
        self.flags
    }

    #[inline(always)]
    pub fn name(&self) -> &'static str {
        self.name
    }

    #[inline(always)]
    pub fn sprite(&self) -> &Option<&'static str> {
        &self.sprite
    }

    #[inline(always)]
    pub fn state_dur(&self) -> &Option<&'a HashMap<&'a State, i32>> {
        &(self.state_dur)
    }

    #[inline(always)]
    pub fn revives(&self) -> bool {
        (self.flags & Role::FLAG_REVIVE) == Role::FLAG_REVIVE
    }

    #[inline(always)]
    pub fn ranged(&self) -> bool {
        (self.flags & Role::FLAG_RANGE) == Role::FLAG_RANGE
    }
    
    #[inline(always)]
    pub fn dmg_type(&self) -> i32 {
        self.dmg_type
    }

    #[inline(always)]
    pub fn m_hp(&self) -> i32 {
        self.m_hp
    }

    #[inline(always)]
    pub fn m_mp(&self) -> i32 {
        self.m_mp
    }

    #[inline(always)]
    pub fn m_sp(&self) -> i32 {
        self.m_sp
    }

    #[inline(always)]
    pub fn hp(&self) -> i32 {
        self.hp
    }

    #[inline(always)]
    pub fn mp(&self) -> i32 {
        self.mp
    }
    
    #[inline(always)]
    pub fn sp(&self) -> i32 {
        self.sp
    }

    pub fn damage(&self, ret: Option<&mut String>, absorber: Option<&mut Actor>, target: &mut Actor, dmg: i32, percent: bool) -> &Role<'a> {
        self
    }

    pub fn new(id: i32, name: &'static str, sprite: Option<&'static str>, m_hp: i32, m_mp: i32, m_sp: i32, hp: i32, mp: i32,
    sp: i32, element: i32, ranged: bool, revives: bool, state_dur: Option<&'a HashMap<&'a State<'a>, i32>>) -> Role<'a> {
        let mut flags = 0;
        if revives {
            flags |= Role::FLAG_REVIVE;
        }
        if ranged {
            flags |= Role::FLAG_RANGE;
        }
        Role {
            id : id,
            flags: flags,
            name: name,
            sprite: sprite,
            state_dur: state_dur,
            dmg_type: element,
            m_hp: m_hp,
            m_mp: m_mp,
            m_sp: m_sp,
            hp: hp,
            mp: mp,
            sp: sp
        }
    }

}

macro_rules! extend_struct {

    ($sub: tt, $base: tt) => {

        impl<'a> $sub<'a> {

            #[inline(always)]
            pub fn base(&self) -> &$base<'a> {
                &(*self.base)
            }

            #[inline(always)]
            pub fn base_mut(&mut self) -> &mut $base<'a> {
                &mut (*self.base)
            }

        }

        impl<'a> std::ops::Deref for $sub<'a> {

            type Target = $base<'a>;
            #[inline(always)]
            fn deref(&self) -> &$base<'a> {
                self.base()
            }

        }

        impl<'a> std::ops::DerefMut for $sub<'a> {
            
            #[inline(always)]
            fn deref_mut(&mut self) -> &mut $base<'a> {
                self.base_mut()
            }

        }

    }

}