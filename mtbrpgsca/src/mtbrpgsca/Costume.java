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

import java.util.Hashtable;

public class Costume extends RolePlay {
    
  protected int atk, def, spi, wis, agi;
  protected Performance[] aSkills;
  protected Hashtable stRes, res;
  
  public int getAttack() {
      return this.atk;
  }
  
  public Costume setAttack(final int value) {
      this.atk = value;
      return this;
  }
  
  public int getDefense() {
      return this.def;
  }
  
  public Costume setDefense(final int value) {
      this.def = value;
      return this;
  }
  
  public int getSpirit() {
      return this.spi;
  }
  
  public Costume setSpirit(final int value) {
      this.spi = value;
      return this;
  }
  
  public int getWisdom() {
      return this.wis;
  }
  
  public Costume setWisdom(final int value) {
      this.wis = value;
      return this;
  }
  
  public int getAgility() {
      return this.agi;
  }
  
  public Costume setAgility(final int value) {
      this.agi = value;
      return this;
  }
  
  public Performance[] getAddedSkills() {
      return this.aSkills;
  }
  
  public Costume setAddedSkills(final Performance[] value) {
      this.aSkills = value;
      return this;
  }
  
  public Hashtable getStateResistance() {
      return this.stRes;
  }
  
  public Costume setStateResistance(final Hashtable value) {
      this.stRes = value;
      return this;
  }
  
  public Hashtable getElementalResistance() {
      return this.res;
  }
  
  public Costume setElementalResistance(final Hashtable value) {
      this.res = value;
      return this;
  }

  public Costume(final int id, final String name, final String sprite, final int hp, final int mp, final int sp,
          final int atk, final int def, final int spi, final int wis, final int agi, final int mInit,
          final boolean range, final Hashtable res, final Performance[] skills, final StateMask[] states,
          final Hashtable stRes) {
    super(id, name, sprite, hp, mp, sp, mInit, range, states);
    this.atk = atk;
    this.def = def;
    this.spi = spi;
    this.wis = wis;
    this.agi = agi;
    this.aSkills = skills;
    this.res = res;
    this.stRes = stRes;
  }
    
}