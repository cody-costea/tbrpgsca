/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#include "trscene.h"

void tr_role_damage(char *ret, TrScene *const scene, TrActor *user, TrActor *target, const TR_NR dmg_type, TR_NR hp_dmg, TR_NR mp_dmg, TR_NR sp_dmg, const TR_NR rnd_dmg, const TR_BOOL percent) {
    if (!TR_HAS_ALL_FLAGS(target->flags, FLAG_INVINCIBLE)) {
        if (percent) {
            hp_dmg = (user->m_hp + (rnd_dmg + (rand() % DMG_RND))) * hp_dmg / 100;
            mp_dmg = (user->m_mp + (rnd_dmg + (rand() % DMG_RND))) * mp_dmg / 100;
            sp_dmg = (user->m_sp + (rnd_dmg + (rand() % DMG_RND))) * sp_dmg / 100;
        } else {
            if (hp_dmg != 0)
            {
                hp_dmg += hp_dmg < 0 ? (-1 * (rnd_dmg + (rand() % DMG_RND))) : (rnd_dmg + (rand() % DMG_RND));
            }
            if (mp_dmg != 0)
            {
                mp_dmg += mp_dmg < 0 ? (-1 * (rnd_dmg + (rand() % DMG_RND))) : (rnd_dmg + (rand() % DMG_RND));
            }
            if (sp_dmg != 0)
            {
                sp_dmg += sp_dmg < 0 ? (-1 * (rnd_dmg + (rand() % DMG_RND))) : (rnd_dmg + (rand() % DMG_RND));
            }
        } {
            TrIndexMap *trg_res_map = user->res;
            if (trg_res_map)
            {
                int res = DEFAULT_RES; {
                    tr_nr i;
                    for (i = 0; i < trg_res_map->count; i += 1) {
                        tr_nr elm = trg_res_map->keys[i];
                        if (elm == -1) {
                            break;
                        }
                        if ((dmg_type & elm) == elm) {
                            res += trg_res_map->values[i];
                        }
                    }
                }
                if (res > 0) {
                    if (res > 7) {
                        res = -7 + (res - 7);
                        if (res > -1) {
                            res = -1 * (res + 2);
                        }
                    } else if (res == 7) {
                        //ret += Ability::ResistTxt.arg(actor.name());
                        return;
                    } else {
                        hp_dmg /= res;
                        mp_dmg /= res;
                        sp_dmg /= res;
                        goto applyChanges;
                    }
                } else {
                    res = -1 * (res - 2);
                }
                hp_dmg *= res;
                mp_dmg *= res;
                sp_dmg *= res;
            } else {
                hp_dmg /= DEFAULT_RES;
                mp_dmg /= DEFAULT_RES;
                sp_dmg /= DEFAULT_RES;
            }
        }
        applyChanges:;
        tr_bool c = 0;
        if (sp_dmg != 0) {
            c = 1;
            //ret += Ability::SuffersTxt.arg(actor.name());
            if (sp_dmg < 0) {
                //ret += "+";
            }
            //ret += (QString("%1 %2").arg(QString::number(-dmgSp), Role::RpTxt));
            tr_actor_set_current_sp(target, target->c_sp - sp_dmg);
        }
        if (mp_dmg != 0) {
            if (c) {
                //ret += ", ";
            } else {
                //ret += Ability::SuffersTxt.arg(actor.name());
                c = 1;
            }
            if (mp_dmg < 0) {
                //ret += "+";
            }
            //ret += (QString("%1 %2").arg(QString::number(-dmgMp), Role::MpTxt));
            tr_actor_set_current_mp(target, target->c_mp - mp_dmg);
        }
        if (hp_dmg != 0) {
            if (c) {
                //ret += ", ";
            } else {
                //ret += Ability::SuffersTxt.arg(actor.name());
                c = 1;
            }
            if (hp_dmg < 0) {
                //ret += "+";
            }
            const int c_hp = target->c_hp;
            if (percent && hp_dmg >= c_hp)
            {
                hp_dmg = c_hp - 1;
            }
            //ret += (QString("%1 %2").arg(QString::number(-dmgHp), Role::HpTxt));
            tr_actor_set_current_hp(target, ret, scene, target->c_hp - hp_dmg, percent);
            /*if (actor.hp < 1)
            {
                ret += Actor::KoTxt.arg(actor.name);
            }*/
        }
        if (c && user) {
            tr_actor_set_current_sp(target, target->c_sp + (sp_dmg / 2));
            tr_actor_set_current_mp(target, target->c_mp + (mp_dmg / 2));
            tr_actor_set_current_hp(user, ret, scene, user->c_hp + (hp_dmg / 2), percent);
        }
    }
}
