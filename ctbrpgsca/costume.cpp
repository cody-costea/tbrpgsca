#include "costume.h"

using namespace tbrpgsca;

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

Costume::Costume(int const id, QString& name, QString& sprite, int const elm, int const hpDmg, int const mpDmg, int const spDmg,
                 int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi,
                 bool const range, bool const automate, bool const confuse, bool const reflect, bool const revive, QMap<int, int>* const res,
                 QVector<Ability*>* const skills, QVector<State*>* const states, QMap<State*, int>* const stRes)
    : Role(id, name, sprite, hpDmg, mpDmg, spDmg, mHp, mMp, mSp, elm, range, revive, states)
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
