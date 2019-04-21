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
  protected Hashtable stRes;
  protected Hashtable res;

  Costume(final int id, final String name, final String sprite, final int hp, final int mp, final int sp,
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