/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#include "play.h"

using namespace tbrpgsca;

Play::PlaySheet::~PlaySheet() {}

Play::PlaySheet::PlaySheet(int const flags) : QSharedData()
{
    this->_play_flags = flags;
}

Play::PlaySheet::PlaySheet() : PlaySheet(0) {}

Play::Play(QObject* const parent, QSharedDataPointer<PlaySheet>& playData) : QObject(parent)
{
    this->_play_data = playData;
}

Play::Play(QObject* const parent, QSharedDataPointer<PlaySheet>&& playData) : Play(parent, static_cast<QSharedDataPointer<PlaySheet>&>(playData)) {}

Play::Play(QObject* const parent, PlaySheet* const playDataPtr) : Play(parent, QSharedDataPointer(playDataPtr)) {}

Play::Play(QObject* const parent, int const flags) : Play(parent, new PlaySheet(flags)) {}

Play::Play(const Play&& play) : Play(static_cast<const Play&>(play)) {}

Play::Play(const Play& play) : QObject(NIL)
{
    this->_play_data = play._play_data;
}

Play::~Play() {}
