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

import java.util.Hashtable;
import java.util.Random;

public final class StateMask extends Costume {
    
  protected Performance[] rSkills;
  protected boolean automate, confuse, inactivate, reflect;
  protected int dur, sRes, dmgHp, dmgMp, dmgSp;

  String inflict(final Actor actor, final boolean always, final boolean indefinite) {
    final Hashtable trgStRes = actor.stRes;
    Object o = null;
    if (always || new Random().nextInt(10) > (trgStRes == null ? 0 : ((o = trgStRes.get(this)) == null
            ? 0 : ((Integer)o).intValue() + this.sRes))) {
      Hashtable trgStates = actor.stateDur;
      if (trgStates == null) {
        trgStates = new Hashtable();
        actor.stateDur = trgStates;
      }
      final int crDur = ((o == null || (o = trgStates.get(this)) == null) ? 0 : ((Integer)o).intValue());
      if (crDur < this.dur || (crDur > -1 && this.dur < 0)) {
        trgStates.put(this, new Integer(indefinite ? -2 : this.dur));
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

  void disableSkills(final Actor actor, final boolean remove) {
    final Performance[] rSkills = this.rSkills;
    if (rSkills == null) {
      return;
    }
    Object q;
    Hashtable iSkills = actor.skillsQty;
    if (remove) {
      if (iSkills == null) {
        return;
      } else {
        for (int i = 0; i < rSkills.length; i++) {
          Performance k = rSkills[i];
          if (k.mQty > 0) {
            iSkills.put(k, new Integer((q = iSkills.get(k)) == null ? 0 : -1 * ((Integer)q).intValue()));
          } else {
            iSkills.remove(k);
          }
        }
      }
    } else {
      if (iSkills == null) {
        iSkills = new Hashtable();
        actor.skillsQty = iSkills;
      }
      for (int i = 0; i < rSkills.length; i++) {
        Performance k = rSkills[i];
        iSkills.put(k, new Integer(k.mQty > 0 ? -1 * ((q = iSkills.get(k)) == null ? 0 : ((Integer)q).intValue()) : 0));
      }
    }
  }

  String apply(final Actor actor, final boolean consume) {
    String s = "";
    final Hashtable sDur = actor.stateDur;
    if (sDur != null) {
      final Object dur = sDur.get(this);
      if (dur != null && actor._hp > 0) {
        final int d = ((Integer)dur).intValue();
        if (d == 0) {
          this.disable(actor);
          sDur.put(this, new Integer(-3));
        }
        else if (d > -3) {
          if (consume) {
            final int rnd = new Random().nextInt(3);
            final int dmghp = (actor.mHp + rnd) * this.dmgHp / 100;
            final int dmgmp = (actor.mMp + rnd) * this.dmgMp / 100;
            final int dmgsp = (actor.mSp + rnd) * this.dmgSp / 100;
            actor.setHp(actor._hp - dmghp);
            actor.setMp(actor._mp - dmgmp);
            actor.setSp(actor._sp - dmgsp);
            if (dmghp != 0 || dmgmp != 0 || dmgsp != 0) {
              s += " " + this.name + " causes " + actor.name + RolePlay.getDmgText(dmghp, dmgmp, dmgsp);
            }
          }
          else {
            if (this.inactivate) {
              if (d > 0 && actor.active) {
                sDur.put(this, new Integer(d - 1));
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

  boolean remove(final Actor actor, final boolean delete, final boolean always) {
    Hashtable sDur = actor.stateDur;
    Object d;
    if (sDur != null && (always || ((d = sDur.get(this)) == null ? -2 : ((Integer)d).intValue()) != -2)) {
      if (((d = sDur.get(this)) == null ? -3 : ((Integer)d).intValue()) > -3) {
        this.disable(actor);
      }
      if (delete) {
        sDur.remove(this);
      } else {
        sDur.put(this, new Integer(-3));
      }
      return true;
    } else {
      return false;
    }
  }

  StateMask(final int id, final String name, final boolean inactivate, final boolean automate, final boolean confuse,
            final boolean reflect, final int dur, final int sRes, final int dmgHp, final int dmgMp, final int dmgSp,
            final int hp, final int mp, final int sp, final int atk, final int def, final int spi, final int wis,
            final int agi, final int mInit, final boolean range, final Hashtable res, final Performance[] aSkills,
            final Performance[] rSkills, final StateMask[] states, final Hashtable stRes) {
    super(id, name, null, hp, mp, sp, atk, def, spi, wis, agi, mInit, range, res, aSkills, states, stRes);
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