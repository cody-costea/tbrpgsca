/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#include "play.h"

using namespace tbrpgsca;

bool Play::hasFlag(int const flag) const
{
    return (this->_flags & flag) == flag;
}

void Play::setFlag(const int flag, const bool value)
{
    int const flags = this->_flags;
    if (value != ((flags & flag) == flag))
    {
        this->_flags = flags ^ flag;
    }
}

Play::Play(int const flags)
{
    this->_flags = flags;
}

Play::~Play()
{

}
