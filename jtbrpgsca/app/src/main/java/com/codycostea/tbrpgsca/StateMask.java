/*
Copyright (C) AD 2013-2019 Claudiu-Stefan Costea

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
package com.codycostea.tbrpgsca;

import android.os.Parcel;
import android.util.SparseIntArray;

import java.util.HashMap;
import java.util.Locale;
import java.util.Random;

public final class StateMask extends Costume {

    public static String causesTxt = " %s causes %s";

    protected final static int FLAG_AUTOMATE = 2;
    protected final static int FLAG_CONFUSE = 4;
    protected final static int FLAG_INACTIVATE = 8;
    protected final static int FLAG_REFLECT = 16;
    protected final static int FLAG_REVIVE = 32;

    protected Performance[] rSkills;
    protected int dur, sRes, dmgHp, dmgMp, dmgSp;

    protected StateMask(final Parcel in) {
        super(in);
        this.rSkills = in.createTypedArray(Performance.CREATOR);
        this.dur = in.readInt();
        this.sRes = in.readInt();
        this.dmgHp = in.readInt();
        this.dmgMp = in.readInt();
        this.dmgSp = in.readInt();
    }

    @Override
    public void writeToParcel(final Parcel dest, final int flags) {
        super.writeToParcel(dest, flags);
        dest.writeTypedArray(this.rSkills, flags);
        dest.writeInt(this.dur);
        dest.writeInt(this.sRes);
        dest.writeInt(this.dmgHp);
        dest.writeInt(this.dmgMp);
        dest.writeInt(this.dmgSp);
    }

    @Override
    public int describeContents() {
        return 0;
    }

    public static final Creator<StateMask> CREATOR = new Creator<StateMask>() {
        @Override
        public StateMask createFromParcel(Parcel in) {
            return new StateMask(in);
        }

        @Override
        public StateMask[] newArray(int size) {
            return new StateMask[size];
        }
    };

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
                flags -= FLAG_INACTIVATE;
                this.flags = flags;
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

    public String inflict(final Interpreter actor, final boolean always, final boolean indefinite) {
        final HashMap<StateMask, Integer> trgStRes = actor.stRes;
        final int stateRes = this.sRes;
        Object o = null;
        if (always || stateRes < 0 || new Random().nextInt(10) > ((trgStRes == null
                ? 0 : ((o = trgStRes.get(this)) == null ? 0 : ((Integer) o).intValue())) + stateRes)) {
            HashMap<StateMask, Integer> trgStates = actor.stateDur;
            if (trgStates == null) {
                trgStates = new HashMap<>();
                actor.stateDur = trgStates;
            }
            final int stateDur = this.dur;
            final int crDur = ((o == null || (o = trgStates.get(this)) == null) ? 0 : ((Integer) o).intValue());
            if (crDur < stateDur || (crDur > -1 && stateDur < 0)) {
                trgStates.put(this, Integer.valueOf(indefinite ? -2 : stateDur));
            }
            actor.updateAttributes(false, this);
            actor.updateResistance(false, this.res, this.stRes);
            actor.updateStates(false, this.aStates);
            actor.updateSkills(false, this.aSkills);
            this.disableSkills(actor, false);
            final String sprite = this.sprite;
            if (sprite != null && sprite.length() > 0) {
                actor.setShapeShifted(true);
                actor.resetSprites();
            }
            return this.apply(actor, false);
        } else {
            return "";
        }
    }

    void disableSkills(final Interpreter actor, final boolean remove) {
        final Performance[] rSkills = this.rSkills;
        if (rSkills == null) {
            return;
        }
        Object q;
        HashMap<Performance, Integer> iSkills = actor.skillsQty;
        if (remove) {
            if (iSkills == null) {
                return;
            } else {
                for (int i = 0; i < rSkills.length; i++) {
                    Performance k = rSkills[i];
                    if (k.mQty > 0) {
                        iSkills.put(k, Integer.valueOf((q = iSkills.get(k)) == null ? 0 : -1 * ((Integer) q).intValue()));
                    } else {
                        iSkills.remove(k);
                    }
                }
            }
        } else {
            if (iSkills == null) {
                iSkills = new HashMap<>();
                actor.skillsQty = iSkills;
            }
            for (int i = 0; i < rSkills.length; i++) {
                Performance k = rSkills[i];
                iSkills.put(k, Integer.valueOf(k.mQty > 0 ? -1 * ((q = iSkills.get(k)) == null ? 0 : ((Integer) q).intValue()) : 0));
            }
        }
    }

    String apply(final Interpreter actor, final boolean consume) {
        final StringBuilder s = new StringBuilder();
        final HashMap<StateMask, Integer> sDur = actor.stateDur;
        if (sDur != null) {
            final Object dur = sDur.get(this);
            if (dur != null && actor._hp > 0) {
                final int d = ((Integer) dur).intValue();
                if (d == 0) {
                    this.disable(actor);
                    sDur.put(this, Integer.valueOf(-3));
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
                            s.append(String.format(Locale.US, this.name, actor.name));
                            s.append(RoleSheet.getDmgText(dmgHp, dmgMp, dmgSp));
                        }
                        if (d > 0) {
                            sDur.put(this, Integer.valueOf(d - 1));
                        }
                    } else {
                        final String spr = this.sprite;
                        if (spr != null && spr.length() > 0) {
                            actor.sprite = spr;
                        }
                        if (this.isInactivating()) {
                            if (d > 0 && actor.isActive()) {
                                sDur.put(this, Integer.valueOf(d - 1));
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
                            if (actorAuto > Interpreter.AUTO_CONFUSED) {
                                actor.automatic = actorAuto < Interpreter.AUTO_ALLY ? Interpreter.AUTO_CONFUSED : Interpreter.AUTO_ENEMY;
                            }
                        } else if (this.isAutomating()) {
                            final int actorAuto = actor.automatic;
                            if (actorAuto > Interpreter.AUTO_ENEMY && actorAuto < Interpreter.AUTO_ALLY)
                                actor.automatic = Interpreter.AUTO_ENRAGED;
                        }
                    }
                }
            }
        }
        return s.toString();
    }

    void disable(final Interpreter actor) {
        actor.updateAttributes(true, this);
        actor.updateResistance(true, this.res, this.stRes);
        actor.updateStates(true, this.aStates);
        actor.updateSkills(true, this.aSkills);
        this.disableSkills(actor, true);
        if (this.isRefelcting()) {
            actor.applyStates(false);
        }
    }

    public boolean remove(final Interpreter actor, final boolean delete, final boolean always) {
        HashMap<StateMask, Integer> sDur = actor.stateDur;
        Object d;
        if (sDur != null && (always || ((d = sDur.get(this)) == null ? -2 : ((Integer) d).intValue()) != -2)) {
            if (((d = sDur.get(this)) == null ? -3 : ((Integer) d).intValue()) > -3) {
                this.disable(actor);
            }
            if (delete) {
                sDur.remove(this);
            } else {
                sDur.put(this, Integer.valueOf(-3));
            }
            return true;
        } else {
            return false;
        }
    }

    StateMask(final int id, final String name, final String sprite, final boolean inactivate, final boolean automate,
              final boolean confuse, final boolean reflect, final boolean revive, final int dur, final int sRes, final int dmgHp,
              final int dmgMp, final int dmgSp, final int hp, final int mp, final int sp, final int atk, final int def, final int spi,
              final int wis, final int agi, final int mInit, final boolean range, final SparseIntArray res, final Performance[] aSkills,
              final Performance[] rSkills, final StateMask[] states, final HashMap<StateMask, Integer> stRes) {
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