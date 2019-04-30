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
        boolean updSprites = true;
        boolean updSkillInfo = true;
        boolean updActorInfo = false;
        boolean updTargetInfo = true;
        boolean updActions = true;
        int target = sceneAct._enIdx;
        int koActors = 0; //bitwise int;
        int crAbility = 0;
        int sprPlay = sceneAct._players.length;
        int sprWait = 7;
        while (true) {
            
            if (updActions) {
                
                updActions = false;
            }
            if (updActorInfo) {
                g.setColor(0x000000);
                g.setClip(0, -3, width, 11);
                g.fillRect(0, -3, width, 11);
                final Actor crActor = sceneAct._players[sceneAct._current];
                final String actText = crActor.name + "(HP:" + crActor._hp + "/" + crActor.mHp + ",MP:"
                        + crActor._mp + "/" + crActor.mMp + ",RP:" + crActor._sp + "/" + crActor.mSp + ")";
                g.setColor(0xFFFFFF);
                g.drawString(actText, 3, -3, Graphics.TOP|Graphics.LEFT);
                this.flushGraphics(0, -3, width, 11);
                updActorInfo = false;
            }
            if (updSkillInfo) {
                final String prfText;
                if (crAbility < 0) {
                    //TODO: item;
                } else {
                    
                }
                updSkillInfo = false;
            }
            if (updTargetInfo) {
                final String trgText;
                final Actor trgActor = sceneAct._players[target];
                if (target < sceneAct._enIdx) {
                    
                } else {
                    trgText = trgActor.name + "(HP: " + trgActor._hp + "/" + trgActor.mHp + ", MP: " + trgActor._mp
                            + "/" + trgActor.mMp + ", RP: " + trgActor._sp + "/" + trgActor.mSp + ")";
                }
                //TODO: draw;
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
                    this.afterAct();
                    updActorInfo = true;
                } else {
                    g.setColor(0x000000);
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
                                        sprImage.crt = sprImage.actor._hp < 0 ? SPR_KO : SPR_IDLE;
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
                        //g.setColor(0xFFFFFF);
                        g.drawString(ex.getMessage(), 5, 105, Graphics.TOP|Graphics.LEFT);
                        ex.printStackTrace();
                    }
                }
            }
            
        }
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
        sprImages[0].crt = 3;
        sprImages[1].crt = SPR_ACT;
        sprImages[3].crt = 0;
        sprImages[4].crt = 1;
        sprImages[5].crt = 0;
        sprImages[6].crt = 0;
        sprImages[7].crt = 2;
        
        (this.thr = new Thread(this)).start();
    }
    
}