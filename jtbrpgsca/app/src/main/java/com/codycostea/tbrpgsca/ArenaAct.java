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

import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.Locale;

import android.content.Context;
import android.graphics.Color;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.drawable.AnimationDrawable;
import android.os.Parcelable;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.Spinner;
import android.widget.TextView;

import org.mozilla.javascript.Scriptable;

public final class ArenaAct extends Activity {

	private static Parcelable[] cachedParty = null, cachedEnemy = null;
	private static String[] cachedScript = null;
	private boolean staticCache;

    public static void Stage(final Activity act, final int arenaDrawableId, final int songId,
                             final Interpreter[] party, final Interpreter[] enemy, final String[] scripts,
                             final int surprise, final boolean escapable, final boolean staticCache) {
        if (staticCache) {
            ArenaAct.cachedParty = party;
            ArenaAct.cachedEnemy = enemy;
            ArenaAct.cachedScript = scripts;
        }
        Intent btInt = new Intent(act, ArenaAct.class);
        btInt.putExtra("static", staticCache);
        btInt.putExtra("escapable", escapable);
        btInt.putExtra("surprise", surprise);
        btInt.putExtra("party", party);
        btInt.putExtra("enemy", enemy);
        btInt.putExtra("scripts", scripts);
        btInt.putExtra("arenaImg", arenaDrawableId);
        btInt.putExtra("song", songId);
        act.startActivityForResult(btInt, 1);
    }

	private ScenePlay scenePlay;

    private int koActors, partySide, otherSide;
    private boolean automatic, escapable;
    private ImageView[] imgActor;
    private Button autoBtn, runBtn, skillActBtn, itemUseBtn;
    private Spinner targetSpn, skillsSpn, itemsSpn;
    private TextView actionsTxt, infoTxt;

    private MediaPlayer songPlayer;

    private class ViewHolder {
        private TextView nameText;
        private boolean usable;

        private ViewHolder(final TextView nameText) {
            this.nameText = nameText;
        }
    }

    private class AbilityArrayAdapter extends ArrayAdapter<Performance> {

        private final int layoutRes;
        private final ArenaAct arenaAct;
        private final boolean asItems;

        private ArrayList<Performance> skills;
        public AbilityArrayAdapter setSkills(final ArrayList<Performance> skills) {
            this.skills = skills;
            this.notifyDataSetChanged();
            return this;
        }

        @Override
        public Performance getItem(final int position) {
            return this.skills.get(position);
        }

        @Override
        public int getCount() {
            return this.skills.size();
        }

        private View prepareView(final int position, View view, final ViewGroup parent) {
            final ViewHolder vHolder;
            final ArenaAct arenaAct = this.arenaAct;
            if (view == null || view.getTag() == null) {
                view = ((LayoutInflater)arenaAct.getSystemService(Context.LAYOUT_INFLATER_SERVICE))
                        .inflate(this.layoutRes, parent, false);
                final TextView txt = view.findViewById(android.R.id.text1);
                vHolder = new ViewHolder(txt);
                view.setTag(vHolder);
            } else {
                vHolder = (ViewHolder)view.getTag();
            }
            final ScenePlay scenePlay = arenaAct.scenePlay;
            final boolean usable = vHolder.usable = this.skills.get(position).canPerform(scenePlay._players[scenePlay._current]);
            vHolder.nameText.setTextColor(usable ? Color.BLACK : Color.GRAY);
            return view;
        }

        @Override
        public View getDropDownView(final int position, View view, final ViewGroup parent) {
            view = this.prepareView(position, view, parent);
            final ViewHolder vHolder = (ViewHolder)view.getTag();
            final Performance skill = this.skills.get(position);
            final ArenaAct context = this.arenaAct;
            final ScenePlay scenePlay = context.scenePlay;
            final Interpreter crActor = scenePlay._players[scenePlay._current];
            final HashMap<Performance, Integer> skillsQtyMap = crActor.skillsQty;
            final LinkedHashMap<Performance, Integer> itemsQtyMap = this.asItems ? crActor._items : null;
            final Integer skillsQtyInt = skillsQtyMap == null ? null : (Integer)skillsQtyMap.get(skill);
            final String skillQtyText = skillsQtyInt == null ? "∞" : String.valueOf(skillsQtyInt.intValue());
            final Integer itemsQtyInt = itemsQtyMap == null ? null : (Integer)itemsQtyMap.get(skill);
            final String itemQtyText = itemsQtyInt == null ? " " : String.format(Locale.US,
                    " X %s ", String.valueOf(itemsQtyInt.intValue()));
            final String trgText;
            switch (skill.trg) {
                case -1:
                    trgText = context.getString(R.string.self);
                    break;
                case 0:
                    trgText = context.getString(R.string.one);
                    break;
                default:
                    trgText = context.getString(R.string.all);
            }
            vHolder.nameText.setText(String.format(Locale.US, "%s%s%s", skill.name, itemQtyText,
                    String.format(context.getString(R.string.skill_info), skill.lvRq, skill.hpC, skill.mpC,
                            skill.spC, skillQtyText, trgText, (skill.hasRange() ? context.getString(R.string.yes) : context.getString(R.string.no)))));
            return view;
        }

        @Override
        public View getView(final int position, View view, final ViewGroup parent) {
            view = this.prepareView(position, view, parent);
            final ViewHolder vHolder = (ViewHolder)view.getTag();
            final Performance skill = this.skills.get(position);
            final ScenePlay scenePlay = this.arenaAct.scenePlay;
            final Interpreter crActor = scenePlay._players[scenePlay._current];
            final LinkedHashMap<Performance, Integer> itemsQtyMap = this.asItems ? crActor._items : null;
            final Integer itemsQtyInt = itemsQtyMap == null ? null : (Integer)itemsQtyMap.get(skill);
            vHolder.nameText.setText(itemsQtyInt == null ? skill.name : String.format(Locale.US,
                    "%s X %s ", skill.name, String.valueOf(itemsQtyInt.intValue())));
            return view;
        }

        public AbilityArrayAdapter(final ArenaAct context, final int layoutRes,
                                   final ArrayList<Performance> skills, final boolean asItems) {
            super(context, layoutRes);
            this.layoutRes = layoutRes;
            this.arenaAct = context;
            this.asItems = asItems;
            this.skills = skills;
        }

    }

    private class ActorArrayAdapter extends ArrayAdapter<Interpreter> {

        private final int layoutRes;
        private final ArenaAct arenaAct;
        private Interpreter[] actors;

        @Override
        public Interpreter getItem(final int position) {
            return this.actors[position];
        }

        @Override
        public int getCount() {
            return this.actors.length;
        }

        private View prepareView(View view, final ViewGroup parent) {
            if (view == null || view.getTag() == null) {
                view = ((LayoutInflater)this.arenaAct.getSystemService(Context.LAYOUT_INFLATER_SERVICE))
                        .inflate(this.layoutRes, parent, false);
                final TextView txt = view.findViewById(android.R.id.text1);
                view.setTag(new ViewHolder(txt));
            }
            return view;
        }

        @Override
        public View getDropDownView(final int position, View view, final ViewGroup parent) {
            view = this.prepareView(view, parent);
            final ViewHolder vHolder = (ViewHolder)view.getTag();
            final Interpreter actor = this.actors[position];
            vHolder.nameText.setText(String.format(Locale.US, "%s (%s: %s)",actor.name,
                    RoleSheet.hpText, (position < this.arenaAct.scenePlay._enIdx) ? String.format(Locale.US,
                            "%d/%d, %s: %d/%d, %s: %d/%d", actor._hp, actor.mHp, RoleSheet.mpText,
                            actor._mp, actor.mMp, RoleSheet.spText, actor._sp, actor.mSp)
                    : String.format(Locale.US, "%.0f%%", (((float)actor._hp) / ((float)actor.mHp) * 100.0f))));
            return view;
        }

        @Override
        public View getView(final int position, View view, final ViewGroup parent) {
            view = this.prepareView(view, parent);
            final ViewHolder vHolder = (ViewHolder)view.getTag();
            vHolder.nameText.setText(this.actors[position].name);
            return view;
        }

        public ActorArrayAdapter(final ArenaAct context, final int layoutRes, final Interpreter[] actors) {
            super(context, layoutRes);
            this.layoutRes = layoutRes;
            this.arenaAct = context;
            this.actors = actors;
        }

    }

    private AbilityArrayAdapter skillsAdapter, itemsAdapter;

    private final View.OnClickListener cAction = new OnClickListener() {
        @Override
        public void onClick(final View v) {
            switch (v.getId()) {
                case R.id.ActBt: {
                    ArenaAct.this.enableControls(false);
                    ArenaAct.this.actionsTxt.append(ArenaAct.this.scenePlay.performSkill(
                            ArenaAct.this.skillsSpn.getSelectedItemPosition(),
                            ArenaAct.this.targetSpn.getSelectedItemPosition(), "\n"));
                    ArenaAct.this.playSpr();
                    break;
                }
                case R.id.AutoBt: {
                    final ScenePlay scenePlay = ArenaAct.this.scenePlay;
                    final boolean automatic = !ArenaAct.this.automatic;
                    ArenaAct.this.autoBtn.setText(ArenaAct.this.getString(automatic ? R.string.bt_manual : R.string.bt_auto));
                    ArenaAct.this.automatic = automatic;
                    if (automatic && ArenaAct.this.skillActBtn.isEnabled()) {
                        ArenaAct.this.enableControls(false);
                        if (scenePlay._players[scenePlay._current].automatic == 0) {
                            ArenaAct.this.afterAct();
                        }
                    }
                    break;
                }
                case R.id.UseBt: {
                    final AbilityArrayAdapter itemsAdapter = ArenaAct.this.itemsAdapter;
                    if (itemsAdapter != null) {
                        ArenaAct.this.enableControls(false);
                        ArenaAct.this.actionsTxt.append(ArenaAct.this.scenePlay.useItem(
                                ArenaAct.this.itemsSpn.getSelectedItemPosition(),
                                ArenaAct.this.targetSpn.getSelectedItemPosition(), "\n"));
                        itemsAdapter.notifyDataSetChanged();
                        ArenaAct.this.playSpr();
                    }
                    break;
                }
                case R.id.RunBt: {
                    final ScenePlay scenePlay = ArenaAct.this.scenePlay;
                    final TextView actionsTxt = ArenaAct.this.actionsTxt;
                    final String escText = scenePlay.escape();
                    actionsTxt.append(scenePlay.setNext("\n", true));
                    actionsTxt.append(escText);
                    if (scenePlay._status == -1) {
                        ArenaAct.this.endingMsg(ArenaAct.this.getString(R.string.escape), ScenePlay.escapeTxt);
                    } else {
                        ArenaAct.this.enableControls(false);
                        ArenaAct.this.afterAct();
                    }
                    break;
                }
            }
        }
    };

    private void setImgTargetClickListener(final ImageView img, final int targetPos) {
        img.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(final View v) {
                final Spinner targetSpn = ArenaAct.this.targetSpn;
                if (targetPos == targetSpn.getSelectedItemPosition()) {
                    final Button skillActBtn = ArenaAct.this.skillActBtn;
                    final ScenePlay scenePlay = ArenaAct.this.scenePlay;
                    final Interpreter crActor = scenePlay._players[scenePlay._current];
                    if (crActor.automatic == 0 && skillActBtn.isEnabled()) {
                        ArenaAct.this.cAction.onClick(skillActBtn);
                    }
                } else {
                    targetSpn.setSelection(targetPos);
                }
            }
        });
    }

    private void endingMsg(final String t, final String s) {
        new AlertDialog.Builder(this)
                .setCancelable(false)
                .setMessage(s)
                .setTitle(t).setPositiveButton(this.getResources().getString(R.string.exit),
                new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(final DialogInterface dialog, final int which) {
                        final Intent outcome = new Intent();
                        final ScenePlay scenePlay = ArenaAct.this.scenePlay;
                        final int enemyIndex = scenePlay._enIdx;
                        outcome.putExtra("outcome", scenePlay._status);
                        if (!ArenaAct.this.staticCache) {
                            final Parcelable[] party = new Parcelable[enemyIndex];
                            final Interpreter[] players = scenePlay._players;
                            for (int i = 0; i < enemyIndex; i++) {
                                party[i] = players[i];
                            }
                            outcome.putExtra("party", party);
                        }
                        ArenaAct.this.setResult(Activity.RESULT_OK, outcome);
                        ArenaAct.this.finish();
                    }
                }).show();
    }

    private void setCrSkills() {
        final Spinner skillsSpn = this.skillsSpn;
        final Spinner targetSpn = this.targetSpn;
        final Button skillActBtn = this.skillActBtn;
        final Interpreter crActor;
        {
            final ScenePlay scenePlay = this.scenePlay;
            crActor = scenePlay._players[scenePlay._current];
        }
        AbilityArrayAdapter skillsAdapter = this.skillsAdapter;
        if (skillsAdapter == null) {
            this.skillsAdapter = skillsAdapter = new AbilityArrayAdapter(this,
                    android.R.layout.simple_spinner_item, crActor.getAvailableSkills(), false);
            skillsSpn.setAdapter(skillsAdapter);
            skillsSpn.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
                @Override
                public void onNothingSelected(final AdapterView<?> parent) {
                    skillActBtn.setEnabled(false);
                }

                @Override
                public void onItemSelected(final AdapterView<?> parent, final View view, final int position, final long id) {
                    final Interpreter crActor;
                    {
                        final ScenePlay scenePlay = ArenaAct.this.scenePlay;
                        crActor = scenePlay._players[scenePlay._current];
                    }
                    final Performance performance = crActor.getAvailableSkills().get(position);
                    skillActBtn.setEnabled(((view != null && ((ViewHolder) view.getTag()).usable)
                            || (view == null && performance.canPerform(crActor)))
                            && ArenaAct.this.canTarget(targetSpn.getSelectedItemPosition(), performance));
                }
            });
            skillActBtn.setOnClickListener(this.cAction);
        } else {
            skillsAdapter.setSkills(crActor.getAvailableSkills());
        }
    }

    private void setCrItems() {
        final Spinner itemsSpn = this.itemsSpn;
        final ScenePlay scenePlay = this.scenePlay;
        final ArrayList<Performance> items = scenePlay._crItems;
        if (items == null || items.size() == 0) {
            if (itemsSpn.isEnabled()) {
                itemsSpn.setSelection(Spinner.INVALID_POSITION);
                itemsSpn.setEnabled(false);
            }
        } else {
            final Spinner targetSpn = this.targetSpn;
            final Button itemUseBtn = this.itemUseBtn;
            AbilityArrayAdapter itemsAdapter = this.itemsAdapter;
            if (itemsAdapter == null) {
                this.itemsAdapter = itemsAdapter = new AbilityArrayAdapter(this,
                        android.R.layout.simple_spinner_item, items, true);
                itemsSpn.setAdapter(itemsAdapter);
                itemsSpn.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
                    @Override
                    public void onNothingSelected(final AdapterView<?> parent) {
                        itemUseBtn.setEnabled(false);
                    }

                    @Override
                    public void onItemSelected(final AdapterView<?> parent, final View view, final int position, final long id) {
                        final Interpreter crActor;
                        {
                            final ScenePlay scenePlay = ArenaAct.this.scenePlay;
                            crActor = scenePlay._players[scenePlay._current];
                        }
                        final Performance performance = (Performance)itemsSpn.getSelectedItem();
                        itemUseBtn.setEnabled(performance != null && ((view != null
                                && ((ViewHolder) view.getTag()).usable) || (view == null && performance.canPerform(crActor)))
                                && ArenaAct.this.canTarget(targetSpn.getSelectedItemPosition(), performance));
                    }
                });
                itemUseBtn.setOnClickListener(this.cAction);
            } else if (itemsAdapter.skills != items) {
                itemsAdapter.setSkills(items);
            }
            if (!itemsSpn.isEnabled()) {
                itemsSpn.setSelection(0);
                itemsSpn.setEnabled(true);
            }
            final int targetPos = targetSpn.getSelectedItemPosition();
            if (targetPos > -1) {
                final Performance item = (Performance)itemsSpn.getSelectedItem();
                itemUseBtn.setEnabled(item.canPerform(scenePlay._players[scenePlay._current]) && this.canTarget(targetPos, item));
            }
        }
    }

    private void playSpr() {
        final ScenePlay scenePlay = this.scenePlay;
        final Interpreter[] players = scenePlay._players;
        final ImageView[] imgActor = this.imgActor;
        final int current = scenePlay._current;
        final Interpreter crActor = players[current];
        final Performance lastAbility = scenePlay._lastAbility;
        final int sprType = lastAbility == null || (lastAbility.trg > -1
                && (lastAbility.dmgType & Performance.DMG_TYPE_ATK) == Performance.DMG_TYPE_ATK) ? 5 : 6;
        final int usrSide = (current < scenePlay._enIdx) ? this.partySide : this.otherSide;
        final AnimationDrawable actAnim = crActor.getBtSprite(this, usrSide, sprType);
        final int iDur = crActor.spritesDur[usrSide][sprType] - 174;
        if (actAnim != null) {
            actAnim.stop();
            imgActor[current].setBackgroundDrawable(actAnim);
            actAnim.start();
        }
        imgActor[current].postDelayed(new Runnable() {
            @Override
            public void run() {
                int dur = iDur;
                Interpreter htActor;
                for (int trg = scenePlay._fTarget; trg <= scenePlay._lTarget; trg++) {
                    if (trg != current && (lastAbility == null || !(lastAbility.dmgType == 2
                            && players[trg].isReflecting()))) {
                        htActor = (players[trg]);
                        final int trgAnim;
                        final int koActors = ArenaAct.this.koActors;
                        final int koBit = 1 << trg;
                        if (htActor._hp > 0) {
                            if ((koActors & koBit) == koBit) {
                                trgAnim = 4;
                                ArenaAct.this.koActors = koActors - koBit;
                            } else {
                                trgAnim = 2;
                            }
                        } else {
                            if ((koActors & koBit) == koBit) continue;
                            trgAnim = 3;
                            ArenaAct.this.koActors = koActors + koBit;
                        }
                        if (lastAbility != null) {
                            final AnimationDrawable abilitySpr = lastAbility.getSpriteAnimation(ArenaAct.this);
                            if (abilitySpr != null) {
                                if (lastAbility.spriteDur > dur) {
                                    dur = lastAbility.spriteDur;
                                }
                                abilitySpr.stop();
                                imgActor[trg].setImageDrawable(abilitySpr);
                                abilitySpr.start();
                            }
                            final int soundDur = lastAbility.playSound(ArenaAct.this);
                            if (soundDur > dur) {
                                dur = soundDur;
                            }
                        }
                        final int trgSide = (trg < scenePlay._enIdx) ? ArenaAct.this.partySide : ArenaAct.this.otherSide;
                        final AnimationDrawable hitAnim = htActor.getBtSprite(ArenaAct.this, trgSide, trgAnim);
                        if (hitAnim != null) {
                            hitAnim.stop();
                            if (htActor.spritesDur[trgSide][trgAnim] > dur) {
                                dur = htActor.spritesDur[trgSide][trgAnim];
                            }
                            imgActor[trg].setBackgroundDrawable(hitAnim);
                            hitAnim.start();
                        }
                    }
                }
                imgActor[current].postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        if (crActor._hp < 1) {
                            ArenaAct.this.koActors += (1 << current);
                            final AnimationDrawable fallAnim = crActor.getBtSprite(ArenaAct.this, usrSide, 3);
                            if (fallAnim != null) {
                                fallAnim.stop();
                                imgActor[current].setBackgroundDrawable(fallAnim);
                                fallAnim.start();
                            }
                        }
                        ArenaAct.this.actionsTxt.append(scenePlay.setNext("", true));
                        ArenaAct.this.afterAct();
                    }
                }, dur);
            }
        }, 174);
    }

    private void afterAct() {
        final ScenePlay scenePlay = this.scenePlay;
        switch (scenePlay._status) {
            case 0: {
                //this.autoBtn.setEnabled(true);
                final Interpreter actor = scenePlay._players[scenePlay._current];
                final TextView infoTxt = this.infoTxt;
                if (this.automatic || actor.automatic != 0) {
                    if (infoTxt.getText().length() > 0) {
                        infoTxt.setText("");
                    }
                    this.actionsTxt.append(scenePlay.executeAI("\n"));
                    this.playSpr();
                } else {
                    this.setCrSkills();
                    this.setCrItems();
                    infoTxt.setText(String.format(this.getString(R.string.cr_actor_info), actor.name, actor._lv, actor._xp, actor._maxp));
                    this.enableControls(true);
                    this.setCrAutoSkill();
                }
                break;
            }
            case 1: {
                this.endingMsg(this.getString(R.string.victory), String.format(Locale.US, ScenePlay.victoryTxt, ""));
                break;
            }
            case -2: {
                this.endingMsg(this.getString(R.string.defeat), String.format(Locale.US, ScenePlay.fallenTxt, ""));
                break;
            }
        }
    }

    private boolean canTarget(final int target, final Performance ability) {
        final ScenePlay scenePlay = this.scenePlay;
        return scenePlay.getGuardian(target, ability) == target
                && (scenePlay._players[target]._hp > 0 || ability.isRestoring());
    }

    private void enableControls(final boolean enable) {
        if (!enable) {
            this.skillActBtn.setEnabled(false);
            this.itemUseBtn.setEnabled(false);
            this.itemsSpn.setEnabled(false);
        }
        this.skillsSpn.setEnabled(enable);
        if (this.escapable) {
            this.runBtn.setEnabled(enable);
        }
    }

    private void setCrAutoSkill() {
        final ScenePlay scenePlay = this.scenePlay;
        final Spinner skillsSpn = this.skillsSpn;
        final int targetPos = this.targetSpn.getSelectedItemPosition();
        final Interpreter crActor = scenePlay._players[scenePlay._current];
        final boolean onPartySide = targetPos < scenePlay._enIdx;
        final int autoSkill = scenePlay.getAIskill(onPartySide ? 1 : 0,
                onPartySide && scenePlay._players[targetPos]._hp < 1);
        if (skillsSpn.getSelectedItemPosition() == autoSkill) {
            final Performance autoAbility = crActor.getAvailableSkills().get(autoSkill);
            this.skillActBtn.setEnabled(autoAbility.canPerform(crActor)
                    && this.canTarget(targetPos, autoAbility));
        } else {
            skillsSpn.setSelection(autoSkill);
        }
    }

    @Override
    public void onCreate(final Bundle savedInstance) {
        super.onCreate(savedInstance);
        this.setContentView(R.layout.activity_arena);
        final Bundle extra = this.getIntent().getExtras();
        final Parcelable[] party;
        final Parcelable[] enemy;
        final String[] jScripts;
        final boolean escapable;
        final int surprised;
        if (extra != null) {
            surprised = extra.getInt("surprise", 0);
            escapable = extra.getBoolean("escapable", true);
            final int songResId = extra.getInt("song", 0);
            if (songResId > 0) {
                final MediaPlayer songPlayer = MediaPlayer.create(this, songResId);
                this.songPlayer = songPlayer;
                songPlayer.start();
            }
            final int arenaResId = extra.getInt("arenaImg", 0);
            if (arenaResId > 0) {
                this.findViewById(R.id.ImgArena).setBackgroundResource(arenaResId);
            }
            if (ArenaAct.cachedParty != null && ArenaAct.cachedEnemy != null
                    && (this.staticCache = extra.getBoolean("static", false))) {
                jScripts = ArenaAct.cachedScript;
                party = ArenaAct.cachedParty;
                enemy = ArenaAct.cachedEnemy;
            } else {
                party = extra.getParcelableArray("party");
                enemy = extra.getParcelableArray("enemy");
                jScripts = extra.getStringArray("scripts");
            }
        } else {
            surprised = 0;
            escapable = true;
            jScripts = null;
            party = null; //TODO: add demo party
            enemy = null; //TODO: add demo enemy
        }
        int partySide, otherSide;
        if (surprised < 0) {
            partySide = 1;
            otherSide = 0;
        } else {
            partySide = 0;
            otherSide = 1;
        }
        this.partySide = partySide;
        this.otherSide = otherSide;
        this.escapable = escapable;
        final org.mozilla.javascript.Context jsContext;
        final Scriptable jsScope;
        if (jScripts == null) {
            jsContext = null;
            jsScope = null;
        } else {
            jsContext = org.mozilla.javascript.Context.enter();
            jsContext.setOptimizationLevel(-1);
            jsScope = jsContext.initSafeStandardObjects();
        }
        final ScenePlay scenePlay = new ScenePlay(party, enemy, surprised,
                jScripts == null ? null : new ScenePlay.SceneRunnable() {
                @Override
                public boolean run(final ScenePlay scene, final String ret) {
                    final String jScript;
                    if (jScripts.length > 0 && (jScript = jScripts[0]) != null) {
                        try {
                            jsScope.put("Scene", jsScope, org.mozilla.javascript.Context.javaToJS(scene, jsScope));
                            jsScope.put("Message", jsScope, org.mozilla.javascript.Context.javaToJS(ret, jsScope));
                            jsScope.put("Activity", jsScope, org.mozilla.javascript.Context.javaToJS(ArenaAct.this, jsScope));
                            jsContext.evaluateString(jsScope, jScript, "OnStart", 1, null);
                        } catch (final Exception e) {
                            Log.e("Rhino", e.getMessage());
                        }
                    }
                    return true;
                }
            }, jScripts == null ? null : new ScenePlay.SceneRunnable() {
            @Override
            public boolean run(final ScenePlay scene, final String ret) {
                final String jScript;
                if (jScripts.length > 1 && (jScript = jScripts[1]) != null) {
                    try {
                        jsScope.put("Scene", jsScope, org.mozilla.javascript.Context.javaToJS(scene, jsScope));
                        jsScope.put("Message", jsScope, org.mozilla.javascript.Context.javaToJS(ret, jsScope));
                        jsScope.put("Activity", jsScope, org.mozilla.javascript.Context.javaToJS(ArenaAct.this, jsScope));
                        jsContext.evaluateString(jsScope, jScript, "OnBeginTurn", 1, null);
                    } catch (final Exception e) {
                        Log.e("Rhino", e.getMessage());
                    }
                }
                return false;
            }
        });
        if (jScripts != null) {
            scenePlay.setBeforeActRunnable(new ScenePlay.SceneRunnable() {
                @Override
                public boolean run(final ScenePlay scene, final String ret) {
                    final String jScript;
                    if (jScripts.length > 2 && (jScript = jScripts[2]) != null) {
                        try {
                            jsScope.put("Scene", jsScope, org.mozilla.javascript.Context.javaToJS(scene, jsScope));
                            jsScope.put("Message", jsScope, org.mozilla.javascript.Context.javaToJS(ret, jsScope));
                            jsScope.put("Activity", jsScope, org.mozilla.javascript.Context.javaToJS(ArenaAct.this, jsScope));
                            jsContext.evaluateString(jsScope, jScript, "BeforeAct", 1, null);
                        } catch (final Exception e) {
                            Log.e("Rhino", e.getMessage());
                        }
                    }
                    return true;
                }
            });
            scenePlay.setAfterActRunnable(new ScenePlay.SceneRunnable() {
                @Override
                public boolean run(final ScenePlay scene, final String ret) {
                    final String jScript;
                    if (jScripts.length > 3 && (jScript = jScripts[3]) != null) {
                        try {
                            jsScope.put("Scene", jsScope, org.mozilla.javascript.Context.javaToJS(scene, jsScope));
                            jsScope.put("Message", jsScope, org.mozilla.javascript.Context.javaToJS(ret, jsScope));
                            jsScope.put("Activity", jsScope, org.mozilla.javascript.Context.javaToJS(ArenaAct.this, jsScope));
                            jsContext.evaluateString(jsScope, jScript, "AfterAct", 1, null);
                        } catch (final Exception e) {
                            Log.e("Rhino", e.getMessage());
                        }
                    }
                    return true;
                }
            });
            scenePlay.setOnEndSceneRunnable(new ScenePlay.SceneRunnable() {
                @Override
                public boolean run(final ScenePlay scene, final String ret) {
                    final String jScript;
                    if (jScripts.length > 4 && (jScript = jScripts[4]) != null) {
                        try {
                            jsScope.put("Scene", jsScope, org.mozilla.javascript.Context.javaToJS(scene, jsScope));
                            jsScope.put("Message", jsScope, org.mozilla.javascript.Context.javaToJS(ret, jsScope));
                            jsScope.put("Activity", jsScope, org.mozilla.javascript.Context.javaToJS(ArenaAct.this, jsScope));
                            jsContext.evaluateString(jsScope, jScript, "OnStop", 1, null);
                        } catch (final Exception e) {
                            Log.e("Rhino", e.getMessage());
                        }
                    }
                    return true;
                }
            });
        }
        final Interpreter[] players = scenePlay._players;
        final int enIdx = scenePlay._enIdx;
        this.scenePlay = scenePlay;
        final Button runBtn = this.findViewById(R.id.RunBt);
        final Button autoBtn = this.findViewById(R.id.AutoBt);
        final Button itemUseBtn = this.findViewById(R.id.UseBt);
        final Spinner itemsSpn = this.findViewById(R.id.ItemBox);
        final Button skillActBtn = this.findViewById(R.id.ActBt);
        final Spinner skillsSpn = this.findViewById(R.id.SkillBox);
        final Spinner targetSpn = this.findViewById(R.id.TargetBox);
        final TextView actionsTxt = this.findViewById(R.id.ItemCost);
        actionsTxt.setMovementMethod(new ScrollingMovementMethod());
        this.infoTxt = this.findViewById(R.id.SkillCost);
        this.skillActBtn = skillActBtn;
        runBtn.setEnabled(escapable);
        this.itemUseBtn = itemUseBtn;
        this.actionsTxt = actionsTxt;
        this.targetSpn = targetSpn;
        this.skillsSpn = skillsSpn;
        this.itemsSpn = itemsSpn;
        this.autoBtn = autoBtn;
        this.runBtn = runBtn;

        int koActors = 0;
        for (int i = 0; i < players.length; i++) {
            if (players[i]._hp < 1) {
                koActors += 1 << i;
            }
        }
        this.koActors = koActors;

        int pos = 0;
        final int partySize = party.length;
        final int enemySize = enemy.length;
        final ArrayList<ImageView> imgViews = new ArrayList<ImageView>(partySize + enemySize);
        ImageView imgView;
        if (surprised < 0) {
            if (partySize > 0) {
                imgView = this.findViewById(R.id.ImgEnemy1);
                this.setImgTargetClickListener(imgView, pos++);
                imgViews.add(imgView);
            }
            if (partySize > 1) {
                imgView = this.findViewById(R.id.ImgEnemy2);
                this.setImgTargetClickListener(imgView, pos++);
                imgViews.add(imgView);
            }
            if (partySize > 2) {
                imgView = this.findViewById(R.id.ImgEnemy3);
                this.setImgTargetClickListener(imgView, pos++);
                imgViews.add(imgView);
            }
            if (partySize > 3) {
                imgView = this.findViewById(R.id.ImgEnemy4);
                this.setImgTargetClickListener(imgView, pos++);
                imgViews.add(imgView);
            }
            if (enemySize > 0) {
                imgView = this.findViewById(R.id.ImgPlayer1);
                this.setImgTargetClickListener(imgView, pos++);
                imgViews.add(imgView);
            }
            if (enemySize > 1) {
                imgView = this.findViewById(R.id.ImgPlayer2);
                this.setImgTargetClickListener(imgView, pos++);
                imgViews.add(imgView);
            }
            if (enemySize > 2) {
                imgView = this.findViewById(R.id.ImgPlayer3);
                this.setImgTargetClickListener(imgView, pos++);
                imgViews.add(imgView);
            }
            if (enemySize > 3) {
                imgView = this.findViewById(R.id.ImgPlayer4);
                this.setImgTargetClickListener(imgView, pos);
                imgViews.add(imgView);
            }
        } else {
            if (partySize > 0) {
                imgView = this.findViewById(R.id.ImgPlayer1);
                this.setImgTargetClickListener(imgView, pos++);
                imgViews.add(imgView);
            }
            if (partySize > 1) {
                imgView = this.findViewById(R.id.ImgPlayer2);
                this.setImgTargetClickListener(imgView, pos++);
                imgViews.add(imgView);
            }
            if (partySize > 2) {
                imgView = this.findViewById(R.id.ImgPlayer3);
                this.setImgTargetClickListener(imgView, pos++);
                imgViews.add(imgView);
            }
            if (partySize > 3) {
                imgView = this.findViewById(R.id.ImgPlayer4);
                this.setImgTargetClickListener(imgView, pos++);
                imgViews.add(imgView);
            }
            if (enemySize > 0) {
                imgView = this.findViewById(R.id.ImgEnemy1);
                this.setImgTargetClickListener(imgView, pos++);
                imgViews.add(imgView);
            }
            if (enemySize > 1) {
                imgView = this.findViewById(R.id.ImgEnemy2);
                this.setImgTargetClickListener(imgView, pos++);
                imgViews.add(imgView);
            }
            if (enemySize > 2) {
                imgView = this.findViewById(R.id.ImgEnemy3);
                this.setImgTargetClickListener(imgView, pos++);
                imgViews.add(imgView);
            }
            if (enemySize > 3) {
                imgView = this.findViewById(R.id.ImgEnemy4);
                this.setImgTargetClickListener(imgView, pos);
                imgViews.add(imgView);
            }
        }

        final ImageView[] imgActor = imgViews.toArray(new ImageView[0]);
        this.imgActor = imgActor;
        for (int i = 0; i < enIdx; i++) {
            imgActor[i].setBackgroundDrawable(players[i].getBtSprite(this, partySide,
            ((koActors & (1 << i)) == (1 << i)) ? 1 : 0));
        }
        for (int i = enIdx; i < players.length; i++) {
            players[i].automatic = 2;
            imgActor[i].setBackgroundDrawable(players[i].getBtSprite(this, otherSide,
            ((koActors & (1 << i)) == (1 << i)) ? 1 : 0));
        }

        final ActorArrayAdapter playersAdapter = new ActorArrayAdapter(this,
                android.R.layout.simple_spinner_item, players);
        targetSpn.setAdapter(playersAdapter);
        targetSpn.setSelection(enIdx);
        targetSpn.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onNothingSelected(final AdapterView<?> parent) {
                skillActBtn.setEnabled(false);
                itemUseBtn.setEnabled(false);
            }

            @Override
            public void onItemSelected(final AdapterView<?> parent, final View view, final int position, final long id) {
                final Interpreter crActor;
                {
                    final ScenePlay scenePlay = ArenaAct.this.scenePlay;
                    crActor = scenePlay._players[scenePlay._current];
                }
                if (crActor.automatic == 0) {
                    if (itemsSpn.isEnabled()) {
                        final View tagView = itemsSpn.getSelectedView();
                        itemUseBtn.setEnabled(tagView != null && ((ViewHolder)tagView.getTag()).usable
                                && ArenaAct.this.canTarget(position, (Performance)itemsSpn.getSelectedItem()));
                    }
                    if (skillsSpn.isEnabled()) {
                        ArenaAct.this.setCrAutoSkill();
                    }
                }
            }
        });

        final View.OnClickListener cAction = this.cAction;
        runBtn.setOnClickListener(cAction);
        autoBtn.setOnClickListener(cAction);
    }

    @Override
    public boolean onKeyDown(final int keyCode, final KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_BACK) {
            this.scenePlay._status = -2;
            this.afterAct();
            return true;
        } else {
            return super.onKeyDown(keyCode, event);
        }
    }

    @Override
    protected void onDestroy() {
        ArenaAct.cachedParty = null;
        ArenaAct.cachedEnemy = null;
        ArenaAct.cachedScript = null;
        final MediaPlayer songPlayer = this.songPlayer;
        if (songPlayer != null) {
            songPlayer.release();
        }
        super.onDestroy();
    }

    @Override
    public void onStart() {
        super.onStart();
        final ScenePlay scenePlay = this.scenePlay;
        if (scenePlay._players[scenePlay._current].automatic != 0) {
            this.enableControls(false);
        }
        this.afterAct();
    }

}