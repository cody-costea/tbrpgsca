/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#include "scene.hpp"
#include "ability.hpp"
#include "costume.hpp"
#include "state.hpp"
#include "role.hpp"
#include "arenawidget.hpp"

using namespace tbrpgsca;

QString Scene::PerformsTxt = "%1 performs %2";
QString Scene::VictoryTxt = "The party has won!";
QString Scene::FallenTxt = "The party has fallen!";
QString Scene::EscapeTxt = "The party has escaped!";
QString Scene::FailTxt = "The party attempted to escape, but failed.";

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
    return crActor ? crActor->_side : -1;
}

const Ability* Scene::getLastAbility() const
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
    QVector<Actor*>* const players = this->_players;
    return players && players->contains(&player);
}

int Scene::getOrderedPlayersSize() const
{
    QVector<Actor*>* const players = this->_players;
    return players ? players->size() : 0;
}
#if USE_TARGET_LIST
int Scene::getTargetedPlayersSize() const
{
    QVector<Actor*>* const players = this->_targets;
    return players ? players->size() : 0;
}

Actor& Scene::getTargetedPlayer(int const n) const
{
    return *(this->_targets->at(n));
}

bool Scene::hasTargetedPlayer(Actor& player) const
{
    QVector<Actor*>* const players = this->_targets;
    return players && players->contains(&player);
}
#endif
Actor* Scene::getGuardian(Actor& user, Actor* target, const Ability& skill) const
{
    int const side = target->_side;
#if ALLOW_NO_GUARDS
    if (this->usesGuards())
#endif
    {
        if (user._side != side && ((!skill.isRanged()) && ((!user.Role::isRanged()) || skill.isOnlyMelee())))
        {
            int pos = -1;
            Actor* fGuard = nullptr,* lGuard = nullptr;
            Actor** guardPos = &fGuard;
            QVector<Actor*>& party = *(this->_parties[side]);
            int const pSize = party.size();
            for (int i = 0; i < pSize; i += 1)
            {
                Actor* const guardian = party.at(i);
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
                else if ((fGuard == nullptr || pos != -1) && guardian->_hp > 0 && (!guardian->hasAnyFlag(FLAG_STUN | FLAG_CONVERT | FLAG_CONFUSE))
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
            for (int i = 1; i < partiesSize; i += 1)
            {
                party = parties.at(i);
                partySize = party->size();
                for (int j = 0; j < partySize; j += 1)
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

template <typename SpriteRun>
void Scene::execute(QString& ret, const SpriteRun* const actorEvent, Actor& user, Actor* const target, const Ability& ability, bool const applyCosts, bool const ko)
{
    Scene& scene = *this;
    //bool const ko = target->_hp < 1;
    //bool const healing = ability.hp < 0;
    //if ((/*healing &&*/ revive) || !ko)
    //{
        const Ability* counter = nullptr;
        ability.execute(ret, this, actorEvent, user, target, applyCosts);
        if (ability._hp > -1)
        //if (!healing)
        {
            if (target->isCountering() && (!target->Costume::isStunned())
                    && (target->_side != user._side || target->hasAnyFlag(FLAG_CONFUSE | FLAG_ENRAGED | FLAG_CONVERT)))
            {
                counter = target->_a_skills->at(0);
                counter->execute(ret, *target, user, false);
            }
        }
        //SpriteRun* const actorEvent = scene._actor_run;
#if USE_TARGET_LIST
        if (actorEvent == nullptr || (*actorEvent)(scene, applyCosts ? &user : nullptr, &ability, (ko && target->_hp > 0),
                                                    target, &user == target ? &ability : counter))
        {
            QVector<Actor*>* targets = scene._targets;
            if (targets == nullptr)
            {
                targets = new QVector<Actor*>(scene._parties[target->_side]->size());
                scene._targets = targets;
            }
            targets->append(target);
        }
#else
        if (actorEvent)
        {
            (*actorEvent)(scene, applyCosts ? &user : nullptr, &ability, (ko && target->_hp > 0),
                                                    target, &user == target ? &ability : counter);
        }
#endif
    //}

}

template <typename SpriteRun>
void Scene::perform(QString& ret, const SpriteRun* const spriteRun, Actor& user, Actor& target, const Ability& ability, bool const item)
{
    Scene& scene = *this;
    ret += Scene::PerformsTxt.arg(user._name, ability._name);
#if USE_TARGET_LIST
    {
        QVector<Actor*>* const targets = scene._targets;
        if (targets)
        {
            targets->clear();
        }
    }
#endif
    QVector<SceneRun*>* events = scene._events;
    if (events && events->size() > EVENT_BEFORE_ACT)
    {
        auto event = events->at(EVENT_BEFORE_ACT);
        if (event && !((*event)(scene, &ret)))
        {
            return;
        }
    }
    bool const revives = ability.isReviving();
    bool const onlyKoTarget = ability.targetsOnlyKO();
    if (ability.isRanged() && ability.targetsAll())
    {
        bool applyCosts = true;
        int const usrSide = user._side;
        bool const sideTarget = ability.targetsSide();
        bool const selfTarget = ability.targetsSelf();
        QVector<QVector<Actor*>*>& parties = scene._parties;
        int const pSize = parties.size();
        for (int j = 0; j < pSize; j += 1)
        {
            if (sideTarget && (!selfTarget) && j == usrSide)
            {
                continue;
            }
            QVector<Actor*>* const players = parties.at(j);
            int const sSize = players->size();
            for (int i = 0; i < sSize; i += 1)
            {
                Actor* const trg = players->at(i);
                bool const trgKo = trg->_hp < 1;
                /*if ((noSelfTarget && trg == &user) || (trgKo && !revives) || (onlyKoTarget && !trgKo))
                {
                    continue;
                }
                else
                {*/
                if ((selfTarget || trg != &user) && (revives || !(trgKo && onlyKoTarget)))
                {
                    scene.execute(ret, spriteRun, user, trg, ability, applyCosts, trgKo);
                    applyCosts = false;
                }
            }
        }
    }
    else if (ability.targetsSide())
    {
        bool applyCosts = true;
        //QVector<Actor*>* const targets = scene.targets;
        int const side = ability.targetsSelf() ? user._side : target._side;
        QVector<Actor*>& party = *(scene._parties[side]);
        int const pSize = party.size();
        for (int i = 0; i < pSize; i += 1)
        {
            Actor* const trg = party.at(i);
            bool const trgKo = trg->_hp < 1;
            //if ((trg->_hp < 1 && revives) || (!onlyKoTarget))
            if (revives || !(trgKo && onlyKoTarget))
            {
                scene.execute(ret, spriteRun, user, trg, ability, applyCosts, trgKo);
                applyCosts = true;
            }
        }
    }
    else
    {
        /*Actor* const trg = &user == &target || ability.targetsSelf() ? &user : (scene.getGuardian(user, &target, ability));
        if ((trg->_hp < 1 && revives) || (!onlyKoTarget))
        {
            scene.execute(ret, spriteRun, user, trg, ability, true, revives);
        }*/
        if (&user == &target || ability.targetsSelf())
        {
            scene.execute(ret, spriteRun, user, &user, ability, true, false);
        }
        else
        {
            bool const trgKo = target._hp < 1;
            if (revives || !(trgKo && onlyKoTarget))
            {
                scene.execute(ret, spriteRun, user, (scene.getGuardian(user, &target, ability)), ability, true, trgKo);
            }
        }
    }
    if (item)
    {
        QMap<const Ability*, int>* const items = user._items;
        if (items)
        {
            items->operator[](&ability) = items->value(&ability, 1) - 1;
        }
    }
    this->_last_ability = &ability;
    user.setCurrentExperience(this, user._xp + 1);
    if (events && events->size() > EVENT_AFTER_ACT)
    {
        auto event = events->at(EVENT_AFTER_ACT);
        if (event && ((*event)(scene, &ret)))
        {
            scene.endTurn(ret, spriteRun, &user);
        }
    }

}

int Scene::getAiSkill(Actor& user, QVector<const Ability*>& skills, int const defSkill, bool const restore) const
{
    const Ability* s = skills[defSkill];
    int ret = defSkill, sSize = skills.size();
    for (int i = defSkill + 1; i < sSize; i += 1)
    {
        const Ability* a = skills.at(i);
        if (a->canPerform(user) && ((defSkill > 0 && (a->_hp < s->_hp)
            && ((restore && a->isReviving()) || !(restore || a->targetsOnlyKO())))
            || (defSkill == 0 && a->_hp > s->_hp)))
        {
            ret = i;
            s = a;
        }
    }
    return ret;
}

template <typename SpriteRun>
void Scene::playAi(QString& ret, const SpriteRun* const spriteRun, Actor& player)
{
    Scene& scene = *this;
    QVector<Actor*>* party;
    QVector<const Ability*>& skills = *(player._a_skills);
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
        if (!(player.hasAnyFlag(FLAG_ENRAGED | FLAG_CONFUSE | FLAG_CONVERT)))
        {
            for (int i = 0; i < sSize; i += 1)
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
            for (int i = 0; i < skillsSize; i += 1)
            {
                const Ability& s = *(skills.at(i));
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
        const Ability& ability = *(skills[scene.getAiSkill(player, skills, skillIndex, heal == 1)]);
        if (ability._hp > -1)
        {
            if (party == nullptr || player.isRandomAi())
            {
                {
                    int trgSide;
                    if (player.isConverted())
                    {
                        trgSide = side;
                    }
                    else if (side > -1 && pSize == 2)
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
                for (int j = 0; j < pSize; j += 1)
                {
                    if (j == side)
                    {
                        continue;
                    }
                    int trg = 0;
                    QVector<Actor*>* const players = parties.at(j);
                    sSize = players->size();
                    if (target == nullptr)
                    {
                        do
                        {
                            target = players->at(trg);
                        } while (((++trg) < sSize) && (target->Costume::isKnockedOut() || target->_side == side));
                    }
                    for (int i = trg + 1; i < sSize; i += 1)
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
            for (int i = 1; i < sSize; i += 1)
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
            scene.perform(ret, spriteRun, player, player, heal < 0
                          ? *(skills[scene.getAiSkill(player, skills, 1, false)])
                          : ability, false);
        }
        else
        {
            scene.perform(ret, spriteRun, player, *target, ability, false);
        }
    }

}

template <typename SpriteRun>
void Scene::endTurn(QString& ret, const SpriteRun* actorEvent, Actor* crActor)
{
    Scene& scene = *this;
    int current = scene._current;
    if (crActor == nullptr)
    {
        crActor = scene._cr_actor;
    }
    bool inactive;
    crActor->setActive(false);
    do
    {
        if (crActor->_hp > 0/* && !(crActor->hasAllFlags(FLAG_INVINCIBLE | FLAG_KO | FLAG_STUN))*/)
        {
            crActor->applyStates(&ret, this, actorEvent, true);
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
                    for (int j = 0; j < pSize; j += 1)
                    {
                        QVector<Actor*>& players = *(parties.at(j));
                        int sSize = players.size();
                        for (int i = 0; i < sSize; i += 1)
                        {
                            Actor* const iPlayer = players.at(i);
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
                    for (int i = 0; i < oSize; i += 1)
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
                for (int i = 0; i < playersSize; i += 1)
                {
                    players.at(i)->_init = 0;
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
        QMap<const Ability*, int>* const regSkills = crActor->_skills_rg_turn;
        if (regSkills)
        {
            QMap<const Ability*, int>* skillsQty = crActor->_skills_cr_qty;
            if (skillsQty == nullptr)
            {
                skillsQty = new QMap<const Ability*, int>();
                crActor->_skills_cr_qty = skillsQty;
            }
            auto const last = regSkills->cend();
            for (auto it = regSkills->cbegin(); it != last; ++it)
            {
                const Ability* const skill = it.key();
                int const skillMaxQty = skill->_m_qty, skillCrQty = skillsQty->value(skill, skillMaxQty);
                if (skillCrQty > -1 && skillCrQty < skillMaxQty)
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
            crActor->applyStates(&ret, this, actorEvent, false);
            if (shapeShifted && (!crActor->Costume::isShapeShifted()))
            {
                //SpriteRun* const actorEvent = scene._actor_run;
                if (actorEvent)
                {
                    ((*actorEvent)(scene, crActor, nullptr, true, nullptr, nullptr));
                }
            }
        }
        crActor->setActive(!(inactive = crActor->Costume::isStunned()));
    } while (inactive);
    scene._cr_actor = crActor;
    scene._current = current;
    scene._original = current;
    QVector<SceneRun*>* const events = scene._events;
    if (events && events->size() > EVENT_NEW_TURN)
    {
        auto event = events->at(EVENT_NEW_TURN);
        if (event && (*event)(scene, &ret) && crActor->hasAnyFlag(FLAG_AI_PLAYER | FLAG_CONVERT | FLAG_ENRAGED | FLAG_CONFUSE)
                /*(crActor->isAiPlayer() || crActor->Costume::isEnraged() || crActor->Costume::isConfused())*/)
        {
            scene.playAi(ret, actorEvent, (*crActor));
        }
    }
    ret += ".";

}

bool Scene::canTarget(Actor& user, const Ability& ability, Actor& target)
{
    int trgHp;
    return ability.canPerform(user) && (ability.targetsSelf() || ((((trgHp = target._hp) > 0 && !ability.targetsOnlyKO())
            || (trgHp < 1 && ability.isReviving())) && ((this->getGuardian(user, &target, ability))) == &target));
}

void Scene::agiCalc()
{
    if (this->_m_init < 1)
    {
        QVector<Actor*>& players = *(this->_players);
        std::sort(players.begin(), players.end(), [](const Actor* const a, const Actor* const b) -> bool
        {
            return (a->_agi > b->_agi);
        });
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

template <typename SpriteRun>
void Scene::operator()(QString& ret, QVector<QVector<Actor*>*>& parties, const SpriteRun* const actorEvent,
                       QVector<SceneRun*>* const events, bool const useGuards, int const surprise, int mInit)
{
    Q_UNUSED(useGuards)
    int partiesSize = parties.size();
    //assert(partiesSize > 1);
    QVector<Actor*>* players;
    Scene& scene = *this;
    bool useInit;
    if (mInit > 0)
    {
        if (mInit > MAX_ROUND)
        {
            mInit = MAX_ROUND;
        }
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
#if USE_TARGET_LIST
    scene._targets = nullptr;
#endif
    //scene._actor_run = actorEvent;
    //scene.current = scene.oldCurrent = 0;
    scene._surprise = surprise;
    Actor* crActor = nullptr;
    for (int i = 0; i < partiesSize; i += 1)
    {
        bool const aiPlayer = i > 0;
        bool const surprised = (surprise == i);
        QVector<Actor*>& party = *(parties.at(i));
        int const pSize = party.size();
        if (crActor == nullptr)
        {
            crActor = party[0];
        }
        for (int j = 0; j < pSize; j += 1)
        {
            Actor& player = *(party.at(j));
            player.setActive(false);
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
        scene.endTurn(ret, actorEvent, crActor);
    }

}

Scene::Scene() : Play(0) {}

template <typename SpriteRun>
Scene::Scene(QString& ret, QVector<QVector<Actor*>*>& parties, const SpriteRun* const actorEvent, QVector<SceneRun*>* const events,
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

#if USE_TEMPLATE
    template void Scene::playAi(QString& ret, const ArenaWidget* const spriteRun, Actor& player);
    template void Scene::endTurn(QString& ret, const ArenaWidget* const spriteRun, Actor* const actor);
    template void Scene::perform(QString& ret, const ArenaWidget* const spriteRun, Actor& user, Actor& target, const Ability& ability, bool const item);
    template void Scene::execute(QString& ret, const ArenaWidget* const actorEvent, Actor& user, Actor* const target,const Ability& ability, bool const applyCosts, bool const ko);
    template void Scene::operator()(QString& ret, QVector<QVector<Actor*>*>& parties, const ArenaWidget* const actorRun, QVector<SceneRun*>* const events,
                                    bool const useGuards, int const surprise, int const mInit);
    template Scene::Scene(QString& ret, QVector<QVector<Actor*>*>& parties, const ArenaWidget* const actorRun, QVector<SceneRun*>* const events,
                          bool const useGuards, int const surprise, int const mInit);
#endif

template void Scene::playAi(QString& ret, const Scene::SpriteAct* const spriteRun, Actor& player);
template void Scene::endTurn(QString& ret, const Scene::SpriteAct* const spriteRun, Actor* const actor);
template void Scene::perform(QString& ret, const Scene::SpriteAct* const spriteRun, Actor& user, Actor& target, const Ability& ability, bool const item);
template void Scene::execute(QString& ret, const Scene::SpriteAct* const actorEvent, Actor& user, Actor* const target, const Ability& ability, bool const applyCosts, bool const ko);
template void Scene::operator()(QString& ret, QVector<QVector<Actor*>*>& parties, const Scene::SpriteAct* const actorRun, QVector<SceneRun*>* const events,
                                bool const useGuards, int const surprise, int const mInit);
template Scene::Scene(QString& ret, QVector<QVector<Actor*>*>& parties, const Scene::SpriteAct* const actorRun, QVector<SceneRun*>* const events,
                      bool const useGuards, int const surprise, int const mInit);
