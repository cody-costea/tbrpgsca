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
    return (a->agility() > b->agility());
}

int Scene::getCurrent() const
{
    return this->_scene_data->_current;
}

int Scene::getStatus() const
{
    return this->_scene_data->_status;
}

Actor* Scene::getCurrentPlayer() const
{
    return this->_scene_data->_cr_actor;
}

int Scene::getCurrentParty() const
{
    Actor* const crActor = this->_scene_data->_cr_actor;
    return crActor == NIL ? -1 : crActor->partySide();
}

Ability* Scene::getLastAbility() const
{
    return this->_scene_data->_last_ability;
}

int Scene::getPartiesSize() const
{
    return this->_scene_data->_parties.size();
}

int Scene::getPartyPlayersSize(const int party) const
{
    return this->_scene_data->_parties[party]->size();
}

Actor& Scene::getPartyPlayer(const int party, const int player) const
{
    return *(this->_scene_data->_parties[party]->at(player));
}

bool Scene::hasPartyPlayer(int const party, Actor& player) const
{
    return this->_scene_data->_parties[party]->contains(&player);
}

Actor& Scene::getOrderedPlayer(int const n) const
{
    return *(this->_scene_data->_players->at(n));
}

bool Scene::hasOrderedPlayer(Actor& player) const
{
    QVector<Actor*>* players = this->_scene_data->_players;
    return players && players->contains(&player);
}

int Scene::getOrderedPlayersSize() const
{
    QVector<Actor*>* players = this->_scene_data->_players;
    return players == NIL ? 0 : players->size();
}

#if USE_TARGET_LIST
int Scene::getTargetedPlayersSize() const
{
    QVector<Actor*>* players = this->_scene_data->_targets;
    return players == NIL ? 0 : players->size();
}

Actor& Scene::getTargetedPlayer(int const n) const
{
    return *(this->_scene_data->_targets->at(n));
}

bool Scene::hasTargetedPlayer(Actor& player) const
{
    QVector<Actor*>* players = this->_scene_data->_targets;
    return players && players->contains(&player);
}
#endif

Actor* Scene::getGuardian(Actor* const user, Actor* target, Ability* const skill) const
{
    assert(user && skill);
    int const side = target->_actor_data->_old_side;
#if ALLOW_NO_GUARDS
    if (this->usesGuards())
#endif
    {
        if (user->_actor_data->_old_side != side && ((!skill->isRanged()) && ((!user->Role::isRanged()) || skill->isOnlyMelee())))
        {
            int pos = -1;
            Actor* fGuard = NIL,* lGuard = NIL;
            Actor** guardPos = &fGuard;
            QVector<Actor*>& party = *(this->_scene_data->_parties[side]);
            int const pSize = party.size();
            for (int i = 0; i < pSize; ++i)
            {
                Actor* const guardian = party[i];
                if (guardian == target)
                {
                    if (fGuard == NIL || i == pSize - 1)
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
                else if ((fGuard == NIL || pos != -1) && guardian->currentHp() > 0
                         && (!guardian->hasAnyPlayFlag(Costume::Attribute::Stun | Costume::Attribute::Confuse | Costume::Attribute::Convert))
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
    if (this->hasCovers())
    {
        int cvrType = target->coverType();
        if (this->hasCovers() && (skill->damageType() & cvrType) == cvrType && target->currentHp() < target->maximumHp() / 3)
        {
            Actor* coverer = NIL;
            QVector<Actor*>& party = *(this->_scene_data->_parties[side]);
            for (Actor* const actor : party)
            {
                if (actor != target && actor->Costume::isCovering() && (coverer == NIL || (actor->currentHp() > coverer->currentHp())))
                {
                    coverer = actor;
                }
            }
            if (coverer)
            {
                return coverer;
            }
        }
    }
#endif
    return target;
}

void Scene::checkStatus(QString* const ret)
{
    assert(ret);
    auto& scene = this->_scene_data;
    if (scene->_status == 0)
    {
        auto& parties = scene->_parties;
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
            scene->_status = -2;
            ret->append(Scene::FallenTxt);
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
            ret->append(Scene::VictoryTxt);
            scene->_status = 1;
        }
    }

}

void Scene::execute(QString& ret, Actor& user, Actor* const target, Ability& ability, bool const applyCosts)
{
    auto& scene = this->_scene_data;
    bool const ko = target->currentHp() < 1;
    //bool const healing = ability.hp < 0;
    if ((/*healing &&*/ ability.isReviving()) || !ko)
    {
        Ability* counter = NIL;
        ability.execute(ret, this, user, target, applyCosts);
        if (ability.currentHp() > -1)
        //if (!healing)
        {
            int cntSize;
            QVector<Ability*>* const counters = target->_costume_data->_counters;
            if (counters && (cntSize = counters->size()) > 0 && (!target->Costume::isStunned())
                    && (target->partySide() != user.partySide() || target->Costume::isConfused()))
            {
                int const usrDmgType = ability.damageType();
                for (int i = 0; i < cntSize; ++i)
                {
                    Ability* const cntSkill = counters->at(i);
                    int cntDmgType = cntSkill->damageType();
                    if (((usrDmgType & cntDmgType) == cntDmgType) && (counter == NIL || (cntSkill->currentHp() > counter->currentHp())))
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
        emit this->spriteAct(this, applyCosts ? &user : NIL, &ability, (ko && target->currentHp() > 0),
                       target, &user == target ? &ability : counter);
#if USE_TARGET_LIST
        QVector<Actor*>* targets = scene->_targets;
        if (targets == NIL)
        {
            targets = new QVector<Actor*>(scene->_parties[target->_actor_data->_old_side]->size());
            scene->_targets = targets;
        }
        targets->append(target);
#endif
    }

}

void Scene::perform(QString* const ret, Actor* const user, Actor* const target, Ability* const ability, bool const item)
{
    assert(ret && user && target && ability);
    ret->append(Scene::PerformsTxt.arg(user->name(), ability->name()));
    auto& scene = this->_scene_data;
#if USE_TARGET_LIST
    {
        QVector<Actor*>* const targets = scene->_targets;
        if (targets)
        {
            targets->clear();
        }
    }
#endif
    QVector<SceneRun*>* events = scene->_events;
    if (events && events->size() > EVENT_BEFORE_ACT)
    {
        auto event = events->at(EVENT_BEFORE_ACT);
        if (event && !((*event)(*this, ret)))
        {
            return;
        }
    }
    if (ability->isRanged() && ability->isTargetingAll())
    {
        bool applyCosts = true;
        int const usrSide = user->partySide();
        bool const sideTarget = ability->isTargetingSide();
        bool const noSelfTarget = !ability->isTargetingSelf();
        QVector<QVector<Actor*>*>& parties = scene->_parties;
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
                if (noSelfTarget && trg == user)
                {
                    continue;
                }
                else
                {
                    this->execute(*ret, *user, trg, *ability, applyCosts);
                }
                applyCosts = false;
            }
        }
    }
    else if (ability->isTargetingSide())
    {
        //QVector<Actor*>* const targets = scene.targets;
        int const side = ability->isTargetingSelf() ? user->partySide() : target->_actor_data->_old_side;
        QVector<Actor*>& party = *(scene->_parties[side]);
        int const pSize = party.size();
        for (int i = 0; i < pSize; ++i)
        {
            /*Actor* const trg = party[i];
            if (targets);
            if (targets)
            {
                targets->append(trg);
            }*/
            this->execute(*ret, *user, party[i], *ability, i == 0);
        }
    }
    else
    {
        this->execute(*ret, *user, &user == &target || ability->isTargetingSelf() ? user
                     : (this->getGuardian(user, target, ability)), *ability, true);
    }
    if (item)
    {
        const auto& items = user->_actor_data->_items;
        if (items)
        {
            items->operator[](ability) = items->value(ability, 1) - 1;
        }
    }
    scene->_last_ability = ability;
    user->setCurrentExperience(this, user->currentExperience() + 1);
    if (events && events->size() > EVENT_AFTER_ACT)
    {
        auto event = events->at(EVENT_AFTER_ACT);
        if (event && ((*event)(*this, ret)))
        {
            this->endTurn(ret);
        }
    }

}

int Scene::getAiSkill(Actor* const user, QVector<Ability*>* const skills, int const defSkill, bool const restore) const
{
    assert(user && skills);
    Ability* s = skills->operator[](defSkill);
    int ret = defSkill, sSize = skills->size();
    for (int i = defSkill + 1; i < sSize; ++i)
    {
        Ability* a = skills->operator[](i);
        if (a->canPerform(user) && ((defSkill > 0 && (a->currentHp() < s->currentHp())
            && (a->isReviving() || !restore)) || (a->currentHp() > s->currentHp())))
        {
            ret = i;
            s = a;
        }
    }
    return ret;
}

void Scene::playAi(QString* const ret, Actor* const player)
{
    assert(ret && player);
    QVector<Actor*>* party;
    auto& scene = *this->_scene_data;
    QVector<QVector<Actor*>*>& parties = scene._parties;
    QVector<Ability*>& skills = *(player->_costume_data->_a_skills);
    int side, sSize, skillIndex = 0, heal = -1, pSize = parties.size();
    if (player->Costume::isConfused())
    {
        party = NIL;
        side = -1;
    }
    else
    {
        side = player->partySide();
        party = parties[side];
        sSize = party->size();
        if (!(player->Costume::isEnraged()))
        {
            for (int i = 0; i < sSize; ++i)
            {
                Actor& iPlayer = *(party->at(i));
                int iHp = iPlayer.currentHp();
                if (iHp < 1)
                {
                    heal = 1;
                }
                else if (iHp < (iPlayer.maximumHp() / 3))
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
                if (s.canPerform(player) && (s.currentHp() < 0 && ((heal == 0) || s.isReviving())))
                {
                    skillIndex = i;
                    break;
                }
            }
        }
    }
    {
        Actor* target = NIL;
        //Ability& ability = scene.getAiSkill(player, skills, skillIndex, heal == 1);
        Ability& ability = *(skills[this->getAiSkill(player, &skills, skillIndex, heal == 1)]);
        if (ability.currentHp() > -1)
        {
            if (party == NIL || player->isRandomAi())
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
                    if (target == NIL)
                    {
                        do
                        {
                            target = players->at(trg);
                        } while (((++trg) < sSize) && (target->Costume::isKnockedOut() || target->partySide() == side));
                    }
                    for (int i = trg + 1; i < sSize; ++i)
                    {
                        Actor* const iPlayer = players->at(i);
                        if (iPlayer->partySide() != side && (!iPlayer->Costume::isKnockedOut()) && iPlayer->currentHp() < target->currentHp())
                        {
                            target = iPlayer;
                        }
                    }
                }
            }
        }
        else
        {
            if (party == NIL)
            {
                party = parties[rand() % pSize];
                sSize = party->size();
            }
            target = party->at(0);
            bool const restore = ability.isReviving();
            for (int i = 1; i < sSize; ++i)
            {
                Actor* const iPlayer = party->at(i);
                int const iHp = iPlayer->currentHp();
                if (iHp < target->currentHp() && (restore || iHp > 0))
                {
                    target = iPlayer;
                }
            }
        }
        if (target == NIL)
        {
            this->perform(ret, player, player, heal < 0 ? (skills[this->getAiSkill
                          (player, &skills, 1, false)]) : &ability, false);
        }
        else
        {
            this->perform(ret, player, target, &ability, false);
        }
    }

}

void Scene::endTurn(QString* const ret)
{
    assert(ret);
    auto& scene = *this->_scene_data;
    int current = scene._current;
    Actor* crActor = scene._cr_actor;
    int cActions = (crActor->currentActions()) - 1;
    crActor->setCurrentActions(cActions);
    while (cActions < 1)
    {
        if (crActor->currentHp() > 0)
        {
            crActor->applyStates(ret, this, true);
        }
        int mInit = scene._m_init;
        if (mInit > 0)
        {
            int cInit = crActor->initiative() - mInit;
            crActor->setInitiative(cInit);
            do
            {
                //QVector<Actor*>* const ordered = scene.players;
                //if (ordered == NIL)
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
                            if (iPlayer->currentHp() > 0)
                            {
                                int const iInit = iPlayer->initiative() + iPlayer->agility();
                                iPlayer->setInitiative(iInit);
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
                    players[i]->setInitiative(0);
                }
                nInit = -1;
            }
            crActor->setInitiative(nInit);
            do
            {
                if (++current == playersSize)
                {
                    scene._m_init = mInit = nInit;
                    current = 0;
                }
                crActor = players[current];
            }
            while (crActor->currentHp() < 1 || crActor->initiative() < mInit);
        }
        //crActor->actions = cActions = crActor->mActions;
        auto& crActorData = crActor->_actor_data;
        QMap<Ability*, int>* const regSkills = crActorData->_skills_rg_turn;
        if (regSkills)
        {
            QMap<Ability*, int>* skillsQty = crActorData->_skills_cr_qty;
            if (skillsQty == NIL)
            {
                skillsQty = new QMap<Ability*, int>();
                crActorData->_skills_cr_qty = skillsQty;
            }
            auto const last = regSkills->cend();
            for (auto it = regSkills->cbegin(); it != last; ++it)
            {
                Ability* const skill = it.key();
                int const skillMaxQty = skill->maximumUses(), skillCrQty = skillsQty->value(skill, skillMaxQty);
                if (skillCrQty < skillMaxQty)
                {
                    int const skillRgTurn = it.value();//regSkills->value(skill, 0);
                    if (skillRgTurn == skill->usesRegen())
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
            crActor->applyStates(ret, this, false);
            if (shapeShifted && (!crActor->Costume::isShapeShifted()))
            {
                emit this->spriteAct(this, crActor, NIL, true, NIL, NIL);
            }
        }
        crActor->setCurrentActions((cActions = crActor->Costume::isStunned() ? 0 : crActor->maximumActions()));
    }
    scene._current = current;
    scene._original = current;
    scene._cr_actor = crActor;
    QVector<SceneRun*>* const events = scene._events;
    if (events && events->size() > EVENT_NEW_TURN)
    {
        auto event = events->at(EVENT_NEW_TURN);
        if (event && (*event)(*this, ret) && crActor->hasAnyPlayFlag(Actor::Attribute::AiPlayer | Costume::Attribute::Enraged | Costume::Attribute::Confuse)
                /*(crActor->isAiPlayer() || crActor->Costume::isEnraged() || crActor->Costume::isConfused())*/)
        {
            this->playAi(ret, crActor);
        }
    }
    ret->append(".");
}

bool Scene::canTarget(Actor* const user, Ability* const ability, Actor* const target)
{
    assert(user && ability && target);
    return ability->canPerform(user) && (ability->isTargetingSelf() || ((target->currentHp() > 0 || ability->isReviving())
            && ((this->getGuardian(user, target, ability))) == target));
}

void Scene::agiCalc()
{
    auto& data = this->_scene_data;
    if (data->_m_init < 1)
    {
        QVector<Actor*>& players = *(data->_players);
        std::sort(players.begin(), players.end(), Scene::actorAgiComp);
        data->_original = -1;
    }
}

void Scene::resetTurn(Actor& actor)
{
    auto& data = this->_scene_data;
    int const mInit = data->_m_init + 1;
    if (mInit < 2)
    {
        if (actor.initiative() > mInit)
        {
            actor.setInitiative(mInit);
        }
        else if (mInit == 1 && actor.initiative() < -1)
        {
            actor.setInitiative(0);
        }
        data->_original = -1;
    }
}

void Scene::escape(QString* const ret)
{
    assert(ret);
    //TODO:
}

void Scene::operator()(QString& ret, QVector<QVector<Actor*>*>& parties, SpriteAct* const actorEvent, QVector<SceneRun*>* const events,
                         bool const useGuards, int const surprise, int const mInit)
{
    int partiesSize = parties.size();
    assert(partiesSize > 1);
    QVector<Actor*>* players;
    auto& scene = *this->_scene_data;
    bool useInit;
    if (mInit > 0)
    {
        scene._m_init = mInit;
        players = NIL;
        useInit = true;
    }
    else
    {
        players = new QVector<Actor*>();
        scene._m_init = 0;
        useInit = false;
    }
    int current = 0;
    Actor* crActor = NIL;
#if USE_TARGET_LIST
    scene._targets = NIL;
#endif
    if (actorEvent)
    {
        connect(this, &Scene::spriteAct, *actorEvent);
    }
    scene._events = events;
    scene._parties = parties;
    scene._players = players;
    //scene.current = scene.oldCurrent = 0;
    scene._surprise = surprise;
    for (int i = 0; i < partiesSize; ++i)
    {
        bool const aiPlayer = i > 0;
        bool const surprised = (surprise == i);
        QVector<Actor*>& party = *(parties[i]);
        int const pSize = party.size();
        if (crActor == NIL)
        {
            crActor = party[0];
        }
        for (int j = 0; j < pSize; ++j)
        {
            Actor& player = *(party[j]);
            player.setCurrentActions(0);
            if (surprised)
            {
                player.setInitiative(useInit ? -(mInit + 1) : -1);
            }
            else
            {
                player.setInitiative(0);
                if (player.agility() > crActor->agility() || crActor->initiative() < 0)
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
            player._actor_data->_old_side = i;
            player.setPartySide(i);
#if ALLOW_COVERING
            if (player.Costume::isCovering())
            {
                this->setHasCovers(true);
            }
#endif
        }
        if (players)
        {
            players->append(party);
        }
    }
#if ALLOW_NO_GUARDS
    this->setUseGuards(useGuards);
#endif
    scene._cr_actor = crActor;
    this->agiCalc();
    if (useInit)
    {
        crActor->setInitiative(mInit);
    }
    else
    {
        current = players->indexOf(crActor);
    }
    SceneRun* event;
    scene._current = current;
    scene._original = current;
    if (events && events->size() > EVENT_BEGIN_SCENE && ((event = events->at(EVENT_BEGIN_SCENE))) && (*event)(*this, &ret))
    {
        this->endTurn(&ret);
    }

}

Scene::Scene(QObject* const parent) : Play(parent, 0)
{
    this->_scene_data = QSharedDataPointer<SceneData>(new SceneData);
}

Scene::Scene(const Scene& scene) : Play(NIL, scene.playFlags())
{
    this->_scene_data = scene._scene_data;
}

Scene::Scene(QString& ret, QVector<QVector<Actor*>*>& parties, SpriteAct* const actorEvent, QVector<SceneRun*>* const events,
             bool const useGuards, int const surprise, int const mInit, QObject* const parent) : Scene(parent)
{
    this->operator()(ret, parties, actorEvent, events, useGuards, surprise, mInit);
}

Scene::SceneData::~SceneData()
{
    auto players = this->_players;
    if (players)
    {
        this->_players = NIL;
        delete players;
    }
}

Scene::~Scene() {}
