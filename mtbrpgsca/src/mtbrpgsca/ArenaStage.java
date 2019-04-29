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
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;
import javax.microedition.lcdui.game.GameCanvas;
import javax.microedition.lcdui.game.LayerManager;
import javax.microedition.lcdui.game.Sprite;

public final class ArenaStage extends GameCanvas implements Runnable {
    
    static final int FRM_TIME = 87;
    static final int FRM_WIDTH = 80;
    static final int FRM_HEIGHT = 80;
    static final int SPR_HIT = 0;
    static final int SPR_FALL_= 1;
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

        protected volatile int crt = -1;

        protected Sprite setCurrent(final int crt) {
            final Sprite spr;
            int old = this.crt;
            if (old == crt) {
                spr = this.spr;
            } else {
                this.crt = crt;
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
            spr.setFrame(crt < -1 ? spr.getFrameSequenceLength() - 1 : 0);
            return spr;
        }

        public int drawFrame(final Graphics gpc) {
            final Sprite spr = this.spr;
            spr.paint(gpc);
            if (this.crt > -1) {
                spr.nextFrame();
                return spr.getFrame();
            } else {
                return -1;
            }
        }

        private Sprite prepareSpr(final int i, final boolean invert) {
            Sprite spr = this.spr;
            if (spr == null) {
                spr = new Sprite(this.img[i], ArenaStage.FRM_WIDTH, ArenaStage.FRM_HEIGHT);
            } else {
                spr.setImage(this.img[i], ArenaStage.FRM_WIDTH, ArenaStage.FRM_HEIGHT);
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
            final Sprite spr = this.spr = this.prepareSpr(0, false);
            spr.setTransform(mirror ? Sprite.TRANS_MIRROR : Sprite.TRANS_NONE);
            spr.setRefPixelPosition(x, y);
        }
    }

    public void playSpr() {
        Thread thr = this.thr;
        if (thr == null) {
            thr = this.thr = new Thread(this);
            thr.start();
        }
    }

    public void run() {
        final Graphics g = this.getGraphics();
        final SpriteImage[] sprites = this.sprites;
        final int yPos = this.sceneYbegin;
        final int width = this.totalWidth;
        final int height = this.totalHeight - (yPos * 2);
        while (true) {
            g.setColor(0x000000);
            g.drawString("width = " + this.totalWidth, 15, 0, Graphics.TOP|Graphics.LEFT);
            g.drawString("height = " + this.totalHeight, 15, 15, Graphics.TOP|Graphics.LEFT);
            g.drawString("sceneYbegin = " + this.sceneYbegin, 20, height + yPos, Graphics.TOP|Graphics.LEFT);
            g.drawString("sceneYend = " + this.sceneYend, 15, height + yPos + 10, Graphics.TOP|Graphics.LEFT);
            g.setClip(0, yPos, width, height);
            g.fillRect(0, yPos, width, height);
            for (int i = 0; i < sprites.length; i++) {
                final SpriteImage sprImage = this.sprites[i];
                final Sprite spr = sprImage.spr;
                if (spr.getFrame() < spr.getFrameSequenceLength()) {
                    if (sprImage.drawFrame(g) == 0) {
                        sprImage.setCurrent(sprImage.actor._hp < 0 ? -2 : -1);
                    }
                }
            }
            this.flushGraphics();
            try {
                Thread.sleep(ArenaStage.FRM_TIME);
            } catch (final InterruptedException ex) {
                //g.setColor(0xFFFFFF);
                g.drawString(ex.getMessage(), 5, 105, Graphics.BOTTOM|Graphics.LEFT);
                ex.printStackTrace();
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
        final int yFactor = height / 10;
        final int sceneYbegin = this.sceneYbegin = 32;
        final int sceneYend = this.sceneYend = sceneYbegin + (yFactor * 3);
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
                        sprImages[i] = new SpriteImage(team[i], xCentre - xFactor, sceneYbegin + yFactor, false);
                        break;
                    case 1:
                        sprImages[i] = new SpriteImage(team[i], xCentre - (xFactor * 2 - xFraction) , sceneYbegin, false);
                        break;
                    case 2:
                        sprImages[i] = new SpriteImage(team[i], xCentre - (xFactor * 2 - xFraction) , sceneYend - yFactor , false);
                        break;
                    case 3:
                        sprImages[i] = new SpriteImage(team[i], xCentre - xFactor, sceneYend, false);
                        break;
                }
                
            //layer.append(sprImages[0].spr);
            }
            team = surprise < 0 ? party : enemy;
            for (int j = 0; j < team.length; j++) {
                switch (j) {
                    case 0:
                        sprImages[i++] = new SpriteImage(team[j], xCentre + xFactor, sceneYbegin, true);
                        break;
                    case 1:
                        sprImages[i++] = new SpriteImage(team[j], xCentre + (xFactor * 2 - xFraction), sceneYbegin + yFactor, true);
                        break;
                    case 2:
                        sprImages[i++] = new SpriteImage(team[j], xCentre + (xFactor * 2 - xFraction), sceneYend, true);
                        break;
                    case 3:
                        sprImages[i++] = new SpriteImage(team[j], xCentre + xFactor, sceneYend - yFactor, true);
                        break;
                }
            }
        } catch (final IOException ex) {
            ex.printStackTrace();
        }
        sprImages[0].setCurrent(3);
        sprImages[1].setCurrent(2);
        sprImages[3].setCurrent(0);
        sprImages[4].setCurrent(1);
        sprImages[5].setCurrent(0);
        sprImages[6].setCurrent(0);
        sprImages[7].setCurrent(2);
        
        this.playSpr();
    }
    
}