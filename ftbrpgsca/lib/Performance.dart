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
import 'package:ftbrpgsca/RolePlay.dart';
import 'package:ftbrpgsca/StateMask.dart';
import 'package:sprintf/sprintf.dart';

class Performance extends RolePlay {

  static const int FLAG_STEAL = 2;
  static const int FLAG_ABSORB = 4;
  static const int FLAG_RESTORE = 8;

  static const int DmgTypeAtk = 1;
  static const int DmgTypeDef = 2;
  static const int DmgTypeSpi = 4;
  static const int DmgTypeWis = 8;
  static const int DmgTypeAgi = 16;

  static String reflectedTxt = ", reflected by %s";
  static String suffersTxt = ", %s suffers";
  static String stolenTxt = ", obtaining %s from %s";
  static String missesTxt = ", but misses";

  String sound;
  int lvRq, hpC, mpC, spC, mQty, rQty, dmgType, trg, elm; //dmgType could be used as a bitwise int, including element types;
  List<StateMask> rStates;

  bool get steal {
    return (this.flags & FLAG_STEAL) == FLAG_STEAL;
  }

  set steal(final bool steal) {
    int flags = this.flags;
    if (steal != ((flags & FLAG_STEAL) == FLAG_STEAL)) {
      flags ^= FLAG_STEAL;
      this.flags = flags;
    }
  }

  bool get absorb {
    return (this.flags & FLAG_ABSORB) == FLAG_ABSORB;
  }

  set absorb(final bool absorb) {
    int flags = this.flags;
    if (absorb != ((flags & FLAG_ABSORB) == FLAG_ABSORB)) {
      flags ^= FLAG_ABSORB;
      this.flags = flags;
    }
  }

  bool get restore {
    return (this.flags & FLAG_RESTORE) == FLAG_RESTORE;
  }

  set restore(final bool restore) {
    int flags = this.flags;
    if (restore != ((flags & FLAG_RESTORE) == FLAG_RESTORE)) {
      flags ^= FLAG_RESTORE;
      this.flags = flags;
    }
  }

  String execute(final Actor user, Actor target, final bool applyCosts) {
    String s = "";
    final Random rnd = new Random();
    int dmg = rnd.nextInt(4);
    if (target.reflects && (this.dmgType == DmgTypeWis)) {
      s += sprintf(Performance.reflectedTxt, [target.name]);
      target = user;
    }
    final bool ko = target.hp < 1;
    final Map<int, int> trgResMap = target.res;
    int res = trgResMap == null ? 3 : (trgResMap[this.elm] ?? 3);
    if (res > 6) {
      res = -1;
    }
    int i = 0;
    int def = 0;
    int canMiss = 0;
    if (this.dmgType & DmgTypeAtk == DmgTypeAtk) {
      canMiss = 2;
      dmg += user.atk;
      def += target.def;
      i++;
    }
    if (this.dmgType & DmgTypeDef == DmgTypeDef) {
      dmg += user.def;
      def += target.def;
      i++;
    }
    if (this.dmgType & DmgTypeSpi == DmgTypeSpi) {
      dmg += user.spi;
      def += target.wis;
      i++;
    }
    if (this.dmgType & DmgTypeWis == DmgTypeWis) {
      dmg += user.wis;
      def += target.spi;
      i++;
    }
    if (this.dmgType & DmgTypeAgi == DmgTypeAgi) {
      canMiss = -canMiss + 4;
      dmg += user.agi;
      def += target.agi;
      i++;
    }
    dmg = i == 0 ? 0 : (this.mInit + (dmg / i)) ~/ (def ~/ i * res + 1);
    if (canMiss == 0 || ((rnd.nextInt(13) + user.agi ~/ canMiss)) > 2 + target.agi ~/ 4) {
      int hpDmg = this.mHp == 0 ? 0 : ((this.mHp < 0 ? -1 : 1) * dmg + this.mHp);
      int mpDmg = this.mMp == 0 ? 0 : ((this.mMp < 0 ? -1 : 1) * dmg + this.mMp);
      int spDmg = this.mSp == 0 ? 0 : ((this.mSp < 0 ? -1 : 1) * dmg + this.mSp);
      if (res < 0) {
        hpDmg = -hpDmg;
        mpDmg = -mpDmg;
        spDmg = -spDmg;
      }
      target.hp -= hpDmg;
      target.mp -= mpDmg;
      target.sp -= spDmg;
      if (this.absorb) {
        user.hp += hpDmg ~/ 2;
        user.mp += mpDmg ~/ 2;
        user.sp += spDmg ~/ 2;
      }
      if (hpDmg != 0 || mpDmg != 0 || spDmg != 0) {
        s += sprintf(Performance.suffersTxt, [target.name]) + RolePlay.getDmgText(hpDmg, mpDmg, spDmg);
      }
      String r;
      final List<StateMask> aStates = this.aStates;
      if (aStates != null) {
        for (int j = 0; j < aStates.length; j++) {
          r = aStates[j].inflict(target, false, false);
          if (r.isNotEmpty) {
            s += r;
          }
        }
      }
      final Map<StateMask, int> trgStateMap = target.stateDur;
      if (trgStateMap != null) {
        final List<StateMask> trgStates = trgStateMap.keys.toList();
        final List<StateMask> rStates = this.rStates;
        if (rStates != null) {
          for (int j = 0; j < rStates.length; j++) {
            for (int k = 0; k < trgStates.length; k++) {
              if (rStates[j] == trgStates[k]) {
                trgStates[k].remove(target, false, false);
              }
            }
          }
        }
      }
      final Map<Performance, int> trgItemMap = target.items;
      if (this.steal && trgItemMap != null && trgItemMap != user.items && trgItemMap.length > 0
          && (rnd.nextInt(12) + user.agi ~/ 4) > 4 + target.agi ~/ 3) {
        //final List<Performance> trgItems = trgItemMap.keys.toList();
        final Performance stolen = trgItemMap.keys.toList()[rnd.nextInt(trgItemMap.length - 1)];
        int trgItemQty = trgItemMap[stolen];
        if (trgItemQty != null && trgItemQty > 0) {
          Map<Performance, int> usrItems = user.items;
          if (usrItems == null) {
            usrItems = new Map();
            user.items = usrItems;
          }
          usrItems[stolen] = (usrItems[stolen] ?? 0) + 1;
          if ((--trgItemQty) == 0) {
            trgItemMap.remove(stolen);
          } else {
            trgItemMap[stolen] = trgItemQty;
          }
          s += sprintf(Performance.stolenTxt, [stolen.name, target.name]);
        }
      }
      s += target.checkStatus();
    }
    else {
      s += Performance.missesTxt;
    }
    if (applyCosts) {
      user.hp -= this.hpC;
      user.mp -= this.mpC;
      user.sp -= this.spC;
      if (this.mQty > 0) {
        Map<Performance, int> usrSkillsQty = user.skillsQty;
        if (usrSkillsQty == null) {
          usrSkillsQty = new Map();
          user.skillsQty = usrSkillsQty;
        }
        usrSkillsQty[this] = (usrSkillsQty[this] ?? this.mQty) - 1;
      }
    }
    if (ko && target.hp > 0) {
      target.applyStates(false);
    }
    return s + user.checkStatus();
  }

  void replenish(final Actor user) {
    if (this.mQty > 0) {
      var usrSkills = user.skillsQty;
      if (usrSkills == null) {
        usrSkills = new Map<Performance, int>();
        user.skillsQty = usrSkills;
      }
      usrSkills[this] = this.mQty;
    }
  }

  bool canPerform(final Actor actor) {
    final skillsQty = actor.skillsQty;
    return this.mpC <= actor.mp && this.hpC < actor.hp && this.spC <= actor.sp && actor.level >= this.lvRq
        && (skillsQty == null || (skillsQty[this] ?? 1) > 0);
  }

  Performance(final int id, final String name, final String sprite, final String sound, final bool steal,
              final bool range, final int lvRq, final int hpC, final int mpC, final int spC, final int dmgType,
              final int atkI, final int hpDmg, final int mpDmg, final int spDmg, final int trg, final int elm,
              final int mQty, final int rQty, final bool absorb, final bool restoreKO, final List<StateMask> aStates,
              final List<StateMask> rStates)
      : super(id, name, sprite, hpDmg, mpDmg, spDmg, atkI, range, aStates) {
    this.sound = sound;
    this.steal = steal;
    this.lvRq = lvRq;
    this.hpC = hpC;
    this.mpC = mpC;
    this.spC = spC;
    this.dmgType = dmgType;
    this.trg = trg;
    this.elm = elm;
    this.mQty = mQty;
    this.rQty = rQty;
    this.absorb = absorb;
    this.restore = restoreKO;
    this.rStates = rStates;
  }

}