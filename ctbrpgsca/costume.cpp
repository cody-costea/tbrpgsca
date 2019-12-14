#include "costume.h"

using namespace tbrpgsca;

Costume::Costume(int const id, const QString& name, const QString& sprite, int const elm, int const hpDmg, int const mpDmg, int const spDmg,
                 int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi,
                 bool const range, bool const automate, bool const confuse, bool const reflect, bool const counter, bool const revive,
                 QMap<int, int>* const res, QVector<Ability*>* const skills, QVector<State*>* const states, QMap<State*, int>* const stRes)
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
    if (counter)
    {
        flags |= FLAG_COUNTER;
    }
    this->flags = flags;
}
