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

public abstract class RolePlay {

  protected String name, sprite;
  protected int id, mHp, mMp, mSp, mInit;
  protected StateMask[] aStates;
  protected boolean _range;

  public boolean getRange() {
    return this._range;
  }

  public RolePlay setRange(final boolean range) {
    this._range = range;
    return this;
  }

  RolePlay(final int id, final String name, final String sprite, final int hp, final int mp,
           final int sp, final int mInit, final boolean range, final StateMask[] states) {
    this.id = id;
    this.name = name;
    this.sprite = sprite;
    this.mHp = hp;
    this.mMp = mp;
    this.mSp = sp;
    this.mInit = mInit;
    this._range = range;
    this.aStates = states;
  }

  static String getDmgText(final int dmghp, final int dmgmp, final int dmgsp) {
    final StringBuffer s = new StringBuffer();
    boolean c = false;
    if (dmghp != 0) {
      s.append(" ");
      if (dmghp < 0) {
        s.append("+");
      }
      s.append(String.valueOf(-dmghp)).append(" HP");
      c = true;
    }
    if (dmgmp != 0) {
      if (c) {
        s.append(",");
      }
      s.append(" ");
      if (dmgmp < 0) {
        s.append("+");
      }
      s.append(String.valueOf(-dmgmp)).append(" MP");
      c = true;
    }
    if (dmgsp != 0) {
      if (c) {
        s.append(",");
      }
      s.append(" ");
      if (dmgsp < 0) {
        s.append("+");
      }
      s.append(String.valueOf(-dmgsp)).append(" RP");
    }
    return s.toString();
  }
    
}