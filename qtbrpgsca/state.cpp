/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#include "state.h"
#include "ability.h"
#include "costume.h"
#include "actor.h"
#include "scene.h"
#include "role.h"

using namespace tbrpgsca;

void State::remove(QString* const ret, Actor& actor) const
{
    const Ailment& state = *this;
    state.blockSkills(actor, true);
    state.adopt(ret, actor, false, true);
    if (this->isConverted() && (!actor.isConverted()))
    {
        actor.setPartySide(static_cast<int>(actor._actor_data->_old_side));
    }

}

bool State::disable(Actor& actor, int const dur, const bool remove)
{
    return this->disable(NIL, actor, remove, dur);
}

bool State::disable(QString* const ret, Actor& actor, int dur, const bool remove)
{
    if (dur == 0)
    {
        dur = this->maximumDuration();
    }
    if (dur > State::EndDur)
    {
        QList<Ailment>* sDur = actor.roleData()._a_states;
        if (sDur == NIL)
        {
            return true;
        }
        else
        {
            int const crDur = this->currentDuration();//sDur->value(this, State::EndDur);
            //if (dur == -2 || (crDur > -2 && (dur == -1 || crDur > -1)))
            if (crDur > -1 || dur <= crDur)
            {
                if (dur > 0 /*&& crDur > 0*/ && crDur > dur)
                {
                    //sDur->operator[](this) = crDur - dur;
                    this->setCurrentDuration(crDur - dur);
                    return false;
                }
                else
                {
                    if (remove)
                    {
                        sDur->removeOne(*this);
                    }
                    else
                    {
                        //sDur->operator[](this) = State::EndDur;
                        this->setCurrentDuration(State::EndDur);
                    }
                    if (crDur > State::EndDur)
                    {
                        this->remove(ret, actor);
                    }
                    return true;
                }
            }
            return crDur <= State::EndDur;
        }
    }
    else
    {
        return false;
    }
}

void State::alter(QString& ret, Actor& actor, const bool consume)
{
    return this->alter(&ret, actor, consume);
}

void State::alter(QString* const ret, Actor& actor, const bool consume)
{
    //QList<Ailment>* sDur = actor.roleData()._a_states;
    //if (sDur /*&& actor.hp > 0*/)
    {
        int const d = this->currentDuration();//sDur->value(this, State::EndDur);
        if (consume)
        {
            if (d > 0)
            {
                //sDur->operator[](this) = d - 1;
                this->setCurrentDuration(d - 1);
            }
        }
        else if (d == 0)
        {
            //sDur->operator[](this) = State::EndDur;
            this->setCurrentDuration(State::EndDur);
            this->remove(ret, actor);
        }
    }

}

void State::apply(QString& ret, Actor& actor) const
{
    State& state = *(const_cast<State*>(this));
    state.Costume::apply(ret, actor);
    state.alter(&ret, actor, true);
}

void State::abandon(QString& ret, Actor& actor) const
{
    this->remove(&ret, actor);
}

State::State(QObject* const parent) : Ailment(parent)
{
    this->_cr_dur = State::EndDur;
}

State::State(const Ailment& ailment) : Ailment(ailment)
{
    this->_cr_dur = State::EndDur;
}

State::State(const State& state) : Ailment(state)
{
    this->_cr_dur = state._cr_dur;
}

State::~State() {}
