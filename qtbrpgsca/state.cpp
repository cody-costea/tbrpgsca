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
    this->dur = 0;
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

bool State::inflict()
{
    int rnd = std::rand() % 11;
    if (this->dur > -2 && rnd > this->res)
    {
        this->dur = this->mdur;
        return true;
    }
    else
    {
        return false;
    }
}

bool State::remove()
{
    if (this->dur > -1)
    {
        this->dur = 0;
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

State* State::GET_STATES()
{
    return new State[State::STATESN]
    {
        State(1, "Regen", false, false, false, -1, 10, 0, 0, 0,
                2, 0, 0, 0, false),
        State(2, "Poison", false, false, false, 10, -7, 0, -2,
                0, -2, 0, 0, 0, false),
        State(3, "Clarity", false, false, false, -1, 0, 7, 0, 0,
                0, 1, 1, 0, false),
        State(4, "Dizziness", false, false, false, 3, 0, -7, 0,
                0, 0, -1, -1, 0, false),
        State(5, "Vigour", false, false, false, -1, 0, 0, 7, 1,
                0, 0, 0, 1, false),
        State(6, "Weakness", false, false, false, 5, 0, 0, -7,
                -1, 0, 0, 0, -1, false),
        State(7, "Berserk", false, true, false, 7, 0, 0, 0, 5,
                -3, 0, 0, 3, false),
        State(8, "Confusion", false, false, true, 3, 0, 0, 0, 0,
                0, 0, 0, 0, false),
        State(9, "Sleep", true, false, false, 5, 0, 0, 0, 0, -3,
                0, 0, -3, false),
        State(10, "Stun", true, false, false, 1, 0, 0, 0, 0, -1,
                0, 0, -1, false),
        State(11, "Reflect", false, false, false, 7, 0, 0, 0,
                0, 0, 0, 0, 0, true)
    };
}

const bool State::STATE_NONE[State::STATESN + 1] = {false,false,false,false,false,false,false,false,false,false,false,false};
const bool State::STATE_POISON[State::STATESN + 1] = {false,false,true,false,false,false,false,false,false,false,false,false};
const bool State::STATE_REGEN[State::STATESN + 1] = {false,true,false,false,false,false,false,false,false,false,false,false};
const bool State::STATE_DIZZINESS[State::STATESN + 1] = {false,false,false,false,true,false,false,false,false,false,false,false};
const bool State::STATE_MADNESS[State::STATESN + 1] = {false,false,false,false,true,false,false,true,true,true,true,false};
const bool State::STATE_CLARITY[State::STATESN + 1] = {false,false,false,true};
const bool State::STATE_WEAKNESS[State::STATESN + 1] = {false,false,false,false,false,false,true};
const bool State::STATE_VIGOUR[State::STATESN + 1] = {false,false,false,false,false,true};
const bool State::STATE_TPOISON[State::STATESN + 1] = {false,false,true,false,true,false,true};
const bool State::STATE_TREGEN[State::STATESN + 1] = {false,true,false,true,false,true};
const bool State::STATE_REVIVE[State::STATESN + 1] = {true};
const bool State::STATE_RVREGEN[State::STATESN + 1] = {true,true};
const bool State::STATE_BERSERK[State::STATESN + 1] = {false,false,false,false,false,false,false,true};
const bool State::STATE_CONFUSION[State::STATESN + 1] = {false,false,false,false,false,false,false,false,true};
const bool State::STATE_CONFCLARITY[State::STATESN + 1] = {false,false,false,true,false,false,false,false,true,true};
const bool State::STATE_CONFTREGEN[State::STATESN + 1] = {false,true,false,true,false,true,false,false,true,true};
const bool State::STATE_CURE[State::STATESN + 1] = {false,false,true,false,true,false,true,true,true,true,true};
const bool State::STATE_SLEEP[State::STATESN + 1] = {false,false,false,false,false,false,false,false,false,true};
const bool State::STATE_STUN[State::STATESN + 1] = {false,false,false,false,false,false,false,false,false,false,true};
const bool State::STATE_CONFSLEEP[State::STATESN + 1] = {false,false,false,false,false,false,false,false,true,true};
const bool State::STATE_DIZZYSTUN[State::STATESN + 1] = {false,false,false,false,true,false,false,false,false,false,true};
const bool State::STATE_REFLECT[State::STATESN + 1] = {false,false,false,false,false,false,false,false,false,false,false,true};
