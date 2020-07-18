/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
use crate::play::*;
use crate::state::*;
use crate::actor::*;
use crate::scene::*;

use std::collections::HashMap;
use std::str;

#[derive(Clone)]
pub struct Role<'a> {
    pub(crate) id: i32,
    pub(crate) flags: i32,
    pub(crate) name: &'static str,
    pub(crate) sprite: Option<&'static str>,
    pub(crate) state_dur: Option<HashMap<&'a State<'a>, i32>>,
    pub(crate) dmg_type: i32,
    pub(crate) m_hp: i32,
    pub(crate) m_mp: i32,
    pub(crate) m_sp: i32,
    pub(crate) hp: i32,
    pub(crate) mp: i32,
    pub(crate) sp: i32,
}

impl<'a> Role<'a> {

    pub const HP_TXT: &'static str = "HP";
    pub const MP_TXT: &'static str = "MP";
    pub const RP_TXT: &'static str = "RP";

    pub const FLAG_RANGE: i32 = 2;
    pub const FLAG_REVIVE: i32 = 1;
    pub const DMG_TYPE_ATK: i32 = 1;
    pub const DMG_TYPE_DEF: i32 = 2;
    pub const DMG_TYPE_SPI: i32 = 4;
    pub const DMG_TYPE_WIS: i32 = 8;
    pub const DMG_TYPE_AGI: i32 = 16;
    pub const ELEMENT_FIRE: i32 = 32;
    pub const ELEMENT_WATER: i32 = 64;
    pub const ELEMENT_THUNDER: i32 = 128;
    pub const ELEMENT_EARTH: i32 = 256;
    pub const ELEMENT_PSYCHIC: i32 = 512;
    pub const ELEMENT_LIGHT: i32 = 1024;
    pub const DEFAULT_RES: i32 = 3;

    #[inline(always)]
    pub fn id(&self) -> i32 {
        self.id
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
    pub fn state_dur(&self) -> Option<&HashMap<&'a State, i32>> {
        self.state_dur.as_ref().clone()
    }

    #[inline(always)]
    pub fn state_dur_mut(&mut self) -> Option<&mut HashMap<&'a State, i32>> {
        if let Some(v) = self.state_dur.as_mut() {
            Some(v)
        } else {
            None
        }
    }

    #[inline(always)]
    pub fn revives(&self) -> bool {
        self.has_flag(Role::FLAG_REVIVE)
    }

    #[inline(always)]
    pub fn ranged(&self) -> bool {
        self.has_flag(Role::FLAG_RANGE)
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

    pub(crate) fn damage(&self, ret: &mut String, scene: &mut Option<&mut dyn Scene>, absorber: Option<&mut Actor>, target: &mut Actor, dmg: i32, percent: bool) {
        if !target.invincible() {
            let mut dmg_hp;
            let mut dmg_mp;
            let mut dmg_sp;
            if percent {
                dmg_hp = (target.m_hp() + dmg) * self.hp() / 100;
                dmg_mp = (target.m_mp() + dmg) * self.mp() / 100;
                dmg_sp = (target.m_sp() + dmg) * self.sp() / 100;
            } else {
                dmg_hp = self.hp();
                dmg_mp = self.mp();
                dmg_sp = self.sp();
                if dmg_hp != 0 {
                    dmg_hp += if dmg_hp < 0 { -1 * dmg } else { dmg };
                }
                if dmg_mp != 0 {
                    dmg_mp += if dmg_mp < 0 { -1 * dmg } else { dmg };
                }
                if dmg_sp != 0 {
                    dmg_sp += if dmg_sp < 0 { -1 * dmg } else { dmg };
                }
            }
            if let Some(trg_res_map) = target.res() {
                let mut res = Role::DEFAULT_RES;
                let dmg_type = self.dmg_type();
                for (&elm, val) in trg_res_map.iter() {
                    if (dmg_type & elm) == elm {
                        res += val;
                    }
                }
                if res > 0 {
                    if res > 7 {
                        res = -7 + (res - 7);
                        if res > -1 {
                            res = -1 * res + 2;
                            dmg_hp *= res;
                            dmg_mp *= res;
                            dmg_sp *= res;
                        }
                    } else if res == 7 {
                        ret.push_str(&*format!(", resisted by {}", target.name()));
                        return
                    } else {
                        dmg_hp /= res;
                        dmg_mp /= res;
                        dmg_sp /= res;
                    }
                } else {
                    res = -1 * res - 2;
                    dmg_hp *= res;
                    dmg_mp *= res;
                    dmg_sp *= res;
                }
            } else {
                dmg_hp /= Role::DEFAULT_RES;
                dmg_mp /= Role::DEFAULT_RES;
                dmg_sp /= Role::DEFAULT_RES;                
            }
            let mut c = false;
            if dmg_sp != 0 {
                c = true;
                ret.push_str(&*format!(", {} suffers ", target.name()));
                if dmg_sp < 0 {
                    ret.push_str("+");
                }
                target.set_sp(target.sp() - dmg_sp);
                ret.push_str(&*format!(" {0} {1} suffers ", -dmg_sp, "RP"));
            }
            if dmg_mp != 0 {
                if c {
                    ret.push_str(", ");
                } else {
                    ret.push_str(&*format!(", {} suffers ", target.name()));
                    c = true;
                }
                if dmg_mp < 0 {
                    ret.push_str("+");
                }
                target.set_mp(target.mp() - dmg_mp);
                ret.push_str(&*format!(" {0} {1} suffers ", -dmg_mp, "MP"));
            }
            if dmg_hp != 0 {
                if c {
                    ret.push_str(", ");
                } else {
                    ret.push_str(&*format!(", {} suffers ", target.name()));
                    c = true;
                }
                if dmg_hp < 0 {
                    ret.push_str("+");
                }
                target.set_hp(target.hp() - dmg_hp);
                ret.push_str(&*format!(" {0} {1} suffers ", -dmg_hp, "HP"));
                if target.hp() < 1 {
                    ret.push_str(&*format!(", {} falls unconscious", target.name()));
                }
                if c {
                    if let Some(absorber) = absorber {
                        absorber.set_sp(absorber.sp() + dmg_sp / 2);
                        absorber.set_mp(absorber.mp() + dmg_mp / 2);
                        absorber.set_hp(absorber.hp() + dmg_hp / 2);
                    }
                }
            }
        }
    }

    pub fn new(id: i32, name: &'static str, sprite: Option<&'static str>, m_hp: i32, m_mp: i32, m_sp: i32, hp: i32, mp: i32,
    sp: i32, element: i32, ranged: bool, revives: bool, state_dur: Option<HashMap<&'a State<'a>, i32>>) -> Role<'a> {
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

macro_rules! implement_comparison {

    ($sub: tt) => {

        impl<'a> Eq for  $sub<'a> {}

        impl<'a> PartialEq for  $sub<'a> {
            fn eq(&self, other: &Self) -> bool {
                self.id() == other.id()
            }
        }

        impl<'a> std::hash::Hash for $sub<'a> {
            fn hash<H: std::hash::Hasher>(&self, state: &mut H) {
                self.id().hash(state);
           } 
        }

        impl<'a> PartialOrd for $sub<'a> {
            fn partial_cmp(&self, other: &Self) -> Option<::std::cmp::Ordering> {
                self.id().partial_cmp(&other.id())
            }
        }

        impl<'a> Ord for  $sub<'a> {
            fn cmp(&self, other: &Self) -> std::cmp::Ordering {
                self.id().cmp(&other.id())
            }
        }

        impl<'a> Play for $sub<'a> {
            
            #[inline(always)]
            fn flags(&self) -> i32 {
                self.flags
            }
            
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

        implement_comparison!($sub);

    }

}

implement_comparison!(Role);