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

import java.io.IOException;
import java.util.Hashtable;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;
import javax.microedition.lcdui.game.GameCanvas;
import javax.microedition.lcdui.game.LayerManager;
import javax.microedition.lcdui.game.Sprite;

public final class ArenaStage extends GameCanvas implements Runnable {
    
    static final int FRM_TIME = 87;
    static final int SPR_DONE = -3;
    static final int SPR_KO = -2;
    static final int SPR_IDLE = -1;
    static final int SPR_HIT = 0;
    static final int SPR_FALL = 1;
    static final int SPR_ACT = 2;
    static final int SPR_RISE = 3;

    private final SceneAct sceneAct;
    
    private final int totalWidth;
    private final int totalHeight;
    
    private final int sceneYbegin;
    private final int sceneYend;
    
    //private final LayerManager layer;

    private Thread thr;
    private SpriteImage[] sprites;

    private final static class SpriteImage {
        protected final Actor actor;

        private final Sprite spr;
        private final Image[] img;
        private final int[][] seq;

        protected volatile int crt;
        private int old = -7;

        protected Sprite setCurrent() {
            final Sprite spr;
            final int crt = this.crt;
            int old = this.old;
            if (old == crt) {
                spr = this.spr;
            } else {
                this.old = crt;
                final boolean rise = crt > 2;
                final int i = rise ? 1 : (crt < 0 ? (-1 * crt) - 1 : crt);
                if (old < 0) {
                    old = (-1 * old) - 1;
                    if (old == i) {
                        spr = this.spr;
                    } else {
                        spr = this.prepareSpr(i, rise);
                    }
                } else {
                    spr = this.prepareSpr(i, rise);
                }
            }
            /*this.old = crt;
            final boolean rise = crt > 2;
            final int i = rise ? 1 : (crt < 0 ? (-1 * crt) - 1 : crt);
            spr = this.prepareSpr(i, rise);*/
            spr.setFrame(crt < -1 ? spr.getFrameSequenceLength() - 1 : 0);
            return spr;
        }

        public int drawFrame(final Graphics gpc) {
            final Sprite spr = this.spr;
            spr.paint(gpc);
            if (this.old > -1) {
                spr.nextFrame();
                return spr.getFrame();
            } else {
                return -1;
            }
        }

        private Sprite prepareSpr(final int i, final boolean invert) {
            Sprite spr = this.spr;
            {
                final Image img = this.img[i];
                final int imgHeight = img.getHeight();
                if (spr == null) {
                    spr = new Sprite(img, imgHeight, imgHeight);
                } else {
                    spr.setImage(img, imgHeight, imgHeight);
                }
            }
            final int frameCount = spr.getRawFrameCount();
            if (frameCount < 7) {
                int[] seq = this.seq[i];
                if (seq == null) {
                    seq = this.seq[i] = new int[(frameCount * 2) + 1];
                    int j;
                    for (j = 0; j < frameCount; j++) {
                        seq[j] = j;
                    }                    
                    int k = frameCount - 1;
                    seq[j++] = k;
                    for (; k > -1; k--) {
                        seq[j++] = k;
                    }
                }
                spr.setFrameSequence(seq);
            } else if (invert) {
                int[] seq = this.seq[i];
                if (seq == null) {
                    seq = this.seq[i] = new int[frameCount];
                    int j = 0;
                    for (int k = frameCount - 1; k > -1; k--) {
                        seq[j++] = k;
                    }
                }                    
                spr.setFrameSequence(seq);
            } else {
                spr.setFrameSequence(null);
            }
            return spr;
        }

        public SpriteImage(final Actor actor, final int x, final int y, final boolean mirror) throws IOException {
            this.actor = actor;
            this.seq = new int[3][];
            final Image[] img = this.img = new Image[3];
            for (int i = 0; i < 3; i++) {
                img[i] = Image.createImage("/bt_" + actor._job.sprite + "_" + (i + 1) + "_l.png");
            }
            final boolean ko = actor._hp < 1;
            final Sprite spr = this.spr = this.prepareSpr(ko ? SPR_FALL : SPR_HIT, false);
            spr.setTransform(mirror ? Sprite.TRANS_MIRROR : Sprite.TRANS_NONE);
            spr.setRefPixelPosition(x, y);
            this.crt = ko ? SPR_KO : SPR_IDLE;
        }
    }

    public void playSpr() {
        
    }
    
    public void afterAct() {
        
    }

    public void run() {
        final Graphics g = this.getGraphics();
        g.setFont(Font.getFont(Font.FACE_SYSTEM, Font.STYLE_PLAIN, Font.SIZE_SMALL));
        final SpriteImage[] sprites = this.sprites;
        final SceneAct sceneAct = this.sceneAct;
        final int yPos = this.sceneYbegin;
        final int width = this.totalWidth;
        final int yEnd = this.sceneYend;
        final int height = this.totalHeight;
        final int scnHeight = yEnd - yPos;
        boolean prfCanPerform = true;
        boolean updSprites = true;
        boolean updSkillInfo = true;
        boolean updActorInfo = true;
        boolean updTargetInfo = true;
        boolean updActions = true;
        boolean newTurn = false;
        boolean afterAct = false;
        try {
        Actor crActor = sceneAct._players[sceneAct._current];
        int target = sceneAct._enIdx;
        int koActors = 0; //bitwise int;
        int crAbility = 0;
        Performance crPrf = (Performance)crActor.getAvailableSkills().elementAt(0);
        int sprPlay = sceneAct._players.length;
        int sprWait = 0;
        int keysState;
        while (true) {
            if (updActions) {
                if (afterAct) {
                        sprites[sceneAct._current].crt = SPR_ACT;
                        sprPlay = 1;
                        for (int i = sceneAct._fTarget; i <= sceneAct._lTarget; i++) {
                            if (i != sceneAct._current) {
                                final int koBit = this.pow(2, i + 1);
                                if (sceneAct._players[i]._hp > 0) {
                                    if ((koActors & koBit) == koBit) {
                                        sprites[i].crt = SPR_RISE;
                                        koActors -= koBit;
                                    } else {                                    
                                        sprites[i].crt = SPR_HIT;
                                    }
                                } else {
                                    sprites[i].crt = SPR_FALL;
                                    koActors += koBit;
                                }
                                sprPlay++;
                            }
                        }
                        updSprites = true;
                        afterAct = false;
                }
                g.setColor(0x0080FF);
                g.setClip(0, yEnd, width, height - yEnd);
                g.fillRect(0, yEnd, width, height - yEnd);
                g.setColor(0xFFFF00);
                //TODO: draw string
                this.flushGraphics(0, yEnd, width, height - yEnd);
                if (newTurn) {
                    if (sceneAct._players[sceneAct._current].automatic == 0) {
                        crActor = sceneAct._players[sceneAct._current];
                        crPrf = (Performance)crActor.getAvailableSkills().elementAt(0);
                        updTargetInfo = true;
                        updSkillInfo = true;
                        updActorInfo = true;
                        updActions = false;
                        crAbility = 0;
                    } else {
                        sceneAct.executeAI("");
                        afterAct = true;
                    }
                    newTurn = false;
                }
            }
            if (updActorInfo) {
                g.setColor(0x0080FF);
                g.setClip(0, -2, width, 13);
                g.fillRect(0, -2, width, 13);
                final String actText = crActor.name + "(HP:" + crActor._hp + "/" + crActor.mHp + ",MP:"
                        + crActor._mp + "/" + crActor.mMp + ",RP:" + crActor._sp + "/" + crActor.mSp + ")";
                g.setColor(0xFFFF00);
                g.drawString(actText, 5, -2, Graphics.TOP|Graphics.LEFT);
                this.flushGraphics(0, -2, width, 13);
                updActorInfo = false;
            }
            if (updSkillInfo) {
                String prfText = "←" + crPrf.name;
                if (crAbility < 0) {
                    prfText += "";//TODO: item;
                } else {
                    prfText += "(Lv:" + crPrf.lvRq + ",HPc:" + crPrf.hpC + ",MPc:" + crPrf.mpC
                            + ",RPc:" + crPrf.spC + ",Nr:" + 0;
                }
                prfText += ")→";
                g.setColor(0x0080FF);
                g.setClip(0, 11, width, 12);
                g.fillRect(0, 11, width, 12);
                g.setColor(prfCanPerform ? 0xFFFF00 : 0xC0C0C0);
                g.drawString(prfText, 0, 9, Graphics.TOP|Graphics.LEFT);
                this.flushGraphics(0, 11, width, 12);
                updSkillInfo = false;
            }
            if (updTargetInfo) {
                final Actor trgActor = sceneAct._players[target];
                String trgText = "↓" + trgActor.name + "(HP:";
                if (target >= sceneAct._enIdx) {
                    trgText += "";
                } else {
                    trgText += trgActor._hp + "/" + trgActor.mHp + ",MP:" + trgActor._mp
                            + "/" + trgActor.mMp + ",RP:" + trgActor._sp + "/" + trgActor.mSp;
                }
                trgText += ")↑";
                g.setColor(0x0080FF);
                g.setClip(0, 23, width, 12);
                g.fillRect(0, 23, width, 12);
                g.setColor(sceneAct.getGuardian(target, crPrf) == target ? 0xFFFF00 : 0xC0C0C0);
                g.drawString(trgText, 2, 21, Graphics.TOP|Graphics.LEFT);
                this.flushGraphics(0, 23, width, 12);
                updTargetInfo = false;
            }
            while (updSprites) {
                /*g.setColor(0xFFFFFF);
                g.setClip(0, -3, width, yPos);
                g.fillRect(0, -3, width, yPos);
                g.setColor(0x000000);
                g.drawString("sprWait = " + sprWait, 15, -3, Graphics.TOP|Graphics.LEFT);
                g.drawString("sprPlay = " + sprPlay, 15, 8, Graphics.TOP|Graphics.LEFT);
                g.drawString("crSpr = " + sprites[1].crt, 15, 19, Graphics.TOP|Graphics.LEFT);
                this.flushGraphics(0, -3, width, yPos);
                g.setColor(0xFFFFFF);
                g.setClip(0, yEnd, width, height - yEnd);
                g.fillRect(0, yEnd, width, height - yEnd);
                g.setColor(0x000000);
                g.drawString("oldSprite = " + oldSprite, 15, yEnd - 3, Graphics.TOP|Graphics.LEFT);
                g.drawString("sprFrame = " + sprFrm2, 15, yEnd + 8, Graphics.TOP|Graphics.LEFT);
                this.flushGraphics(0, yEnd, width, height - yEnd);*/
                if (sprPlay < 1) {
                    sprWait = 5;
                    updSprites = false;
                    final String ret = sceneAct.setNext("", true);
                    updActions = true;
                    newTurn = true;
                } else {
                    g.setColor(0x007F00);
                    g.setClip(0, yPos, width, scnHeight);
                    g.fillRect(0, yPos, width, scnHeight);
                    for (int i = 0; i < sprites.length; i++) {
                        final SpriteImage sprImage = sprites[i];
                        final int crtSprite = sprImage.crt;
                        if (crtSprite != SPR_DONE && (crtSprite == SPR_ACT || sprWait == 0)) {
                            if (sprImage.old != crtSprite) {
                                sprImage.setCurrent();
                            }
                            final Sprite spr = sprImage.spr;
                            if (spr.getFrame() < spr.getFrameSequenceLength()) {
                                switch (sprImage.drawFrame(g)) {
                                    case -1:
                                        sprImage.crt = SPR_DONE;
                                        sprPlay--;
                                        break;
                                    case 0:
                                        sprImage.crt = sprImage.actor._hp < 1 ? SPR_KO : SPR_IDLE;
                                        break;
                                }
                            }
                        } else {
                            sprImage.drawFrame(g);
                        }
                    }
                    if (sprWait > 0) {
                        sprWait--;
                    }
                    this.flushGraphics(0, yPos, width, scnHeight);
                    try {
                        Thread.sleep(ArenaStage.FRM_TIME);
                    } catch (final InterruptedException ex) {
                        g.setColor(0xFFFFFF);
                        g.drawString(ex.getMessage(), 5, 105, Graphics.TOP|Graphics.LEFT);
                        ex.printStackTrace();
                    }
                }
            }
            if (!updActions && !updSprites && (keysState = this.getKeyStates()) > 0) {
                if ((keysState & UP_PRESSED) != 0) {
                    if (target > 0) {
                        target--;
                        updTargetInfo = true;
                    }
                } else if ((keysState & DOWN_PRESSED) != 0) {
                    if (target < sceneAct._players.length - 1) {
                        target++;
                        updTargetInfo = true;
                    }
                }
                if ((keysState & LEFT_PRESSED) != 0) {
                    final Hashtable items = crActor._items;
                    if (crAbility > 0 || (items != null && crAbility > -1 * items.size() - 1)) {
                        crPrf = --crAbility < 0 ? null : (Performance)crActor.getAvailableSkills().elementAt(crAbility);
                        prfCanPerform = crPrf.canPerform(crActor);
                        updSkillInfo = true;
                    }
                } else if ((keysState & RIGHT_PRESSED) != 0) {
                    if (crAbility < crActor._skills.size() - 1) {
                        crPrf = ++crAbility < 0 ? null : (Performance)crActor.getAvailableSkills().elementAt(crAbility);
                        prfCanPerform = crPrf.canPerform(crActor);
                        updSkillInfo = true;
                    }
                }
                if (prfCanPerform && (keysState & FIRE_PRESSED) != 0) {
                    final String ret = sceneAct.executeAbility(crPrf, target, "");
                    updActions = true;
                    afterAct = true;
                }
                try {
                    Thread.sleep(500);
                } catch (final InterruptedException ex) {
                    g.setColor(0xFFFFFF);
                    g.drawString(ex.getMessage(), 5, 105, Graphics.TOP|Graphics.LEFT);
                    ex.printStackTrace();
                }
            }
        }
        } catch (final Exception ex) {
                    g.setColor(0xFFFFFF);
                    g.drawString(ex.getMessage(), 0, 50, Graphics.TOP|Graphics.LEFT);
                    ex.printStackTrace();
        }
    }
    
    int pow(final int x, final int y) {
        int z = 1;
        for (int i = 0; i < y; i++) {
            z *= x;
        }
        return z;
    }

    public ArenaStage(final String string, final Actor[] party, final Actor[] enemy, final int surprise) {
        super(true);
        final SceneAct sceneAct = this.sceneAct = new SceneAct(party, enemy, surprise);
        final int height = this.totalHeight = this.getHeight();
        final int width = this.totalWidth = this.getWidth();
        final int xFactor = ((width / 4) + (width / (width / 10))) + (width % 30) / 3;
        final int xFraction = xFactor / ((width / 30) / 2);
        final int xCentre = width / 2;
        final int sceneYbegin = this.sceneYbegin = 35;
        final int sceneYend = this.sceneYend = height - 21; //+ (yFactor * 3);        
        final int yCentre = ((sceneYend) / 2) - sceneYbegin;
        final int yFactor = (yCentre / 2);
        final int enIdx = sceneAct._enIdx;
        final Actor[] players = sceneAct._players;
        final int len = sceneAct._players.length;
        //final LayerManager layer = this.layer = new LayerManager();
        for (int i = enIdx; i < len; i++) {
            players[i].automatic = 2;
        }
        final SpriteImage[] sprImages = this.sprites = new SpriteImage[len];
        try {
            Actor[] team = surprise < 0 ? enemy : party;
            int i;
            for (i = 0; i < team.length; i++) {
                switch (i) {
                    case 0:
                        sprImages[i] = new SpriteImage(team[i], xCentre - xFactor, yCentre - yFactor / 5, false);
                        break;
                    case 1:
                        sprImages[i] = new SpriteImage(team[i], xCentre - (xFactor * 2 - xFraction), yCentre - yFactor, false);
                        break;
                    case 2:
                        sprImages[i] = new SpriteImage(team[i], xCentre - (xFactor * 2 - xFraction), yCentre + (yFactor - yFactor / 3), false);
                        break;
                    case 3:
                        sprImages[i] = new SpriteImage(team[i], xCentre - xFactor, yCentre + yFactor + yFactor / 2, false);
                        break;
                }
                
            //layer.append(sprImages[0].spr);
            }
            team = surprise < 0 ? party : enemy;
            for (int j = 0; j < team.length; j++) {
                switch (j) {
                    case 0:
                        sprImages[i++] = new SpriteImage(team[j], xCentre + xFactor, yCentre - yFactor, true);
                        break;
                    case 1:
                        sprImages[i++] = new SpriteImage(team[j], xCentre + (xFactor * 2 - xFraction), yCentre - yFactor / 5, true);
                        break;
                    case 2:
                        sprImages[i++] = new SpriteImage(team[j], xCentre + (xFactor * 2 - xFraction), yCentre + yFactor + yFactor / 2, true);
                        break;
                    case 3:
                        sprImages[i++] = new SpriteImage(team[j], xCentre + xFactor, yCentre + (yFactor - yFactor / 3), true);
                        break;
                }
            }
        } catch (final IOException ex) {
            ex.printStackTrace();
        }
        
        (this.thr = new Thread(this)).start();
    }
    
}