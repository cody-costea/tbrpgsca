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

#include "job.h"

using namespace qtbrpgsca;

Job::Job(int id, QString job, int hpp, int mpp, int spp, int atkp, int defp,
         int spip, int wisp, int agip, int* jres, Ability** jobSkills, int jSkillsNr)
{
    this->jobId = id;
    this->jobName = job;
    this->hpp = hpp;
    this->mpp = mpp;
    this->spp = spp;
    this->atkp = atkp;
    this->defp = defp;
    this->spip = spip;
    this->wisp = wisp;
    this->agip = agip;
    this->jres = jres;
    this->jobSkills = jobSkills;
    this->jSkillsNr = jSkillsNr;
}

Job::~Job()
{

}
