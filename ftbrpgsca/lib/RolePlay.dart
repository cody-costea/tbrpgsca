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
import 'package:flutter/material.dart';
import 'package:ftbrpgsca/StateMask.dart';

abstract class RolePlay {

  static const int FLAG_RANGE = 1;

  static String hpText = "HP";
  static String mpText = "MP";
  static String spText = "RP";

  String name, sprite;
  int id, mHp, mMp, mSp, mInit;
  @protected int flags = 0;
  List<StateMask> aStates;

  bool get range {
    return (this.flags & FLAG_RANGE) == FLAG_RANGE;
  }

  set range(final bool range) {
    int flags = this.flags;
    if (range != ((flags & FLAG_RANGE) == FLAG_RANGE)) {
      flags ^= FLAG_RANGE;
      this.flags = flags;
    }
  }

  RolePlay(final int id, final String name, final String sprite, final int hp, final int mp,
      final int sp, final int mInit, final bool range, final List<StateMask> states) {
    this.id = id;
    this.name = name;
    this.sprite = sprite;
    this.mHp = hp;
    this.mMp = mp;
    this.mSp = sp;
    this.mInit = mInit;
    this.range = range;
    this.aStates = states;
  }

  static String getDmgText(final int dmghp, final int dmgmp, final int dmgsp) {
    final StringBuffer s = new StringBuffer();
    bool c = false;
    if (dmghp != 0) {
      s.write(" ");
      if (dmghp < 0) {
        s.write("+");
      }
      s.write("${-dmghp} ${RolePlay.hpText}");
      c = true;
    }
    if (dmgmp != 0) {
      if (c) {
        s.write(",");
      }
      s.write(" ");
      if (dmgmp < 0) {
        s.write("+");
      }
      s.write("${-dmgmp} ${RolePlay.mpText}");
      c = true;
    }
    if (dmgsp != 0) {
      if (c) {
        s.write(",");
      }
      s.write(" ");
      if (dmgsp < 0) {
        s.write("+");
      }
      s.write("${-dmgsp} ${RolePlay.spText}");
    }
    return s.toString();
  }

}