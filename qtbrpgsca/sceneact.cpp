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

#include "sceneact.h"

using namespace qtbrpgsca;

SceneAct::SceneAct(Actor* party, int partyNr, bool copyParty, Actor* enemy, int enemyNr, bool copyEnemy, int surprise)
{
    int i, k;
    this->status = 0;
    this->battlerNr = partyNr + enemyNr;
    this->battler = new Actor*[this->battlerNr];
    this->bSkills = new QVector<Ability>[this->battlerNr];
    this->sprites.reserve(this->battlerNr);
    this->battlerNames.reserve(this->battlerNr);
    this->copyParty = copyParty;
    this->copyEnemy = copyEnemy;
    for (i = 0; i < partyNr; i++)
    {        
        this->battler[i] = copyParty ? new Actor(party[i]) : &party[i];
        if (surprise < 0)
        {
            this->battler[i]->active = false;
        }
        this->setBattler(i);
    }
    this->enemyIndex = i;
    this->fTarget = i;
    this->lTarget = i;
    this->current = surprise < 0 ? i : 0;
    for (i = 0; i < enemyNr; i++)
    {        
        k = i + this->enemyIndex;
        this->battler[k] = copyEnemy ? new Actor(enemy[i]) : &enemy[i];
        this->battler[k]->automatic = 2;
        if (surprise > 0)
        {
            this->battler[k]->active = false;
        }
        this->setBattler(k);
    }
    this->setCurrentActive(false);
}

void SceneAct::setBattler(int i)
{
    int j;
    this->bSkills[i].reserve(this->battler[i]->rSkillsNr + this->battler[i]->jSkillsNr
                             + this->battler[i]->extraSkills.size());
    for (j = 0; j < this->battler[i]->rSkillsNr; j++)
    {
        this->bSkills[i].append(*this->battler[i]->raceSkills[j]);
    }
    for (j = 0; j < this->battler[i]->jSkillsNr; j++)
    {
        this->bSkills[i].append(*this->battler[i]->jobSkills[j]);
    }
    for (j = 0; j < this->battler[i]->extraSkills.size(); j++)
    {
        this->bSkills[i].append(*this->battler[i]->extraSkills[j]);
    }
    this->battlerNames.append(this->battler[i]->name);
    this->sprites.append(/*this->battler[i]->jobName*/"hero");
}

bool SceneAct::setCurrentActive(bool activate)
{
    for (int i = 0; i < this->battlerNr; i++)
    {
        if (activate && this->battler[i]->hp > 0)
        {
            this->battler[i]->active = true;
        }

        if (this->current != i && (this->battler[i]->active && (!this->battler[this->current]->active ||
                this->battler[i]->agi > this->battler[this->current]->agi)))
        {
            this->current = i;
        }
    }
    this->crItems = this->battler[this->current]->items;
    if (!this->battler[this->current]->automatic)
    {
        this->itemNames.clear();
        int i;
        if (this->crItems != NULL)
        {
            for (i = 0; i < this->crItems->size(); i++)
            {
                this->itemNames.append((*this->crItems)[i].name);
            }
        }
        this->skillNames.clear();
        for (i = 0; i < this->bSkills[this->current].size(); i++)
        {
            this->skillNames.append(this->bSkills[this->current][i].name);
        }
    }
    return this->battler[this->current]->active;
}

QString SceneAct::endTurn(QString ret)
{
    if (this->status == 0)
    {
        do
        {
            this->battler[this->current]->active = false;
            if (!(this->setCurrentActive(false)))
            {
                this->setCurrentActive(true);
            }
            ret += this->battler[this->current]->applyStates(true);
    
            bool noParty = true;
            bool noEnemy = true;
            for (int i = 0; i < this->battlerNr; i++)
            {
                if (this->battler[i]->hp > 0)
                {
                    if (i < this->enemyIndex)
                    {
                        noParty = false;
                        i = this->enemyIndex - 1;
                    }
                    else
                    {
                        noEnemy = false;
                        break;
                    }
                }
            }
    
            if (noEnemy)
            {
                this->status = 1;
                ret += "";
            }
    
            if (noParty)
            {
                this->status = -2;
                ret += "";
            }
        } while (this->status == 0 && !(this->battler[this->current]->active));
    }

    /*if (this->status == 0 && this->battler[this->current]->automatic)
    {
        ret += this->executeAI(ret);
    }*/

    return ret;
}

QString SceneAct::executeAbility(Ability& skill, int target, QString ret)
{
    switch (skill.trg)
    {
        default:
            this->fTarget = target;
            this->lTarget = target;
            break;
        case -1:
            this->fTarget = this->current;
            this->lTarget = this->current;
            break;
        case 1:
            if (target < this->enemyIndex)
            {
                this->fTarget = 0;
                this->lTarget = enemyIndex - 1;
            }
            else
            {
                this->fTarget = enemyIndex;
                this->lTarget = this->battlerNr - 1;
            }
            break;
        case 2:
            this->fTarget = 0;
            this->lTarget = this->battlerNr - 1;
            break;
        case -2:
            if (this->current < this->enemyIndex)
            {
                this->fTarget = 0;
                this->lTarget = enemyIndex - 1;
            }
            else
            {
                this->fTarget = enemyIndex;
                this->lTarget = this->battlerNr - 1;
            }
            break;
    }

    for (int i = this->fTarget; i <= this->lTarget; i++)
    {
        ret += this->battler[this->current]->execAbility(skill, *(this->battler[i]), (i == this->fTarget));
    }

    //ret += this->endTurn(ret);

    return ret;
}

bool SceneAct::checkIfAI()
{
    return this->battler[this->current]->automatic != 0;
}

QString SceneAct::executeAI(QString ret)
{
    return this->setAItarget(this->getAIskill(this->checkAIheal(-1)), ret);
}

int SceneAct::checkAIheal(int ret) {
    bool nHeal = false;
    int f;
    int l;
    if (this->current < this->enemyIndex)
    {
        f = this->enemyIndex;
        l = this->battlerNr;
    }
    else
    {
        f = 0;
        l = this->enemyIndex;
    }
    for (int i = f; i < l; i++)
    {
        if (this->battler[i]->hp < (this->battler[i]->maxhp / 3))
        {
            nHeal = true;
            break;
        }
    }
    if (nHeal)
    {
        for (int i = 0; i < bSkills[this->current].size(); i++)
        {
            Ability& s = this->bSkills[this->current][i];
            if (s.hpdmg < 0 && s.mpc <= this->battler[this->current]->mp
                    && s.hpc <= this->battler[this->current]->hp
                    && s.spc <= this->battler[this->current]->sp
                    && this->battler[this->current]->lv >= s.lvrq)
            {
                ret = i;
                break;
            }
        }
    }
    else
    {
        ret = 0;
    }
    return ret;
}

Ability& SceneAct::getAIskill(int healSkill)
{
    if (healSkill < 0)
        healSkill = 0;
    Ability* ret = &this->bSkills[this->current][healSkill];
    for (int i = healSkill + 1; i < this->bSkills[this->current].size(); i++)
    {
        Ability* a = &this->bSkills[this->current][i];
        if (a->mpc <= this->battler[this->current]->mp
                && a->hpc < this->battler[this->current]->hp
                && a->spc <= this->battler[this->current]->sp
                && this->battler[this->current]->lv >= a->lvrq)
        {
            if (healSkill > 0)
            {
                if (a->hpdmg < ret->hpdmg && ret->hpdmg < 0)
                    ret = a;
            } else if (a->hpdmg > ret->hpdmg)
                ret = a;
        }
    }
    return *ret;
}

QString SceneAct::setAItarget(Ability& ability, QString ret)
{
    int f;
    int l;
    if (this->current < this->enemyIndex && !ability.hpdmg < 0)
    {
        f = this->enemyIndex;
        l = this->battlerNr;
    }
    else
    {
        f = 0;
        l = this->enemyIndex;
    }
    int target = f;

    while ((this->battler[target]->hp < 1) && (ability.hpdmg > 1) && target < l)
        target++;

    for (int i = target; i < l; i++)
    {
        if (this->battler[i]->hp < this->battler[target]->hp
                && ((this->battler[i]->hp > 0 && !ability.hpdmg < 0) || ability.hpdmg < 0))
        {
            target = i;
        }
    }
    return this->executeAbility(ability, target, ret);
}

QString SceneAct::performSkill(int index, int target, QString ret)
{
    return ret += this->executeAbility(this->bSkills[this->current][index], target, ret);
}

QString SceneAct::useItem(int index, int target, QString ret)
{
    return ret += this->executeAbility((*this->crItems)[index], target, ret);
}

int SceneAct::getCurrent()
{
    return this->current;
}

int SceneAct::getEnemyIndex()
{
    return this->enemyIndex;
}

int SceneAct::getFirstTarget()
{
    return this->fTarget;
}

int SceneAct::getLastTarget()
{
    return this->lTarget;
}

int SceneAct::getStatus()
{
    return this->status;
}

int SceneAct::getBattlerNr()
{
    return this->battlerNr;
}

QStringList SceneAct::getBattlerNames()
{
    return this->battlerNames;
}

QString SceneAct::getBattlerDesc(int i) {
    return this->battlerNames[i] + ", Lv:" + QString::number(this->battler[i]->lv)
            + "\nHP:" + QString::number(this->battler[i]->hp)
            + "/" + QString::number(this->battler[i]->maxhp)
            + ", MP:" + QString::number(this->battler[i]->mp)
            + "/" + QString::number(this->battler[i]->maxmp)
            + ",\nRP:" + QString::number(this->battler[i]->sp)
            + "/" + QString::number(this->battler[i]->maxsp)
            + ", XP:" + QString::number(this->battler[i]->exp)
            + "/" + QString::number(this->battler[i]->maxp);
}

QStringList SceneAct::getCrSkillsNames()
{
    return this->skillNames;
}

QString SceneAct::getCrSkillDesc(int i) {
    return this->skillNames[i]
            + (this->bSkills[this->current][i].mqty < 1 ? ""
                : " * " + QString::number(this->bSkills[this->current][i].qty))
            + ", Lv:" + QString::number(this->bSkills[this->current][i].lvrq)
            + "\nHPc:" + QString::number(this->bSkills[this->current][i].hpc)
            + ", MPc:" + QString::number(this->bSkills[this->current][i].mpc)
            + ", RPc:" + QString::number(this->bSkills[this->current][i].spc)
            + "\nHPd:" + QString::number(this->bSkills[this->current][i].hpdmg)
            + ", MPd:" + QString::number(this->bSkills[this->current][i].mpdmg)
            + ", RPd:" + QString::number(this->bSkills[this->current][i].spdmg);
}

bool SceneAct::checkCrSkill(int i) {
    return this->battler[this->current]->lv >= this->bSkills[this->current][i].lvrq
            && this->battler[this->current]->hp >= this->bSkills[this->current][i].hpc
            && this->battler[this->current]->mp >= this->bSkills[this->current][i].mpc
            && this->battler[this->current]->sp >= this->bSkills[this->current][i].spc
            && (this->bSkills[this->current][i].mqty < 1 || this->bSkills[this->current][i].qty > 0);
}

QStringList SceneAct::getCrItemsNames()
{
    return this->itemNames;
}

QString SceneAct::getCrItemDesc(int i) {
    return this->itemNames[i]
            + " * " + QString::number((*this->battler[this->current]->items)[i].qty)
            + "\nHPc:" + QString::number((*this->battler[this->current]->items)[i].hpc)
            + ", MPc:" + QString::number((*this->battler[this->current]->items)[i].mpc)
            + ", RPc:" + QString::number((*this->battler[this->current]->items)[i].spc)
            + "\nHPd:" + QString::number((*this->battler[this->current]->items)[i].hpdmg)
            + ", MPd:" + QString::number((*this->battler[this->current]->items)[i].mpdmg)
            + ", RPd:" + QString::number((*this->battler[this->current]->items)[i].spdmg);
}

bool SceneAct::checkCrItem(int i) {
    return this->battler[this->current]->lv >= (*this->battler[this->current]->items)[i].lvrq
            && this->battler[this->current]->hp >= (*this->battler[this->current]->items)[i].hpc
            && this->battler[this->current]->mp >= (*this->battler[this->current]->items)[i].mpc
            && this->battler[this->current]->sp >= (*this->battler[this->current]->items)[i].spc
            && ((*this->battler[this->current]->items)[i].qty < 0
                || (*this->battler[this->current]->items)[i].qty > 0);
}

QStringList SceneAct::getSprites()
{
    return this->sprites;
}

bool SceneAct::checkIfKO(int actor)
{
    return actor < this->battlerNr && this->battler[actor]->hp < 1;
}

bool SceneAct::checkIfReflects(int user, int skill, int target)
{
    return user < this->battlerNr && target < this->battlerNr && skill < this->bSkills[user].size()
            && this->battler[target]->reflect && this->bSkills[user][skill].dmgtype == 2;
}

SceneAct::~SceneAct()
{
    if (this->copyParty)
    {
        for (int i = 0; i < this->enemyIndex; i++)
        {
            delete this->battler[i];
        }
    }
    if (this->copyEnemy)
    {
        for (int i = this->enemyIndex; i < this->battlerNr; i++)
        {
            delete this->battler[i];
        }
    }
    delete[] this->battler;
    delete this->bSkills;
}
