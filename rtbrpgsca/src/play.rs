
/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

pub trait Play {

    fn flags(&self) -> i32;

    #[inline(always)]
    fn has_flag(&self, flag: i32) -> bool {
        (self.flags() & flag) == flag
    }

}

pub trait PlayMut: Play {

    fn set_flags(&mut self, value: i32);

    fn flags_mut(&mut self) -> &mut i32;

    #[inline]
    fn set_flag(&mut self, flag: i32, value: bool) {
        let flags = self.flags();
        if value != ((flags & flag) == flag) {
            self.set_flags(flags ^ flag);
        }
    }

}