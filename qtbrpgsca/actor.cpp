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

#include "actor.h"

using namespace qtbrpgsca;

Actor::Actor(int id, QString name, QString race, QString job, int lv, int maxlv,
             int maxhp, int maxmp, int maxsp, int matk, int mdef, int mspi,
             int mwis, int magi, int hpp, int mpp, int spp, int atkp, int defp,
             int spip, int wisp, int agip, int* rres, int* jres, Ability** raceSkills,
             int rSkillsNr, Ability** jobSkills, int jSkillsNr, QVector<Ability>* items)
    : Race(0, race, maxhp, maxmp, maxsp, matk, mdef, mspi, mwis, magi, rres, raceSkills, rSkillsNr),
      Job(0, job, hpp, mpp, spp, atkp, defp, spip, wisp, agip, jres, jobSkills, jSkillsNr)
{
    this->stats(id, name, lv, maxlv, items);
}

Actor::Actor(int id, QString name, Race* race, Job* job, int lv, int maxlv, QVector<Ability>* items)
    : Race(race->raceId, race->raceName, race->maxhp, race->maxmp, race->maxsp, race->matk,
           race->mdef, race->mspi, race->mwis, race->magi, race->rres, race->raceSkills, race->rSkillsNr),
      Job(job->jobId, job->jobName, job->hpp, job->mpp, job->spp, job->atkp, job->defp,
          job->spip, job->wisp, job->agip, job->jres, job->jobSkills, job->jSkillsNr)
{
    this->stats(id, name, lv, maxlv, items);
}

void Actor::stats(int id, QString name, int lv, int maxlv, QVector<Ability>* items)
{
    this->id = id;
    this->name = name;
    this->automatic = 0;
    this->lv = lv;
    this->maxlv = maxlv;
    if (this->freeItems && this->items != NULL)
    {
        delete this->items;
        this->freeItems = false;
    }
    this->items = items;
    if (this->res != NULL)
    {
        delete[] this->res;
    }
    this->res = new int[RESN];
    //this->setStates();
    this->levelUp();
    this->recover();
}

void Actor::recover()
{
    this->calcRes();
    this->hp = this->maxhp;
    this->mp = this->maxmp;
    this->sp = 0;
    this->active = true;
    this->guards = true;
    this->atk = this->matk;
    this->def = this->mdef;
    this->spi = this->mspi;
    this->wis = this->mwis;
    this->agi = this->magi;
    if (this->state != NULL)
    {
        for (int i = 0; i < this->state->size(); i++)
        {
            this->state->operator [](i)->remove(*this);
        }
    }
    this->applyStates(false);
}

void Actor::changeJob(Job* job)
{
    this->jobId = job->jobId;
    this->jobName = job->jobName;
    this->hpp = job->hpp;
    this->mpp = job->mpp;
    this->spp = job->spp;
    this->atkp = job->spp;
    this->defp = job->defp;
    this->spip = job->spip;
    this->wisp = job->wisp;
    this->agip = job->agip;
    this->jres = job->jres;
    this->jobSkills = job->jobSkills;
}

void Actor::levelUp()
{
    while (this->maxp <= this->exp && this->lv < this->maxlv)
    {
        this->maxp *= 2;
        this->lv++;
        this->maxhp += 3;
        this->maxmp += 2;
        this->maxsp += 2;
        this->matk++;
        this->mdef++;
        this->mwis++;
        this->mspi++;
        this->magi++;
        if (this->lv % 2 == 1)
        {
            this->maxhp += this->hpp;
            this->mdef += this->defp;
            this->mspi += this->spip;
            this->magi += this->agip;
        }
        else
        {
            this->maxmp += this->mpp;
            this->maxsp += this->spp;
            this->matk += this->atkp;
            this->mwis += this->wisp;
        }
    }
}

void Actor::checkRes(int r)
{
    if (r < 8 && r >= 0)
    {
        if (this->res[r] > 7)
            this->res[r] = 7;
        if (this->res[r] < 0)
            this->res[r] = 0;
    }
}

void Actor::calcRes()
{
    if (this->res == NULL)
    {
        this->res = new int[RESN];
    }
    for (int i = 0; i < RESN; i++)
    {
        this->res[i] = 3 + this->rres[i] + this->jres[i];
    }
}

QString Actor::execAbility(Ability& ability, Actor& target, bool applyCosts)
{
    QString s = this->name + " performs " + ability.name;
    if (applyCosts)
    {
        this->hp -= ability.hpc;
        this->mp -= ability.mpc;
        this->sp -= ability.spc;
        if (ability.qty > 0)
            ability.qty--;
    }
    int dmg = std::rand() % 4;
    int res = (target.res[ability.element] < 7) ? target.res[ability.element]
            : -1;
    switch (ability.dmgtype)
    {
    case 1:
        dmg += (ability.atki + ((this->def + this->atk) / 2))
                / (target.def * res + 1);
        break;
    case 2:
        dmg += (ability.atki + this->wis) / (target.spi * res + 1);
        break;
    case 3:
        dmg += (ability.atki + this->spi) / (target.wis * res + 1);
        break;
    case 4:
        dmg += (ability.atki + ((this->agi + this->atk) / 2))
                / (((target.agi + target.def) / 2) * res + 1);
        break;
    case 5:
        dmg += (ability.atki + ((this->wis + this->atk) / 2))
                / (((target.spi + target.def) / 2) * res + 1);
        break;
    case 6:
        dmg += (ability.atki + ((this->agi + this->wis + this->atk) / 3))
                / (((target.agi + target.spi) / 2) * res + 1);
        break;
    case 7:
        dmg += (ability.atki + ((this->spi + this->atk + this->def) / 3))
                / (((target.wis + target.def) / 2) * res + 1);
        break;
    default:
        dmg += (ability.atki + this->atk) / (target.def * res + 1);
    }
    if (ability.dmgtype == 2
            || ability.dmgtype == 3
            || (ability.qty > 0 && ability.mqty < 1)
            || (ability.dmgtype != 4 && ((std::rand() % 13) + this->agi / 5) > 2 + target.agi / 4)
            || (ability.dmgtype == 4 && ((std::rand() % 13) + this->agi / 3) > 2 + target.agi / 4))
    {
        int dmghp = (ability.hpdmg != 0) ? (((ability.hpdmg < 0 ? -1 : 1) * dmg) + ability.hpdmg)
                : 0;
        int dmgmp = (ability.mpdmg != 0) ? (((ability.mpdmg < 0 ? -1 : 1) * dmg) + ability.mpdmg)
                : 0;
        int dmgsp = (ability.spdmg != 0) ? (((ability.spdmg < 0 ? -1 : 1) * dmg) + ability.spdmg)
                : 0;
        if (res < 0)
        {
            dmghp = -dmghp;
            dmgmp = -dmgmp;
            dmgsp = -dmgsp;
        }
        target.hp -= dmghp;
        target.mp -= dmgmp;
        target.sp -= dmgsp;
        if (ability.absorb)
        {
            this->hp += dmghp / 2;
            this->mp += dmgmp / 2;
            this->sp += dmgsp / 2;
        }
        bool c = false;
        if (dmghp != 0 || dmgmp != 0 || dmgsp != 0)
            s += (", " + target.name + " suffers");
        if (dmghp != 0)
        {
            s += (" ");
            if (dmghp < 1)
                s += "+";
            s += QString::number(-dmghp) + " HP";
            c = true;
        }
        if (dmgmp != 0)
        {
            if (c)
                s += ",";
            s += " ";
            if (dmgmp < 1)
                s += "+";
            s += QString::number(-dmgmp) + " MP";
            c = true;
        }
        if (dmgsp != 0)
        {
            if (c)
                s += ",";
            s += " ";
            if (dmgsp < 1)
                s += "+";
            s += QString::number(-dmgsp) + " RP";
        }
        for (int i = 0; i < ability.staten; i++)
        {
            ability.state[i]->inflict(target);
        }
        for (int i = 0; i < ability.rstaten; i++)
        {
            ability.rstate[i]->remove(target);
        }
        if (ability.steal
                && target.items != NULL
                && target.items != this->items
                && target.items->size() > 0
                && (((std::rand() % 12) + this->agi / 4) > 4 + target.agi / 3))
        {
            int itemId = (std::rand() % target.items->size());
            if (itemId < target.items->size()
                    && target.items->operator [](itemId).qty > 0)
            {
                bool found = false;
                if (this->items != NULL)
                {
                    for (int i = 0; i < this->items->size(); i++)
                        if (this->items->operator [](i).id == target.items
                               ->at(itemId).id)
                        {
                            this->items->operator [](i).qty++;
                            found = true;
                            break;
                        }
                }
                else
                {
                    this->items = new QVector<Ability>();
                    this->freeItems = true;
                }
                if (!found)
                {
                    Ability newItem = target.items->operator [](itemId);
                    this->items->append(newItem);
                    this->items->operator [](this->items->size() - 1).qty = 1;
                }
                target.items->operator [](itemId).qty--;
                s += ", " + target.items->operator [](itemId).name + " stolen";
                if (target.items->operator [](itemId).qty == 0)
                    target.items->removeAt(itemId);
            }
        }
    } else
        s += ", but misses";

    s += target.applyStates(false);
    s += this->checkStatus();

    return s;
}

QString Actor::applyState(State& state, bool consume)
{
    QString s = "";
    if (consume)
    {
        int rnd = std::rand() % 4;
        int dmghp = ((this->maxhp + rnd) * state.hpm) / 100;
        int dmgmp = ((this->maxmp + rnd) * state.mpm) / 100;
        int dmgsp = ((this->maxsp + rnd) * state.spm) / 100;
        this->hp += dmghp;
        this->mp += dmgmp;
        this->sp += dmgsp;
        bool c = false;
        if (dmghp != 0 || dmgmp != 0 || dmgsp != 0)
            s += (state.name + " causes " + this->name);
        if (dmghp != 0)
        {
            s += (" ");
            if (dmghp >= 0)
                s += "+";
            s += QString::number(dmghp) + " HP";
            c = true;
        }
        if (dmgmp != 0)
        {
            if (c)
                s += ",";
            s += " ";
            if (dmgmp >= 0)
                s += "+";
            s += QString::number(dmgmp) + " MP";
            c = true;
        }
        if (dmgsp != 0)
        {
            if (c)
                s += ",";
            s += " ";
            if (dmgsp >= 0)
                s += "+";
            s += QString::number(dmgsp) + " RP";
        }
        if (this->stateDur != NULL && this->stateDur->operator [](&state) > 0)
        {
            this->stateDur->operator [](&state)--;
        }
    }
    this->atk = this->matk + state.atkm;
    this->def = this->mdef + state.defm;
    this->spi = this->mspi + state.spim;
    this->wis = this->mwis + state.wism;
    this->agi = this->magi + state.agim;
    for (int i = 0; i < RESN; i++)
    {
        this->res[i] += state.resm[i];
        this->checkRes(i);
    }
    if (state.inactive)
    {
        this->active = false;
        this->guards = false;
    }
    if (state.reflect)
        this->reflect = true;
    if (state.automatic && this->automatic < 2)
        this->automatic = 1;
    if (state.confusion)
        this->automatic = (this->automatic < 2) ? -1 : -2;
    return s;
}

QString Actor::checkStatus()
{
    QString s = "";
    if (this->hp > this->maxhp)
        this->hp = this->maxhp;
    if (this->mp > this->maxmp)
        this->mp = this->maxmp;
    if (this->sp > this->maxsp)
        this->sp = this->maxsp;
    if (this->hp < 1)
    {
        s += " (and falls unconcious)";
        this->active = false;
        this->guards = false;
        this->sp = 0;
        if (this->state != NULL)
        {
            for (int i = 0; i < this->state->size(); i++)
                this->state->operator [](i)->remove(*this);
        }
    }
    if (this->hp < -this->maxhp)
        this->hp = -this->maxhp;
    if (this->mp < 0)
        this->mp = 0;
    if (this->sp < 0)
        this->sp = 0;
    return s;
}

QString Actor::applyStates(bool consume)
{
    QString s = "";
    if (this->automatic < 2 && this->automatic > -2)
        this->automatic = 0;
    else
        this->automatic = 2;
    if (consume && this->hp > 0)
    {
        this->active = true;
    }
    this->atk = this->matk;
    this->def = this->mdef;
    this->spi = this->mspi;
    this->wis = this->mwis;
    this->agi = this->magi;
    this->calcRes();
    this->reflect = false;
    this->guards = true;
    bool c = false;
    if (this->state != NULL)
    {
        for (int i = 0; i < this->state->size(); i++)
            if (this->stateDur->operator [](this->state->operator [](i)) != 0 && this->hp > 0)
            {
                QString r = this->applyState(*(this->state->operator [](i)), consume);
                if (r.length() > 0)
                {
                    if (c)
                        s += ", ";
                    if (consume && !c)
                    {
                        s += "\n";
                        c = true;
                    }
                    s += r;
                }
            }
    }
    s += checkStatus();
    if (c && consume)
        s += ".";
    return s;
}

Actor::~Actor()
{
    delete[] this->res;
    if (this->freeItems)
    {
        delete this->items;
    }
    if (this->state != NULL)
    {
        delete this->state;
        delete this->stateDur;
        delete this->stateRes;
    }
    if (this->extraSkills != NULL)
    {
        delete this->extraSkills;
    }
}
