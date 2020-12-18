/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#include "state.h"

using namespace tbrpgsca;

State::State(QObject* const parent) : Ailment(parent) {}

State::State(const Ailment& ailment) : Ailment(ailment)
{
    this->_cr_dur = 0;
}

State::State(const State& state) : Ailment(state)
{
    this->_cr_dur = state._cr_dur;
}

State::~State() {}
