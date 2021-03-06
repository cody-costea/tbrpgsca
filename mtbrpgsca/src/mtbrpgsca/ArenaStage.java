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
import javax.microedition.lcdui.game.Sprite;

public final class ArenaStage extends GameCanvas implements Runnable {
    
    private static final int FRM_TIME = 71;
    private static final int SPR_DONE = -3;
    private static final int SPR_KO = -2;
    private static final int SPR_IDLE = -1;
    private static final int SPR_HIT = 0;
    private static final int SPR_FALL = 1;
    private static final int SPR_ACT = 2;
    private static final int SPR_RISE = 3;
    
    private final Midlet app;

    private final SceneAct sceneAct;
        
    private Image arenaImg = null;
    
    private final int totalWidth;
    private final int totalHeight;
    
    private final int sceneYbegin;
    private final int sceneYend;
    
    private final Thread thr;
    private final SpriteImage[] sprites;
    private final SpriteImage[] updSprOrd;
    private Hashtable prfSprMap; //<Performance, Image>
    
    private volatile int target;
    private volatile Actor crActor;
    private volatile int crAbility;
    private volatile Performance crPrf;
    private volatile boolean automatic = false;
    private volatile boolean updSkillInfo = true;
    private volatile boolean updTargetInfo = true;
    private volatile boolean updActions = true;
    private volatile boolean afterAct = false;
    private volatile boolean prfCanPerform = true;
    private volatile String ret = "";

    private final static class SpriteImage {
    
        private static int RefPixel = 80;
    
        protected final Actor actor;        
        private final Sprite spr;
        private final Image[] img;
        private final int[][] seq;

        private Sprite perfSpr = null;
        private Image lastPrfImg = null;
        private boolean prfSprPlay = false;

        private int old = -7;
        protected volatile int crt;

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
                    if (old == i && !rise) {
                        spr = this.spr;
                    } else {
                        spr = this.prepareSpr(i, rise);
                    }
                } else {
                    spr = this.prepareSpr(i, rise);
                }
            }
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
        
        public int drawSkillFrame(final Graphics gpc) {
            final Sprite spr = this.perfSpr;
            spr.paint(gpc);
            spr.nextFrame();
            return spr.getFrame();
        }
        
        public Sprite setSkillSprite(final Image img) {
            if (img != this.lastPrfImg) {
                final int imgHeight = img.getHeight();
                Sprite spr = this.perfSpr;
                if (spr == null) {
                    final Sprite actorSpr = this.spr;
                    spr = this.perfSpr = new Sprite(img, imgHeight, imgHeight);
                    spr.setRefPixelPosition(actorSpr.getRefPixelX(), actorSpr.getRefPixelY());
                } else {
                    spr.setImage(img, imgHeight, imgHeight);
                }
                this.lastPrfImg = img;
                spr.setFrameSequence(null);
                spr.setFrame(0);
            }
            this.prfSprPlay = true;
            return spr;
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
    
        static Image ResizeImg(final Image original, final int nWidth, final int nHeight) {
            if (original == null) {
                return null;
            }
            final int oHeight = original.getHeight();
            final int oWidth = original.getWidth();
            final int[] oPixels = new int[oHeight * oWidth];
            original.getRGB(oPixels, 0, oWidth, 0, 0, oWidth, oHeight);
            final int[] nPixels = new int[nHeight * nWidth];
            for (int i = 0; i < nHeight; i++) {
                int y = i * oHeight / nHeight;
                for (int j = 0; j < nWidth; j++) {
                    int x = j * oWidth / nWidth;
                    nPixels[i * nWidth + j] = oPixels[y * oWidth + x];
                }
            }
            return Image.createRGBImage(nPixels, nWidth, nHeight, true);
        }

        public SpriteImage(final Actor actor, final int x, final int y, final boolean mirror) throws IOException {
            this.actor = actor;
            this.seq = new int[3][];
            final Image[] img = this.img = new Image[3];
            for (int i = 0; i < 3; i++) {
                final Image sprImage = Image.createImage("/bt_" + actor._job.sprite + "_" + (i + 1) + "_l.png");
                final int imgHeight = sprImage.getHeight();
                final int refPixel = SpriteImage.RefPixel;
                img[i] = imgHeight != refPixel ? SpriteImage.ResizeImg(sprImage, (sprImage.getWidth() / imgHeight) * refPixel, refPixel) : sprImage;
            }
            final boolean ko = actor._hp < 1;
            final Sprite spr = this.spr = this.prepareSpr(ko ? SPR_FALL : SPR_HIT, false);
            spr.setTransform(mirror ? Sprite.TRANS_MIRROR : Sprite.TRANS_NONE);
            spr.setRefPixelPosition(x, y);
            this.crt = ko ? SPR_KO : SPR_IDLE;
        }
        
    }
    
    public int setAutoSkill() {
        final int target = this.target;
        final SceneAct sceneAct = this.sceneAct;
        final boolean onPartySide = target < sceneAct._enIdx;
        final int crAbility = this.crAbility = sceneAct.getAIskill(onPartySide ? 1 : 0, onPartySide && sceneAct._players[target]._hp < 1);
        this.crPrf = (Performance)this.crActor.getAvailableSkills().elementAt(crAbility);
        this.prfCanPerform = true;
        return crAbility;
    }

    public void run() {
        final Graphics g = this.getGraphics();
        final Font f = Font.getFont(Font.FACE_SYSTEM, Font.STYLE_PLAIN, Font.SIZE_SMALL);
        g.setFont(f);
        final SpriteImage[] sprites = this.sprites;
        final SpriteImage[] sprImgOrd = this.updSprOrd;
        final SceneAct sceneAct = this.sceneAct;
        final int yPos = this.sceneYbegin;
        final int width = this.totalWidth;
        final int yEnd = this.sceneYend;
        final int height = this.totalHeight;
        final int scnHeight = yEnd - yPos;
        final Image arenaImg = this.arenaImg = SpriteImage.ResizeImg(this.arenaImg, width, scnHeight);
        boolean updSprites = true;
        boolean updActorInfo = true;
        boolean newTurn = false;
        try {
        int koActors = 0; //bitwise int;
        int sprPlay = sceneAct._players.length;
        int sprWait = 0;
        String prev = "";
        while (true) {
            if (this.updActions) {
                if (this.afterAct) {
                    this.afterAct = false;
                    final Performance lastAbility = sceneAct._lastAbility;
                    if (lastAbility != null) {
                        final int current = sceneAct._current;
                        final SpriteImage crSprite = sprites[current];
                        crSprite.crt = crSprite.actor._hp > 0 ? SPR_ACT : SPR_FALL;
                        final boolean hasPrfSpr = lastAbility.sprite != null;
                        sprPlay = 1;
                        for (int i = sceneAct._fTarget; i <= sceneAct._lTarget; i++) {
                            if (i != current) {
                                final int koBit = 1 << i;
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
                            if (hasPrfSpr) {
                                sprites[i].setSkillSprite(this.getAbilityImage(lastAbility));
                            }
                        }
                    }
                    updSprites = true;
                } else if (newTurn) {
                    if (sceneAct._status != 0) {
                        break;
                    }
                    newTurn = false;
                    if (this.automatic || sceneAct._players[sceneAct._current].automatic != Actor.AUTO_NONE) {
                        ret = sceneAct.executeAI("");
                        this.afterAct = true;
                    } else {
                        this.crActor = sceneAct._players[sceneAct._current];
                        this.setAutoSkill();
                        this.updTargetInfo = true;
                        this.updSkillInfo = true;
                        updActorInfo = true;
                        this.updActions = false;
                    }
                }
                if (ret.length() > 0) {
                    g.setColor(0x0080FF);
                    g.setClip(0, yEnd, width, height - yEnd);
                    g.fillRect(0, yEnd, width, height - yEnd);
                    g.setColor(0xFFFF00);
                    g.drawString(prev, 0, yEnd - 3, Graphics.TOP|Graphics.LEFT);
                    g.drawString(prev = ret, 0, yEnd + 8, Graphics.TOP|Graphics.LEFT);
                    ret = "";
                }
                this.flushGraphics(0, yEnd, width, height - yEnd);
            }
            if (updActorInfo) {
                g.setColor(0x0080FF);
                g.setClip(0, -2, width, 13);
                g.fillRect(0, -2, width, 13);
                final Actor crActor = this.crActor;
                final String actText = crActor.name + "(HP:" + crActor._hp + "/" + crActor.mHp + ",MP:"
                        + crActor._mp + "/" + crActor.mMp + ",RP:" + crActor._sp + "/" + crActor.mSp + ")";
                g.setColor(0xFFFF00);
                g.drawString(actText, 5, -2, Graphics.TOP|Graphics.LEFT);
                this.flushGraphics(0, -2, width, 13);
                updActorInfo = false;
            }
            if (this.updSkillInfo) {
                final Performance crPrf = this.crPrf;
                String prfText = "←" + crPrf.name;
                if (this.crAbility < 0) {
                    final Object objPrfQty = this.crActor.getItems().get(crPrf);
                    prfText += "*" + (objPrfQty == null ? '0'
                            : ((Integer)this.crActor.getItems().get(crPrf)).intValue());//TODO: item;
                }
                prfText += "(Lv:" + crPrf.lvRq + ",HPc:" + crPrf.hpC + ",MPc:" + crPrf.mpC
                        + ",RPc:" + crPrf.spC + ",Nr:" + 0;
                prfText += ")→";
                g.setColor(0x0080FF);
                g.setClip(0, 11, width, 12);
                g.fillRect(0, 11, width, 12);
                g.setColor(this.prfCanPerform ? 0xFFFF00 : 0xC0C0C0);
                g.drawString(prfText, 0, 9, Graphics.TOP|Graphics.LEFT);
                this.flushGraphics(0, 11, width, 12);
                this.updSkillInfo = false;
            }
            if (this.updTargetInfo) {
                final int target = this.target;
                final Actor trgActor = sceneAct._players[target];
                String trgText = "↓" + trgActor.name + "(HP:";
                if (target >= sceneAct._enIdx) {
                    trgText += ((int)(trgActor._hp / ((float)trgActor.mHp) * 100.0)) + "%";
                } else {
                    trgText += trgActor._hp + "/" + trgActor.mHp + ",MP:" + trgActor._mp
                            + "/" + trgActor.mMp + ",RP:" + trgActor._sp + "/" + trgActor.mSp;
                }
                trgText += ")↑";
                g.setColor(0x0080FF);
                g.setClip(0, 23, width, 12);
                g.fillRect(0, 23, width, 12);
                g.setColor(sceneAct.getGuardian(target, this.crPrf) == target ? 0xFFFF00 : 0xC0C0C0);
                g.drawString(trgText, 2, 21, Graphics.TOP|Graphics.LEFT);
                this.flushGraphics(0, 23, width, 12);
                this.updTargetInfo = false;
            }
            while (updSprites) {
                if (sprPlay < 1) {
                    sprWait = 5;
                    updSprites = false;
                    ret = sceneAct.setNext("", true);
                    this.updActions = true;
                    newTurn = true;
                } else {
                    g.setClip(0, yPos, width, scnHeight);
                    if (arenaImg == null) {
                        g.setColor(0x007F00);
                        g.fillRect(0, yPos, width, scnHeight);
                    } else {
                        g.drawImage(arenaImg, 0, yPos, Graphics.TOP | Graphics.LEFT);
                    }
                    for (int i = 0; i < 8; i++) {
                        final SpriteImage sprImage = sprImgOrd[i];
                        if (sprImage == null) {
                            continue;
                        }
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
                        if (sprImage.prfSprPlay) {
                            final Sprite perfSpr = sprImage.perfSpr;
                            if (perfSpr.getFrame() < perfSpr.getFrameSequenceLength()) {
                                if (sprImage.drawSkillFrame(g) == 0) {
                                    sprImage.prfSprPlay = false;
                                }
                            }
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
        }
        } catch (final Exception ex) {
            g.setColor(0xFFFFFF);
            g.drawString(ex.getMessage(), 0, 50, Graphics.TOP|Graphics.LEFT);
            ex.printStackTrace();
        }
    }
    
    protected void keyReleased(final int keyCode) {
        if (this.updActions) {
            return;
        }
        final SceneAct sceneAct = this.sceneAct;
        if (sceneAct._status == 0) {
            final int gameAction = getGameAction(keyCode);
            if (gameAction == FIRE || keyCode == KEY_NUM5 || keyCode == KEY_NUM0) {
                final Performance crPrf;
                final int target = this.target;
                if ((crPrf = this.crPrf).isRestoring() || sceneAct._players[target]._hp > 0) {
                    this.ret = this.crAbility < 0
                            ? sceneAct.useAbility(crPrf, target, "")
                            : sceneAct.executeAbility(crPrf, target, "");
                    this.updActions = true;
                    this.afterAct = true;
                }
            } else if (gameAction == UP || keyCode == KEY_NUM2) {
                int target;
                if ((target = this.target) > 0) {
                    final int enIdx = sceneAct._enIdx;
                    if ((this.target = --target) < enIdx) {
                        final int crAbility = this.crAbility = sceneAct.getAIskill(1, sceneAct._players[target]._hp < 1);
                        this.crPrf = (Performance)this.crActor.getAvailableSkills().elementAt(crAbility);
                        this.updSkillInfo = true;
                    }
                    this.updTargetInfo = true;
                }
            } else if (gameAction == DOWN || keyCode == KEY_NUM8) {
                if (this.target < sceneAct._players.length - 1) {
                    final int enIdx = sceneAct._enIdx;
                    final int target;
                    if (this.target++ < enIdx && (target = this.target) >= enIdx) {
                        final int crAbility = this.crAbility = sceneAct.getAIskill(0, false);
                        this.crPrf = (Performance)this.crActor.getAvailableSkills().elementAt(crAbility);
                        this.updSkillInfo = true;
                    }
                    this.updTargetInfo = true;
                }
            } else if (gameAction == LEFT || keyCode == KEY_NUM4) {
                final Actor crActor = this.crActor;
                final Hashtable items = crActor._items;
                int crAbility = this.crAbility;
                if (crAbility > 0 || (items != null && crAbility > -1 * items.size())) {
                    final Performance crPrf = this.crPrf = (this.crAbility = --crAbility) < 0
                            ? sceneAct._crItems[(crAbility * -1) - 1]
                            : (Performance)crActor.getAvailableSkills().elementAt(crAbility);
                    this.prfCanPerform = crPrf.canPerform(crActor);
                    this.updSkillInfo = true;
                }
            } else if (gameAction == RIGHT || keyCode == KEY_NUM6) {
                final Actor crActor = this.crActor;
                int crAbility = this.crAbility;
                if (crAbility < crActor._skills.size() - 1) {
                    final Performance crPrf = this.crPrf = (this.crAbility = ++crAbility) < 0
                            ? sceneAct._crItems[(crAbility * -1) - 1]
                            : (Performance)crActor.getAvailableSkills().elementAt(crAbility);
                    this.prfCanPerform = crPrf.canPerform(crActor);
                    this.updSkillInfo = true;
                }
            } else if (gameAction == GAME_A || keyCode == KEY_NUM1 || keyCode == KEY_STAR) {
                final boolean automatic = this.automatic = !this.automatic;
                if (automatic && this.crActor.automatic == 0) {
                    this.setAutoSkill();
                    this.ret = sceneAct.executeAbility(this.crPrf, this.target, "");
                    this.updActions = true;
                    this.afterAct = true;
                }
            } else if (gameAction == GAME_C || keyCode == KEY_NUM7) {
                final int crAbility = this.crAbility;
                if (crAbility == this.setAutoSkill()) {                
                    this.ret = sceneAct.executeAbility(this.crPrf, this.target, "");
                    this.updActions = true;
                    this.afterAct = true;
                } else {
                    this.updSkillInfo = true;
                }
            } else if (gameAction == GAME_B || keyCode == KEY_NUM3 || keyCode == KEY_POUND) {
                this.ret = sceneAct.escape();
                this.updActions = true;
                this.afterAct = true;
            } else if (gameAction == GAME_D || keyCode == KEY_NUM9) {
                this.ret = "The party has given up!";
                sceneAct._lastAbility = null;
                sceneAct._status = -2;
                this.updActions = true;
                this.afterAct = true;
            }
        } else {
            this.app.notifyDestroyed();
        }
    }
    
    private Image getAbilityImage(final Performance ability) throws IOException {
        Hashtable prfSprMap = this.prfSprMap;
        if (prfSprMap == null) {
            prfSprMap = this.prfSprMap = new Hashtable();
        }
        Object img = prfSprMap.get(ability);
        if (img == null) {
            final Image sprImage = Image.createImage("/bt_" + ability.sprite + ".png");
            final int imgHeight = sprImage.getHeight();
            final int refPixel = SpriteImage.RefPixel;
            img = imgHeight != refPixel ? SpriteImage.ResizeImg(sprImage, (sprImage.getWidth() / imgHeight) * refPixel, refPixel) : sprImage;
            prfSprMap.put(ability, img);
        }
        return (Image)img;
    }

    public ArenaStage(final Midlet app, final Actor[] party, final Actor[] enemy,
            final int surprise, final String arenaImg, final String arenaSnd) {
        super(false);
        this.app = app;
        final SceneAct sceneAct = this.sceneAct = new SceneAct(party, enemy, surprise);
        final int height = this.totalHeight = this.getHeight();
        final int width = this.totalWidth = this.getWidth();
        final int refPixel = 80 - (((width - 240) * -1) / 8);
        int xFactor = refPixel / 2;
        final int xCloser = xFactor + (xFactor = (xFactor / 2) + (xFactor / 3));
        final int xFurther = refPixel + xFactor;
        final int xCentre = width / 2;
        final int sceneYbegin = this.sceneYbegin = 35;
        final int sceneYend = this.sceneYend = height - 21;
        final int yCentre = ((sceneYend / 2) - (sceneYbegin - (sceneYbegin / 3))) + ((80 - refPixel) / 3);
        final int yCloser = (yCentre / 5);
        final int yFurther = (yCloser * 2) + yCloser;
        final int enIdx = sceneAct._enIdx;
        final Actor[] players = sceneAct._players;
        final int len = sceneAct._players.length;
        for (int i = enIdx; i < len; i++) {
            players[i].automatic = Actor.AUTO_ALLY;
        }
        final SpriteImage[] sprImages = this.sprites = new SpriteImage[len];
        final SpriteImage[] sprImgOrd = this.updSprOrd = new SpriteImage[8];
        try {
            if (arenaImg != null && arenaImg.length() > 0) {
                this.arenaImg = Image.createImage("/bg_" + arenaImg + ".png");
            }
            SpriteImage sprImage;
            final boolean surprised = surprise < 0;
            int i, x = surprised ? 1 : -1;
            for (i = 0; i < party.length; i++) {
                switch (i) {
                    case 0:
                        sprImage = sprImages[i] = new SpriteImage(party[i], xCentre + (xCloser * x), yCentre - yCloser, surprised);
                        sprImgOrd[2] = sprImage;
                        break;
                    case 1:
                        sprImage = sprImages[i] = new SpriteImage(party[i], xCentre + (xFurther * x), yCentre - yFurther, surprised);
                        sprImgOrd[0] = sprImage;
                        break;
                    case 2:
                        sprImage = sprImages[i] = new SpriteImage(party[i], xCentre + (xFurther * x), yCentre + yCloser, surprised);
                        sprImgOrd[4] = sprImage;
                        break;
                    case 3:
                        sprImage = sprImages[i] = new SpriteImage(party[i], xCentre + (xCloser * x), yCentre + yFurther, surprised);
                        sprImgOrd[6] = sprImage;
                        break;
                }               
            }
            x = surprised ? -1 : 1;
            for (int j = 0; j < enemy.length; j++) {
                switch (j) {
                    case 0:
                        sprImage = sprImages[i++] = new SpriteImage(enemy[j], xCentre + (xCloser * x), yCentre - yFurther, !surprised);
                        sprImgOrd[1] = sprImage;
                        break;
                    case 1:
                        sprImage = sprImages[i++] = new SpriteImage(enemy[j], xCentre + (xFurther * x), yCentre - yCloser, !surprised);
                        sprImgOrd[3] = sprImage;
                        break;
                    case 2:
                        sprImage = sprImages[i++] = new SpriteImage(enemy[j], xCentre + (xFurther * x), yCentre + yFurther, !surprised);
                        sprImgOrd[5] = sprImage;
                        break;
                    case 3:
                        sprImage = sprImages[i++] = new SpriteImage(enemy[j], xCentre + (xCloser * x), yCentre + yCloser, !surprised);
                        sprImgOrd[7] = sprImage;
                        break;
                }
            }
        } catch (final IOException ex) {
            ex.printStackTrace();
        }
        this.target = sceneAct._enIdx;
        this.crActor = sceneAct._players[sceneAct._current];
        final int crAbility = this.crAbility = sceneAct.getAIskill(0, false);
        this.crPrf = (Performance)crActor.getAvailableSkills().elementAt(crAbility);
        (this.thr = new Thread(this)).start();
    }
    
}