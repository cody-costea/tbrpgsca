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
    return (a->agi > b->agi);
}

int Scene::getCurrent() const
{
    return this->current;
}

int Scene::getStatus() const
{
    return this->status;
}

Actor* Scene::getCurrentPlayer() const
{
    return this->crActor;
}

int Scene::getCurrentParty() const
{
    Actor* const crActor = this->crActor;
    return crActor == nullptr ? -1 : crActor->side;
}

Ability* Scene::getLastAbility() const
{
    return this->lastAbility;
}

int Scene::getPartiesSize() const
{
    return this->parties.size();
}

int Scene::getPartyPlayersSize(const int party) const
{
    return this->parties[party]->size();
}

Actor& Scene::getPartyPlayer(const int party, const int player) const
{
    return *(this->parties[party]->at(player));
}

bool Scene::hasPartyPlayer(int const party, Actor& player) const
{
    return this->parties[party]->contains(&player);
}

Actor& Scene::getOrderedPlayer(int const n) const
{
    return *(this->players->at(n));
}

bool Scene::hasOrderedPlayer(Actor& player) const
{
    QVector<Actor*>* players = this->players;
    return players != nullptr && players->contains(&player);
}

int Scene::getOrderedPlayersSize() const
{
    QVector<Actor*>* players = this->players;
    return players == nullptr ? 0 : players->size();
}

int Scene::getTargetedPlayersSize() const
{
    QVector<Actor*>* players = this->targets;
    return players == nullptr ? 0 : players->size();
}

Actor& Scene::getTargetedPlayer(int const n) const
{
    return *(this->targets->at(n));
}

bool Scene::hasTargetedPlayer(Actor& player) const
{
    QVector<Actor*>* players = this->targets;
    return players != nullptr && players->contains(&player);
}

Actor& Scene::getGuardian(Actor& user, Actor& target, Ability& skill) const
{
    if (user.oldSide != target.oldSide && (!skill.isRanged() && (!user.isRanged() || skill.isOnlyMelee())))
    {
        int pos = -1;
        int side = target.oldSide;
        Actor* fGuard = nullptr,* lGuard = nullptr;
        Actor** guardPos = &fGuard;
        QVector<Actor*>& party = *(this->parties[side]);
        int pSize = party.size();
        for (int i = 0; i < pSize; i++)
        {
            Actor* const guardian = party[i];
            if (guardian == &target)
            {
                if (fGuard == nullptr || i == pSize - 1)
                {
                    return target;//break;
                }
                else
                {
                    pos = i;
                    guardPos = &lGuard;
                    continue;
                }
            }
            else if ((fGuard == nullptr || pos != -1) && guardian->hp > 0 && (!guardian->isStunned()) && (!guardian->isConfused()))
            {
                (*guardPos) = guardian;
            }
        }
        if (fGuard != nullptr && lGuard != nullptr)
        {
            return *((pos < (pSize / 2)) ? fGuard : lGuard);
        }
    }
    return target;
}

Scene& Scene::checkStatus(QString& ret)
{
    Scene& scene = *this;
    if (scene.status == 0)
    {
        auto parties = scene.parties;
        auto party = parties[0];
        int partySize = party->size();
        {
            for (int k = 0; k < partySize; ++k)
            {
                if (!(party->at(k)->isKnockedOut()))
                {
                    goto enemyCheck;
                }
            }
            scene.status = -2;
            ret = ret % Scene::FallenTxt;
            return scene;
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
                    if (!(party->at(j)->isKnockedOut()))
                    {
                        return scene;
                    }
                }
            }
            ret = ret % Scene::VictoryTxt;
            scene.status = 1;
        }
    }
    return scene;
}

Scene& Scene::execute(QString& ret, Actor& user, Actor* target, Ability& ability, bool const applyCosts)
{
    Scene& scene = *this;
    bool const healing = ability.hp < 0;
    if ((healing && ability.isReviving()) || target->hp > 0)
    {
        int cntSize;
        Ability* counter = nullptr;
        QVector<Ability*>* counters;
        bool const ko = target->hp < 1;
        ability.execute(ret, this, user, target, applyCosts);
        if ((!healing) && ((counters = target->counters) != nullptr) && (cntSize = counters->size()) > 0
                && (!target->isStunned()) && (target->side != user.side || target->isConfused()))
        {
            int const usrDmgType = ability.dmgType;
            for (int i = 0; i < cntSize; ++i)
            {
                Ability* cntSkill = counters->at(i);
                int cntDmgType = cntSkill->dmgType;
                if (((usrDmgType & cntDmgType) == cntDmgType) && cntSkill->hp > counter->hp)
                {
                    counter = cntSkill;
                }
            }
            if (counter != nullptr)
            {
                counter->execute(ret, *target, user, false);
            }
        }
        ActorAct* const actorEvent = scene.actorEvent;
        if (actorEvent == nullptr || ((*actorEvent)(scene, user, &ability, (ko && target->hp > 0), target, counter)))
        {
            QVector<Actor*>* targets = scene.targets;
            if (targets == nullptr)
            {
                targets = new QVector<Actor*>(scene.parties[target->oldSide]->size());
                scene.targets = targets;
            }
            targets->append(target);
        }
    }
    return scene;
}

Scene& Scene::perform(QString& ret, Actor& user, Actor& target, Ability& ability, bool const item)
{
    Scene& scene = *this;
    ret = ret % Scene::PerformsTxt.arg(user.name, ability.name);
    {
        QVector<Actor*>* targets = scene.targets;
        if (targets != nullptr)
        {
            targets->clear();
        }
    }
    QVector<SceneAct*>* events = scene.events;
    if (events != nullptr && events->size() > EVENT_BEFORE_ACT)
    {
        auto event = events->at(EVENT_BEFORE_ACT);
        if (event != nullptr && !((*event)(scene, &ret)))
        {
            return scene;
        }
    }
    if (ability.isRanged() && ability.targetsAll())
    {
        bool applyCosts = true;
        int const usrSide = user.side;
        bool const sideTarget = ability.targetsSide();
        bool const noSelfTarget = !ability.targetsSelf();
        QVector<QVector<Actor*>*>& parties = scene.parties;
        int const pSize = parties.size();
        for (int j = 0; j < pSize; ++j)
        {
            QVector<Actor*>* players = parties[j];
            int sSize = players->size();
            if (sideTarget && noSelfTarget && j == usrSide)
            {
                continue;
            }
            for (int i = 0; i < sSize; ++i)
            {
                Actor* const trg = players->at(i);
                if (trg == &user)
                {
                    if (noSelfTarget)
                    {
                        continue;
                    }
                    else
                    {
                        scene.execute(ret, user, &user, ability, applyCosts);
                    }
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
        int side = ability.targetsSelf() ? user.side : target.oldSide;
        QVector<Actor*>& party = *(scene.parties[side]);
        int pSize = party.size();
        for (int i = 0; i < pSize; ++i)
        {
            Actor* const trg = party[i];
            targets->append(trg);
            scene.execute(ret, user, trg, ability, i == 0);
        }
    }
    else
    {
        if (&user == &target || ability.targetsSelf())
        {
            scene.execute(ret, user, &user, ability, true);
        }
        else
        {
            scene.execute(ret, user, &(scene.getGuardian(user, target, ability)), ability, true);
        }
    }
    if (item)
    {
        QMap<Ability*, int>* items = user.items;
        if (items != nullptr)
        {
            items->operator[](&ability) = items->value(&ability, 1) - 1;
        }
    }
    this->lastAbility = &ability;
    user.setExperience(user.xp + 1);
    if (events != nullptr && events->size() > EVENT_AFTER_ACT)
    {
        auto event = events->at(EVENT_AFTER_ACT);
        if (event != nullptr && ((*event)(scene, &ret)))
        {
            scene.endTurn(ret, &user);
        }
    }
    return scene;
}

int Scene::getAiSkill(Actor& user, QVector<Ability*>& skills, int const defSkill, bool const restore) const
{
    int ret = defSkill, sSize = skills.size();
    Ability* s = skills[defSkill];
    for (int i = defSkill + 1; i < sSize; ++i)
    {
        Ability* a = skills[i];
        if (a->canPerform(user) && ((defSkill > 0 && (a->hp < s->hp)
            && (a->isReviving() || !restore)) || (a->hp > s->hp)))
        {
            ret = i;
            s = a;
        }
    }
    return ret;
}

Scene& Scene::playAi(QString& ret, Actor& player)
{
    Scene& scene = *this;
    QVector<Actor*>* party;
    QVector<Ability*>& skills = *(player.aSkills);
    QVector<QVector<Actor*>*>& parties = scene.parties;
    int side, sSize, skillIndex = 0, heal = -1, pSize = parties.size();
    if (player.isConfused())
    {
        party = nullptr;
        side = -1;
    }
    else
    {
        side = player.side;
        party = parties[side];
        sSize = party->size();
        if (!(player.isEnraged()))
        {
            for (int i = 0; i < sSize; ++i)
            {
                Actor& iPlayer = *(party->at(i));
                int iHp = iPlayer.hp;
                if (iHp < 1)
                {
                    heal = 1;
                }
                else if (iHp < (iPlayer.mHp / 3))
                {
                    heal = 0;
                }
            }
        }
        if (heal > -1)
        {
            int skillsSize = skills.size();
            for (int i = 0; i < skillsSize; ++i)
            {
                Ability& s = *(skills[i]);
                if (s.canPerform(player) && (s.hp < 0 && ((heal == 0) || s.isReviving())))
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
        if (ability.hp > -1)
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
                        if (side == trgSide)
                        {
                            if (++trgSide == pSize)
                            {
                                trgSide = 0;
                            }
                        }
                    }
                    party = parties[trgSide];
                    sSize = party->size();
                }
                int trg = rand() % sSize;
                target = party->at(trg);
                while (target->isKnockedOut())
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
                    QVector<Actor*>* players = parties[j];
                    sSize = players->size();
                    if (target == nullptr)
                    {
                        int trg = 0;
                        do
                        {
                            target = players->at(trg);
                        } while (((++trg) < sSize) && (target->isKnockedOut() || target->side == side));
                    }
                    for (int i = 1; i < sSize; ++i)
                    {
                        Actor* const iPlayer = players->at(i);
                        if (iPlayer->side != side && (!iPlayer->isKnockedOut()) && iPlayer->hp < target->hp)
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
                int const iHp = iPlayer->hp;
                if (iHp < target->hp && (restore || iHp > 0))
                {
                    target = iPlayer;
                }
            }
        }
        scene.perform(ret, player, *target, ability, false);
    }
    return scene;
}

Scene& Scene::endTurn(QString& ret, Actor* crActor)
{
    Scene& scene = *this;
    int current = scene.current;
    if (crActor == nullptr)
    {
        crActor = scene.crActor;
    }
    --(crActor->actions);
    while (crActor->actions < 1)
    {
        if (crActor->hp > 0 && !(crActor->isInvincible() && crActor->isKnockedOut() && crActor->isStunned()))
        {
            crActor->applyStates(&ret, this, true);
        }
        int mInit = scene.mInit;
        if (mInit > 0)
        {
            //int next = current;
            //Actor* nxActor = crActor;
            int cInit = crActor->init - mInit;
            crActor->init = cInit;
            do
            {
                QVector<Actor*>* ordered = scene.players;
                if (ordered == nullptr)
                {
                    QVector<QVector<Actor*>*>& parties = scene.parties;
                    int const pSize = parties.size();
                    for (int j = 0; j < pSize; ++j)
                    {
                        QVector<Actor*>& players = *(parties[j]);
                        int sSize = players.size();
                        for (int i = 0; i < sSize; ++i)
                        {
                            Actor* const iPlayer = players[i];
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
                    }
                }
                else
                {
                    int const oSize = ordered->size();
                    for (int i = 0; i < oSize; ++i)
                    {
                        Actor* iPlayer = ordered->at(i);
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
                }
            }
            while (crActor->init < mInit);
            //crActor = nxActor;
            //current = next;
        }
        else
        {
            current = scene.oldCurrent;
            QVector<Actor*>& players = *(scene.players);
            int playersSize = players.size();
            int nInit = mInit - 1;
            if (nInit == MIN_ROUND)
            {
                scene.mInit = mInit = 0;
                for (int i = 0; i < playersSize; ++i)
                {
                    players[i]->init = 0;
                }
                nInit = -1;
            }
            crActor->init = nInit;
            do
            {
                if (++current == playersSize)
                {
                    scene.mInit = mInit = nInit;
                    current = 0;
                }
                crActor = players[current];
            }
            while (crActor->init < mInit || crActor->hp < 1);
        }
        crActor->actions = crActor->mActions;
        QMap<Ability*, int>* regSkills = crActor->skillsRgTurn;
        if (regSkills != nullptr)
        {
            QMap<Ability*, int>* skillsQty = crActor->skillsCrQty;
            if (skillsQty == nullptr)
            {
                skillsQty = new QMap<Ability*, int>();
                crActor->skillsCrQty = skillsQty;
            }
            auto const last = regSkills->cend();
            for (auto it = regSkills->cbegin(); it != last; ++it)
            {
                Ability* const skill = it.key();
                int skillMaxQty = skill->mQty;
                int skillCrQty = skillsQty->value(skill, skillMaxQty);
                if (skillCrQty < skillMaxQty)
                {
                    int skillRgTurn = it.value();//regSkills->value(skill, 0);
                    if (skillRgTurn == skill->rQty)
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
        {
            bool const shapeShifted = crActor->isShapeShifted();
            crActor->applyStates(&ret, this, false);
            if (shapeShifted && (!crActor->isShapeShifted()))
            {
                ActorAct* const actorEvent = scene.actorEvent;
                if (actorEvent != nullptr)
                {
                    ((*actorEvent)(scene, *crActor, nullptr, true, nullptr, nullptr));
                }
            }
        }
        if (crActor->isStunned())
        {
            crActor->actions = 0;
        }
    }
    scene.crActor = crActor;
    scene.current = current;
    scene.oldCurrent = current;
    QVector<SceneAct*>* const events = scene.events;
    if (events != nullptr && events->size() > EVENT_NEW_TURN)
    {
        auto event = events->at(EVENT_NEW_TURN);
        if (event != nullptr && (*event)(scene, &ret) && (crActor->isAiPlayer() || crActor->isEnraged() || crActor->isConfused()))
        {
            scene.playAi(ret, (*crActor));
        }
    }
    ret = ret % ".";
    return scene;
}

bool Scene::canTarget(Actor& user, Ability& ability, Actor& target)
{
    return ability.canPerform(user) && (ability.targetsSelf() || ((target.hp > 0 || ability.isReviving())
            && (&(this->getGuardian(user, target, ability))) == &target));
}

void Scene::agiCalc()
{
    if (this->mInit < 1)
    {
        QVector<Actor*>& players = *(this->players);
        std::sort(players.begin(), players.end(), Scene::actorAgiComp);
        this->oldCurrent = 0;
    }
}

void Scene::resetTurn(Actor& actor)
{
    int const mInit = this->mInit + 1;
    if (mInit < 2)
    {
        if (actor.init > mInit)
        {
            actor.init = mInit;
        }
        else if (mInit == 1 && actor.init < -1)
        {
            actor.init = 0;
        }
        this->oldCurrent = 0;
    }
}

Scene& Scene::operator()(QString& ret, QVector<QVector<Actor*>*>& parties, ActorAct* const actorEvent, QVector<SceneAct*>* const events, int const surprise, int const mInit)
{
    int partiesSize = parties.size();
    assert(partiesSize > 1);
    QVector<Actor*>* players;
    Scene& scene = *this;
    bool useInit;
    if (mInit > 0)
    {
        scene.mInit = mInit;
        players = nullptr;
        useInit = true;
    }
    else
    {
        players = new QVector<Actor*>();
        scene.mInit = 0;
        useInit = false;
    }
    int current = 0;
    scene.events = events;
    scene.parties = parties;
    scene.players = players;
    scene.targets = nullptr;
    scene.actorEvent = actorEvent;
    scene.current = scene.oldCurrent = 0;
    scene.surprise = surprise;
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
            player.actions = 0;
            if (surprised)
            {
                player.init = useInit ? -(mInit + 1) : -1;
            }
            else
            {
                player.init = 0;
                if (player.agi > crActor->agi || crActor->init < 0)
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
            player.oldSide = i;
            player.side = i;
        }
        if (players != nullptr)
        {
            players->append(party);
        }
    }
    scene.crActor = crActor;
    scene.agiCalc();
    if (useInit)
    {
        crActor->init = mInit;
    }
    else
    {
        current = players->indexOf(crActor);
    }
    SceneAct* event;
    scene.current = current;
    scene.oldCurrent = current;
    if (events != nullptr && events->size() > EVENT_BEGIN_SCENE && ((event = events->at(EVENT_BEGIN_SCENE)) != nullptr) && (*event)(scene, &ret))
    {
        scene.endTurn(ret, crActor);
    }
    return scene;
}

Scene::Scene()
{

}

Scene::Scene(QString& ret, QVector<QVector<Actor*>*>& parties, ActorAct* const actorEvent, QVector<SceneAct*>* const events, int const surprise, int const mInit)
{
    this->operator()(ret, parties, actorEvent, events, surprise, mInit);
}

Scene::~Scene()
{
    auto players = this->players;
    if (players != nullptr)
    {
        this->players = nullptr;
        delete players;
    }
}
