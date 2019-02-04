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
import 'dart:async';

import 'package:dtbrpgsca/Costume.dart';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:dtbrpgsca/Actor.dart';
import 'package:dtbrpgsca/SceneAct.dart';
import 'package:dtbrpgsca/Performance.dart';

int _waitTime = 0;

class ActorSprite extends StatefulWidget {
  
  final SpriteState spriteState;
  
  @override
  State<StatefulWidget> createState() {
    return this.spriteState;
  }

  ActorSprite({Key key, this.spriteState}) : super(key: key);
}

class SpriteState extends State<ActorSprite> {

  String _name;
  String _sprite;
  String _pos;

  Actor _player;

  Map<String, List<int>> _sprTime;
  Map<String, List<String>> _sprFiles;
  Map<String, int> _sprFullTime;

  String _idleSpr;
  String _koSpr;

  int _counter = 0;

  set actor(final Actor value) {
    this._player = value;
    this.name = value.job.name.toLowerCase();
  }

  Actor get actor {
    return this._player;
  }

  set name(final String value) {
    this._name = value;
    this._idleSpr = null;
    this._koSpr = null;
    this._sprFiles = new Map();
    this._sprFullTime = new Map();
    this._sprTime = new Map();
    this._prepareSpr(this._sprite, true);

  }

  String get name {
    return this._name;
  }

  set sprite(final String value) {
    this._sprite = value;
    this._prepareSpr(value, true);
  }

  String get sprite {
    return this._sprite;
  }

  set pos(final String value) {
    this._pos = value;
    this._idleSpr = null;
    this._koSpr = null;
    this._sprFiles = new Map();
    this._sprFullTime = new Map();
    this._sprTime = new Map();
    this._prepareSpr(this._sprite, true);
  }

  String get pos {
    return this._pos;
  }

  String get idleSpr {
    String idleSpr = this._idleSpr;
    if (idleSpr == null) {
      idleSpr = 'frm_bt_${_name}_${_pos}_idle.png';
      this._idleSpr = idleSpr;
    }
    return idleSpr;
  }

  String get koSpr {
    String koSpr = this._koSpr;
    if (koSpr == null) {
      koSpr = 'frm_bt_${_name}_${_pos}_ko.png';
      this._koSpr = koSpr;
    }
    return koSpr;
  }

  Future<String> _readSprInfo(final String spr) async {
    return await rootBundle.loadString('assets/sprites/$_name/spr_${_pos}_$spr.txt');
  }
  
  void _prepareSpr(final String crSprite, final bool play) {
    if (crSprite != null && this._sprFiles[crSprite] == null) {
      this._readSprInfo(crSprite).then((sprInfo) {
        final List<String> sprites = sprInfo.split("\\");
        this._sprTime[crSprite] = new List();
        this._sprFiles[crSprite] = new List();
        int sprFullTime = 0;
        int sprTime;
        for (int i = 0; i < sprites.length; i++) {
          final List<String> sprLine = sprites[i].split(":");
          this._sprFiles[crSprite].add(sprLine[0]);
          try {
            sprTime = sprLine.length > 1 ? int.parse(sprLine[1]) : 87;
          } catch (_) {
            sprTime = 87;
          }
          this._sprTime[crSprite].add(sprTime);
          sprFullTime += sprTime;
        }
        this._sprFullTime[crSprite] = sprFullTime;
        if (play) {
          this._playSpr();
        }
      });
    } else if (play) {
      this._playSpr();
    }
  }

  void _playSpr() {
    final int sprFullTime = this._sprFullTime[this._sprite];
    if (sprFullTime > _waitTime) {
      _waitTime = sprFullTime;
    }
    this.setState(() {
      this._counter = 0;
    });
  }

  @override
  Widget build(final BuildContext context) {
    List<String> crSprList;
    final int counter = this._counter;
    final String crSprite = this._sprite;
    if (crSprite != null && (crSprList = this._sprFiles[crSprite]) != null && counter < crSprList.length) {
      new Timer(Duration(milliseconds: this._sprTime[crSprite][counter]), () {
        this.setState(() {
          this._counter++;
        });
      });
    }
    return Image (
        //fit: BoxFit.fill,
        image: AssetImage('assets/sprites/$_name/${crSprList == null || counter >= crSprList.length
            ? (this.actor == null || this.actor.hp > 0 ? this.idleSpr : this.koSpr)
            : crSprList[counter]}'),
        gaplessPlayback: true,
        width: 128,
        height: 128
    );
  }

  @override
  void initState() {
    super.initState();

  }

  SpriteState(final Actor actor, final String pos, final bool aot) {
    if (actor == null || pos == null) {
      throw Exception("Null parameters cannot be accepted.");
    }
    this._pos = pos;
    this._player = actor;
    this._name = actor.job.name.toLowerCase();
    this._sprFiles = new Map();
    this._sprFullTime = new Map();
    this._sprTime = new Map();
    if (aot) {
      this._prepareSpr("hit", false);
      this._prepareSpr("fallen", false);
      this._prepareSpr("risen", false);
      this._prepareSpr("act", false);
      this._prepareSpr("cast", false);
    }
  }
}

class ArenaStage extends StatefulWidget {

  final List<Actor> party;
  final List<Actor> enemy;
  final int surprise;

  @override
  State<StatefulWidget> createState() {
    return new ArenaState(this.party, this.enemy, this.surprise);
  }

  ArenaStage({Key key, this.party, this.enemy, this.surprise}) : super(key: key);
}

class ArenaState extends State<ArenaStage> {

  SceneAct _sceneAct;

  List<SpriteState> _actorSprites;

  List<Actor> _koActors;

  Performance _crSkill;
  Performance _crItem;

  List<Performance> _crSkills;
  List<Performance> _crItems;

  bool _automatic = false;
  bool _activeBtn = true;
  bool get activeBtn {
    return this._activeBtn;
  }
  set activeBtn(final bool active) {
    this.setState(() {
      this._activeBtn = active;
    });
  }

  @override
  Widget build(final BuildContext context) {
    return new MaterialApp(
        title: 'Turn-Based RPG Simple Combat Arena',
        theme: ThemeData.dark(),
        home: Scaffold(
            body: Container(
                height: double.infinity,
                width: double.infinity,
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.stretch,
                  mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                  children: <Widget>[
                    Row(
                      children: <Widget>[
                        Expanded(
                          child: MaterialButton(
                            onPressed: this._activeBtn ? () {
                              this._automatic = true;
                              this._execAI();
                              //this._actorSprites[0].sprite = 'cast';
                            } : () {
                              this._automatic = false;
                            },
                            child: Text('Auto'),
                          )
                        ),
                        Expanded(
                          child: MaterialButton(
                            onPressed: null, //TODO
                            child: Text('Run')
                          )
                        )
                      ],
                    ),
                    Expanded(
                      child: Center(
                        child: Stack(
                          children: <Widget>[
                            Align(
                                alignment: Alignment(-0.3, -0.7),
                                child: ActorSprite(spriteState: this._actorSprites[0])
                            ),
                            Align(
                                alignment: Alignment(-0.7, -1),
                                child: ActorSprite(spriteState: this._actorSprites[1])
                            ),
                            Align(
                                alignment: Alignment(-0.9, -0.5),
                                child: ActorSprite(spriteState: this._actorSprites[2])
                            ),
                            Align(
                                alignment: Alignment(-0.5, -0.2),
                                child: ActorSprite(spriteState: this._actorSprites[3])
                            ),
                            Align(
                                alignment: Alignment(0.9, -0.7),
                                child: ActorSprite(spriteState: this._actorSprites[6])
                            ),
                            Align(
                                alignment: Alignment(0.5, -1),
                                child: ActorSprite(spriteState: this._actorSprites[7])
                            ),
                            Align(
                                alignment: Alignment(0.3, -0.5),
                                child: ActorSprite(spriteState: this._actorSprites[4])
                            ),
                            Align(
                                alignment: Alignment(0.7, -0.2),
                                child: ActorSprite(spriteState: this._actorSprites[5])
                            ),
                            Align(
                                alignment: Alignment(0, 0.1),
                                child: SingleChildScrollView(
                                  scrollDirection: Axis.vertical,
                                  child: Text("Test!!!"),
                                )
                            )
                          ],
                        ),
                      )
                    ),
                    Row(
                      children: <Widget>[
                        Expanded(
                          child: MaterialButton(
                            onPressed: this._activeBtn ? () {
                              this._execSkill(this._crSkill);
                            } : null,
                            child: Text('Execute'),
                          )
                        ),
                        Expanded(
                          child: DropdownButton(
                            items: this._crSkillsOptions,
                            onChanged: ((final Performance value) {
                                this.setState(() {
                                  this._crSkill = value;
                                });
                              }
                            ),
                            value: this._crSkill
                          )
                        )
                      ]
                    ),
                    Row(
                      children: <Widget>[
                        Expanded(
                          child: MaterialButton(
                            onPressed: this._activeBtn ? () {
                              this._sceneAct.executeAbility(this._sceneAct.players[this._sceneAct.current].availableSkills[2], this._sceneAct.enemyIndex, "");
                              this._afterAct();
                            } : null,
                            child: Text('Use'),
                          )
                        )
                      ],
                    )
                  ],
                )
            )
        )
    );
  }

  List<DropdownMenuItem<Performance>> get _crSkillsOptions {
    final List<Performance> crSkills = this._crSkills;
    final List<DropdownMenuItem<Performance>> ret = new List();
    if (crSkills != null) {
      for (Performance ability in crSkills) {
        if (ability.canPerform(this._sceneAct.players[this._sceneAct.current])) {
          ret.add(new DropdownMenuItem(
              value: ability,
              child: Text(ability.name)
          )
          );
        }
      }
    }
    return ret;
  }

  void _execAI() {
    this._sceneAct.executeAI("");
    this._afterAct();
  }

  void _execSkill(final Performance ability) {
    this._sceneAct.executeAbility(ability, this._sceneAct.enemyIndex, "");
    this._afterAct();
  }

  void _afterAct() {
    this.activeBtn = false;
    final List<Actor> koActors = this._koActors;
    int crt = this._sceneAct.current;
    Performance lastAbility;
    this._actorSprites[crt].sprite = this._sceneAct.players[crt].hp > 0
        ? (((lastAbility = this._sceneAct.lastAbility).dmgType & Performance.DmgTypeSpi == Performance.DmgTypeSpi
            || lastAbility.dmgType == Performance.DmgTypeWis) ? 'cast' : 'act') : 'fallen';
    for (int trg = this._sceneAct.firstTarget; trg <= this._sceneAct.lastTarget; trg++) {
      if (trg != crt) {
        final SpriteState trgSprite = this._actorSprites[trg];
        final Actor trgActor = trgSprite.actor;
        final bool ko = koActors.contains(trgActor);
        if (trgActor.hp > 0) {
          if (ko) {
            trgSprite.sprite = 'risen';
            koActors.remove(trgActor);
          } else {
            trgSprite.sprite = 'hit';
          }
        } else {
          trgSprite.sprite = 'fallen';
          if (!ko) {
            koActors.add(trgActor);
          }
        }
      }
    }
    new Timer(Duration(milliseconds: _waitTime), () {
      _waitTime = 0;
      this._sceneAct.endTurn("");
      final List<Actor> players = this._sceneAct.players;
      if (players[crt].hp < 1) {
        koActors.add(players[crt]);
        this._actorSprites[crt].sprite = 'fallen';
        //TODO: text
      }
      if (this._sceneAct.status != 0) {
        //TODO:
      } else if (this._automatic || players[(crt = this._sceneAct.current)].automatic != 0) {
        this._execAI();
      } else {
        this.activeBtn = true;
        this.setState(() {
          this._crSkills = players[crt].availableSkills;
          this._crItems = this._sceneAct.crItems[crt];
        });
      }
    });
  }

  ArenaState(final List<Actor> party, final List<Actor> enemy, final int surprise) {
    for (int i = 0; i < enemy.length; i++) {
      enemy[i].automatic = 2;
    }
    final SceneAct sceneAct = this._sceneAct = SceneAct(party, enemy, surprise);
    final List<Actor> players = sceneAct.players;
    final int current = sceneAct.current;
    this._crSkills = players[current].availableSkills;
    //this._crItems = sceneAct.crItems[current];
    this._crSkill = this._crSkills[0];
    this._koActors = new List()..length = players.length;
    this._actorSprites = surprise < 0 ? [
        SpriteState(players[4], "l", true),
        SpriteState(players[5], "l", true),
        SpriteState(players[6], "l", true),
        SpriteState(players[7], "l", true),
        SpriteState(players[0], "r", true),
        SpriteState(players[1], "r", true),
        SpriteState(players[2], "r", true),
        SpriteState(players[3], "r", true)
      ]
        : [
      SpriteState(players[0], "l", true),
      SpriteState(players[1], "l", true),
      SpriteState(players[2], "l", true),
      SpriteState(players[3], "l", true),
      SpriteState(players[4], "r", true),
      SpriteState(players[5], "r", true),
      SpriteState(players[6], "r", true),
      SpriteState(players[7], "r", true)
    ];
  }

}