/*
Copyright (C) AD 2018 Claudiu-Stefan Costea

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "ability.h"
#include "state.h"

using namespace qtbrpgsca;

Ability::Ability(int id, QString name, QString anim, QString audio, bool steal,
                 bool range, int lvrq, int hpc, int mpc, int spc, int dmgtype,
                 int atkp, int hpdmg, int mpdmg, int spdmg, int trg, int element,
                 int mqty, int rqty, bool absorb, bool restore, State** state,
                 int staten, State** rstate, int rstaten)
{
    this->id = id;
    this->name = name;
    this->anim = anim;
    this->audio = audio;
    this->steal = steal;
    this->lvrq = lvrq;
    this->hpc = hpc;
    this->mpc = mpc;
    this->spc = spc;
    this->atki = atkp;
    this->hpdmg = hpdmg;
    this->mpdmg = mpdmg;
    this->spdmg = spdmg;
    this->absorb = absorb;
    this->restore = restore;
    this->dmgtype = dmgtype;
    this->trg = trg;
    this->state = state;
    this->staten = staten;
    this->rstate = rstate;
    this->rstaten = rstaten;
    this->range = range;
    this->element = element;
    this->qty = mqty > 0 ? mqty : 0;
    this->mqty = mqty;
    this->rqty = rqty;
    this->tqty = 0;
}

Ability::Ability(int id, QString name, QString anim, QString audio, int hpdmg,
                 int mpdmg, int spdmg, int trg, int element, bool restore,
                 State** state, int staten, State** rstate, int rstaten) :
    Ability(id, name, anim, audio, false, true, 0, 0, 0, 0, 0, 0, hpdmg, mpdmg, spdmg, trg,
            element, 0, -1, false, restore, state, staten, rstate, rstaten)
{

}

Ability::Ability() :
    Ability(0, "", "", "", false, false, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, false, NULL, 0, NULL, 0)
{

}

Ability::~Ability()
{

}
