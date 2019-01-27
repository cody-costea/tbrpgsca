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
import 'package:dtbrpgsca/Costume.dart';
import 'package:dtbrpgsca/Performance.dart';
import 'package:dtbrpgsca/StateMask.dart';

class Actor extends Costume {
  Costume _race, _job;
  int _lv = 1, maxLv, _hp, _mp, _sp, _xp, maxp, automatic, init;
  bool active, reflects, guards, _ranged;
  Map<Performance, int> _items, skillsQty, skillsQtyRgTurn;
  Map<StateMask, int> stateDur;
  Map<int, Costume> equipment;

  final List<Performance> _skills = new List();
  List<Performance> get availableSkills {
    return this._skills;
  }

  Costume get race {
    return this._race;
  }

  set race(final Costume race) {
    this.switchCostume(this._race, race);
    this._race = race;
  }

  Costume get job {
    return this._job;
  }

  set job(final Costume job) {
    this.switchCostume(this._job, job);
    this._job = job;
  }

  int get level {
    return this._lv;
  }

  set level(final int lv) {
    final int nLv = lv > this.maxLv ? this.maxLv : (lv < 1 ? 1 : lv);
    while (this._lv < nLv) {
      this._xp = this.maxp;
      this.levelUp();
    }
  }

  int get exp {
    return this._xp;
  }

  set exp(final int xp) {
    this._xp = xp;
    this.levelUp();
  }

  int get hp {
    return this._hp;
  }

  int get mp {
    return this._mp;
  }

  int get sp {
    return this._sp;
  }

  set hp(final int hp) {
    this._hp = hp > this.mHp ? this.mHp : (hp < 0 ? 0 : hp);
  }

  set mp(final int mp) {
    this._mp = mp > this.mMp ? this.mMp : (mp < 0 ? 0 : mp);
  }

  set sp(final int sp) {
    this._sp = sp > this.mSp ? this.mSp : (sp < 0 ? 0 : sp);
  }

  bool get ranged {
    return this._ranged;
  }

  @override
  bool get range {
    bool ranged = this._ranged;
    if (ranged == null) {
      final Map<int, Costume> equipment = this.equipment;
      final Map<StateMask, int> states = this.stateDur;
      ranged = super.range || this.job.range || this.race.range;
      /*|| (equipment != null && equipment. { it.range }
          || (states !== null && states.any { it.value != 0 && it.key.range }))*/
      if (!ranged && equipment != null) {
        for (Costume c in equipment.values) {
          if (c.range) {
            ranged = true;
            break;
          }
        }
      }
      if (!ranged && states != null) {
        for (StateMask s in states.keys) {
          if (s.range) {
            ranged = true;
            break;
          }
        }
      }
      this._ranged = ranged;
    }
    return ranged;
  }

  @override
  set range(final bool range) {
    super.range = range;
    this._ranged = range ? true : null;
  }

  Map<Performance, int> get items {
    Map<Performance, int> items = this._items;
    if (items == null) {
      items = new Map();
      this._items = items;
    }
    return items;
  }

  set items(final Map<Performance, int> items) {
    this._items = items;
  }

  void levelUp() {
    while (this.maxp <= this._xp && this._lv < this.maxLv) {
      this.maxp *= 2;
      this._lv++;
      this.mHp += 3;
      this.mMp += 2;
      this.mSp += 2;
      this.atk++;
      this.def++;
      this.wis++;
      this.spi++;
      this.agi++;
    }
  }

  void checkRegSkill(final Performance skill) {
    if (skill.rQty > 0) {
      Map<Performance, int> regSkills = this.skillsQtyRgTurn;
      if (regSkills == null) {
        regSkills = new Map();
        this.skillsQtyRgTurn = regSkills;
      }
      regSkills[skill] = 0;
    }
  }

  void switchCostume(final Costume oldRole, final Costume newRole) {
    if (oldRole != null) {
      this.updateSkills(true, oldRole.aSkills);
      this.updateAttributes(true, oldRole);
      this.updateResistance(true, oldRole.res, oldRole.stRes);
      this.updateStates(true, oldRole.aStates);
    }
    if (newRole != null) {
      this.updateStates(false, newRole.aStates);
      this.updateResistance(false, newRole.res, newRole.stRes);
      this.updateAttributes(false, newRole);
      this.updateSkills(false, newRole.aSkills);
    }
  }

  void updateStates(final bool remove, final List<StateMask> states) {
    if (states == null) {
      return;
    }
    final Map<StateMask, int> aStates = this.stateDur;
    if (remove) {
      if (aStates == null) {
        return;
      }
      for (StateMask k in states) {
        k.remove(this, true, true);
      }
    }
    else {
      for (StateMask k in states) {
        k.inflict(this, true, true);
      }
    }
  }

  void updateAttributes(final bool remove, final Costume role) {
    int i;
    if (remove) {
      i = -1;
      if (role.range) {
        this._ranged = null;
      }
    } else {
      if (role.range) {
        this._ranged = true;
      }
      i = 1;
    }
    this.mHp += i * role.mHp;
    this.mMp += i * role.mMp;
    this.mSp += i * role.mSp;
    this.atk += i * role.atk;
    this.def += i * role.def;
    this.mSp += i * role.spi;
    this.wis += i * role.wis;
    this.agi += i * role.agi;
    //this.mActions += i * role.mActions
  }

  void updateSkills(final bool remove, final List<Performance> abilities) {
    if (abilities == null) {
      return;
    }
    if (remove) {
      for (Performance k in abilities) {
        this.availableSkills.remove(k);
        if (k.rQty > 0) {
          final Map<Performance, int> regSkills = this.skillsQtyRgTurn;
          if (regSkills != null) {
            regSkills.remove(k);
          }
        }
      }
    }
    else {
      //this.availableSkills.ensureCapacity(this.availableSkills.size + abilities.size)
      for (Performance k in abilities) {
        this.availableSkills.add(k);
        this.checkRegSkill(k);
      }
    }
  }

  void updateResistance(final bool remove, final Map<int, int> resMap, Map<StateMask, int> stResMap) {
    if (resMap != null) {
      Map<int, int> r = this.res;
      if (r == null) {
        if (remove) {
          return;
        }
        else {
          r = new Map();
          this.res = r;
        }
      }
      for (int i in resMap.keys) {
        for (int k in r.keys) {
          if (!remove || i == k) {
            r[k] = (r[k] ?? 3) + (((remove) ? -1 : 1) * (resMap[i] ?? 0));
          }
        }
      }
    }
    if (stResMap != null) {
      Map<StateMask, int> rs = this.stRes;
      if (rs == null) {
        if (remove) {
          return;
        } else {
          rs = new Map();
          this.stRes = rs;
        }
      }
      for (StateMask i in stResMap.keys) {
        for (StateMask k in rs.keys) {
          if (!remove || i == k) {
            rs[k] = (rs[k] ?? 3) + (((remove) ? -1 : 1) * (stResMap[i] ?? 0));
          }
        }
      }
    }
  }

  String checkStatus() {
    String s = "";
    if (this.hp < 1) {
      s += ""; //ko text;
      this.active = false;
      this.guards = false;
      this.sp = 0;
      final Map<StateMask, int> sDur = this.stateDur;
      if (sDur != null) {
        for (StateMask state in sDur.keys) {
          state.remove(this, false, false);
        }
      }
    }
    return s;
  }

  String applyStates(final bool consume) {
    String s = "";
    if (!consume) {
      if (this.automatic < 2 && this.automatic > -2) {
        this.automatic = 0;
      }
      else {
        this.automatic = 2;
      }
      if (this.hp > 0) {
        //if (consume) this.actions = this.mActions
        this.guards = true;
      }
      this.reflects = false;
    }
    bool c = false;
    final Map<StateMask, int> sDur = this.stateDur;
    if (sDur != null) {
      for (MapEntry<StateMask, int> state in sDur.entries) {
        if (state.value > -3 && this.hp > 0) {
          final String r = state.key.apply(this, consume);
          if (r.length > 0) {
            if (c) s += ", ";
            if (consume && !c) {
              //s += "\n"
              c = true;
            }
            s += r;
          }
        }
      }
    }
    s += this.checkStatus();
    if (c && consume) s += ".";
    return s;
  }

  void recover() {
    this._hp = this.mHp;
    this._mp = this.mMp;
    this._sp = 0;
    this.active = true;
    final Map<StateMask, int> sDur = this.stateDur;
    if (sDur != null) {
      for (StateMask state in sDur.keys) {
        state.remove(this, true, false);
      }
      if (sDur.length == 0) {
        this.stateDur = null;
      }
    }
    this.applyStates(false);
    final Map<int, int> res = this.res;
    if (res != null) {
      for (MapEntry<int, int> r in res.entries) {
        if (r.value == 3) {
          res.remove(r.key);
        }
      }
      if (res.length == 0) {
        this.res = null;
      }
    }
    final Map<StateMask, int> stRes = this.stRes;
    if (stRes != null) {
      for (MapEntry<StateMask, int> r in stRes.entries) {
        if (r.value == 0) {
          stRes.remove(r.key);
        }
      }
      if (stRes.length == 0) {
        this.stRes = null;
      }
    }
    final Map<Performance, int> skillQty = this.skillsQty;
    if (skillQty != null) {
      for (Performance ability in skillQty.keys) {
        skillQty[ability] = ability.mQty;
      }
    }
  }

  Costume unequipPos(final int pos) {
    final Map<int, Costume> equipment = this.equipment;
    if (equipment == null) {
      return null;
    } else {
      final Costume r = equipment[pos];
      this.switchCostume(r, null);
      return r;
    }
  }

  int unequipItem(final Costume item) {
    final Map<int, Costume> e = this.equipment;
    if (e != null) {
      for (int k in e.keys) {
        if (e[k] == item) {
          this.unequipPos(k);
          return k;
        }
      }
    }
    return null;
  }

  Costume equipItem(final int pos, final Costume item) {
    final Costume r = this.unequipPos(pos);
    Map<int, Costume> e = this.equipment;
    if (equipment == null) {
      e = new Map();
      this.equipment = e;
    }
    e[pos] = item;
    this.switchCostume(null, item);
    return r;
  }

  Actor(final int id, final String name, final Costume race, final Costume job, final int level, final int maxLv,
        final int mInit, final int mHp, final int mMp, final int mSp, final int atk, final int def, final int spi,
        final int wis, final int agi, final bool range, final Map<int, int> res, final List<Performance> skills,
        final List<StateMask> states, final Map<StateMask, int> stRes)
      : super(id, name, mHp, mMp, mSp, atk, def, spi, wis, agi, mInit, range, res, skills, states, stRes) {
    this._xp = 0;
    this.maxp = 15;
    this.active = true;
    this.range = range;
    this.guards = true;
    this.reflects = false;
    this.automatic = 0;
    this.mInit = mInit;
    this.mHp = mHp;
    this.mMp = mMp;
    this.mSp = mSp;
    this.maxLv = maxLv;
    this.race = race;
    this.job = job;
    this.level = level;
    this.hp = this.mHp;
    this.mp = this.mMp;
    this.sp = this.mSp;
  }
}