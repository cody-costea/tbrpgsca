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
import 'package:dtbrpgsca/StateMask.dart';

abstract class RolePlay {
  String name;
  int id, mHp, mMp, mSp, mInit;
  List<StateMask> aStates;
  bool _range;

  bool get range {
    return this._range;
  }

  set range(final bool range) {
    this._range = range;
  }

  RolePlay(final int id, final String name, final int hp, final int mp, final int sp,
       final int mInit, final bool range, final List<StateMask> states) {
    this.id = id;
    this.name = name;
    this.mHp = hp;
    this.mMp = mp;
    this.mSp = sp;
    this.mInit = mInit;
    this._range = range;
    this.aStates = states;
  }

}