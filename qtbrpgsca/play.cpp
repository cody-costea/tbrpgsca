/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#include "play.h"

using namespace tbrpgsca;

Play::Play(QObject* const parent, int const flags) : QObject(parent)
{
    this->_flags = flags;
}

Play::~Play() {}
