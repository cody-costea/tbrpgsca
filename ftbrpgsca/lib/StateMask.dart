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

  static String causesTxt = " %s causes %s";

  List<Performance> rSkills;
  int dur, sRes, dmgHp, dmgMp, dmgSp;

  bool get automate {
    return (this.flags & FLAG_AUTOMATE) == FLAG_AUTOMATE;
  }

  set automate(final bool automate) {
    int flags = this.flags;
    if (automate != ((flags & FLAG_AUTOMATE) == FLAG_AUTOMATE)) {
      flags ^= FLAG_AUTOMATE;
      this.flags = flags;
    }
  }

  bool get confuse {
    return (this.flags & FLAG_CONFUSE) == FLAG_CONFUSE;
  }

  set confuse(final bool confuse) {
    int flags = this.flags;
    if (confuse != ((flags & FLAG_CONFUSE) == FLAG_CONFUSE)) {
      flags ^= FLAG_CONFUSE;
      this.flags = flags;
    }
  }

  bool get inactivate {
    return (this.flags & FLAG_INACTIVATE) == FLAG_INACTIVATE;
  }

  set inactivate(final bool inactivate) {
    int flags = this.flags;
    if (inactivate != ((flags & FLAG_INACTIVATE) == FLAG_INACTIVATE)) {
      flags ^= FLAG_INACTIVATE;
      this.flags = flags;
    }
  }

  bool get reflect {
    return (this.flags & FLAG_REFLECT) == FLAG_REFLECT;
  }

  set reflect(final bool reflect) {
    int flags = this.flags;
    if (reflect != ((flags & FLAG_REFLECT) == FLAG_REFLECT)) {
      flags ^= FLAG_REFLECT;
      this.flags = flags;
    }
  }

  String inflict(final Actor actor, final bool always, final bool indefinite) {
    final Map<StateMask, int> trgStRes = actor.stRes;
    if (always || (new Random().nextInt(10) > (trgStRes == null ? 0 : (trgStRes[this] ?? 0) + this.sRes))) {
      Map<StateMask, int> trgStates = actor.stateDur;
      if (trgStates == null) {
        trgStates = new Map();
        actor.stateDur = trgStates;
      }
      final int crDur = (trgStates[this] ?? 0);
      if (crDur < this.dur || (crDur > -1 && this.dur < 0)) {
        trgStates[this] = indefinite ? -2 : this.dur;
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
        }
        else if (dur > -3) {
          if (consume) {
            final int rnd = new Random().nextInt(3);
            final int dmghp = (actor.mHp + rnd) * this.dmgHp ~/ 100;
            final int dmgmp = (actor.mMp + rnd) * this.dmgMp ~/ 100;
            final int dmgsp = (actor.mSp + rnd) * this.dmgSp ~/ 100;
            actor.hp -= dmghp;
            actor.mp -= dmgmp;
            actor.sp -= dmgsp;
            if (dmghp != 0 || dmgmp != 0 || dmgsp != 0) {
              s += sprintf(StateMask.causesTxt, [this.name, actor.name]) + RolePlay.getDmgText(dmghp, dmgmp, dmgsp);
            }
          }
          else {
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
            if (this.automate && actor.automatic < 2) {
              actor.automatic = 1;
            }
            if (this.confuse) {
              actor.automatic = actor.automatic < 2 ? -1 : -2;
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
      if (sDur[this] ?? -3 > -3) {
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

  StateMask(final int id, final String name, final bool inactivate, final bool automate, final bool confuse, final bool reflect,
            final int dur, final int sRes, final int dmgHp, final int dmgMp, final int dmgSp, final int hp, final int mp, final int sp,
            final int atk, final int def, final int spi, final int wis, final int agi, final int mInit, final bool range, final Map<int, int> res,
            final List<Performance> aSkills, final List<Performance> rSkills, final List<StateMask> states, final Map<StateMask, int> stRes)
      : super(id, name, null, hp, mp, sp, atk, def, spi, wis, agi, mInit, range, res, aSkills, states, stRes) {
    this.inactivate = inactivate;
    this.automate = automate;
    this.confuse = confuse;
    this.reflect = reflect;
    this.dur = dur;
    this.sRes = sRes;
    this.dmgHp = dmgHp;
    this.dmgMp = dmgMp;
    this.dmgSp = dmgSp;
    this.rSkills = rSkills;
  }

}