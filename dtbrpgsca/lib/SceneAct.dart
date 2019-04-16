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

import 'package:dtbrpgsca/Actor.dart';
import 'package:dtbrpgsca/Performance.dart';
import 'package:sprintf/sprintf.dart';

class SceneAct {

  static String performsTxt = "%s performs %s";
  static String victoryTxt = "The party has won!";
  static String fallenTxt = "The party has fallen!";
  static String escapeTxt = "The party has escaped!";
  static String failTxt = "The party attempted to escape, but failed.";

  bool _useInit;
  int _status = 0;
  int _enIdx;
  int _surprise;
  int _current;
  int _fTarget;
  int _lTarget;
  List<Actor> _players;
  Map<int, List<Performance>> _crItems;
  Performance _lastAbility;

  int get status {
    return this._status;
  }

  int get current {
    return this._current;
  }

  int get firstTarget {
    return this._fTarget;
  }

  int get lastTarget {
    return this._lTarget;
  }

  int get enemyIndex {
    return this._enIdx;
  }

  Performance get lastAbility {
    return this._lastAbility;
  }

  List<Actor> get players {
    return this._players;
  }

  bool get useInit {
    return this._useInit;
  }

  Map<int, List<Performance>> get crItems {
    return this._crItems;
}

  String setNext(String ret, final bool endTurn) {
    if (this._status == 0) {
      int initInc;
      int minInit = 1;
      final bool useInit = this._useInit;
      final Actor oldActor = this._players[this._current];
      Actor crActor = oldActor;
      if (endTurn) {
        crActor.active = false;
        if (useInit) {
          crActor.init = 0;
        }
        ret += crActor.applyStates(true);
      }
      Actor nxActor;
      do {
        bool noParty = true;
        bool noEnemy = true;
        initInc = minInit;
        for (int i = 0; i < this._players.length; i++) {
          nxActor = this._players[i];
          if (nxActor.hp > 0) {
            if (i < this._enIdx) {
              if (noParty) {
                noParty = false;
              }
            } else {
              if (noEnemy) {
                noEnemy = false;
              }
            }
            if (useInit) {
              nxActor.init += initInc;
              int nInit = nxActor.init;
              final int mInit = nxActor.mInit < 1
                  ? this._players.length
                  : nxActor.mInit;
              if (nInit < mInit) {
                nInit -= mInit;
                if (initInc == 1) {
                  minInit = -1;
                }
                if (nxActor != crActor) {
                  final int cInit = crActor.init -
                      (crActor.mInit < 1 ? this._players.length : crActor.mInit);
                  if (cInit < nInit || (cInit == nInit && nxActor.agi > crActor.agi)) {
                    nxActor.active = true;
                    nxActor.applyStates(false);
                    if (nxActor.active) {
                      this._current = i;
                      crActor = nxActor;
                    } else {
                      nxActor.init = 0;
                      if (ret.length > 0) {
                        ret = "\n" + ret;
                      }
                      ret = nxActor.applyStates(true) + ret;
                    }
                  }
                }
              }
              if (minInit > 0 && minInit > mInit) {
                minInit = mInit;
              }
            } else {
              if (minInit != 1) {
                nxActor.active = true;
              }
              if (crActor != nxActor && nxActor.active && (!crActor.active || nxActor.agi > crActor.agi)) {
                nxActor.applyStates(false);
                if (nxActor.active) {
                  if (initInc > 0) {
                    initInc = 0;
                  }
                  crActor = nxActor;
                  this._current = i;
                } else {
                  if (ret.length > 0) {
                    ret = "\n" + ret;
                  }
                  ret = nxActor.applyStates(true) + ret;
                }
              }
            }
          }
        }
        if (noParty) {
          this._status = -2;
          ret = SceneAct.fallenTxt + ret;
          return ret;
        } else if (noEnemy) {
          this._status = 1;
          ret = SceneAct.victoryTxt + ret;
          return ret;
        } else if (minInit != 0 && !useInit) {
          minInit = 0;
        }
      } while (initInc == 1 && minInit > -1);
      if (oldActor == crActor) {
        if (useInit) {
          oldActor.active = true;
        }
        oldActor.applyStates(false);
        if (!oldActor.active) {
          return this.setNext(ret, true);
        }
      } else {
        if (crActor.automatic == 0) {
          final Map<Performance, int> crItems = crActor.items;
          if (crItems != null) {
            Map<int, List<Performance>> crItemsMap = this._crItems;
            if (crItemsMap == null) {
              crItemsMap = new Map();
              this._crItems = crItemsMap;
            }
            crItemsMap[this._current] = crItems.keys.toList();
          }
        }
      }
      final Map<Performance, int> regSkills = crActor.skillsQtyRgTurn;
      if (regSkills != null) {
        for (Performance skill in regSkills.keys) {
          final Map<Performance, int> skillsQty = crActor.skillsQty;
          if (skillsQty != null &&
              (skillsQty[skill] ?? skill.mQty) < skill.mQty) {
            if (regSkills[skill] == skill.rQty) {
              skillsQty[skill] = (skillsQty[skill] ?? 0) + 1;
              regSkills[skill] = 0;
            } else {
              regSkills[skill] = (regSkills[skill] ?? 0) + 1;
            }
          }
        }
      }
    }
    return ret;
  }

  int getGuardian(final int target, final Performance skill) {
    if (skill.range || (skill.range == null && this._players[this._current].range)) {
      return target;
    }
    int f;
    int l;
    if (this._current < this._enIdx) {
      if (target <= this._enIdx || target == this._players.length - 1) {
        return target;
      }
      f = this._enIdx;
      l = this._players.length - 1;
    } else {
      if (target >= this._enIdx - 1 || target == 0) {
        return target;
      }
      f = 0;
      l = this._enIdx - 1;
    }
    int difF = 0;
    int difL = 0;
    int guardF = target;
    int guardL = target;
    for (int i = f; i < target; i++) {
      if (this._players[i].hp > 0 && this._players[i].guards) {
        if (guardF == target) {
          guardF = i;
        }
        difF++;
      }
    }
    if (difF == 0) {
      return target;
    } else {
      for (int i = l; i > target; i--) {
        if (this._players[i].hp > 0 && this._players[i].guards) {
          if (guardL == target) {
            guardL = i;
          }
          difL++;
        }
      }
      return difL == 0 ? target : (difF < difL ? guardF : guardL);
    }
  }

  String executeAbility(final Performance skill, int target, String ret) {
    switch (skill.trg) {
      case 1:
        if (target < this._enIdx) {
          this._fTarget = 0;
          this._lTarget = this._enIdx - 1;
        } else {
          this._fTarget = this._enIdx;
          this._lTarget = this._players.length - 1;
        }
        break;
      case 2:
        this._fTarget = 0;
        this._lTarget = this._players.length - 1;
        break;
      case -2:
        if (this._current < this._enIdx) {
          this._fTarget = 0;
          this._lTarget = this._enIdx - 1;
        } else {
          this._fTarget = this._enIdx;
          this._lTarget = this._players.length - 1;
        }
        break;
      case -1:
        this._fTarget = this._lTarget = this._current;
        break;
      default:
        target = this.getGuardian(target, skill);
        this._fTarget = target;
        this._lTarget = target;
    }
    bool applyCosts = true;
    ret += sprintf("\n${SceneAct.performsTxt}", [this.players[this.current].name, skill.name]);
    for (int i = this._fTarget; i <= this._lTarget; i++) {
      if ((skill.mHp < 0 && skill.restore) || this._players[i].hp > 0) {
        ret += skill.execute(this._players[this._current], this._players[i], applyCosts);
        applyCosts = false;
      }
    }
    ret += ".";
    this._players[this._current].exp++;
    this._players[this._current].levelUp();
    this._lastAbility = skill;
    return ret;
  }

  String executeAI(String ret) {
    int skillIndex = 0;
    bool nHeal = false;
    bool nRestore = false;
    bool party = this._current < this._enIdx;
    int f;
    int l;
    if (party) {
      f = 0;
      l = this._enIdx;
    } else {
      f = this._enIdx;
      l = this._players.length;
    }
    for (int i = f; i < l; i++) {
      if (this._players[i].hp < 1) {
        nRestore = true;
      } else if (this._players[i].hp < this._players[i].mHp ~/ 3) {
        nHeal = true;
      }
    }
    final Actor crActor = this._players[this._current];
    if (nRestore || nHeal) {
      for (int i = 0; i < crActor.availableSkills.length; i++) {
        final Performance s = crActor.availableSkills[i];
        if ((s.restore || (nHeal && s.mHp < 0)) && s.canPerform(this.players[this.current])) {
          skillIndex = i;
        }
        break;
      }
    }
    final Performance ability = crActor.availableSkills[this.getAIskill(skillIndex, nRestore)];
    final bool atkSkill = ability.mHp > -1;
    if (atkSkill) {
      if (party) {
        f = this._enIdx;
        l = this._players.length;
      } else {
        f = 0;
        l = this._enIdx;
      }
    }
    int target = f;
    while (this._players[target].hp < 1 && (atkSkill || !ability.restore) && target < l) {
      target++;
    }
    for (int i = target; i < l; i++) {
      if (this._players[i].hp < this._players[target].hp && (this._players[i].hp > 0 || ability.restore)) {
        target = i;
      }
    }
    return this.executeAbility(ability, target, ret);
  }

  int getAIskill(final int defSkill, final bool nRestore) {
    int ret = defSkill;
    final Actor crActor = this._players[this._current];
    Performance s = crActor.availableSkills[defSkill];
    for (int i = defSkill + 1; i < crActor.availableSkills.length; i++) {
      final Performance a = crActor.availableSkills[i];
      if (a.canPerform(crActor)) {
        if (defSkill > 0) {
          if (a.mHp < s.mHp && (a.restore || !nRestore)) {
            s = a;
            ret = i;
          }
        } else if (a.mHp > s.mHp) {
          s = a;
          ret = i;
        }
      }
    }
    return ret;
  }

  String performSkill(final int index, final int target, final String ret) {
    return this.executeAbility(this._players[this._current].availableSkills[index], target, ret);
  }

  String useItem(final Performance item, final int target, final String ret) {
    final Map<Performance, int> crItemsMap = this._players[this._current].items;
    if (crItemsMap != null) {
      final int itemQty = (crItemsMap[item] ?? 1) - 1;
      if (itemQty > 0) {
        crItemsMap[item] = itemQty;
      } else {
        crItems.remove(item);
        crItemsMap.remove(item);
      }
    }
    return this.executeAbility(item, target, ret);
  }

  String escape() {
    final int enIdx = this._enIdx;
    final List<Actor> players = this._players;
    final int surprise = this._surprise;
    int pAgiSum = 0;
    int eAgiSum = 0;
    if (surprise < 1) {
      for (int i = 0; i < enIdx; i++) {
        pAgiSum += players[i].agi;
      }
      pAgiSum ~/= enIdx;
      for (int i = enIdx; i < players.length; i++) {
        eAgiSum += players[i].agi;
      }
      eAgiSum ~/= (players.length - enIdx);
    }
    this._lastAbility = null;
    if (surprise > 0 || (new Random().nextInt(7) + pAgiSum > eAgiSum)) {
      this._status = -1;
      return SceneAct.escapeTxt;
    } else {
      return SceneAct.failTxt;
    }
  }

  SceneAct(final List<Actor> party, final List<Actor> enemy, final int surprise) {
    bool useInit = false;
    final int enIdx = this._fTarget = this._lTarget = this._enIdx = party.length;
    final List<Actor> players = this._players = party + enemy;
    this._current = surprise < 0 ? enIdx : 0;
    for (int i = 0; i < players.length; i++) {
      if (!useInit && players[i].mInit != 0) {
        useInit = true;
      }
      final int iInit = players[i].mInit > 0 ? players[i].mInit : players.length;
      if ((surprise < 0 && i < enIdx) || (surprise > 0 && i >= enIdx)) {
        players[i].init = 0;
        players[i].active = false;
      } else {
        players[i].init = iInit;
        players[i].active = players[i].hp > 0;
      }
      for (int j = 0; j < players.length; j++) {
        if (j == i) {
          continue;
        } else {
          final int jInit = players[j].mInit > 0 ? players[j].mInit : players.length;
          if (iInit < jInit) {
            players[j].init -= (jInit - iInit);
          }
        }
      }
    }
    this._surprise = surprise;
    this._useInit = useInit;
    this.setNext("", false);
  }

}