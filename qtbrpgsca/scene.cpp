/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#include "scene.h"
#include "ability.h"
#include "costume.h"
#include "state.h"
#include "scene.h"
#include "role.h"

using namespace tbrpgsca;

QString Scene::PerformsTxt = "%1 performs %2";
QString Scene::VictoryTxt = "The party has won!";
QString Scene::FallenTxt = "The party has fallen!";
QString Scene::EscapeTxt = "The party has escaped!";
QString Scene::FailTxt = "The party attempted to escape, but failed.";

bool Scene::actorAgiComp(Actor* const a, Actor* const b)
{
    return (a->_agi > b->_agi);
}

int Scene::getCurrent() const
{
    return this->_current;
}

int Scene::getStatus() const
{
    return this->_status;
}

Actor* Scene::getCurrentPlayer() const
{
    return this->_cr_actor;
}

int Scene::getCurrentParty() const
{
    Actor* const crActor = this->_cr_actor;
    return crActor == nullptr ? -1 : crActor->_side;
}

Ability* Scene::getLastAbility() const
{
    return this->_last_ability;
}

int Scene::getPartiesSize() const
{
    return this->_parties.size();
}

int Scene::getPartyPlayersSize(const int party) const
{
    return this->_parties[party]->size();
}

Actor& Scene::getPartyPlayer(const int party, const int player) const
{
    return *(this->_parties[party]->at(player));
}

bool Scene::hasPartyPlayer(int const party, Actor& player) const
{
    return this->_parties[party]->contains(&player);
}

Actor& Scene::getOrderedPlayer(int const n) const
{
    return *(this->_players->at(n));
}

bool Scene::hasOrderedPlayer(Actor& player) const
{
    QVector<Actor*>* players = this->_players;
    return players && players->contains(&player);
}

int Scene::getOrderedPlayersSize() const
{
    QVector<Actor*>* players = this->_players;
    return players == nullptr ? 0 : players->size();
}

int Scene::getTargetedPlayersSize() const
{
    QVector<Actor*>* players = this->_targets;
    return players == nullptr ? 0 : players->size();
}

Actor& Scene::getTargetedPlayer(int const n) const
{
    return *(this->_targets->at(n));
}

bool Scene::hasTargetedPlayer(Actor& player) const
{
    QVector<Actor*>* players = this->_targets;
    return players && players->contains(&player);
}

Actor* Scene::getGuardian(Actor& user, Actor* target, Ability& skill) const
{
    int const side = target->_old_side;
#if ALLOW_NO_GUARDS
    if (this->usesGuards())
#endif
    {
        if (user._old_side != side && ((!skill.isRanged()) && ((!user.Role::isRanged()) || skill.isOnlyMelee())))
        {
            int pos = -1;
            Actor* fGuard = nullptr,* lGuard = nullptr;
            Actor** guardPos = &fGuard;
            QVector<Actor*>& party = *(this->_parties[side]);
            int const pSize = party.size();
            for (int i = 0; i < pSize; ++i)
            {
                Actor* const guardian = party[i];
                if (guardian == target)
                {
                    if (fGuard == nullptr || i == pSize - 1)
                    {
#if ALLOW_COVERING
                        goto coverCheck;
#else
                        return target;
#endif
                    }
                    else
                    {
                        pos = i;
                        guardPos = &lGuard;
                        continue;
                    }
                }
                else if ((fGuard == nullptr || pos != -1) && guardian->_hp > 0 && (!guardian->hasAnyFlag(FLAG_STUN | FLAG_CONFUSE | FLAG_CONVERT))
                         /*(!guardian->Costume::isStunned()) && (!guardian->Costume::isConfused())*/)
                {
                    (*guardPos) = guardian;
                }
            }
            if (fGuard && lGuard)
            {
#if ALLOW_COVERING
                target = ((pos < (pSize / 2)) ? fGuard : lGuard);
#else
                return ((pos < (pSize / 2)) ? fGuard : lGuard);
#endif
            }
        }
    }
#if ALLOW_COVERING
    coverCheck:
    if (this->hasCovers() && (skill._dmg_type & DMG_TYPE_ATK) == DMG_TYPE_ATK && target->_hp < target->_m_hp / 3)
    {
        Actor* coverer = nullptr;
        QVector<Actor*>& party = *(this->_parties[side]);
        for (Actor* const actor : party)
        {
            if (actor != target && actor->Costume::isCovering() && (coverer == nullptr || (actor->_hp > coverer->_hp)))
            {
                coverer = actor;
            }
        }
        if (coverer)
        {
            return coverer;
        }
    }
#endif
    return target;
}

void Scene::checkStatus(QString& ret)
{
    Scene& scene = *this;
    if (scene._status == 0)
    {
        auto& parties = scene._parties;
        auto party = parties[0];
        int partySize = party->size();
        {
            for (int k = 0; k < partySize; ++k)
            {
                if (!(party->at(k)->Costume::isKnockedOut()))
                {
                    goto enemyCheck;
                }
            }
            scene._status = -2;
            ret += Scene::FallenTxt;
            return;
        }
        enemyCheck:
        {
            int const partiesSize = parties.size();
            for (int i = 1; i < partiesSize; ++i)
            {
                party = parties[i];
                partySize = party->size();
                for (int j = 0; j < partySize; ++j)
                {
                    if (!(party->at(j)->Costume::isKnockedOut()))
                    {
                        return;
                    }
                }
            }
            ret += Scene::VictoryTxt;
            scene._status = 1;
        }
    }

}

void Scene::execute(QString& ret, Actor& user, Actor* const target, Ability& ability, bool const applyCosts)
{
    Scene& scene = *this;
    bool const ko = target->_hp < 1;
    //bool const healing = ability.hp < 0;
    if ((/*healing &&*/ ability.isReviving()) || !ko)
    {
        Ability* counter = nullptr;
        ability.execute(ret, this, user, target, applyCosts);
        if (ability._hp > -1)
        //if (!healing)
        {
            int cntSize;
            QVector<Ability*>* const counters = target->_counters;
            if (counters && (cntSize = counters->size()) > 0 && (!target->Costume::isStunned())
                    && (target->_side != user._side || target->Costume::isConfused()))
            {
                int const usrDmgType = ability._dmg_type;
                for (int i = 0; i < cntSize; ++i)
                {
                    Ability* const cntSkill = counters->at(i);
                    int cntDmgType = cntSkill->_dmg_type;
                    if (((usrDmgType & cntDmgType) == cntDmgType) && (counter == nullptr || (cntSkill->_hp > counter->_hp)))
                    {
                        counter = cntSkill;
                    }
                }
                if (counter)
                {
                    counter->execute(ret, *target, user, false);
                }
            }
        }
        SpriteCall* const actorEvent = scene._actor_run;
        if (actorEvent == nullptr || ((*actorEvent)(scene, applyCosts ? &user : nullptr, &ability, (ko && target->_hp > 0),
                                                    target, &user == target ? &ability : counter)))
        {
            QVector<Actor*>* targets = scene._targets;
            if (targets == nullptr)
            {
                targets = new QVector<Actor*>(scene._parties[target->_old_side]->size());
                scene._targets = targets;
            }
            targets->append(target);
        }
    }

}

void Scene::perform(QString& ret, Actor& user, Actor& target, Ability& ability, bool const item)
{
    Scene& scene = *this;
    ret += Scene::PerformsTxt.arg(user._name, ability._name);
    {
        QVector<Actor*>* const targets = scene._targets;
        if (targets)
        {
            targets->clear();
        }
    }
    QVector<SceneRun*>* events = scene._events;
    if (events && events->size() > EVENT_BEFORE_ACT)
    {
        auto event = events->at(EVENT_BEFORE_ACT);
        if (event && !((*event)(scene, &ret)))
        {
            return;
        }
    }
    if (ability.isRanged() && ability.targetsAll())
    {
        bool applyCosts = true;
        int const usrSide = user._side;
        bool const sideTarget = ability.targetsSide();
        bool const noSelfTarget = !ability.targetsSelf();
        QVector<QVector<Actor*>*>& parties = scene._parties;
        int const pSize = parties.size();
        for (int j = 0; j < pSize; ++j)
        {
            if (sideTarget && noSelfTarget && j == usrSide)
            {
                continue;
            }
            QVector<Actor*>* const players = parties[j];
            int const sSize = players->size();
            for (int i = 0; i < sSize; ++i)
            {
                Actor* const trg = players->at(i);
                if (noSelfTarget && trg == &user)
                {
                    continue;
                }
                else
                {
                    scene.execute(ret, user, trg, ability, applyCosts);
                }
                applyCosts = false;
            }
        }
    }
    else if (ability.targetsSide())
    {
        //QVector<Actor*>* const targets = scene.targets;
        int const side = ability.targetsSelf() ? user._side : target._old_side;
        QVector<Actor*>& party = *(scene._parties[side]);
        int const pSize = party.size();
        for (int i = 0; i < pSize; ++i)
        {
            /*Actor* const trg = party[i];
            if (targets);
            if (targets)
            {
                targets->append(trg);
            }*/
            scene.execute(ret, user, party[i], ability, i == 0);
        }
    }
    else
    {
        scene.execute(ret, user, &user == &target || ability.targetsSelf() ? &user
                     : (scene.getGuardian(user, &target, ability)), ability, true);
    }
    if (item)
    {
        QMap<Ability*, int>* const items = user._items;
        if (items)
        {
            items->operator[](&ability) = items->value(&ability, 1) - 1;
        }
    }
    this->_last_ability = &ability;
    user.setExperience(this, user._xp + 1);
    if (events && events->size() > EVENT_AFTER_ACT)
    {
        auto event = events->at(EVENT_AFTER_ACT);
        if (event && ((*event)(scene, &ret)))
        {
            scene.endTurn(ret, &user);
        }
    }

}

int Scene::getAiSkill(Actor& user, QVector<Ability*>& skills, int const defSkill, bool const restore) const
{
    Ability* s = skills[defSkill];
    int ret = defSkill, sSize = skills.size();
    for (int i = defSkill + 1; i < sSize; ++i)
    {
        Ability* a = skills[i];
        if (a->canPerform(user) && ((defSkill > 0 && (a->_hp < s->_hp)
            && (a->isReviving() || !restore)) || (a->_hp > s->_hp)))
        {
            ret = i;
            s = a;
        }
    }
    return ret;
}

void Scene::playAi(QString& ret, Actor& player)
{
    Scene& scene = *this;
    QVector<Actor*>* party;
    QVector<Ability*>& skills = *(player._a_skills);
    QVector<QVector<Actor*>*>& parties = scene._parties;
    int side, sSize, skillIndex = 0, heal = -1, pSize = parties.size();
    if (player.Costume::isConfused())
    {
        party = nullptr;
        side = -1;
    }
    else
    {
        side = player._side;
        party = parties[side];
        sSize = party->size();
        if (!(player.Costume::isEnraged()))
        {
            for (int i = 0; i < sSize; ++i)
            {
                Actor& iPlayer = *(party->at(i));
                int iHp = iPlayer._hp;
                if (iHp < 1)
                {
                    heal = 1;
                }
                else if (iHp < (iPlayer._m_hp / 3))
                {
                    heal = 0;
                }
            }
        }
        if (heal > -1)
        {
            int const skillsSize = skills.size();
            for (int i = 0; i < skillsSize; ++i)
            {
                Ability& s = *(skills[i]);
                if (s.canPerform(player) && (s._hp < 0 && ((heal == 0) || s.isReviving())))
                {
                    skillIndex = i;
                    break;
                }
            }
        }
    }
    {
        Actor* target = nullptr;
        //Ability& ability = scene.getAiSkill(player, skills, skillIndex, heal == 1);
        Ability& ability = *(skills[scene.getAiSkill(player, skills, skillIndex, heal == 1)]);
        if (ability._hp > -1)
        {
            if (party == nullptr || player.isRandomAi())
            {
                {
                    int trgSide;
                    if (side > -1 && pSize == 2)
                    {
                        trgSide = side == 0 ? 1 : 0;
                    }
                    else
                    {
                        trgSide = rand() % pSize;
                        if (side == trgSide && (++trgSide == pSize))
                        {
                            trgSide = 0;
                        }
                    }
                    party = parties[trgSide];
                    sSize = party->size();
                }
                int trg = rand() % sSize;
                target = party->at(trg);
                while (target->Costume::isKnockedOut())
                {
                    if (++trg == sSize)
                    {
                        trg = 0;
                    }
                    target = party->at(trg);
                }
            }
            else
            {
                for (int j = 0; j < pSize; ++j)
                {
                    if (j == side)
                    {
                        continue;
                    }
                    int trg = 0;
                    QVector<Actor*>* const players = parties[j];
                    sSize = players->size();
                    if (target == nullptr)
                    {
                        do
                        {
                            target = players->at(trg);
                        } while (((++trg) < sSize) && (target->Costume::isKnockedOut() || target->_side == side));
                    }
                    for (int i = trg + 1; i < sSize; ++i)
                    {
                        Actor* const iPlayer = players->at(i);
                        if (iPlayer->_side != side && (!iPlayer->Costume::isKnockedOut()) && iPlayer->_hp < target->_hp)
                        {
                            target = iPlayer;
                        }
                    }
                }
            }
        }
        else
        {
            if (party == nullptr)
            {
                party = parties[rand() % pSize];
                sSize = party->size();
            }
            target = party->at(0);
            bool const restore = ability.isReviving();
            for (int i = 1; i < sSize; ++i)
            {
                Actor* const iPlayer = party->at(i);
                int const iHp = iPlayer->_hp;
                if (iHp < target->_hp && (restore || iHp > 0))
                {
                    target = iPlayer;
                }
            }
        }
        if (target == nullptr)
        {
            scene.perform(ret, player, player, heal < 0 ? *(skills[scene.getAiSkill
                          (player, skills, 1, false)]) : ability, false);
        }
        else
        {
            scene.perform(ret, player, *target, ability, false);
        }
    }

}

void Scene::endTurn(QString& ret, Actor* crActor)
{
    Scene& scene = *this;
    int current = scene._current;
    if (crActor == nullptr)
    {
        crActor = scene._cr_actor;
    }
    int cActions = --(crActor->_actions);
    while (cActions < 1)
    {
        if (crActor->_hp > 0)
        {
            crActor->applyStates(&ret, this, true);
        }
        int mInit = scene._m_init;
        if (mInit > 0)
        {
            int cInit = crActor->_init - mInit;
            crActor->_init = cInit;
            do
            {
                //QVector<Actor*>* const ordered = scene.players;
                //if (ordered == nullptr)
                {
                    QVector<QVector<Actor*>*>& parties = scene._parties;
                    int const pSize = parties.size();
                    for (int j = 0; j < pSize; ++j)
                    {
                        QVector<Actor*>& players = *(parties[j]);
                        int sSize = players.size();
                        for (int i = 0; i < sSize; ++i)
                        {
                            Actor* const iPlayer = players[i];
                            if (iPlayer->_hp > 0)
                            {
                                int const iInit = iPlayer->_init + iPlayer->_agi;
                                iPlayer->_init = iInit;
                                if (iInit > cInit)
                                {
                                    cInit = iInit;
                                    crActor = iPlayer;
                                    current = i;
                                }
                            }
                        }
                    }
                }
                /*else
                {
                    int const oSize = ordered->size();
                    for (int i = 0; i < oSize; ++i)
                    {
                        Actor* const iPlayer = ordered->at(i);
                        if (iPlayer->hp > 0)
                        {
                            int iInit = iPlayer->init + iPlayer->agi;
                            iPlayer->init = iInit;
                            if (iInit > cInit)
                            {
                                cInit = iInit;
                                crActor = iPlayer;
                                current = i;
                            }
                        }
                    }
                }*/
            }
            while (cInit < mInit);
        }
        else
        {
            current = scene._original;
            QVector<Actor*>& players = *(scene._players);
            int playersSize = players.size();
            int nInit = mInit - 1;
            if (nInit == MIN_ROUND)
            {
                scene._m_init = mInit = 0;
                for (int i = 0; i < playersSize; ++i)
                {
                    players[i]->_init = 0;
                }
                nInit = -1;
            }
            crActor->_init = nInit;
            do
            {
                if (++current == playersSize)
                {
                    scene._m_init = mInit = nInit;
                    current = 0;
                }
                crActor = players[current];
            }
            while (crActor->_hp < 1 || crActor->_init < mInit);
        }
        //crActor->actions = cActions = crActor->mActions;
        QMap<Ability*, int>* const regSkills = crActor->_skills_rg_turn;
        if (regSkills)
        {
            QMap<Ability*, int>* skillsQty = crActor->_skills_cr_qty;
            if (skillsQty == nullptr)
            {
                skillsQty = new QMap<Ability*, int>();
                crActor->_skills_cr_qty = skillsQty;
            }
            auto const last = regSkills->cend();
            for (auto it = regSkills->cbegin(); it != last; ++it)
            {
                Ability* const skill = it.key();
                int const skillMaxQty = skill->_m_qty, skillCrQty = skillsQty->value(skill, skillMaxQty);
                if (skillCrQty < skillMaxQty)
                {
                    int const skillRgTurn = it.value();//regSkills->value(skill, 0);
                    if (skillRgTurn == skill->_r_qty)
                    {
                        skillsQty->operator[](skill) = skillCrQty + 1;
                        regSkills->operator[](skill) = 0;
                    }
                    else
                    {
                        regSkills->operator[](skill) = skillRgTurn + 1;
                    }
                }
            }
        }
        /*if (crActor->isStunned())
        {
            crActor->actions = cActions = 0;
        }*/
        {
            bool const shapeShifted = crActor->Costume::isShapeShifted();
            crActor->applyStates(&ret, this, false);
            if (shapeShifted && (!crActor->Costume::isShapeShifted()))
            {
                SpriteCall* const actorEvent = scene._actor_run;
                if (actorEvent)
                {
                    ((*actorEvent)(scene, crActor, nullptr, true, nullptr, nullptr));
                }
            }
        }
        crActor->_actions = cActions = crActor->Costume::isStunned() ? 0 : crActor->_m_actions;
    }
    scene._cr_actor = crActor;
    scene._current = current;
    scene._original = current;
    QVector<SceneRun*>* const events = scene._events;
    if (events && events->size() > EVENT_NEW_TURN)
    {
        auto event = events->at(EVENT_NEW_TURN);
        if (event && (*event)(scene, &ret) && crActor->hasAnyFlag(FLAG_AI_PLAYER | FLAG_ENRAGED | FLAG_CONFUSE)
                /*(crActor->isAiPlayer() || crActor->Costume::isEnraged() || crActor->Costume::isConfused())*/)
        {
            scene.playAi(ret, (*crActor));
        }
    }
    ret += ".";

}

bool Scene::canTarget(Actor& user, Ability& ability, Actor& target)
{
    return ability.canPerform(user) && (ability.targetsSelf() || ((target._hp > 0 || ability.isReviving())
            && ((this->getGuardian(user, &target, ability))) == &target));
}

void Scene::agiCalc()
{
    if (this->_m_init < 1)
    {
        QVector<Actor*>& players = *(this->_players);
        std::sort(players.begin(), players.end(), Scene::actorAgiComp);
        this->_original = -1;
    }
}

void Scene::resetTurn(Actor& actor)
{
    int const mInit = this->_m_init + 1;
    if (mInit < 2)
    {
        if (actor._init > mInit)
        {
            actor._init = mInit;
        }
        else if (mInit == 1 && actor._init < -1)
        {
            actor._init = 0;
        }
        this->_original = -1;
    }
}

void Scene::operator()(QString& ret, QVector<QVector<Actor*>*>& parties, SpriteCall* const actorEvent, QVector<SceneRun*>* const events,
                         bool const useGuards, int const surprise, int const mInit)
{
    int partiesSize = parties.size();
    assert(partiesSize > 1);
    QVector<Actor*>* players;
    Scene& scene = *this;
    bool useInit;
    if (mInit > 0)
    {
        scene._m_init = mInit;
        players = nullptr;
        useInit = true;
    }
    else
    {
        players = new QVector<Actor*>();
        scene._m_init = 0;
        useInit = false;
    }
    int current = 0;
    scene._events = events;
    scene._parties = parties;
    scene._players = players;
    scene._targets = nullptr;
    scene._actor_run = actorEvent;
    //scene.current = scene.oldCurrent = 0;
    scene._surprise = surprise;
    Actor* crActor = nullptr;
    for (int i = 0; i < partiesSize; ++i)
    {
        bool const aiPlayer = i > 0;
        bool const surprised = (surprise == i);
        QVector<Actor*>& party = *(parties[i]);
        int const pSize = party.size();
        if (crActor == nullptr)
        {
            crActor = party[0];
        }
        for (int j = 0; j < pSize; ++j)
        {
            Actor& player = *(party[j]);
            player._actions = 0;
            if (surprised)
            {
                player._init = useInit ? -(mInit + 1) : -1;
            }
            else
            {
                player._init = 0;
                if (player._agi > crActor->_agi || crActor->_init < 0)
                {
                    crActor = &player;
                    if (useInit)
                    {
                        current = j;
                    }
                }
            }
            if (aiPlayer)
            {
                //player.setRandomAi(true);
                player.setAiPlayer(true);
            }
            player._old_side = i;
            player._side = i;
#if ALLOW_COVERING
            if (player.Costume::isCovering())
            {
                scene.setHasCovers(true);
            }
#endif
        }
        if (players)
        {
            players->append(party);
        }
    }
#if ALLOW_NO_GUARDS
    scene.setUseGuards(useGuards);
#endif
    scene._cr_actor = crActor;
    scene.agiCalc();
    if (useInit)
    {
        crActor->_init = mInit;
    }
    else
    {
        current = players->indexOf(crActor);
    }
    SceneRun* event;
    scene._current = current;
    scene._original = current;
    if (events && events->size() > EVENT_BEGIN_SCENE && ((event = events->at(EVENT_BEGIN_SCENE))) && (*event)(scene, &ret))
    {
        scene.endTurn(ret, crActor);
    }

}

Scene::Scene() : Play(0)
{

}

Scene::Scene(QString& ret, QVector<QVector<Actor*>*>& parties, SpriteCall* const actorEvent, QVector<SceneRun*>* const events,
             bool const useGuards, int const surprise, int const mInit) : Play(0)
{
    this->operator()(ret, parties, actorEvent, events, useGuards, surprise, mInit);
}

Scene::~Scene()
{
    auto players = this->_players;
    if (players)
    {
        this->_players = nullptr;
        delete players;
    }
}
