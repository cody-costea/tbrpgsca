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
  
  protected final static int FLAG_AUTOMATE = 2;
  protected final static int FLAG_CONFUSE = 4;
  protected final static int FLAG_INACTIVATE = 8;
  protected final static int FLAG_REFLECT = 16;
  protected final static int FLAG_REVIVE = 32;
    
  protected Performance[] rSkills;
  protected int dur, sRes, dmgHp, dmgMp, dmgSp;
  
  public int getDuration() {
      return this.dur;
  }
  
  public StateMask setDuration(final int value) {
      this.dur = value;
      return this;
  }
  
  public int getResistance() {
      return this.sRes;
  }
  
  public StateMask setResistance(final int value) {
      this.sRes = value;
      return this;
  }
  
  public int getHpDamage() {
      return this.dmgHp;
  }
  
  public StateMask setHpDamage(final int value) {
      this.dmgHp = value;
      return this;
  }
  
  public int getMpDamage() {
      return this.dmgMp;
  }
  
  public StateMask setMpDamage(final int value) {
      this.dmgMp = value;
      return this;
  }
  
  public int getSpDamage() {
      return this.dmgHp;
  }
  
  public StateMask setSpDamage(final int value) {
      this.dmgHp = value;
      return this;
  }
  
  public Performance[] getRemovedSkills() {
      return this.rSkills;
  }
  
  public StateMask setRemovedSkills(final Performance[] value) {
      this.rSkills = value;
      return this;
  }
  
  public boolean isAutomating() {
      return (this.flags & FLAG_AUTOMATE) == FLAG_AUTOMATE;
  }
  
  public StateMask setAutomating(final boolean value) {
    if (value) {
        this.flags |= FLAG_AUTOMATE;
    } else {
        int flags = this.flags;
        if ((flags & FLAG_AUTOMATE) == FLAG_AUTOMATE) {
            this.flags = flags ^ FLAG_AUTOMATE;
        }
    }
      return this;
  }
  
  public boolean isConfusing() {
      return (this.flags & FLAG_CONFUSE) == FLAG_CONFUSE;
  }
  
  public StateMask setConfusing(final boolean value) {
    if (value) {
        this.flags |= FLAG_CONFUSE;
    } else {
        int flags = this.flags;
        if ((flags & FLAG_CONFUSE) == FLAG_CONFUSE) {
            this.flags = flags ^ FLAG_CONFUSE;
        }
    }
    return this;
  }
  
  public boolean isInactivating() {
      return (this.flags & FLAG_INACTIVATE) == FLAG_INACTIVATE;
  }
  
  public StateMask setInactivating(final boolean value) {
    if (value) {
        this.flags |= FLAG_INACTIVATE;
    } else {
        int flags = this.flags;
        if ((flags & FLAG_INACTIVATE) == FLAG_INACTIVATE) {
            this.flags = flags ^ FLAG_INACTIVATE;
        }
    }
    return this;
  }
  
  public boolean isRefelcting() {
      return (this.flags & FLAG_REFLECT) == FLAG_REFLECT;
  }
  
  public StateMask setReflecting(final boolean value) {
    if (value) {
        this.flags |= FLAG_REFLECT;
    } else {
        int flags = this.flags;
        if ((flags & FLAG_REFLECT) == FLAG_REFLECT) {
            this.flags = flags ^ FLAG_REFLECT;
        }
    }
    return this;
  }

    public boolean isReviving() {
        return (this.flags & FLAG_REVIVE) == FLAG_REVIVE;
    }

    public StateMask setReviving(final boolean value) {
        if (value) {
            this.flags |= FLAG_REVIVE;
        } else {
            int flags = this.flags;
            if ((flags & FLAG_REVIVE) == FLAG_REVIVE) {
                this.flags = flags ^ FLAG_REVIVE;
            }
        }
        return this;
    }

  public String inflict(final Actor actor, final boolean always, final boolean indefinite) {
    final Hashtable trgStRes = actor.stRes;
    final int stateRes = this.sRes;
    Object o = null;
    if (always || stateRes < 0 || new Random().nextInt(10) > (trgStRes == null
            ? 0 : ((o = trgStRes.get(this)) == null ? 0 : ((Integer)o).intValue() + stateRes))) {
      Hashtable trgStates = actor.stateDur;
      if (trgStates == null) {
        trgStates = new Hashtable();
        actor.stateDur = trgStates;
      }
      final int stateDur = this.dur;
      final int crDur = ((o == null || (o = trgStates.get(this)) == null) ? 0 : ((Integer)o).intValue());
      if (crDur < stateDur || (crDur > -1 && stateDur < 0)) {
        trgStates.put(this, new Integer(indefinite ? -2 : stateDur));
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
        } else if (d > -3) {
          if (consume) {
            final int rnd = new Random().nextInt(3);
            final int dmgHp = (actor.mHp + rnd) * this.dmgHp / 100;
            final int dmgMp = (actor.mMp + rnd) * this.dmgMp / 100;
            final int dmgSp = (actor.mSp + rnd) * this.dmgSp / 100;
            final int actorHp = actor._hp;
            actor.setCurrentHp(actorHp > dmgHp ? actorHp - dmgHp : 1);
            actor.setCurrentMp(actor._mp - dmgMp);
            actor.setCurrentSp(actor._sp - dmgSp);
            if (dmgHp != 0 || dmgMp != 0 || dmgSp != 0) {
              s += " " + this.name + " causes " + actor.name + RolePlay.getDmgText(dmgHp, dmgMp, dmgSp);
            }
            if (d > 0) {
              sDur.put(this, new Integer(d - 1));
            }
          } else {
            if (this.isInactivating()) {
              if (d > 0 && actor.isActive()) {
                sDur.put(this, new Integer(d - 1));
              }
              actor.setActive(false);
              actor.setGuarding(false);
            }
            if (this.isRefelcting()) {
              actor.setReflecting(true);
            }
            if (this.isReviving()) {
                actor.setReviving(true);
            }
            if (this.isConfusing()) {
                final int actorAuto = actor.automatic;
                if (actorAuto > Actor.AUTO_CONFUSED) {
                    actor.automatic = actorAuto < Actor.AUTO_ALLY ? Actor.AUTO_CONFUSED : Actor.AUTO_ENEMY;
                }
            } else if (this.isAutomating()) {
                final int actorAuto = actor.automatic;
                if (actorAuto > Actor.AUTO_ENEMY && actorAuto < Actor.AUTO_ALLY)
                    actor.automatic = Actor.AUTO_ENRAGED;
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
    if (this.isRefelcting()) {
      actor.applyStates(false);
    }
  }

  public boolean remove(final Actor actor, final boolean delete, final boolean always) {
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

  StateMask(final int id, final String name, final String sprite, final boolean inactivate, final boolean automate,
            final boolean confuse, final boolean reflect, final boolean revive, final int dur, final int sRes, final int dmgHp,
            final int dmgMp, final int dmgSp, final int hp, final int mp, final int sp, final int atk, final int def, final int spi,
            final int wis, final int agi, final int mInit, final boolean range, final Hashtable res, final Performance[] aSkills,
            final Performance[] rSkills, final StateMask[] states, final Hashtable stRes) {
    super(id, name, sprite, hp, mp, sp, atk, def, spi, wis, agi, mInit, range, res, aSkills, states, stRes);
    if (inactivate) {
        this.flags |= FLAG_INACTIVATE;
    }
    if (automate) {
        this.flags |= FLAG_AUTOMATE;
    }
    if (confuse) {
        this.flags |= FLAG_CONFUSE;
    }
    if (reflect) {
        this.flags |= FLAG_REFLECT;
    }
    if (revive) {
        this.flags |= FLAG_REVIVE;
    }
    this.dur = dur;
    this.sRes = sRes;
    this.dmgHp = dmgHp;
    this.dmgMp = dmgMp;
    this.dmgSp = dmgSp;
    this.rSkills = rSkills;
  }
    
}