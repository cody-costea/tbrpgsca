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
import javax.microedition.lcdui.Form;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;
import javax.microedition.lcdui.game.GameCanvas;
import javax.microedition.lcdui.game.Sprite;

public class ArenaStage extends Form {
    
    private static class SceneImage implements Runnable {
        static final int FRM_TIME = 87;
        static final int FRM_WIDTH = 80;
        static final int FRM_HEIGHT = 80;
        static final int SPR_HIT = 0;
        static final int SPR_FALL_= 1;
        static final int SPR_ACT = 2;
        static final int SPR_RISE = 3;
        
        private ArenaStage arenaStage;
        private SceneAct sceneAct;
        private int width;
        private int height;
        
        private Image img;
        private Graphics gpc;
        
        private Thread thr;
        
        private SpriteImage[] sprites;
        
        private static class SpriteImage {
            protected final Actor actor;
            
            private final Sprite spr;
            private final Image[] img;
            private final int[][] seq;
            
            protected volatile int frm = 0;            
            protected volatile int crt = -1;
            
            protected Sprite setCurrent(final int crt) {
                this.crt = crt;
                final boolean rise = crt > 2;
                final Sprite spr = this.prepareSpr(crt < 0 ? (-1 * crt) - 1 : (rise ? 1 : crt), rise);
                this.frm = crt < -1 ? spr.getFrameSequenceLength() - 1 : 0;
                return spr;
            }
        
            public void drawFrame(final Graphics gpc) {
                final Sprite spr = this.spr;
                spr.paint(gpc);
                if (this.crt > -1) {
                    spr.setFrame(++this.frm);
                }
            }
            
            private Sprite prepareSpr(final int i, final boolean invert) {
                Sprite spr = this.spr;
                if (spr == null) {
                    spr = new Sprite(this.img[i], SceneImage.FRM_WIDTH, SceneImage.FRM_HEIGHT);
                } else {
                    spr.setImage(this.img[i], SceneImage.FRM_WIDTH, SceneImage.FRM_HEIGHT);
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
                this.seq = new int[2][];
                final Image[] img = this.img = new Image[3];
                for (int i = 0; i < 3; i++) {
                    img[i] = Image.createImage("/bt_" + actor._job.sprite + "_" + (i + 1) + ".png");
                }
                final Sprite spr = this.spr = this.prepareSpr(0, false);
                spr.setRefPixelPosition(x, y);
                spr.setTransform(mirror ? Sprite.TRANS_MIRROR : Sprite.TRANS_NONE);
            }
        }
        
        public Image getImage() {
            Image img = this.img;
            if (img == null) {
                img = Image.createImage(this.width, this.height);
                this.gpc = img.getGraphics();
                this.img = img;
                final Thread thr = this.thr = new Thread(this);
                thr.start();
            }
            return this.img;
        }

        public void run() {
            final SpriteImage[] sprites = this.sprites;
            while (true) {
                for (int i = 0; i < sprites.length; i++) {
                    this.arenaStage.append(String.valueOf(i));
                    final SpriteImage sprImage = this.sprites[i];
                    if (sprImage.crt > -1) {
                        if (sprImage.frm < sprImage.spr.getFrameSequenceLength()) {                        
                            sprImage.drawFrame(this.gpc);
                        } else {
                            sprImage.setCurrent(sprImage.actor._hp < 0 ? -2 : -1);
                            sprImage.drawFrame(this.gpc);
                        }
                    }
                }
                try {
                    Thread.sleep(SceneImage.FRM_TIME);
                } catch (final InterruptedException ex) {
                    this.arenaStage.append(ex.getMessage());
                    ex.printStackTrace();
                }
            }
        }
        
        public SceneImage(final ArenaStage arenaStage, final SceneAct sceneAct, final int width, final int height) throws IOException {
            this.sceneAct = sceneAct;
            this.arenaStage = arenaStage;
            this.width = width;
            this.height = height;
            final int enIdx = sceneAct._enIdx;
            final Actor[] players = sceneAct._players;
            final int len = sceneAct._players.length;
            final SpriteImage[] sprImages = this.sprites = new SpriteImage[len];
            int i = 0;
            sprImages[i] = new SpriteImage(players[i], 50, 50, i++ >= enIdx);
            /*if (i < len) {
                sprImages[i] = new SpriteImage(players[i], 40, 40, i++ >= enIdx);
            }
            if (i < len) {
                sprImages[i] = new SpriteImage(players[i], 40, 40, i++ >= enIdx);
            }*/
        }
    }

    public ArenaStage(final String string, final Actor[] party, final Actor[] enemy, final int surprise) {
        super(string);
        final SceneAct sceneAct = new SceneAct(party, enemy, surprise);
        final int height = this.getHeight();
        final int width = this.getWidth();
        try {
            final SceneImage sceneImg = new SceneImage(this, sceneAct, 200, 200);
            this.append(sceneImg.getImage());
            sceneImg.sprites[0].setCurrent(0);
        } catch (IOException ex) {
            this.append(ex.getMessage());
            ex.printStackTrace();
        }
    }
    
}