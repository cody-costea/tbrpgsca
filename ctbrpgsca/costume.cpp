#include "costume.h"
#include "actor.h"
#include "role.h"

#include <QStringBuilder>

using namespace tbrpgsca;

QString Costume::CausesTxt = " %s causes %s";

inline int Costume::getAttack() const
{
    return this->atk;
}

inline int Costume::getDefense() const
{
    return this->def;
}

inline int Costume::getSpirit() const
{
    return this->spi;
}

inline int Costume::getWisdom() const
{
    return this->wis;
}

inline int Costume::getAgility() const
{
    return this->agi;
}

inline int Costume::getMaximumActions() const
{
    return this->mActions;
}

inline Ability& Costume::getAddedSkill(int const n) const
{
    return *(this->aSkills->at(n));
}

inline bool Costume::hasAddedSkill(Ability& skill) const
{
    QVector<Ability*>* aSkills = this->aSkills;
    return aSkills != nullptr && aSkills->contains(&skill);
}

inline int Costume::getAddedSkillsSize() const
{
    QVector<Ability*>* aSkills = this->aSkills;
    return aSkills == nullptr ? 0 : aSkills->size();
}

inline Ability& Costume::getCounterSkill(int const n) const
{
    return *(this->counters->at(n));
}

inline bool Costume::hasCounterSkill(Ability& skill) const
{
    QVector<Ability*>* counters = this->counters;
    return counters != nullptr && counters->contains(&skill);
}

inline int Costume::getCounterSkillsSize() const
{
    QVector<Ability*>* counters = this->counters;
    return counters == nullptr ? 0 : counters->size();
}

inline int Costume::getElementResistance(const int element) const
{
    QMap<int, int>* res = this->res;
    return res == nullptr ? 3 : res->value(element, 3);
}

inline int Costume::getStateResistance(State* const state) const
{
    QMap<State*, int>* stRes = this->stRes;
    return stRes == nullptr ? 0 : stRes->value(state, 0);
}

inline bool Costume::isReflecting() const
{
    return (this->flags & FLAG_REFLECT) == FLAG_REFLECT;
}

inline bool Costume::isCountering() const
{
    QVector<Ability*>* counters = this->counters;
    return counters != nullptr && counters->size() > 0;
}

inline bool Costume::isAutomated() const
{
    return (this->flags & FLAG_AUTOMATE) == FLAG_AUTOMATE;
}

inline bool Costume::isConfused() const
{
    return (this->flags & FLAG_CONFUSE) == FLAG_CONFUSE;
}

inline Costume& Costume::adopt(Actor& actor)
{
    return this->adopt(nullptr, actor);
}

inline Costume& Costume::abandon(Actor& actor)
{
    return this->abandon(nullptr, actor);
}

inline Costume& Costume::apply(QString& ret, Actor& actor)
{
    return this->apply(ret, nullptr, actor);
}

Costume& Costume::adopt(Scene* scene, Actor& actor)
{
    Costume& costume = *this;
    actor.dmgType |= costume.dmgType;
    actor.flags |= costume.flags;
    actor.updateAttributes(false, scene, costume);
    actor.updateResistance(false, costume.res, costume.stRes);
    actor.updateCounters(false, *(costume.counters));
    actor.updateSkills(false, *(costume.aSkills));
    return costume;
}

Costume& Costume::abandon(Scene* scene, Actor& actor)
{
    Costume& costume = *this;
    {
        int const roleElm = costume.dmgType;
        int const actorElm = actor.dmgType;
        if ((actorElm & roleElm) == roleElm)
        {
            actor.dmgType = actorElm ^ roleElm;
        }
    }
    {
        int const roleFlags = costume.flags;
        int const actorFlags = actor.flags;
        if ((actorFlags & roleFlags) == roleFlags)
        {
            actor.flags = actorFlags ^ roleFlags;
        }
        /*if ((roleFlags & FLAG_RANGE) == FLAG_RANGE && (actorFlags & FLAG_RANGE) == FLAG_RANGE)
        {
            actor.flags = actorFlags ^ FLAG_RANGE;
        }
        if ((roleFlags & FLAG_REVIVE) == FLAG_REVIVE && (actorFlags & FLAG_REVIVE) == FLAG_REVIVE)
        {
            actor.flags = actorFlags ^ FLAG_REVIVE;
        }*/
    }
    actor.updateAttributes(true, scene, costume);
    actor.updateResistance(true, costume.res, costume.stRes);
    actor.updateCounters(true, *(costume.counters));
    actor.updateSkills(true, *(costume.aSkills));
    actor.refreshFlags();
    return costume;
}

Costume& Costume::apply(QString& ret, Scene* scene, Actor& actor)
{
    Costume& role = *this;
    int rnd = std::rand() % 3;
    int dmgHp = (actor.mHp + rnd) * role.hp / 100;
    int dmgMp = (actor.mMp + rnd) * role.mp / 100;
    int dmgSp = (actor.mSp + rnd) * role.sp / 100;
    int actorHp = actor.hp;
    if (scene == nullptr)
    {
        actor.setCurrentHp(actorHp > dmgHp ? actorHp - dmgHp : 1);
    }
    else
    {
        actor.setCurrentHp(actorHp > dmgHp ? actorHp - dmgHp : 1, ret, *scene);
    }
    actor.setCurrentMp(actor.mp - dmgMp);
    actor.setCurrentRp(actor.sp - dmgSp);
    ret = ret % QString(Costume::CausesTxt).arg(role.name, actor.name);
    Role::AddDmgText(ret, dmgHp, dmgMp, dmgSp);
    return role;
}

Costume::Costume(int const id, QString& name, QString& sprite, int const elm, int const hpDmg, int const mpDmg, int const spDmg,
                 int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi,
                 bool const range, bool const automate, bool const confuse, bool const reflect, bool const revive, QMap<int, int>* const res,
                 QVector<Ability*>* const skills, QMap<State*, int>* const stRes)
    : Role(id, name, sprite, hpDmg, mpDmg, spDmg, mHp, mMp, mSp, elm, range, revive)
{
    this->atk = atk;
    this->def = def;
    this->spi = spi;
    this->wis = wis;
    this->agi = agi;
    this->res = res;
    this->stRes = stRes;
    this->aSkills = skills;
    int flags = this->flags;
    if (automate)
    {
        flags |= FLAG_AUTOMATE;
    }
    if (confuse)
    {
        flags |= FLAG_CONFUSE;
    }
    if (reflect)
    {
        flags |= FLAG_REFLECT;
    }
    this->flags = flags;
}

Costume::Costume(Costume& costume) : Role(costume)
{
    this->atk = costume.atk;
    this->def = costume.def;
    this->spi = costume.spi;
    this->wis = costume.wis;
    this->agi = costume.agi;
    this->res = costume.res;
    this->stRes = costume.stRes;
    this->aSkills = costume.aSkills;
}

Costume::~Costume()
{

}
