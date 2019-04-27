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

import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Random;

public final class Performance extends RolePlay {
    
  public static final int DmgTypeAtk = 1;
  public static final int DmgTypeDef = 2;
  public static final int DmgTypeSpi = 4;
  public static final int DmgTypeWis = 8;
  public static final int DmgTypeAgi = 16;

  protected String sound;
  protected boolean steal, absorb, restore;
  protected int lvRq, hpC, mpC, spC, mQty, rQty, dmgType, trg;
  protected Integer elm;
  protected StateMask[] rStates;

  public String execute(final Actor user, Actor target, final boolean applyCosts) {
    String s = "";
    final Random rnd = new Random();
    int dmg = rnd.nextInt(4);
    if (target.reflects && ((this.dmgType & DmgTypeWis) == DmgTypeWis)) {
      s += ", reflected by " + target.name;
      target = user;
    }
    Object o;
    final boolean ko = target._hp < 1;
    final Hashtable trgResMap = target.res;
    int res = trgResMap == null ? 3 : ((o = trgResMap.get(this.elm)) == null ? 3 : ((Integer)o).intValue());
    if (res > 6) {
      res = -1;
    }
    int i = 0;
    int def = 0;
    boolean canMiss = false;
    if ((this.dmgType & DmgTypeAtk) == DmgTypeAtk) {
      canMiss = true;
      dmg += user.atk;
      def += target.def;
      i++;
    }
    if ((this.dmgType & DmgTypeDef) == DmgTypeDef) {
      dmg += user.def;
      def += target.def;
      i++;
    }
    if ((this.dmgType & DmgTypeSpi) == DmgTypeSpi) {
      dmg += user.spi;
      def += target.wis;
      i++;
    }
    if ((this.dmgType & DmgTypeWis) == DmgTypeWis) {
      dmg += user.wis;
      def += target.spi;
      i++;
    }
    if ((this.dmgType & DmgTypeAgi) == DmgTypeAgi) {
      if (!canMiss) {
        canMiss = true;
      }
      dmg += user.agi;
      def += target.agi;
      i++;
    }
    if (i == 0) {
      i = 1;
    }
    dmg = (this.mInit + (dmg / i)) / (def / i * res + 1);
    if (!canMiss || ((rnd.nextInt(13) + user.agi / 4)) > 2 + target.agi / 4) {
      int hpDmg = this.mHp == 0 ? 0 : ((this.mHp < 0 ? -1 : 1) * dmg + this.mHp);
      int mpDmg = this.mMp == 0 ? 0 : ((this.mMp < 0 ? -1 : 1) * dmg + this.mMp);
      int spDmg = this.mSp == 0 ? 0 : ((this.mSp < 0 ? -1 : 1) * dmg + this.mSp);
      if (res < 0) {
        hpDmg = -hpDmg;
        mpDmg = -mpDmg;
        spDmg = -spDmg;
      }
      target._hp -= hpDmg;
      target._mp -= mpDmg;
      target._sp -= spDmg;
      if (this.absorb) {
        user._hp += hpDmg / 2;
        user._mp += mpDmg / 2;
        user._sp += spDmg / 2;
      }
      if (hpDmg != 0 || mpDmg != 0 || spDmg != 0) {
        s += ", " + target.name + " suffers" + RolePlay.getDmgText(hpDmg, mpDmg, spDmg);
      }
      String r;
      final StateMask[] aStates = this.aStates;
      if (aStates != null) {
        for (int j = 0; j < aStates.length; j++) {
          r = aStates[j].inflict(target, false, false);
          if (r.length() > 0) {
            s += r;
          }
        }
      }
      final Hashtable trgStateMap = target.stateDur;
      if (trgStateMap != null) {
        final StateMask[] rStates = this.rStates;
        if (rStates != null) {
          for (int j = 0; j < rStates.length; j++) {
            final Enumeration trgStates = trgStateMap.keys();
            while (trgStates.hasMoreElements()) {
              final StateMask state = (StateMask)trgStates.nextElement();
              if (rStates[j].equals(state)) {
                state.remove(target, false, false);
              }
            }
          }
        }
      }
      final Hashtable trgItemMap = target._items;
      if (this.steal && trgItemMap != null && trgItemMap != user._items && trgItemMap.size() > 0
          && (rnd.nextInt(12) + user.agi / 4) > 4 + target.agi / 3) {
        final int rndItem = rnd.nextInt(trgItemMap.size() - 1);
        final Enumeration itemsEnum = trgItemMap.keys();
        Performance stolen = null;
        for (int j = 0; j <= rndItem && itemsEnum.hasMoreElements(); j++) {
            if (j == rndItem) {
                stolen = (Performance)itemsEnum.nextElement();
            } else {
                itemsEnum.nextElement();
            }
        }
        int trgItemQty;
        final Object itemQtyObj = trgItemMap.get(stolen);
        if (itemQtyObj != null && ((trgItemQty = ((Integer)itemQtyObj).intValue()) > 0)) {
          Hashtable usrItems = user._items;
          if (usrItems == null) {
            usrItems = new Hashtable();
            user._items = usrItems;
          }
          final Object usrItem;
          usrItems.put(stolen, new Integer((usrItem = usrItems.get(stolen)) == null ? 0 : ((Integer)usrItem).intValue() + 1));
          trgItemQty--;
          if (trgItemQty == 0) {
            trgItemMap.remove(stolen);              
          } else {
            trgItemMap.put(stolen, new Integer(trgItemQty));
          }
          s += ", obtaining " + stolen.name + " from " + target.name;
        }
      }
      s += target.checkStatus();
    }
    else {
      s += ", but misses";
    }
    if (applyCosts) {
      user._hp -= this.hpC;
      user._mp -= this.mpC;
      user._sp -= this.spC;
      if (this.mQty > 0) {
        Hashtable usrSkillsQty = user.skillsQty;
        if (usrSkillsQty == null) {
          usrSkillsQty = new Hashtable();
          user.skillsQty = usrSkillsQty;
        }
        final Object usrItemQty;
        usrSkillsQty.put(this, new Integer(((usrItemQty = usrSkillsQty.get(this)) == null ? this.mQty : ((Integer)usrItemQty).intValue()) - 1));
      }
    }
    if (ko && target._hp > 0) {
      target.applyStates(false);
    }
    return s + user.checkStatus();
  }

  void replenish(final Actor user) {
    if (this.mQty > 0) {
      Hashtable usrSkills = user.skillsQty;
      if (usrSkills == null) {
        usrSkills = new Hashtable();
        user.skillsQty = usrSkills;
      }
      usrSkills.put(this, new Integer(this.mQty));
    }
  }

  boolean canPerform(final Actor actor) {
    final Object itemQty;
    final Hashtable skillsQty = actor.skillsQty;
    return this.mpC <= actor._mp && this.hpC < actor._hp && this.spC <= actor._sp && actor._lv >= this.lvRq
        && (skillsQty == null || ((itemQty = skillsQty.get(this)) == null ? 1 : ((Integer)itemQty).intValue()) > 0);
  }

  Performance(final int id, final String name, final String sprite, final String sound, final boolean steal,
              final boolean range, final int lvRq, final int hpC, final int mpC, final int spC, final int dmgType,
              final int atkI, final int hpDmg, final int mpDmg, final int spDmg, final int trg, final int elm, //TODO: int elm to integer
              final int mQty, final int rQty, final boolean absorb, final boolean restoreKO, final StateMask[] aStates,
              final StateMask[] rStates) {
    super(id, name, sprite, hpDmg, mpDmg, spDmg, atkI, range, aStates);
    this.sound = sound;
    this.steal = steal;
    this.lvRq = lvRq;
    this.hpC = hpC;
    this.mpC = mpC;
    this.spC = spC;
    this.dmgType = dmgType;
    this.trg = trg;
    this.elm = new Integer(elm);
    this.mQty = mQty;
    this.rQty = rQty;
    this.absorb = absorb;
    this.restore = restoreKO;
    this.rStates = rStates;
  }
    
}