/*
Copyright (C) AD 2017 Claudiu-Stefan Costea

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

#include "race.h"

using namespace qtbrpgsca;

Race::Race(int id, QString race, int maxhp, int maxmp, int maxsp, int matk, int mdef,
           int mspi, int mwis, int magi, int* rres, Ability** raceSkills, int rSkillsNr)
{
    this->raceId = id;
    this->raceName = race;
    this->maxhp = maxhp;
    this->maxmp = maxmp;
    this->maxsp = maxsp;
    this->matk = matk;
    this->mdef = mdef;
    this->mspi = mspi;
    this->mwis = mwis;
    this->magi = magi;
    this->rres = rres;
    this->raceSkills = raceSkills;
    this->rSkillsNr = rSkillsNr;
}

Race::~Race()
{

}
