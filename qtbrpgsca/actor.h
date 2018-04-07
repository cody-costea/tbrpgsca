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

#ifndef ACTOR_H
#define ACTOR_H

#include <QString>
#include <race.h>
#include <job.h>
#include <state.h>
#include <ability.h>
#include <QVector>

namespace qtbrpgsca
{
    class Actor : public Race, public Job
    {
    public:
        QString name;
        int id, hp, mp, sp, atk, def, spi, wis, agi, lv, maxlv,
        exp = 0, maxp = 15, automatic = 0,* res = NULL;
        bool active = true, reflect = false, guards = true;
        QVector<Ability>* items = NULL;
        QVector<Ability*>* extraSkills = NULL;
        QVector<State*>* state = NULL;
        QMap<State*, int>* stateDur = NULL;
        QMap<State*, int>* stateRes = NULL;

        Actor(int id, QString name, QString race, QString job, int lv, int maxlv,
              int maxhp, int maxmp, int maxsp, int matk, int mdef, int mspi, int mwis,
              int magi, int hpp, int mpp, int spp, int atkp, int defp, int spip,
              int wisp, int agip, int* rres, int* jres, Ability** raceSkills, int rSkillsNr,
              Ability** jobSkills, int jSkillsNr, QVector<Ability>* items);

        Actor(int id, QString name, Race* race, Job* job, int lv, int maxlv, QVector<Ability>* items);

        QString execAbility(Ability& ability, Actor& target, bool applyCosts);

        void recover();

        void changeJob(Job* job);

        void levelUp();

        QString checkStatus();

        QString applyState(State& state, bool consume);

        QString applyStates(bool consume);

        ~Actor();

        static const int RESN = 8;

    private:
        bool freeItems = false;

        void calcRes();
        void stats(int id, QString name, int lv, int maxlv, QVector<Ability>* items);
        void checkRes(int i);
    };

}
#endif // ACTOR_H
