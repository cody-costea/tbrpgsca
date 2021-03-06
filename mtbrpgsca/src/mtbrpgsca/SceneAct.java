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

package mtbrpgsca;

import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Random;
import java.util.Vector;

public final class SceneAct {

    public interface SceneRun {
        boolean run(final SceneAct scene, final String ret);
    }

    protected SceneRun onStart, onStop, onNewTurn, beforeAct, afterAct;
    public SceneRun getOnBeginSceneRunnable() {
        return this.onStart;
    }
    public SceneRun getOnEndSceneRunnable() {
        return this.onStop;
    }
    public SceneRun getOnNewTurnRunnable() {
        return this.onNewTurn;
    }
    public SceneRun getBeforeActRunnable() {
        return this.beforeAct;
    }
    public SceneRun getAfterActRunnable() {
        return this.afterAct;
    }
    public SceneAct setOnBeginSceneRunnable(final SceneRun run) {
        this.onStart = run;
        return this;
    }
    public SceneAct setOnEndSceneRunnable(final SceneRun run) {
        this.onStop = run;
        return this;
    }
    public SceneAct setOnNewTurnRunnable(final SceneRun run) {
        this.onNewTurn = run;
        return this;
    }
    public SceneAct setBeforeActRunnable(final SceneRun run) {
        this.beforeAct = run;
        return this;
    }
    public SceneAct setAfterActRunnable(final SceneRun run) {
        this.afterAct = run;
        return this;
    }
    
  protected int _surprise;
  public int getSurprise() {
    return this._surprise;
  }

  protected int _status = 0;
  public int getStatus() {
    return this._status;
  }

  protected volatile int _current;
  public int getCurrent() {
    return this._current;
  }

  protected volatile int _fTarget;
  public int getFirstTarget() {
    return this._fTarget;
  }

  protected volatile int _lTarget;
  public int getLastTarget() {
    return this._lTarget;
  }

  protected volatile int _enIdx;
  public int getEnemyIndex() {
    return this._enIdx;
  }

  protected volatile Performance _lastAbility;
  public Performance getLastAbility() {
    return this._lastAbility;
  }

  protected Actor[] _players;
  public Actor[] getPlayers() {
    return this._players;
  }

  protected boolean _useInit;
  public boolean getUseInit() {
    return this._useInit;
  }

  protected Performance[] _crItems;
  public Performance[] getCrItems() {
    return this._crItems;
}

  public String setNext(String ret, final boolean endTurn) {
    if (this._status == 0) {
      int initInc;
      int minInit = 1;
      final boolean useInit = this._useInit;
      final Actor oldActor = this._players[this._current];
      Actor crActor = oldActor;
      if (endTurn) {
        crActor.setActive(false);
        if (useInit) {
          crActor.init = 0;
        }
        ret += crActor.applyStates(true);
      }
      Actor nxActor;
      do {
        boolean noParty = true;
        boolean noEnemy = true;
        initInc = minInit;
        for (int i = 0; i < this._players.length; i++) {
          nxActor = this._players[i];
          if (nxActor._hp > 0) {
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
              int nInit = nxActor.init + initInc;
              nxActor.init = nInit;
              int mInit = nxActor.mInit;
              if (mInit < 1) {
                mInit = this._players.length;
              }
              if (nInit > mInit) {
                nInit -= mInit;
                if (initInc == 1) {
                  minInit = -1;
                }
                if (nxActor != crActor) {
                  final int cInit = crActor.init - (crActor.mInit < 1 ? this._players.length : crActor.mInit);
                  if (cInit < nInit || (cInit == nInit && nxActor.agi > crActor.agi)) {
                    nxActor.setActive(true);
                    nxActor.applyStates(false);
                    if (nxActor.isActive()) {
                      this._current = i;
                      crActor = nxActor;
                    } else {
                      nxActor.init = 0;
                      if (ret.length() > 0) {
                        ret = "\n" + ret;
                      }
                      ret = nxActor.applyStates(true) + ret;
                    }
                  }
                }
              }
              /*if (minInit > 0 && minInit > mInit) {
                minInit = mInit;
              }*/
            } else {
              if (minInit != 1) {
                nxActor.setActive(true);
              }
              if (crActor != nxActor && nxActor.isActive() && (!crActor.isActive() || nxActor.agi > crActor.agi)) {
                nxActor.applyStates(false);
                if (nxActor.isActive()) {
                  if (initInc > 0) {
                    initInc = 0;
                  }
                  crActor = nxActor;
                  this._current = i;
                } else {
                  if (ret.length() > 0) {
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
          ret = "The party has fallen!" + ret;
          final SceneRun onStop = this.onStop;
          return onStop == null || onStop.run(this, ret)
                  ? ret : this.setNext(ret, endTurn);
        } else if (noEnemy) {
          this._status = 1;
          ret = "The party has won!" + ret;
          final SceneRun onStop = this.onStop;
          return onStop == null || onStop.run(this, ret)
                  ? ret : this.setNext(ret, endTurn);
        } else if (minInit != 0 && !useInit) {
          minInit = 0;
        }
      } while (initInc == 1 && minInit > -1);
      if (oldActor == crActor) {
        if (useInit) {
          oldActor.setActive(true);
        }
        oldActor.applyStates(false);
        if (!oldActor.isActive()) {
          return this.setNext(ret, true);
        }
      } else {
        if (crActor.automatic == Actor.AUTO_NONE) {
          final Hashtable crItems = crActor._items;
          if (crItems == null) {
              this._crItems = null;
          } else {
              int i = 0;
              final int len = crItems.size();
              final Performance[] items = new Performance[len];
              final Enumeration itemsEnum = crItems.keys();
              while (itemsEnum.hasMoreElements()) {
                  items[i++] = (Performance)itemsEnum.nextElement();
              }
              this._crItems = items;
          }
        }
      }
      final Hashtable regSkills = crActor.skillsQtyRgTurn;
      if (regSkills != null) {
        final Enumeration regSkillsEnum = regSkills.keys();
        final Hashtable skillsQty = crActor.skillsQty;
        while (regSkillsEnum.hasMoreElements()) {
          final Performance skill = (Performance)regSkillsEnum.nextElement();
          int skillQtyInt = 0;
          Object skillQtyObj;
          if (skillsQty != null && ((skillQtyObj = skillsQty.get(skill)) == null
                  ? skill.mQty : (skillQtyInt = ((Integer)skillQtyObj).intValue())) < skill.mQty) {
            if (((Integer)regSkills.get(skill)).intValue() == skill.rQty) {
              skillsQty.put(skill, new Integer(skillQtyInt + 1));
              regSkills.put(skill, new Integer(0));
            } else {
              regSkills.put(skill, new Integer(((skillQtyObj = regSkills.get(skill)) == null
                      ? 0 : ((Integer)skillQtyObj).intValue()) + 1));
            }
          }
        }
      }
      final SceneRun onNewTurn = this.onNewTurn;
      if (endTurn && onNewTurn != null && onNewTurn.run(this, ret)
              && crActor.automatic != 0) {
          return this.executeAI("");
      }
    }
    return ret;
  }

    public int getGuardian(final int target, final Performance skill) {
        final Actor[] players = this._players;
        final int current = this._current;
        if (skill.hasRange() || (/*skill._range == null &&*/ players[current].hasRange())) {
            return target;
        }
        int f, l;
        final int enIdx = this._enIdx;
        if (current < enIdx) {
            final int pLength = players.length - 1;
            if (target <= enIdx || target == pLength) {
                return target;
            }
            f = enIdx;
            l = pLength;
        } else {
            if (target >= enIdx - 1 || target == 0) {
                return target;
            }
            f = 0;
            l = enIdx - 1;
        }
        int difF = 0;
        int difL = 0;
        int guardF = target;
        int guardL = target;
        for (int i = f; i < target; i++) {
            final Actor iPlayer = players[i];
            if (iPlayer._hp > 0 && iPlayer.isGuarding()) {
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
                final Actor iPlayer = players[i];
                if (iPlayer._hp > 0 && iPlayer.isGuarding()) {
                    if (guardL == target) {
                        guardL = i;
                    }
                    difL++;
                }
            }
            return difL == 0 ? target : (difF < difL ? guardF : guardL);
        }
    }

  public String executeAbility(final Performance skill, int target, String ret) {
    final SceneRun beforeAct = this.beforeAct;
    if (beforeAct == null || beforeAct.run(this, ret)) {
        switch (skill.trg) {
          case Performance.TRG_ENEMY:
            if (target < this._enIdx) {
              this._fTarget = 0;
              this._lTarget = this._enIdx - 1;
            } else {
              this._fTarget = this._enIdx;
              this._lTarget = this._players.length - 1;
            }
            break;
          case Performance.TRG_ALL:
            this._fTarget = 0;
            this._lTarget = this._players.length - 1;
            break;
          case Performance.TRG_PARTY:
            if (this._current < this._enIdx) {
              this._fTarget = 0;
              this._lTarget = this._enIdx - 1;
            } else {
              this._fTarget = this._enIdx;
              this._lTarget = this._players.length - 1;
            }
            break;
          case Performance.TRG_SELF:
            this._fTarget = this._lTarget = this._current;
            break;
          default:
            target = this.getGuardian(target, skill);
            this._fTarget = target;
            this._lTarget = target;
        }
        boolean applyCosts = true;
        final Actor crActor = this._players[this._current];
        ret += crActor.name + " performs " + skill.name;
        for (int i = this._fTarget; i <= this._lTarget; i++) {
          if ((skill.mHp < 0 && skill.isRestoring()) || this._players[i]._hp > 0) {
            ret += skill.execute(crActor, this._players[i], applyCosts);
            applyCosts = false;
          }
        }
        ret += ".";
        this._lastAbility = skill;
        final SceneRun afterAct = this.afterAct;
        if (afterAct == null || afterAct.run(this, ret)) {
            crActor._xp++;
            crActor.levelUp();
        }
    }
    return ret;
  }

    public String executeAI(String ret) {
        int skillIndex = 0;
        boolean nHeal = false;
        boolean nRestore = false;
        final int enIdx = this._enIdx;
        boolean party = this._current < enIdx;
        final Actor[] players = this._players;
        int f;
        int l;
        if (party) {
            f = 0;
            l = enIdx;
        } else {
            f = enIdx;
            l = players.length;
        }
        for (int i = f; i < l; i++) {
            if (players[i]._hp < 1) {
                nRestore = true;
            } else if (players[i]._hp < players[i].mHp / 3) {
                nHeal = true;
            }
        }
        final Actor crActor = players[this._current];
        final Vector crSkills = crActor.getAvailableSkills();
        if (nRestore || nHeal) {
            for (int i = 0; i < crSkills.size(); i++) {
                final Performance s = (Performance)crSkills.elementAt(i);
                if ((s.isRestoring() || (nHeal && s.mHp < 0)) && s.canPerform(crActor)) {
                    skillIndex = i;
                    break;
                }
            }
        }
        final Performance ability = (Performance)crSkills.elementAt(this.getAIskill(skillIndex, nRestore));
        final boolean atkSkill = ability.mHp > -1;
        if (atkSkill) {
            if (party) {
                f = enIdx;
                l = players.length;
            } else {
                f = 0;
                l = enIdx;
            }
        }
        int target = f;
        Actor trgActor = players[target];
        final boolean restore = ability.isRestoring();
        while (trgActor._hp < 1 && (atkSkill || !restore) && target < l) {
            trgActor = players[++target];
        }
        for (int i = target + 1; i < l; i++) {
            final Actor iPlayer = players[i];
            if (iPlayer._hp < trgActor._hp && (iPlayer._hp > 0 || restore)) {
                trgActor = iPlayer;
                target = i;
            }
        }
        return this.executeAbility(ability, target, ret);
    }

    public int getAIskill(final int defSkill, final boolean nRestore) {
        int ret = defSkill;
        final Actor crActor = this._players[this._current];
        final Vector crSkills = crActor.getAvailableSkills();
        Performance s = (Performance)crSkills.elementAt(defSkill);
        for (int i = defSkill + 1; i < crSkills.size(); i++) {
            final Performance a = (Performance)crSkills.elementAt(i);
            if (a.canPerform(crActor)) {
                if (defSkill > 0) {
                    if (a.mHp < s.mHp && (a.isRestoring() || !nRestore)) {
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

  public String performSkill(final int index, final int target, final String ret) {
    return this.executeAbility((Performance)this._players[this._current].getAvailableSkills().elementAt(index), target, ret);
  }

  public String useAbility(final Performance item, final int target, final String ret) {
    final Hashtable crItemsMap = this._players[this._current]._items;
    if (crItemsMap != null) {
      final Object itemQtyObj;
      final int itemQty = ((itemQtyObj = crItemsMap.get(item)) == null
              ? 1 : ((Integer)itemQtyObj).intValue()) - 1;
      if (itemQty > 0) {
        crItemsMap.put(item, new Integer(itemQty));
      } else {
        crItemsMap.remove(item);
      }
    }
    return this.executeAbility(item, target, ret);
  }

  public String useItem(final int index, final int target, final String ret) {
    final Performance[] crItems = this._crItems;
    if (crItems != null && index < crItems.length) {
      return this.useAbility((Performance)crItems[index], target, ret);
    } else {
      return ret;
    }
  }

  public String escape() {
    this._lastAbility = null;
    final int surprise = this._surprise;
    if (surprise < 0) {
      return "The party attempted to escape, but failed.";
    }
    final int enIdx = this._enIdx;
    final Actor[] players = this._players;
    int pAgiSum = 0;
    int eAgiSum = 0;
    if (surprise < 1) {
      for (int i = 0; i < enIdx; i++) {
        final Actor actor = players[i];
        if (actor._hp > 0) {
          pAgiSum += actor.agi;
        }
      }
      pAgiSum /= enIdx;
      for (int i = enIdx; i < players.length; i++) {
        final Actor actor = players[i];
        if (actor._hp > 0) {
          eAgiSum += actor.agi;
        }
      }
      eAgiSum /= (players.length - enIdx);
    }
    if (surprise > 0 || (new Random().nextInt(7) + pAgiSum > eAgiSum)) {
      this._status = -1;
      final SceneRun onStop = this.onStop;
      final String ret = "The party has escaped!";
      if (onStop == null || onStop.run(this, ret)) {
        return ret;
      } else {
        this._status = 0;
      }
    }
    return "The party attempted to escape, but failed.";
  }

  public SceneAct(final Actor[] party, final Actor[] enemy, final int surprise) {
    boolean useInit = false;
    final int enIdx = this._fTarget = this._lTarget = this._enIdx = party.length;
    final Actor[] players = this._players = new Actor[party.length + enemy.length];//this._players = party + enemy;
    this._current = surprise < 0 ? enIdx : 0;
    for (int i = 0; i < players.length; i++) {
      Actor iPlayer = players[i];
      if (iPlayer == null) {
        iPlayer = players[i] = i < enIdx ? party[i] : enemy[i + enIdx];
      }
      if (!useInit && iPlayer.mInit != 0) {
        useInit = true;
      }
      final int iInit = iPlayer.mInit > 0 ? iPlayer.mInit : players.length;
      if ((surprise < 0 && i < enIdx) || (surprise > 0 && i >= enIdx)) {
        iPlayer.init = 0;
        iPlayer.setActive(false);
      } else {
        iPlayer.init = iInit;
        iPlayer.setActive(players[i]._hp > 0);
      }
      for (int j = 0; j < players.length; j++) {
        if (j == i) {
          continue;
        } else {
          Actor jPlayer = players[j];
          if (jPlayer == null) {
            jPlayer = players[j] = j < enIdx ? party[j] : enemy[j - enIdx];
          }
          final int jInit = jPlayer.mInit > 0 ? jPlayer.mInit : players.length;
          if (iInit < jInit) {
            jPlayer.init -= (jInit - iInit);
          }
        }
      }
    }
    this._surprise = surprise;
    this._useInit = useInit;
    final String ret = this.setNext("", false);
    final SceneRun onStart = this.onStart;
    if (onStart == null || onStart.run(this, ret)) {
        final SceneRun onNewTurn = this.onNewTurn;
        if (onNewTurn != null && onNewTurn.run(this, ret)
                && players[this._current].automatic != 0) {
            this.executeAI("");
        }
    }
  }
    
}