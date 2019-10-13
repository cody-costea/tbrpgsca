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

import android.os.Parcelable;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.Locale;
import java.util.Random;

public final class ScenePlay {

    public static String performsTxt = "%s performs %s";
    public static String victoryTxt = "The party has won!%s";
    public static String fallenTxt = "The party has fallen!%s";
    public static String escapeTxt = "The party has escaped!";
    public static String failTxt = "The party attempted to escape, but failed.";

    public interface SceneRunnable {
        boolean run(final ScenePlay scene, final String ret);
    }

    protected SceneRunnable onStart, onStop, onNewTurn, beforeAct, afterAct;
    public SceneRunnable getOnBeginSceneRunnable() {
        return this.onStart;
    }
    public SceneRunnable getOnEndSceneRunnable() {
        return this.onStop;
    }
    public SceneRunnable getOnNewTurnRunnable() {
        return this.onNewTurn;
    }
    public SceneRunnable getBeforeActRunnable() {
        return this.beforeAct;
    }
    public SceneRunnable getAfterActRunnable() {
        return this.afterAct;
    }
    public ScenePlay setOnBeginSceneRunnable(final SceneRunnable run) {
        this.onStart = run;
        return this;
    }
    public ScenePlay setOnEndSceneRunnable(final SceneRunnable run) {
        this.onStop = run;
        return this;
    }
    public ScenePlay setOnNewTurnRunnable(final SceneRunnable run) {
        this.onNewTurn = run;
        return this;
    }
    public ScenePlay setBeforeActRunnable(final SceneRunnable run) {
        this.beforeAct = run;
        return this;
    }
    public ScenePlay setAfterActRunnable(final SceneRunnable run) {
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

    protected Interpreter[] _players;
    public Interpreter[] getPlayers() {
        return this._players;
    }

    protected boolean _useInit;
    public boolean getUseInit() {
        return this._useInit;
    }

    protected ArrayList<Performance> _crItems;
    public ArrayList<Performance> getCrItems() {
        return this._crItems;
    }

    public String setNext(String ret, final boolean endTurn) {
        if (this._status == 0) {
            int initInc;
            int minInit = 1;
            StringBuilder retBuilder = new StringBuilder(ret);
            final Interpreter[] _players = this._players;
            final boolean useInit = this._useInit;
            final Interpreter oldActor = _players[this._current];
            Interpreter crActor = oldActor;
            if (endTurn) {
                crActor.setActive(false);
                if (useInit) {
                    crActor.init = 0;
                }
                retBuilder.append(crActor.applyStates(true));
            }
            Interpreter nxActor;
            final int enIdx = this._enIdx;
            do {
                boolean noParty = true;
                boolean noEnemy = true;
                initInc = minInit;
                for (int i = 0; i < _players.length; i++) {
                    nxActor = _players[i];
                    if (nxActor._hp > 0) {
                        if (i < enIdx) {
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
                                mInit = _players.length;
                            }
                            if (nInit > mInit) {
                                nInit -= mInit;
                                if (initInc == 1) {
                                    minInit = -1;
                                }
                                if (nxActor != crActor) {
                                    final int cInit = crActor.init - (crActor.mInit < 1 ? _players.length : crActor.mInit);
                                    if (cInit < nInit || (cInit == nInit && nxActor.agi > crActor.agi)) {
                                        nxActor.setActive(true);
                                        nxActor.applyStates(false);
                                        if (nxActor.isActive()) {
                                            this._current = i;
                                            crActor = nxActor;
                                        } else {
                                            nxActor.init = 0;
                                            if (retBuilder.length() > 0) {
                                                retBuilder.append("\n");
                                            }
                                            retBuilder.append(nxActor.applyStates(true));
                                        }
                                    }
                                }
                            }
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
                                    if (retBuilder.length() > 0) {
                                        retBuilder.append("\n");
                                    }
                                    retBuilder.append(nxActor.applyStates(true));
                                }
                            }
                        }
                    }
                }
                if (noParty) {
                    this._status = -2;
                    ret = retBuilder.toString();
                    final SceneRunnable onStop = this.onStop;
                    return onStop == null || onStop.run(this, ret)
                            ? String.format(Locale.US, ScenePlay.failTxt, ret)
                            : this.setNext(ret + "\n", endTurn);
                } else if (noEnemy) {
                    this._status = 1;
                    ret = retBuilder.toString();
                    final SceneRunnable onStop = this.onStop;
                    return onStop == null || onStop.run(this, ret)
                            ? String.format(Locale.US, ScenePlay.victoryTxt, ret)
                            : this.setNext(ret + "\n", endTurn);
                } else if (minInit != 0 && !useInit) {
                    minInit = 0;
                }
            } while (initInc == 1 && minInit > -1);
            ret = retBuilder.toString();
            if (oldActor == crActor) {
                if (useInit) {
                    oldActor.setActive(true);
                }
                oldActor.applyStates(false);
                if (!oldActor.isActive()) {
                    return this.setNext(ret, true);
                }
            } else {
                if (crActor.automatic == Interpreter.AUTO_NONE) {
                    final LinkedHashMap<Performance, Integer> crItems = crActor._items;
                    if (crItems == null) {
                        this._crItems = null;
                    } else {
                        final int len = crItems.size();
                        final ArrayList<Performance> items = new ArrayList<Performance>(len);
                        for (Performance ability : crItems.keySet()) {
                            items.add(ability);
                        }
                        this._crItems = items;
                    }
                }
            }
            final HashMap<Performance, Integer> regSkills = crActor.skillsQtyRgTurn;
            if (regSkills != null) {
                final HashMap<Performance, Integer> skillsQty = crActor.skillsQty;
                for (Performance skill : regSkills.keySet()) {
                    int skillQtyInt = 0;
                    Object skillQtyObj;
                    if (skillsQty != null && ((skillQtyObj = skillsQty.get(skill)) == null
                            ? skill.mQty : (skillQtyInt = ((Integer)skillQtyObj).intValue())) < skill.mQty) {
                        if (regSkills.get(skill).intValue() == skill.rQty) {
                            skillsQty.put(skill, Integer.valueOf(skillQtyInt + 1));
                            regSkills.put(skill, Integer.valueOf(0));
                        } else {
                            regSkills.put(skill, Integer.valueOf(((skillQtyObj = regSkills.get(skill)) == null
                                    ? 0 : ((Integer)skillQtyObj).intValue()) + 1));
                        }
                    }
                }
            }
            final SceneRunnable onNewTurn = this.onNewTurn;
            if (endTurn && onNewTurn != null && onNewTurn.run(this, ret)
                    && crActor.automatic != 0) {
                return this.executeAI("");
            }
        }
        return ret;
    }

    public int getGuardian(final int target, final Performance skill) {
        final Interpreter[] players = this._players;
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
            final Interpreter iPlayer = players[i];
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
                final Interpreter iPlayer = players[i];
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
        final SceneRunnable beforeAct = this.beforeAct;
        if (beforeAct == null || beforeAct.run(this, ret)) {
            final StringBuilder retBuilder = new StringBuilder(ret);
            final int enIdx = this._enIdx, fTarget, lTarget;
            final Interpreter[] players = this._players;
            final int current = this._current;
            switch (skill.trg) {
                case Performance.TRG_ENEMY:
                    if (target < enIdx) {
                        this._fTarget = fTarget = 0;
                        this._lTarget = lTarget = enIdx - 1;
                    } else {
                        this._fTarget = fTarget = enIdx;
                        this._lTarget = lTarget = players.length - 1;
                    }
                    break;
                case Performance.TRG_ALL:
                    this._fTarget = fTarget = 0;
                    this._lTarget = lTarget = players.length - 1;
                    break;
                case Performance.TRG_PARTY:
                    if (this._current < enIdx) {
                        this._fTarget = fTarget = 0;
                        this._lTarget = lTarget = enIdx - 1;
                    } else {
                        this._fTarget = fTarget = enIdx;
                        this._lTarget = lTarget = players.length - 1;
                    }
                    break;
                case Performance.TRG_SELF:
                    this._fTarget = this._lTarget = fTarget = lTarget = current;
                    break;
                default:
                    this._fTarget = this._lTarget = fTarget = lTarget = this.getGuardian(target, skill);
            }
            boolean applyCosts = true;
            final Interpreter crActor = players[current];
            retBuilder.append(String.format(Locale.US, ScenePlay.performsTxt, crActor.name, skill.name));
            for (int i = fTarget; i <= lTarget; i++) {
                final Interpreter iPlayer = players[i];
                if ((skill.mHp < 0 && skill.isRestoring()) || iPlayer._hp > 0) {
                    retBuilder.append(skill.execute(crActor, iPlayer, applyCosts));
                    applyCosts = false;
                }
            }
            retBuilder.append(".");
            this._lastAbility = skill;
            ret = retBuilder.toString();
            final SceneRunnable afterAct = this.afterAct;
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
        final Interpreter[] players = this._players;
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
        final Interpreter crActor = players[this._current];
        final ArrayList<Performance> crSkills = crActor.getAvailableSkills();
        if (nRestore || nHeal) {
            for (int i = 0; i < crSkills.size(); i++) {
                final Performance s = crSkills.get(i);
                if ((s.isRestoring() || (nHeal && s.mHp < 0)) && s.canPerform(crActor)) {
                    skillIndex = i;
                    break;
                }
            }
        }
        final Performance ability = crSkills.get(this.getAIskill(skillIndex, nRestore));
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
        Interpreter trgActor = players[target];
        final boolean restore = ability.isRestoring();
        while (trgActor._hp < 1 && (atkSkill || !restore) && target < l) {
            trgActor = players[++target];
        }
        for (int i = target + 1; i < l; i++) {
            final Interpreter iPlayer = players[i];
            if (iPlayer._hp < trgActor._hp && (iPlayer._hp > 0 || restore)) {
                trgActor = iPlayer;
                target = i;
            }
        }
        return this.executeAbility(ability, target, ret);
    }

    public int getAIskill(final int defSkill, final boolean nRestore) {
        int ret = defSkill;
        final Interpreter crActor = this._players[this._current];
        final ArrayList<Performance> crSkills = crActor.getAvailableSkills();
        Performance s = crSkills.get(defSkill);
        for (int i = defSkill + 1; i < crSkills.size(); i++) {
            final Performance a = crSkills.get(i);
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
        return this.executeAbility((Performance)this._players[this._current].getAvailableSkills().get(index), target, ret);
    }

    public String useAbility(final Performance item, final int target, final String ret) {
        final LinkedHashMap<Performance, Integer> crItemsMap = this._players[this._current]._items;
        if (crItemsMap != null) {
            final Object itemQtyObj;
            final int itemQty = ((itemQtyObj = crItemsMap.get(item)) == null
                    ? 1 : ((Integer)itemQtyObj).intValue()) - 1;
            if (itemQty > 0) {
                crItemsMap.put(item, Integer.valueOf(itemQty));
            } else {
                crItemsMap.remove(item);
            }
        }
        return this.executeAbility(item, target, ret);
    }

    public String useItem(final int index, final int target, final String ret) {
        final ArrayList<Performance> crItems = this._crItems;
        if (crItems != null && index < crItems.size()) {
            return this.useAbility(crItems.get(index), target, ret);
        } else {
            return ret;
        }
    }

    public String escape() {
        this._lastAbility = null;
        final int surprise = this._surprise;
        if (surprise < 0) {
            return ScenePlay.failTxt;
        }
        final int enIdx = this._enIdx;
        final Interpreter[] players = this._players;
        final int pLength = players.length;
        int pAgiSum = 0;
        int eAgiSum = 0;
        if (surprise < 1) {
            for (int i = 0; i < enIdx; i++) {
                final Interpreter actor = players[i];
                if (actor._hp > 0) {
                    pAgiSum += actor.agi;
                }
            }
            pAgiSum /= enIdx;
            for (int i = enIdx; i < pLength; i++) {
                final Interpreter actor = players[i];
                if (actor._hp > 0) {
                    eAgiSum += actor.agi;
                }
            }
            eAgiSum /= (pLength - enIdx);
        }
        if (surprise > 0 || (new Random().nextInt(7) + pAgiSum > eAgiSum)) {
            this._status = -1;
            final SceneRunnable onStop = this.onStop;
            final String ret = ScenePlay.escapeTxt;
            if (onStop == null || onStop.run(this, ret)) {
                return ret;
            } else {
                this._status = 0;
            }
        }
        return ScenePlay.failTxt;
    }

    public ScenePlay(final Parcelable[] party, final Parcelable[] enemy, final int surprise,
                     final SceneRunnable onBeginScene, final SceneRunnable onNewTurn) {
        boolean useInit = false;
        final int pLength = party.length + enemy.length;
        final int enIdx = this._fTarget = this._lTarget = this._enIdx = party.length;
        final Interpreter[] players = this._players = new Interpreter[pLength];
        this._current = surprise < 0 ? enIdx : 0;
        for (int i = 0; i < pLength; i++) {
            Interpreter iPlayer = players[i];
            if (iPlayer == null) {
                iPlayer = players[i] = i < enIdx ? (Interpreter)party[i] : (Interpreter)enemy[i + enIdx];
            }
            if (!useInit && iPlayer.mInit != 0) {
                useInit = true;
            }
            int iInit = iPlayer.mInit;
            if (iInit < 1) {
                iInit = pLength;
            }
            if ((surprise < 0 && i < enIdx) || (surprise > 0 && i >= enIdx)) {
                iPlayer.init = 0;
                iPlayer.setActive(false);
            } else {
                iPlayer.init = iInit;
                iPlayer.setActive(iPlayer._hp > 0);
            }
            for (int j = 0; j < pLength; j++) {
                if (j == i) {
                    continue;
                } else {
                    Interpreter jPlayer = players[j];
                    if (jPlayer == null) {
                        jPlayer = players[j] = j < enIdx ? (Interpreter)party[j] : (Interpreter)enemy[j - enIdx];
                    }
                    int jInit = jPlayer.mInit;
                    if (jInit < 1) {
                        jInit = pLength;
                    }
                    if (iInit < jInit) {
                        jPlayer.init -= (jInit - iInit);
                    }
                }
            }
        }
        this.onNewTurn = onNewTurn;
        this.onStart = onBeginScene;
        this._surprise = surprise;
        this._useInit = useInit;
        final String ret = this.setNext("", false);
        if (onBeginScene == null || onBeginScene.run(this, ret)) {
            if (onNewTurn != null && onNewTurn.run(this, ret)
                    && players[this._current].automatic != 0) {
                this.executeAI("");
            }
        }
    }

}