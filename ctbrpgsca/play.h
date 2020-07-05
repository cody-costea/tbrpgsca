/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef PLAY_H
#define PLAY_H

namespace tbrpgsca
{

    class Play
    {
    public:
        int flags() const;
        bool hasFlag(int const flag) const;
    protected:
        int _flags;

        int flags(int const flags);
        void setFlags(int const flags);
        Play& withFlags(int const flags);
        int flags(int const flags, bool const returnOld);

        Play& withFlag(int const flag, bool const value);
        void setFlag(int const flag, bool const value);

        Play(int const flags);

        ~Play();
    };

}

#endif // PLAY_H
