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

#include "state.h"
#include "actor.h"

using namespace qtbrpgsca;

State::State(int id, QString name, bool inactive, bool automatic, bool confusion, int dur, int hpm,
             int mpm, int spm, int atkm, int defm, int wism, int spim, int agim, bool reflect)
{
    this->id = id;
    this->name = name;
    this->inactive = inactive;
    this->confusion = confusion;
    this->automatic = confusion ? true : automatic;
    this->reflect = reflect;
    this->mdur = dur;
    this->res = 0;
    this->hpm = hpm;
    this->mpm = mpm;
    this->spm = spm;
    this->atkm = atkm;
    this->defm = defm;
    this->spim = spim;
    this->wism = wism;
    this->agim = agim;
    this->resm = new int[Actor::RESN] { 0 };
}

bool State::inflict(Actor& actor)
{
    if (actor.stateDur != NULL && actor.stateDur->operator [](this) < 0)
    {
        return true;
    }
    int rnd = std::rand() % 11;
    int res = this->res;
    if (actor.stateRes != NULL && actor.stateRes->contains(this))
    {
        res += actor.stateRes->operator [](this);
    }
    if (rnd > res)
    {
        if (actor.state == NULL)
        {
            actor.state = new QVector<State*>();
            actor.stateDur = new QMap<State*, int>();
            actor.stateRes = new QMap<State*, int>();
        }
        if (actor.state->contains(this))
        {
            if (actor.stateDur->operator [](this) >= this->mdur)
            {
                return true;
            }
        }
        else
        {
            actor.state->append(this);
        }
        actor.stateDur->operator [](this) = this->mdur;
        return true;
    }
    return false;
}

bool State::remove(Actor& actor)
{
    if (actor.stateDur != NULL && actor.stateDur->contains(this) && actor.stateDur->operator [](this) > -1)
    {
        actor.stateDur->operator [](this) = 0;
        return true;
    }
    else
    {
        return false;
    }
}

State::~State()
{
    delete[] this->resm;
}
