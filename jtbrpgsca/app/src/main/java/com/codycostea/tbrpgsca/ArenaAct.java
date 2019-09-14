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
import java.util.Hashtable;
import java.util.Vector;

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

public final class ArenaAct extends Activity {

	private static Parcelable[] cachedParty = null, cachedEnemy = null;
	private static Performance[] cachedSkill = null;
	private static ArrayList<Performance> cachedItem = null;
	private static String[] cachedScript = null;

	private ScenePlay scenePlay;

    private int koActors, partySide, otherSide;
    private boolean automatic, escapable;

    private ImageView[] imgActor;
    private Button autoBtn, runBtn, skillActBtn, itemUseBtn;
    private Spinner targetSpn, skillsSpn, itemsSpn;
    private TextView actionsTxt, infoTxt;

    private String[] jScripts;

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

        private Vector<Performance> skills;
        public AbilityArrayAdapter setSkills(final Vector<Performance> skills) {
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
            vHolder.nameText.setTextColor(usable ? Color.WHITE : Color.GRAY);
            return view;
        }


        @Override
        public View getDropDownView(final int position, View view, final ViewGroup parent) {
            view = this.prepareView(position, view, parent);
            final ViewHolder vHolder = (ViewHolder)view.getTag();
            final Performance skill = this.skills.get(position);
            final ArenaAct context = this.arenaAct;
            final ScenePlay scenePlay = context.scenePlay;
            final Actor crActor = scenePlay._players[scenePlay._current];
            final Hashtable skillsQtyMap = crActor.skillsQty;
            final Hashtable itemsQtyMap = this.asItems ? crActor._items : null;
            final Integer skillsQtyInt = skillsQtyMap == null ? null : (Integer)skillsQtyMap.get(skill);
            final String skillQtyText = skillsQtyInt == null ? "∞" : String.valueOf(skillsQtyInt.intValue());
            final Integer itemsQtyInt = itemsQtyMap == null ? null : (Integer)itemsQtyMap.get(skill);
            final String itemQtyText = itemsQtyInt == null ? " " : " x " + String.valueOf(itemsQtyInt.intValue()) + " ";
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
            vHolder.nameText.setText(skill.name + itemQtyText + String.format(context.getString(R.string.skill_info),
                    skill.lvRq, skill.hpC, skill.mpC, skill.spC, skillQtyText, trgText, (skill.hasRange()
                            ? context.getString(R.string.yes) : context.getString(R.string.no))));
            return view;
        }

        @Override
        public View getView(final int position, View view, final ViewGroup parent) {
            view = this.prepareView(position, view, parent);
            final ViewHolder vHolder = (ViewHolder)view.getTag();
            final Performance skill = this.skills.get(position);
            final ScenePlay scenePlay = this.arenaAct.scenePlay;
            final Actor crActor = scenePlay._players[scenePlay._current];
            final Hashtable itemsQtyMap = this.asItems ? crActor._items : null;
            final Integer itemsQtyInt = itemsQtyMap == null ? null : (Integer)itemsQtyMap.get(skill);
            vHolder.nameText.setText(itemsQtyInt == null ? skill.name : skill.name + " x "
                    + String.valueOf(itemsQtyInt.intValue()) + " ");
            return view;
        }

        public AbilityArrayAdapter(final ArenaAct context, final int layoutRes,
                                   final Vector<Performance> skills, final boolean asItems) {
            super(context, layoutRes);
            this.layoutRes = layoutRes;
            this.arenaAct = context;
            this.asItems = asItems;
            this.skills = skills;
        }
    }

    private class ActorArrayAdapter extends ArrayAdapter<Actor> {

        private final int layoutRes;
        private final ArenaAct arenaAct;
        private Actor[] actors;

        @Override
        public Actor getItem(final int position) {
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
            final Actor actor = this.actors[position];
            vHolder.nameText.setText(actor.name + " " + Costume.hpText + ": "
                    + (((position < this.arenaAct.scenePlay._enIdx) ? actor._hp + "/"
                    + actor.mHp + ", " + Costume.mpText + ": " + actor._mp +"/" + actor.mMp
                    + ", " + Costume.spText + ": " + actor._sp + "/" + actor.mSp
                    : "%.2f".format((((float)actor._hp) / ((float)actor.mHp) * 100.0f) + "%") + ")")));
            return view;
        }

        @Override
        public View getView(final int position, View view, final ViewGroup parent) {
            view = this.prepareView(view, parent);
            final ViewHolder vHolder = (ViewHolder)view.getTag();
            vHolder.nameText.setText(this.actors[position].name);
            return view;
        }

        public ActorArrayAdapter(final ArenaAct context, final int layoutRes, final Actor[] actors) {
            super(context, layoutRes);
            this.layoutRes = layoutRes;
            this.arenaAct = context;
            this.actors = actors;
        }
    }

    private ActorArrayAdapter playersAdapter;
    private AbilityArrayAdapter skillsAdapter, itemsAdapter;

    private final View.OnClickListener cAction = new OnClickListener() {
        @Override
        public void onClick(final View v) {
            switch (v.getId()) {
                case R.id.ActBt: {
                    ArenaAct.this.enableControls(false);
                    ArenaAct.this.actionsTxt.append(ArenaAct.this.scenePlay.performSkill(
                            ArenaAct.this.skillsSpn.getSelectedItemPosition(),
                            ArenaAct.this.targetSpn.getSelectedItemPosition(), ""));
                    ArenaAct.this.playSpr();
                    break;
                }
                case R.id.AutoBt: {
                    final ScenePlay scenePlay = ArenaAct.this.scenePlay;
                    final boolean automatic = !ArenaAct.this.automatic;
                    ArenaAct.this.automatic = automatic;
                    if (automatic) {
                        ArenaAct.this.enableControls(false);
                        if (scenePlay._players[scenePlay._current].automatic == 0) {
                            ArenaAct.this.afterAct();
                        }
                    } else {
                        ArenaAct.this.autoBtn.setEnabled(false);
                    }
                    break;
                }
                case R.id.UseBt: {
                    final AbilityArrayAdapter itemsAdapter = ArenaAct.this.itemsAdapter;
                    if (itemsAdapter != null) {
                        ArenaAct.this.enableControls(false);
                        ArenaAct.this.actionsTxt.append(ArenaAct.this.scenePlay.useItem(
                                ArenaAct.this.itemsSpn.getSelectedItemPosition(),
                                ArenaAct.this.targetSpn.getSelectedItemPosition(), ""));
                        itemsAdapter.notifyDataSetChanged();
                        ArenaAct.this.playSpr();
                    }
                    break;
                }
                case R.id.RunBt: {
                    final ScenePlay scenePlay = ArenaAct.this.scenePlay;
                    final TextView actionsTxt = ArenaAct.this.actionsTxt;
                    final String escText = scenePlay.escape();
                    actionsTxt.append(scenePlay.setNext("", true));
                    actionsTxt.append("\n" + escText);
                    if (scenePlay._status == -1) {
                        ArenaAct.this.endingMsg(ArenaAct.this.getString(R.string.escape), ScenePlay.escapeTxt);
                    }
                    else {
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
                    final Actor crActor = scenePlay._players[scenePlay._current];
                    if (crActor.automatic == 0 && skillActBtn.isEnabled()) {
                        ArenaAct.this.cAction.onClick(skillActBtn);
                    }
                }
                else {
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
                        outcome.putExtra("Outcome", ArenaAct.this.scenePlay._status);
                        ArenaAct.this.setResult(Activity.RESULT_OK, outcome);
                        ArenaAct.this.finish();
                    }
                });
    }

    private void setCrSkills() {
        final Spinner skillsSpn = this.skillsSpn;
        final Spinner targetSpn = this.targetSpn;
        final Button skillActBtn = this.skillActBtn;
        final Actor crActor;
        {
            final ScenePlay scenePlay = this.scenePlay;
            crActor = scenePlay._players[scenePlay._current];
        }
        AbilityArrayAdapter skillsAdapter = this.skillsAdapter;
        if (skillsAdapter == null) {
            skillsAdapter = new AbilityArrayAdapter(this, android.R.layout.simple_spinner_dropdown_item,
                    crActor.getAvailableSkills(), false);
            skillsSpn.setAdapter(skillsAdapter);
            this.skillsAdapter = skillsAdapter;
            skillsSpn.setOnItemClickListener(new AdapterView.OnItemClickListener() {
                //@Override
                public void onNothingSelected(final AdapterView<?> parent) {
                    skillActBtn.setEnabled(false);
                }

                @Override
                public void onItemClick(final AdapterView<?> parent, final View view, final int position, final long id) {
                    final Actor crActor;
                    {
                        final ScenePlay scenePlay = ArenaAct.this.scenePlay;
                        crActor = scenePlay._players[scenePlay._current];
                    }
                    final Vector<Performance> availableSkills = crActor.getAvailableSkills();
                    skillActBtn.setEnabled(((view != null && ((ViewHolder) view.getTag()).usable)
                            || (view == null && availableSkills.get(position).canPerform(crActor)))
                            && ArenaAct.this.canTarget(targetSpn.getSelectedItemPosition(), availableSkills.get(position)));
                }
            });
            skillActBtn.setOnClickListener(this.cAction);
        } else {
            skillsAdapter.setSkills(crActor.getAvailableSkills());
        }
    }

    private void setCrItems() {

    }

    private void playSpr() {
        final ScenePlay scenePlay = this.scenePlay;
        final Actor[] players = scenePlay._players;
        final ImageView[] imgActor = this.imgActor;
        final int current = scenePlay._current;
        final Actor crActor = players[current];
        final Performance lastAbility = scenePlay._lastAbility;
        final int sprType = (lastAbility == null || lastAbility.trg < 0
                || lastAbility.dmgType == 2 || lastAbility.dmgType == 3) ? 6 : 5;
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
                Actor htActor;
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
                final Actor actor = scenePlay._players[scenePlay._current];
                final TextView infoTxt = this.infoTxt;
                if (this.automatic || actor.automatic != 0) {
                    if (infoTxt.getText().length() > 0) {
                        infoTxt.setText("");
                    }
                    this.actionsTxt.append(scenePlay.executeAI(""));
                    this.playSpr();
                }
                else {
                    this.setCrSkills();
                    this.setCrItems();
                    infoTxt.setText(String.format(this.getString(R.string.cr_actor_info), actor.name, actor._lv, actor._xp, actor._maxp));
                    this.enableControls(true);
                    this.autoBtn.setEnabled(true);
                    this.setCrAutoSkill();
                }
                break;
            }
            case 1: {
                this.endingMsg(this.getString(R.string.victory), ScenePlay.victoryTxt);
                break;
            }
            case -2: {
                this.endingMsg(this.getString(R.string.defeat), ScenePlay.fallenTxt);
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
        final Actor crActor = scenePlay._players[scenePlay._current];
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

    private void renounce() {
        this.scenePlay._status = -2;
        this.afterAct();
    }

    @Override
    public void onCreate(final Bundle savedInstance) {
        super.onCreate(savedInstance);
        this.setContentView(R.layout.activity_arena);
        final Bundle extra = this.getIntent().getExtras();
        final Parcelable[] party;
        final Parcelable[] enemy;
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
            party = extra.getParcelableArray("party");
            enemy = extra.getParcelableArray("enemy");
            final String[] jScripts = extra.getStringArray("scripts");
            if (jScripts != null) {
                this.jScripts = jScripts;
            }
        } else {
            surprised = 0;
            escapable = true;
            party = null;
            enemy = null;
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
        final ScenePlay scenePlay = new ScenePlay(party, enemy, surprised);
        final Actor[] players = scenePlay._players;
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
                android.R.layout.simple_spinner_dropdown_item, players);
        this.playersAdapter = playersAdapter;
        targetSpn.setAdapter(playersAdapter);
        targetSpn.setSelection(enIdx);
        targetSpn.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            //@Override
            public void onNothingSelected(final AdapterView<?> parent) {
                skillActBtn.setEnabled(false);
                itemUseBtn.setEnabled(false);
            }

            @Override
            public void onItemClick(final AdapterView<?> parent, final View view, final int position, final long id) {
                final Actor crActor;
                {
                    final ScenePlay scenePlay = ArenaAct.this.scenePlay;
                    crActor = scenePlay._players[scenePlay._current];
                }
                if (crActor.automatic == 0) {
                    if (itemsSpn.isEnabled()) {
                        itemUseBtn.setEnabled(((ViewHolder)itemsSpn.getSelectedView().getTag()).usable
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
    protected void onDestroy() {
        ArenaAct.cachedParty = null;
        ArenaAct.cachedEnemy = null;
        ArenaAct.cachedSkill = null;
        ArenaAct.cachedItem = null;
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