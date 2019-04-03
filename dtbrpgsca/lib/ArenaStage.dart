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
import 'dart:typed_data';

import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:dtbrpgsca/Actor.dart';
import 'package:dtbrpgsca/SceneAct.dart';
import 'package:dtbrpgsca/Performance.dart';

const int FRAME_TIME = 87;

const int SPR_HIT = 0;
const int SPR_FALLEN = 1;
const int SPR_RISEN = 2;
const int SPR_ACT = 3;
const int SPR_CAST = 4;

List<Actor> _koActors;

Map<String, List<int>> _skillSprTime = new Map();
Map<String, List<String>> _skillSprFiles = new Map();
Map<String, int> _skillSprFullTime = new Map();

final MemoryImage emptyImage = new MemoryImage(new Uint8List(0));

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
  String _pos;
  int _sprite;

  Actor _player;

  List<List<int>> _sprTime;
  List<List<String>> _sprFiles;
  List<int> _sprFullTime;

  String _skillName;

  String _idleSpr;
  String _koSpr;

  int _counter = 0;
  int _skillCnt = 0;

  Function _onClick;

  set actor(final Actor value) {
    this._player = value;
    this.name = value.job.sprite;
  }

  Actor get actor {
    return this._player;
  }

  set name(final String value) {
    this._name = value;
    this._idleSpr = null;
    this._koSpr = null;
    this._sprFiles = new List(5);
    this._sprFullTime = new List(5);
    this._sprTime = new List(5);
    this._prepareSpr(this._sprite, true);
  }

  String get name {
    return this._name;
  }

  set sprite(final int value) {
    this._sprite = value;
    this._prepareSpr(value, true);
  }

  int get sprite {
    return this._sprite;
  }

  set pos(final String value) {
    this._pos = value;
    this._idleSpr = null;
    this._koSpr = null;
    this._sprFiles = new List(5);
    this._sprFullTime = new List(5);
    this._sprTime = new List(5);
    this._prepareSpr(this._sprite, true);
  }

  String get pos {
    return this._pos;
  }

  String get skillSprite {
    return this._skillName;
  }

  set skillSprite(final String value) {
    this._skillName = value;
    this._skillCnt = 0;
    this._prepareSkillSpr(value, false);
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

  Future<String> _readSprInfo(final int spr) async {
    return await rootBundle.loadString('assets/sprites/$_name/spr_${_pos}_$spr.txt');
  }

  Future<String> _readSkillSprInfo(final String spr) async {
    return await rootBundle.loadString('assets/sprites/skills/$spr/spr.txt');
  }

  void _prepareSpr(final int crSprite, final bool play) {
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
            sprTime = sprLine.length > 1 ? int.parse(sprLine[1]) : FRAME_TIME;
            if (sprTime < 10) {
              sprTime *= FRAME_TIME;
            }
          } catch (_) {
            sprTime = FRAME_TIME;
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

  void _prepareSkillSpr(final String crSkillSpr, final bool play) {
    if (crSkillSpr != null && _skillSprFiles[crSkillSpr] == null) {
      this._readSkillSprInfo(crSkillSpr).then((sprInfo) {
        final List<String> sprites = sprInfo.split("\\");
        _skillSprTime[crSkillSpr] = new List();
        _skillSprFiles[crSkillSpr] = new List();
        int sprFullTime = 0;
        int sprTime;
        for (int i = 0; i < sprites.length; i++) {
          final List<String> sprLine = sprites[i].split(":");
          _skillSprFiles[crSkillSpr].add(sprLine[0]);
          try {
            sprTime = sprLine.length > 1 ? int.parse(sprLine[1]) : FRAME_TIME;
          } catch (_) {
            sprTime = FRAME_TIME;
          }
          _skillSprTime[crSkillSpr].add(sprTime);
          sprFullTime += sprTime;
        }
        _skillSprFullTime[crSkillSpr] = sprFullTime;
        if (play) {
          this._playSpr();
        }
      });
    } else if (play) {
      this._playSpr();
    }
  }

  void _playSpr() {
    final int actorSpr = this._sprite;
    final String skillSpr = this._skillName;
    final int actorWait = actorSpr == null ? 0 : this._sprFullTime[actorSpr];
    final int skillWait = skillSpr == null ? 0 : _skillSprFullTime[skillSpr];
    final int sprFullTime = actorWait > skillWait ? actorWait : skillWait;
    if (sprFullTime > _waitTime) {
      _waitTime = sprFullTime;
    }
    this.setState(() {
      this._counter = 0;
      //this._skillCnt = 0;
      // this._skillCnt++;
    });
  }

  @override
  Widget build(final BuildContext context) {
    final int actorCnt = this._counter;
    final int skillCnt = this._skillCnt;
    final int crSprite = this._sprite;
    final String crSkillSpr = this._skillName;
    final List<String> crSprList = crSprite == null ? null : this._sprFiles[crSprite];
    final List<String> crSkSprList = crSkillSpr == null ? null : _skillSprFiles[crSkillSpr];
    final int actorWait = crSprList != null && actorCnt < crSprList.length ? this._sprTime[crSprite][actorCnt] : 0;
    final int skillWait = crSkSprList != null && skillCnt < crSkSprList.length ? _skillSprTime[crSkillSpr][skillCnt] : 0;
    final int waitTime = actorWait > skillWait ? actorWait : skillWait;
    if (waitTime > 0) {
      new Timer(Duration(milliseconds: waitTime), () {
        this.setState(() {
          if (actorWait > 0) {
            this._counter++;
          }
          if (skillWait > 0) {
            this._skillCnt++;
          }
        });
      });
    }
    final List<Actor> koActors = _koActors;
    return GestureDetector(
      onTap: this._onClick,
      child: Stack(
        children: <Widget>[
          Image(
              image: AssetImage('assets/sprites/$_name/${actorWait == 0
                  ? (koActors == null || this.actor == null || !koActors.contains(this.actor) ? this.idleSpr : this.koSpr)
                  : crSprList[actorCnt]}'),
              gaplessPlayback: true,
              width: 128,
              height: 128
          ),
          Image(
            image: skillWait == 0 ? emptyImage
                : AssetImage('assets/sprites/skills/$_skillName/${crSkSprList[skillCnt]}'),
            gaplessPlayback: true,
            width: 128,
            height: 128
          )
        ],
      )
    );
  }

  @override
  void initState() {
    super.initState();

  }

  SpriteState(final Actor actor, final String pos, final bool aot, final Function onClick) {
    if (actor == null || pos == null) {
      throw Exception("Null parameters cannot be accepted.");
    }
    this._pos = pos;
    this._player = actor;
    this._name = actor.job.sprite;
    this._sprFiles = new List(5);
    this._sprFullTime = new List(5);
    this._sprTime = new List(5);
    if (aot) {
      this._prepareSpr(SPR_HIT, false);
      this._prepareSpr(SPR_FALLEN, false);
      this._prepareSpr(SPR_RISEN, false);
      this._prepareSpr(SPR_ACT, false);
      this._prepareSpr(SPR_CAST, false);
    }
    this._onClick = onClick;
  }
}

class ArenaStage extends StatefulWidget {

  final List<Actor> party;
  final List<Actor> enemy;
  final String arenaImg;
  final String arenaSnd;
  final int surprise;


  @override
  State<StatefulWidget> createState() {
    return new ArenaState(this.party, this.enemy, this.arenaImg, this.arenaSnd, this.surprise);
  }

  ArenaStage({Key key, this.party, this.enemy, this.arenaImg, this.arenaSnd, this.surprise}) : super(key: key);
}

class ArenaState extends State<ArenaStage> {

  SceneAct _sceneAct;

  List<SpriteState> _actorSprites;

  Actor _crActor;
  Performance _crSkill;
  Performance _crItem;

  String _arenaImg;
  String _arenaSnd;

  List<DropdownMenuItem<Performance>> _crSkills;
  List<DropdownMenuItem<Performance>> _crItems;
  List<DropdownMenuItem<int>> _players;

  TextEditingController _actionsTxt = new TextEditingController();
  ScrollController _scrollController = new ScrollController();

  int _target;

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

  List<DropdownMenuItem<Performance>> _emptyAbilities;
  List<DropdownMenuItem<Performance>> get emptyAbilities {
    List<DropdownMenuItem<Performance>> emptyAbilities = this._emptyAbilities;
    if (emptyAbilities == null) {
      emptyAbilities = new List();
      emptyAbilities.add(new DropdownMenuItem(
        child: new Text(''),
      ));
      this._emptyAbilities = emptyAbilities;
    }
    return emptyAbilities;
  }

  @override
  Widget build(final BuildContext context) {
    return new MaterialApp(
        title: 'Turn-Based RPG Simple Combat Arena',
        theme: ThemeData.dark(),
        debugShowCheckedModeBanner: false,
        home: Scaffold(
            body: Container(
                height: double.infinity,
                width: double.infinity,
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.stretch,
                  mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                  children: <Widget>[
                    Text(this._crActor.automatic == 0 ? "   ${this._crActor.name}: "
                      "${this._crActor.hp}/${this._crActor.mHp} HP, "
                      "${this._crActor.mp}/${this._crActor.mMp} MP, "
                      "${this._crActor.sp}/${this._crActor.mSp} RP"
                    : ""),
                    Expanded(
                        child: Center(
                          child: Stack(
                            children: <Widget>[
                              Align(
                                alignment: Alignment(-1, -1),
                                child: FractionallySizedBox(
                                  widthFactor: 1,
                                  heightFactor: 0.69,
                                  child: Image(
                                    image: this._arenaImg == null || this._arenaImg.length == 0 ? emptyImage
                                        : AssetImage('assets/sprites/arena/$_arenaImg}'),
                                    gaplessPlayback: true,
                                    fit: BoxFit.fill,
                                  )
                                )
                              ),
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
                                alignment: Alignment(0, 1),
                                child: FractionallySizedBox(
                                  widthFactor: 1,
                                  heightFactor: 0.31,
                                  child: SingleChildScrollView(
                                    controller: this._scrollController,
                                    scrollDirection: Axis.vertical,
                                    child: TextField(
                                      decoration: InputDecoration(
                                        border: InputBorder.none
                                      ),
                                      controller: this._actionsTxt,
                                      textAlign: TextAlign.start,
                                      maxLines: null,
                                      enabled: false
                                    )
                                  ),
                                ),
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
                                  items: this._crSkills,
                                  onChanged: ((final Performance value) {
                                    this.setState(() {
                                      this._crSkill = value;
                                    });
                                  }),
                                  value: this._crSkill
                              )
                          )
                        ]
                    ),
                    Row(
                      children: <Widget>[
                        Expanded(
                            child: DropdownButton(
                                items: this._players,
                                onChanged: ((final int value) {
                                  this.setState(() {
                                    this._target = value;
                                  });
                                }),
                                value: this._target
                            )
                        ),
                        Expanded(
                            child: Row(
                              children: <Widget>[
                                Expanded(
                                    child: MaterialButton(
                                      onPressed: this._activeBtn ? () {
                                        this._automatic = true;
                                        this._execAI();
                                      } : () {
                                        this._automatic = false;
                                      },
                                      child: Text(this._automatic ? 'Manual' : 'Auto'),
                                    )
                                ),
                                Expanded(
                                    child: MaterialButton(
                                        onPressed: null, //TODO
                                        child: Text('Run')
                                    )
                                )
                              ],
                            )
                        )
                      ],
                    ),
                    Row(
                        children: <Widget>[
                          Expanded(
                              child: MaterialButton(
                                onPressed: this._activeBtn && this._crItem != null ? () {
                                  this._execSkill(this._crItem);
                                } : null,
                                child: Text('Use'),
                              )
                          ),
                          Expanded(
                              child: DropdownButton(
                                items: this._crItems,
                                onChanged: ((final Performance value) {
                                  this.setState(() {
                                    this._crItem = value;
                                  });
                                }),
                                value: this._crItem
                              )
                          )
                        ]
                    ),
                  ],
                )
            )
        )
    );
  }

  void _execAI() {
    this._afterAct(this._sceneAct.executeAI(""));
  }

  void _execSkill(final Performance ability) {
    if (ability != null) {
      this._afterAct(this._sceneAct.executeAbility(ability, this._target, ""));
    }
  }

  List<DropdownMenuItem<int>> _preparePlayers(final List<Actor> players) {
    final List<DropdownMenuItem<int>> ret = new List();
    if (players != null) {
      for (int i = 0; i < players.length; i++) {
        ret.add(new DropdownMenuItem(
            value: i,
            child: Text(" ${players[i].name}")
          )
        );
      }
    }
    return ret;
  }

  List<DropdownMenuItem<Performance>> _prepareAbilities(final Iterable<Performance> abilities, final int crt) {
    final List<DropdownMenuItem<Performance>> ret = new List();
    if (abilities != null) {
      for (Performance ability in abilities) {
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

  void _afterAct(String ret) {
    final List<DropdownMenuItem<Performance>> emptyAbilities = this.emptyAbilities;
    this.setState(() {
      this._activeBtn = false;
      this._crSkills = emptyAbilities;
      this._crItems = emptyAbilities;
      this._crSkill = emptyAbilities[0].value;
      this._crItem = emptyAbilities[0].value;
    });
    Performance lastAbility;
    int crt = this._sceneAct.current;
    final List<Actor> koActors = _koActors;
    final List<Actor> players = this._sceneAct.players;
    final Actor crActor = players[crt];
    final Actor trgActor = players[this._sceneAct.firstTarget];
    if (crActor.automatic != 0 && trgActor.automatic == 0) {
      this.setState(() {
        this._crActor = trgActor;
      });
    }
    this._actorSprites[crt].sprite = crActor.hp > 0
        ? (((lastAbility = this._sceneAct.lastAbility).dmgType & Performance.DmgTypeSpi == Performance.DmgTypeSpi
        || lastAbility.dmgType == Performance.DmgTypeWis) ? SPR_CAST : SPR_ACT) : SPR_FALLEN;
    new Timer(Duration(milliseconds: FRAME_TIME * 5), () {
      for (int trg = this._sceneAct.firstTarget; trg <= this._sceneAct.lastTarget; trg++) {
        if (trg != crt) {
          final SpriteState trgSprite = this._actorSprites[trg];
          final Actor trgActor = trgSprite.actor;
          final bool ko = koActors.contains(trgActor);
          trgSprite.skillSprite = lastAbility.sprite;
          if (trgActor.hp > 0) {
            if (ko) {
              trgSprite.sprite = SPR_RISEN;
              koActors.remove(trgActor);
            } else {
              trgSprite.sprite = SPR_HIT;
            }
          } else {
            trgSprite.sprite = SPR_FALLEN;
            if (!ko) {
              koActors.add(trgActor);
            }
          }
        }
      }
      new Timer(Duration(milliseconds: _waitTime), () {
        _waitTime = 0;
        ret = this._sceneAct.setNext(ret, true);
        //final List<Actor> players = this._sceneAct.players;
        if (players[crt].hp < 1) {
          koActors.add(players[crt]);
          this._actorSprites[crt].sprite = SPR_FALLEN;
        }
        this._actionsTxt.text = "$ret${this._actionsTxt.text}";
        if (this._sceneAct.status != 0) {
          //TODO:
        } else {
          if (this._automatic || players[(crt = this._sceneAct.current)].automatic != 0) {
            this._execAI();
          } else {
            this.activeBtn = true;
            this.setState(() {
              this._crActor = players[crt];
              final List<Performance> skills = players[crt].availableSkills;
              this._crSkills = this._prepareAbilities(players[crt].availableSkills, crt);
              this._crSkill = skills[0];
              final Map<int, List<Performance>> items = this._sceneAct.crItems;
              this._crItems = (items == null || items[crt] == null || items[crt].length == 0)
                  ? this.emptyAbilities : this._prepareAbilities(this._sceneAct.crItems[crt], crt);
              this._setCrAutoSkill();
            });
          }
        }
      });
    });
  }

  void _setCrAutoSkill() {
    final int target = this._target;
    final SceneAct sceneAct = this._sceneAct;
    final bool onPartySide = target < sceneAct.enemyIndex;
    this._crSkill = this._crActor.availableSkills[sceneAct.getAIskill(onPartySide ? 1 : 0,
        onPartySide && sceneAct.players[target].hp < 1)];
  }

  Function _getOnClick(final int trgIndex) {
    return () {
      if (this._target == trgIndex) {
        if (this._activeBtn) {
          this._execSkill(this._crSkill);
        }
      } else {
        this.setState(() {
          this._target = trgIndex;
          if (this._activeBtn) {
            this._setCrAutoSkill();
          }
        });
      }
    };
  }

  ArenaState(final List<Actor> party, final List<Actor> enemy, this._arenaImg, this._arenaSnd, final int surprise) {
    for (int i = 0; i < enemy.length; i++) {
      enemy[i].automatic = 2;
    }
    final SceneAct sceneAct = this._sceneAct = SceneAct(party, enemy, surprise);
    final List<Actor> players = sceneAct.players;
    final int current = sceneAct.current;
    final Actor crActor = this._crActor = players[current];
    this._crSkills = this._prepareAbilities(players[current].availableSkills, current);
    final Map<int, List<Performance>> items = this._sceneAct.crItems;
    this._crItems = (items == null || items[current] == null || items[current].length == 0)
        ? this.emptyAbilities : this._prepareAbilities(this._sceneAct.crItems[current], current);
    this._crSkill = this._crSkills[0].value;
    this._target = this._sceneAct.enemyIndex;
    this._players = this._preparePlayers(players);
    _koActors = new List()..length = players.length;
    this._actorSprites = surprise < 0 ? [
      SpriteState(players[4], "l", true, this._getOnClick(4)),
      SpriteState(players[5], "l", true, this._getOnClick(5)),
      SpriteState(players[6], "l", true, this._getOnClick(6)),
      SpriteState(players[7], "l", true, this._getOnClick(7)),
      SpriteState(players[0], "r", true, this._getOnClick(0)),
      SpriteState(players[1], "r", true, this._getOnClick(1)),
      SpriteState(players[2], "r", true, this._getOnClick(2)),
      SpriteState(players[3], "r", true, this._getOnClick(3))
    ] : [
      SpriteState(players[0], "l", true, this._getOnClick(0)),
      SpriteState(players[1], "l", true, this._getOnClick(1)),
      SpriteState(players[2], "l", true, this._getOnClick(2)),
      SpriteState(players[3], "l", true, this._getOnClick(3)),
      SpriteState(players[4], "r", true, this._getOnClick(4)),
      SpriteState(players[5], "r", true, this._getOnClick(5)),
      SpriteState(players[6], "r", true, this._getOnClick(6)),
      SpriteState(players[7], "r", true, this._getOnClick(7))
    ];
    if (crActor.automatic != 0) {
      this._execAI();
    } else {
      this._setCrAutoSkill();
    }
  }

}