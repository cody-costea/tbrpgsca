/*
Copyright (C) AD 2019 Claudiu-Stefan Costea

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
import 'dart:math';

import 'package:ftbrpgsca/Actor.dart';
import 'package:ftbrpgsca/Costume.dart';
import 'package:ftbrpgsca/Performance.dart';
import 'package:ftbrpgsca/RolePlay.dart';
import 'package:sprintf/sprintf.dart';

class StateMask extends Costume {
  static const int FLAG_AUTOMATE = 2;
  static const int FLAG_CONFUSE = 4;
  static const int FLAG_INACTIVATE = 8;
  static const int FLAG_REFLECT = 16;
  static const int FLAG_REVIVE = 32;

  static String causesTxt = " %s causes %s";

  List<Performance> rSkills;
  int dur, sRes, dmgHp, dmgMp, dmgSp;

  bool get automate {
    return (this.flags & FLAG_AUTOMATE) == FLAG_AUTOMATE;
  }

  set automate(final bool automate) {
    int flags = this.flags;
    if (automate != ((flags & FLAG_AUTOMATE) == FLAG_AUTOMATE)) {
      this.flags = flags ^ FLAG_AUTOMATE;
    }
  }

  bool get confuse {
    return (this.flags & FLAG_CONFUSE) == FLAG_CONFUSE;
  }

  set confuse(final bool confuse) {
    int flags = this.flags;
    if (confuse != ((flags & FLAG_CONFUSE) == FLAG_CONFUSE)) {
      this.flags = flags ^ FLAG_CONFUSE;
    }
  }

  bool get inactivate {
    return (this.flags & FLAG_INACTIVATE) == FLAG_INACTIVATE;
  }

  set inactivate(final bool inactivate) {
    int flags = this.flags;
    if (inactivate != ((flags & FLAG_INACTIVATE) == FLAG_INACTIVATE)) {
      this.flags = flags ^ FLAG_INACTIVATE;
    }
  }

  bool get reflect {
    return (this.flags & FLAG_REFLECT) == FLAG_REFLECT;
  }

  set reflect(final bool reflect) {
    int flags = this.flags;
    if (reflect != ((flags & FLAG_REFLECT) == FLAG_REFLECT)) {
      this.flags = flags ^ FLAG_REFLECT;
    }
  }

  bool get revive {
    return (this.flags & FLAG_REVIVE) == FLAG_REVIVE;
  }

  set revive(final bool revive) {
    int flags = this.flags;
    if (revive != ((flags & FLAG_REVIVE) == FLAG_REVIVE)) {
      this.flags = flags ^ FLAG_REVIVE;
    }
  }

  String inflict(final Actor actor, final bool always, final bool indefinite) {
    final int stateRes = this.sRes;
    final Map<StateMask, int> trgStRes = actor.stRes;
    if (always ||
        stateRes < 0 ||
        (new Random().nextInt(10) >
            (trgStRes == null ? 0 : (trgStRes[this] ?? 0) + stateRes))) {
      Map<StateMask, int> trgStates = actor.stateDur;
      if (trgStates == null) {
        trgStates = new Map();
        actor.stateDur = trgStates;
      }
      final int stateDur = this.dur;
      final int crDur = (trgStates[this] ?? 0);
      if (crDur < stateDur || (crDur > -1 && stateDur < 0)) {
        trgStates[this] = indefinite ? -2 : stateDur;
      }
      actor.updateAttributes(false, this);
      actor.updateResistance(false, this.res, this.stRes);
      actor.updateStates(false, this.aStates);
      actor.updateSkills(false, this.aSkills);
      this.disableSkills(actor, false);
      return this.apply(actor, false);
    } else {
      return "";
    }
  }

  void disableSkills(final Actor actor, final bool remove) {
    final List<Performance> rSkills = this.rSkills;
    if (rSkills == null) {
      return;
    }
    Map<Performance, int> iSkills = actor.skillsQty;
    if (remove) {
      if (iSkills == null) {
        return;
      } else {
        for (Performance k in rSkills) {
          if (k.mQty > 0) {
            iSkills[k] = -1 * (iSkills[k] ?? 0);
          } else {
            iSkills.remove(k);
          }
        }
      }
    } else {
      if (iSkills == null) {
        iSkills = new Map();
        actor.skillsQty = iSkills;
      }
      for (Performance k in rSkills) {
        iSkills[k] = (k.mQty > 0 ? -1 * (iSkills[k] ?? 0) : 0);
      }
    }
  }

  String apply(final Actor actor, final bool consume) {
    String s = "";
    final Map<StateMask, int> sDur = actor.stateDur;
    if (sDur != null) {
      final int dur = sDur[this];
      if (dur != null && actor.hp > 0) {
        if (dur == 0) {
          this.disable(actor);
          sDur[this] = -3;
        } else if (dur > -3) {
          if (consume) {
            final int rnd = new Random().nextInt(3);
            final int dmgHp = (actor.mHp + rnd) * this.dmgHp ~/ 100;
            final int dmgMp = (actor.mMp + rnd) * this.dmgMp ~/ 100;
            final int dmgSp = (actor.mSp + rnd) * this.dmgSp ~/ 100;
            final int actorHp = actor.hp;
            actor.hp = (actorHp > dmgHp) ? actorHp - dmgHp : 1;
            actor.mp -= dmgMp;
            actor.sp -= dmgSp;
            if (dmgHp != 0 || dmgMp != 0 || dmgSp != 0) {
              s += sprintf(StateMask.causesTxt, [this.name, actor.name]) +
                  RolePlay.getDmgText(dmgHp, dmgMp, dmgSp);
            }
            if (dur > 0) {
              sDur[this] = dur - 1;
            }
          } else {
            final String sprite = this.sprite;
            if (sprite != null && sprite.length > 0) {
              actor.sprite = sprite;
              actor.shapeShift = true;
            }
            if (this.inactivate) {
              if (dur > 0 && actor.active) {
                sDur[this] = dur - 1;
              }
              actor.active = false;
              actor.guards = false;
            }
            if (this.reflect) {
              actor.reflects = true;
            }
            if (this.revive) {
              actor.revives = true;
            }
            if (this.confuse) {
              final int actorAuto = actor.automatic;
              if (actorAuto > -1) {
                actor.automatic = actorAuto < 2 ? -1 : -2;
              }
            } else if (this.automate) {
              final int actorAuto = actor.automatic;
              if (actorAuto < 2 && actorAuto > -2) {
                actor.automatic = 1;
              }
            }
          }
        }
      }
    }
    return s;
  }

  void disable(final Actor actor) {
    actor.updateAttributes(true, this);
    actor.updateResistance(true, this.res, this.stRes);
    actor.updateStates(true, this.aStates);
    actor.updateSkills(true, this.aSkills);
    this.disableSkills(actor, true);
    if (this.reflect) {
      actor.applyStates(false);
    }
  }

  bool remove(final Actor actor, final bool delete, final bool always) {
    final Map<StateMask, int> sDur = actor.stateDur;
    if (sDur != null && (always || (sDur[this] ?? -2) != -2)) {
      if ((sDur[this] ?? -3) > -3) {
        this.disable(actor);
      }
      if (delete) {
        sDur.remove(this);
      } else {
        sDur[this] = -3;
      }
      return true;
    } else {
      return false;
    }
  }

  StateMask(
      final int id,
      final String name,
      final String sprite,
      final bool inactivate,
      final bool automate,
      final bool confuse,
      final bool reflect,
      final bool revive,
      final int dur,
      final int sRes,
      final int dmgHp,
      final int dmgMp,
      final int dmgSp,
      final int hp,
      final int mp,
      final int sp,
      final int atk,
      final int def,
      final int spi,
      final int wis,
      final int agi,
      final int mInit,
      final bool range,
      final Map<int, int> res,
      final List<Performance> aSkills,
      final List<Performance> rSkills,
      final List<StateMask> states,
      final Map<StateMask, int> stRes)
      : super(id, name, sprite, hp, mp, sp, atk, def, spi, wis, agi, mInit,
            range, res, aSkills, states, stRes) {
    this.inactivate = inactivate;
    this.automate = automate;
    this.confuse = confuse;
    this.reflect = reflect;
    this.revive = revive;
    this.dur = dur;
    this.sRes = sRes;
    this.dmgHp = dmgHp;
    this.dmgMp = dmgMp;
    this.dmgSp = dmgSp;
    this.rSkills = rSkills;
  }
}
