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

#ifndef DEMOLIB_H
#define DEMOLIB_H

#include <actor.h>
#include <ability.h>
#include <state.h>
#include <QVector>

namespace qtbrpgsca
{

    class DemoLib
    {
    private:
        int* STD_RES = new int[Actor::RESN] {0, 0, 0, 0, 0, 0, 0, 0};

        QVector<Ability>* getItems()
        {
            QVector<Ability>* items = new QVector<Ability>();
            items->append(Ability(1, "Potion", -25,0,0, 0,0, State::STATE_NONE, State::STATE_NONE));
            return items;
        }

    public:
        DemoLib()
        {

        }

        Ability* SKILLS[57] =
        {
            new Ability(1, "Attack", false,false, 0, 0,0,0, 0,1, 10,0,-3, 0,1, -1,-1, false, State::STATE_NONE, State::STATE_CONFSLEEP),
            new Ability(2, "Defend", false,true, 0, 0,0,0, 3,1, 0,-1,-2, -1,0, -1,-1, false, State::STATE_NONE, State::STATE_NONE),
            new Ability(3, "Heal", false,true, 1, 0,3,0, 3,1, -25,0,0, 0,6, -1,-1, false, State::STATE_REVIVE, State::STATE_NONE),
            new Ability(4, "Meditate", false,true, 1, 0,0,0, 3,1, -1,-7,10, -1,6, -1,-1, false, State::STATE_NONE, State::STATE_DIZZINESS),
            new Ability(5, "Cure", false,true, 3, 0,7,0, 3,1, -17,0,0, 0,7, -1,-1, false, State::STATE_REVIVE, State::STATE_CURE),
            new Ability(6, "Clarity", false,true, 3, 0,0,7, 3,1, 0,-3,0, 0,6, -1,-1, false, State::STATE_CLARITY, State::STATE_MADNESS),
            new Ability(7, "Regen", false,true, 4, 0,10,0, 3,1, -37,0,0, 0,6, -1,-1, false, State::STATE_RVREGEN, State::STATE_POISON),
            new Ability(8, "Prayer", false,true, 5, 0,7,0, 3,1, -23,0,0, 1,0, -1,-1, false, State::STATE_REVIVE, State::STATE_NONE),
            new Ability(9, "Smite", false,false, 1, 1,0,2, 1,1, 4,3,-4, 0,1, -1,-1, false, State::STATE_NONE, State::STATE_CONFCLARITY),
            new Ability(10, "Hit", false,false, 1, 3,0,1, 0,1, 12,0,-4, 0,1, -1,-1, false, State::STATE_NONE, State::STATE_CONFSLEEP),
            new Ability(11, "Bash", false,false, 3, 3,0,5, 1,1, 7,5,-5, 0,1, -1,-1, false, State::STATE_DIZZINESS, State::STATE_CONFCLARITY),
            new Ability(12, "Smash", false,false, 3, 5,0,3, 0,1, 18,0,-7, 0,1, -1,-1, false, State::STATE_NONE, State::STATE_CONFSLEEP),
            new Ability(13, "Berserk", false,false, 4, 7,0,4, 3,1, 0,0,0, -1,0, -1,-1, false, State::STATE_BERSERK, State::STATE_WEAKNESS),
            new Ability(14, "Shock", false,false, 4, 4,0,7, 1,1, 10,5,-9, 0,7, -1,-1, false, State::STATE_DIZZYSTUN, State::STATE_CONFCLARITY),
            new Ability(15, "Crush", false,false, 5, 7,4,0, 0,1, 25,0,-11, 0,1, -1,-1, false, State::STATE_STUN, State::STATE_CONFSLEEP),
            new Ability(16, "Strike", false,true, 1, 0,0,0, 4,1, 13,0,-3, 0,1, 5,3, false, State::STATE_NONE, State::STATE_CONFSLEEP),
            new Ability(17, "Steal", true,true, 1, 0,0,0, 3,1, 0,0,0, 0,1, 5,3, false, State::STATE_NONE, State::STATE_NONE),
            new Ability(18, "Dash", false,true, 3, 0,0,0, 4,1, 15,0,-5, 0,1, 3,7, false, State::STATE_NONE, State::STATE_CONFSLEEP),
            new Ability(19, "Poison", false,true, 3, 0,0,0, 4,2, 5,0,2, 0,1, 3,7, false, State::STATE_POISON, State::STATE_REGEN),
            new Ability(20, "Mug", true,true, 4, 0,0,0, 4,2, 15,0,-7, 0,1, 1,5, false, State::STATE_NONE, State::STATE_CONFSLEEP),
            new Ability(21, "Toxic Gas", false,true, 4, 0,10,0, 6,3, 1,1,1, 1,1, 3,7, false, State::STATE_TPOISON, State::STATE_TREGEN),
            new Ability(22, "Cheer", false,true, 4, 0,10,0, 3,-1, 0,0,-5, -2,0, 3,7, false, State::STATE_VIGOUR, State::STATE_CURE),
            new Ability(23, "Venom Blade", false,false, 5, 0,0,10, 4,1, 17,0,-9, 0,1, 3,7, false, State::STATE_POISON, State::STATE_CONFTREGEN),
            new Ability(24, "Absorb", false,true, 1, 0,0,3, 2,1, 0,7,-3, 0,6, -1,-1, true, State::STATE_NONE, State::STATE_NONE),
            new Ability(25, "Drain", false,true, 3, 0,10,0, 2,1, 15,0,-3, 0,6, -1,-1, true, State::STATE_NONE, State::STATE_NONE),
            new Ability(26, "Fireball", false,true, 1, 0,3,0, 2,1, 11,0,-5, 0,2, -1,-1, false, State::STATE_NONE, State::STATE_SLEEP),
            new Ability(27, "Iceshard", false,true, 1, 0,3,0, 2,1, 11,0,-5, 0,3, -1,-1, false, State::STATE_NONE, State::STATE_SLEEP),
            new Ability(28, "Lighting", false,true, 1, 0,3,0, 2,1, 11,0,-5, 0,4, -1,-1, false, State::STATE_NONE, State::STATE_SLEEP),
            new Ability(29, "Rock", false,true, 1, 0,3,0, 2,1, 11,0,-5, 0,5, -1,-1, false, State::STATE_NONE, State::STATE_SLEEP),
            new Ability(30, "Darkness", false,true, 1, 0,3,0, 2,1, 11,0,-5, 0,6, -1,-1, false, State::STATE_NONE, State::STATE_SLEEP),
            new Ability(31, "Flame", false,true, 3, 0,5,0, 2,1, 15,0,-7, 1,2, -1,-1, false, State::STATE_NONE, State::STATE_SLEEP),
            new Ability(32, "Blizzard", false,true, 3, 0,5,0, 2,1, 15,0,-7, 1,3, -1,-1, false, State::STATE_NONE, State::STATE_SLEEP),
            new Ability(33, "Storm", false,true, 3, 0,5,0, 2,1, 15,0,-7, 1,4, -1,-1, false, State::STATE_NONE, State::STATE_SLEEP),
            new Ability(34, "Earthquake", false,true, 3, 0,5,0, 2,1, 15,0,-7, 1,5, -1,-1, false, State::STATE_NONE, State::STATE_SLEEP),
            new Ability(35, "Eclipse", false,true, 3, 0,5,0, 2,1, 13,0,-7, 1,6, -1,-1, false, State::STATE_NONE, State::STATE_SLEEP),
            new Ability(36, "Flare", false,true, 5, 0,12,0, 2,2, 25,0,-9, 0,1, -1,-1, false, State::STATE_NONE, State::STATE_SLEEP),
            new Ability(37, "Light Ray", false,true, 1, 0,3,0, 3,1, 11,0,-5, 0,7, -1,-1, false, State::STATE_NONE, State::STATE_SLEEP),
            new Ability(38, "Light Burst", false,true, 3, 0,5,0, 3,1, 15,0,-7, 1,7, -1,-1, false, State::STATE_NONE, State::STATE_SLEEP),
            new Ability(39, "Confusion", false,true, 5, 0,15,0, 2,1, 0,0,0, 0,6, -1,-1, false, State::STATE_CONFUSION, State::STATE_CLARITY),
            new Ability(40, "Sleep", false,true, 4, 0,0,17, 4,1, 3,0,17, 0,1, -1,-1, false, State::STATE_SLEEP, State::STATE_CLARITY),
            new Ability(41, "Slash", false,true, 5, 0,10,0, 4,1, 15,0,0, 1,1, 3,7, false, State::STATE_NONE, State::STATE_CONFSLEEP),
            new Ability(42, "Fire Wpn", false,false, 2, 0,3,2, 5,1, 17,0,-7, 0,2, -1,-1, false, State::STATE_NONE, State::STATE_CONFSLEEP),
            new Ability(43, "Ice Wpn", false,false, 2, 0,3,2, 5,1, 17,0,-7, 0,3, -1,-1, false, State::STATE_NONE, State::STATE_CONFSLEEP),
            new Ability(44, "Electric Wpn", false,false, 2, 0,3,2, 5,1, 17,0,-7, 0,4, -1,-1, false, State::STATE_NONE, State::STATE_CONFSLEEP),
            new Ability(45, "Stone Wpn", false,false, 2, 0,3,2, 5,1, 17,0,-7, 0,5, -1,-1, false, State::STATE_NONE, State::STATE_CONFSLEEP),
            new Ability(46, "Dark Wpn", false,false, 2, 0,3,2, 5,1, 17,0,-7, 0,6, -1,-1, false, State::STATE_NONE, State::STATE_CONFSLEEP),
            new Ability(47, "Vampiric Wpn", false,false, 5, 0,10,0, 5,1, 21,0,-9, 0,6, 3,7, true, State::STATE_NONE, State::STATE_CONFSLEEP),
            new Ability(48, "Reflect", false,true, 5, 0,10,0, 3,1, 0,0,0, 0,0, -1,-1, false, State::STATE_REFLECT, State::STATE_NONE),
            new Ability(49, "Meteor", false,true, 5, 0,17,0, 2,1, 19,0,-11, 1,1, -1,-1, false, State::STATE_NONE, State::STATE_SLEEP),
            new Ability(50, "Syphon", false,true, 4, 0,15,0, 2,1, 13,0,-3, 1,6, -1,-1, true, State::STATE_NONE, State::STATE_NONE),
            new Ability(51, "Dragon Breath", false,false, 4, 0,13,7, 5,1, 15,0,-11, 1,1, -1,-1, false, State::STATE_NONE, State::STATE_CONFSLEEP),
            new Ability(52, "Light Wpn", false,false, 2, 0,3,2, 7,1, 17,0,-5, 0,7, -1,-1, false, State::STATE_NONE, State::STATE_CONFSLEEP),
            new Ability(53, "Heal", false,true, 1, 0,3,0, 2,1, -25,0,0, 0,7, -1,-1, false, State::STATE_REVIVE, State::STATE_NONE),
            new Ability(54, "Meditate", false,true, 1, 0,0,2, 2,1, -3,-7,0, -1,7, -1,-1, false, State::STATE_NONE, State::STATE_DIZZINESS),
            new Ability(55, "Cure", false,true, 3, 0,7,0, 2,1, -17,0,0, 0,7, -1,-1, false, State::STATE_REVIVE, State::STATE_CURE),
            new Ability(56, "Clarity", false,true, 3, 0,0,7, 2,1, 0,-3,0, 0,7, -1,-1, false, State::STATE_CLARITY, State::STATE_MADNESS),
            new Ability(57, "Absorb", false,true, 1, 0,0,3, 3,1, 0,7,-3, 0,7, -1,-1, true, State::STATE_NONE, State::STATE_NONE)
        };

        Ability* STD_SKILLS[2] = {this->SKILLS[0], this->SKILLS[1]};


        Race* RACES[4] =
        {
            new Race(1, "Elf", 40,25,10, 7,5,15,12,11, new int[Actor::RESN]{0,-1,1,1,1,1,0,0}, STD_SKILLS, 2),
            new Race(2, "Human", 47,15,13, 9,11,9,11,10, new int[Actor::RESN]{0,0,0,0,0,0,-1,1}, STD_SKILLS, 2),
            new Race(3, "Half-Orc", 55,7,13, 17,12,5,7,9, new int[Actor::RESN]{0,1,-1,-1,-1,-1,0,0}, STD_SKILLS, 2),
            new Race(4, "Gnome", 40,15,20, 12,8,10,5,15, new int[Actor::RESN]{0,0,0,0,0,0,1,-1}, STD_SKILLS, 2)
        };

        Job* JOBS[4] =
        {
            new Job(1, "Berserker",1,0,0, 1,0,0,0,1, new int[8]{0,1,-1,-1,-1,-1},
            new Ability*[6]{SKILLS[8],SKILLS[9],SKILLS[10],SKILLS[11],SKILLS[12],SKILLS[14]}, 6),
            new Job(2, "Sorcerer",0,1,0, 0,0,1,0,1, new int[Actor::RESN]{0,-1,1,1,1,1,1,-1},
            new Ability*[14]{SKILLS[23],SKILLS[24],SKILLS[25],SKILLS[26],SKILLS[27],SKILLS[28],
                            SKILLS[29],SKILLS[30],SKILLS[31],SKILLS[32],SKILLS[33],SKILLS[34],
                            SKILLS[35],SKILLS[38]}, 14),
            new Job(3, "Monk",0,1,0, 0,1,0,1,0, new int[Actor::RESN]{0,0,0,0,0,0,-7,7},
            new Ability*[6]{SKILLS[2],SKILLS[3],SKILLS[4],SKILLS[5],SKILLS[6],SKILLS[7]}, 6),
            new Job(4, "Rogue",0,0,1, 1,0,0,0,1, new int[Actor::RESN]{},
            new Ability*[6]{SKILLS[15],SKILLS[16],SKILLS[17],SKILLS[18],SKILLS[19],SKILLS[38]}, 6),

        };

        QVector<Ability>* ITEMS = this->getItems();

        Actor PARTY[4] =
        {
            Actor(1, "Cody", RACES[1], JOBS[3], 1, 9, ITEMS),
            Actor(2, "Victoria", RACES[0], JOBS[2], 1, 9, ITEMS),
            Actor(3, "Stephanie", RACES[3], JOBS[1], 1, 9, ITEMS),
            Actor(4, "George", RACES[2], JOBS[0], 1, 9, ITEMS)
        };

        Actor ENEMY[4] =
        {
            Actor(5, "Goblin", RACES[3], JOBS[3], 1, 9, NULL),
            Actor(7, "Lizard", RACES[0], JOBS[1], 1, 9, NULL),
            Actor(8, "Troll", RACES[1], JOBS[0], 1, 9, NULL),
            Actor(6, "Ogre", RACES[2], JOBS[0], 1, 9, NULL)
        };

        ~DemoLib()
        {
            int i;
            for (i = 0; i < 4; i++)
            {
                delete this->RACES[i];
            }

            for (i = 0; i < 4; i++)
            {
                delete this->JOBS[i];
            }

            for (i = 0; i < 57; i++)
            {
                delete this->SKILLS[i];
            }
        }

    };

}

#endif // DEMOLIB_H
