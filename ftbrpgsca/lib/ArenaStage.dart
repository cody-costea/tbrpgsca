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
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:ftbrpgsca/Actor.dart';
import 'package:ftbrpgsca/SceneAct.dart';
import 'package:ftbrpgsca/Performance.dart';
//import 'package:audioplayers/audio_cache.dart';
import 'package:flutter/scheduler.dart';
import 'package:sprintf/sprintf.dart';

const int FRAME_TIME = 71;//87;
const int SPR_HIT = 0;
const int SPR_FALLEN = 1;
const int SPR_RISEN = 2;
const int SPR_ACT = 3;
const int SPR_CAST = 4;

int _koActors = 0;
Map<String, List<int>> _skillSprTime = new Map();
Map<String, List<String>> _skillSprFiles = new Map();
Map<String, int> _skillSprFullTime = new Map();
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
  int _koBit;
  int _sprite;
  bool shapeShift = false;
  List<List<int>> _sprTime;
  List<List<String>> _sprFiles;
  List<int> _sprFullTime;
  String _skillName;
  String _idleSpr;
  String _koSpr;
  int _counter = 0;
  int _skillCnt = 0;
  Function _onClick;
  Actor _player;

  set actor(final Actor value) {
    this._player = value;
    this.name = value.sprite;//value.job.sprite;
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
    this._skillCnt = -1;
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
    return await rootBundle.loadString('assets/sprites/actors/$_name/spr_${_pos}_$spr.txt');
  }

  Future<String> _readSkillSprInfo(final String spr) async {
    return await rootBundle.loadString('assets/sprites/abilities/$spr/spr.txt');
  }

  void _prepareSpr(final int crSprite, final bool play) {
    {
      final Actor crActor = this._player;
      bool crShapeShift = crActor.shapeShift;
      if (this.shapeShift != crShapeShift) {
        String name = crActor.sprite;//crShapeShift ? crActor.sprite : crActor.job.sprite;
        this.shapeShift = crShapeShift;
        this._idleSpr = null;
        this._koSpr = null;
        this._sprFiles = new List(5);
        this._sprFullTime = new List(5);
        this._sprTime = new List(5);
        this._name = name;
      }
    }
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
        this._prepareSkillSpr(this._skillName, play);
      });
    } else {
      this._prepareSkillSpr(this._skillName, play);
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
            if (sprTime < 10) {
              sprTime *= FRAME_TIME;
            }
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
      if (this._skillCnt < 0) {
        this._skillCnt++;
      }
    });
  }

  @override
  Widget build(final BuildContext context) {
    final Actor actor = this._player;
    if (actor == null) {
      return SizedBox();
    }
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
    final int koBit = this._koBit;
    return GestureDetector(
      onTap: this._onClick,
      child: Stack(
        children: <Widget>[
          Image(
              image: AssetImage('assets/sprites/actors/$_name/${actorWait == 0
                  ? (_koActors & koBit == koBit ? this.koSpr : this.idleSpr)
                  : crSprList[actorCnt]}'),
              gaplessPlayback: true,
              width: 128,
              height: 128
          ),
          skillWait == 0 ? SizedBox() : Image(
            image:  AssetImage('assets/sprites/abilities/$_skillName/${crSkSprList[skillCnt]}'),
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

  SpriteState(final Actor actor, final int index, final String pos, final bool aot, final Function onClick) {
    if (actor == null || pos == null) {
      return;
    }
    this._pos = pos;
    this._player = actor;
    this._name = actor.sprite;//actor.job.sprite;
    this._sprFiles = new List(5);
    this._sprFullTime = new List(5);
    this._sprTime = new List(5);
    this._koBit = 1 << index;
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

  final navigatorKey = GlobalKey<NavigatorState>();

  SceneAct _sceneAct;
  List<SpriteState> _actorSprites;
  Actor _crActor;
  Performance _crSkill;
  Performance _crItem;
  String _arenaImg;
  List<DropdownMenuItem<Performance>> _crSkills;
  List<DropdownMenuItem<Performance>> _crItems;
  List<DropdownMenuItem<int>> _players;
  TextEditingController _actionsTxt = new TextEditingController();
  ScrollController _scrollController = new ScrollController();
  int _target;
  int _surprise;
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

  /*AudioCache _flutterSound;
  AudioCache get flutterSound {
    AudioCache flutterSound = this._flutterSound;
    if (flutterSound == null) {
      flutterSound = new AudioCache(prefix: 'audio/');
      this._flutterSound = flutterSound;
    }
    return flutterSound;
  }*/

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
    final List<SpriteState> actorSprites = this._actorSprites;
    final bool surprised = this._surprise < 0;
    final Performance crSkill = this._crSkill;
    final SceneAct sceneAct = this._sceneAct;
    final Performance crItem = this._crItem;
    final bool activeBtn = this._activeBtn;
    final String arenaImg = this._arenaImg;
    final Actor crActor = this._crActor;
    return new MaterialApp(
        navigatorKey: this.navigatorKey,
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
                    Text(crActor.automatic == 0 ? "   ${crActor.name} - "
                      "Lv: ${crActor.level}/${crActor.maxLv}, "
                      "HP: ${crActor.hp}/${crActor.mHp}, "
                      "MP: ${crActor.mp}/${crActor.mMp}, "
                      "RP: ${crActor.sp}/${crActor.mSp}, "
                      "XP: ${crActor.exp}/${crActor.mExp}"
                    : "", overflow: TextOverflow.ellipsis),
                    Expanded(
                        child: Center(
                          child: Stack(
                            children: <Widget>[
                              Align(
                                alignment: Alignment(-1, -1),
                                child: FractionallySizedBox(
                                  widthFactor: 1,
                                  heightFactor: 0.69,
                                  child: arenaImg == null || arenaImg.length == 0 ? SizedBox() : Image(
                                    image: AssetImage('assets/sprites/arena/$arenaImg'),
                                    fit: BoxFit.fill,
                                    height: null,
                                    width: null
                                  )
                                )
                              ),
                              Align(
                                  alignment: Alignment(-0.7, -1),
                                  child: ActorSprite(spriteState: actorSprites[surprised ? 6 : 1])
                              ),
                              Align(
                                  alignment: Alignment(-0.3, -0.7),
                                  child: ActorSprite(spriteState: actorSprites[surprised ? 7 : 0])
                              ),
                              Align(
                                  alignment: Alignment(-0.9, -0.5),
                                  child: ActorSprite(spriteState: actorSprites[surprised ? 5 : 2])
                              ),
                              Align(
                                  alignment: Alignment(-0.5, -0.2),
                                  child: ActorSprite(spriteState: actorSprites[surprised ? 4 : 3])
                              ),
                              Align(
                                  alignment: Alignment(0.5, -1),
                                  child: ActorSprite(spriteState: actorSprites[surprised ? 0 : 7])
                              ),
                              Align(
                                  alignment: Alignment(0.9, -0.7),
                                  child: ActorSprite(spriteState: actorSprites[surprised ? 1 : 6])
                              ),
                              Align(
                                  alignment: Alignment(0.3, -0.5),
                                  child: ActorSprite(spriteState: actorSprites[surprised ? 3 : 4])
                              ),
                              Align(
                                  alignment: Alignment(0.7, -0.2),
                                  child: ActorSprite(spriteState: actorSprites[surprised ? 2 : 5])
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
                                onPressed: activeBtn && this._canPerform(crSkill) ? () {
                                  this._execSkill(crSkill);
                                } : null,
                                child: Text('Execute', overflow: TextOverflow.fade),
                              )
                          ),
                          Expanded(
                              child: DropdownButton(
                                  isExpanded: true,
                                  items: this._crSkills,
                                  onChanged: ((final Performance value) {
                                    this.setState(() {
                                      this._crSkill = value;
                                    });
                                  }),
                                  value: crSkill
                              )
                          )
                        ]
                    ),
                    Row(
                      children: <Widget>[
                        Expanded(
                            child: DropdownButton(
                                isExpanded: true,
                                items: this._players,
                                onChanged: ((final int value) {
                                  final int trg = this._target;
                                  final int enIdx = sceneAct.enemyIndex;
                                  final bool autoSkill = (trg < enIdx && value >= enIdx)
                                      || (trg >= enIdx && value < enIdx);
                                  this.setState(() {
                                    this._target = value;
                                    if (autoSkill) {
                                      this._setCrAutoSkill();
                                    }
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
                                      onPressed: activeBtn ? () {
                                        this._automatic = true;
                                        this._execAI();
                                      } : () {
                                        this._automatic = false;
                                      },
                                      child: Text(this._automatic ? 'Manual' : 'Auto', overflow: TextOverflow.fade),
                                    )
                                ),
                                Expanded(
                                    child: MaterialButton(
                                        onPressed: this._surprise < 0 ? null : () {
                                          this._afterAct("\n${sceneAct.escape()}");
                                        },
                                        child: Text('Run', overflow: TextOverflow.fade)
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
                                onPressed: activeBtn && crItem != null
                                    && this._canPerform(crItem) ? () {
                                  this._afterAct(sceneAct.useAbility(crItem, this._target, ""));
                                } : null,
                                child: Text('Use', overflow: TextOverflow.fade),
                              )
                          ),
                          Expanded(
                              child: DropdownButton(
                                isExpanded: true,
                                items: this._crItems,
                                onChanged: ((final Performance value) {
                                  this.setState(() {
                                    this._crItem = value;
                                  });
                                }),
                                value: crItem
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

  bool _canPerform(final Performance ability) {
    final int trgIndex = this._target;
    final SceneAct sceneAct = this._sceneAct;
    final Actor trgActor = sceneAct.players[trgIndex];
    return ability.canPerform(this._crActor)
        && trgIndex == sceneAct.getGuardian(trgIndex, ability)
        && (trgActor.hp > 0 || (ability.mHp < 0 && ability.restore));
  }

  List<DropdownMenuItem<int>> _preparePlayers(final List<Actor> players) {
    final List<DropdownMenuItem<int>> ret = new List();
    if (players != null) {
      final int enIdx = this._sceneAct.enemyIndex;
      for (int i = 0; i < players.length; i++) {
        final Actor actor = players[i];
        ret.add(new DropdownMenuItem(
            value: i,
            child: Text("${actor.name} (${i < enIdx ? "HP: ${actor.hp}/${actor.mHp}, "
                "MP: ${actor.mp}/${actor.mMp}, RP: ${actor.sp}/${actor.mSp}"
                : sprintf("HP: %.2f%%", [(actor.hp / actor.mHp) * 100.0])})",
                overflow: TextOverflow.ellipsis)
          )
        );
      }
    }
    return ret;
  }

  List<DropdownMenuItem<Performance>> _prepareAbilities(final Iterable<Performance> abilities,
      final Map<Performance, int> items, final int crt) {
    final List<DropdownMenuItem<Performance>> ret = new List();
    if (abilities != null) {
      final Actor crActor = this._crActor;
      for (Performance ability in abilities) {
        if (ability.canPerform(crActor)) {
          final String trg = ability.trg == 0 ? "One" : (ability.trg < 0 ? "Self" : "All");
          final String qty = (crActor.skillsQty == null || ability.mQty < 1
              || (!crActor.skillsQty.containsKey(ability)) ? "∞" : crActor.skillsQty[ability].toString());
          ret.add(new DropdownMenuItem(
              value: ability,
              child: Text("${ability.name} ${items == null || (!items.containsKey(ability)) ?
                  "(Lv: ${ability.lvRq}, HP: ${ability.hpC}, MP: ${ability.mpC}, "
                  "RP: ${ability.spC}, Nr: $qty, Trg: $trg, Range: ${ability.range ? "Yes" : "No"})"
                  : " x ${items[ability].toString()}"}",
                  overflow: TextOverflow.ellipsis)
            )
          );
        }
      }
    }
    return ret;
  }

  void _afterAct(String ret) {
    final List<DropdownMenuItem<Performance>> emptyAbilities = this.emptyAbilities;
    final List<Actor> players = this._sceneAct.players;
    this.setState(() {
      this._activeBtn = false;
      this._crSkills = emptyAbilities;
      this._crItems = emptyAbilities;
      this._crSkill = emptyAbilities[0].value;
      this._crItem = emptyAbilities[0].value;
      this._players = this._preparePlayers(players);
    });
    final Performance lastAbility = this._sceneAct.lastAbility;
    if (lastAbility == null) {
      this._endTurn(ret);
    } else {
      int crt = this._sceneAct.current;
      final int koActors = _koActors;
      final Actor crActor = players[crt];
      final Actor trgActor = players[this._sceneAct.firstTarget];
      if (crActor.automatic != 0 && trgActor.automatic == 0) {
        this.setState(() {
          this._crActor = trgActor;
        });
      }
      final String skillSnd = lastAbility.sound;
      /*if (skillSnd != null && skillSnd.length > 0) {
        this.flutterSound.play(skillSnd);
      }*/
      this._actorSprites[crt].sprite = crActor.hp > 0
          ? ((lastAbility.dmgType & Performance.DmgTypeSpi ==
          Performance.DmgTypeSpi
          || lastAbility.dmgType == Performance.DmgTypeWis)
          ? SPR_CAST
          : SPR_ACT) : SPR_FALLEN;
      new Timer(Duration(milliseconds: FRAME_TIME * 5), () {
        for (int trg = this._sceneAct.firstTarget; trg <=
            this._sceneAct.lastTarget; trg++) {
          if (trg != crt) {
            final int koBit = 1 << trg;
            final SpriteState trgSprite = this._actorSprites[trg];
            final Actor trgActor = trgSprite.actor;
            final bool ko = koActors & koBit == koBit;
            trgSprite.skillSprite = lastAbility.sprite;
            if (trgActor.hp > 0) {
              if (ko) {
                trgSprite.sprite = SPR_RISEN;
                _koActors = koActors - koBit;
              } else {
                trgSprite.sprite = SPR_HIT;
              }
            } else {
              trgSprite.sprite = SPR_FALLEN;
              if (!ko) {
                _koActors = koActors + koBit;
              }
            }
          }
        }
        new Timer(Duration(milliseconds: _waitTime), () {
          this._endTurn(ret);
        });
      });
    }
  }

  void _endTurn(String ret) {
    _waitTime = 0;
    final SceneAct sceneAct = this._sceneAct;
    int crt = sceneAct.current;
    final List<Actor> players = sceneAct.players;
    final String r = sceneAct.setNext("", true);
    if (players[crt].hp < 1) {
      _koActors += 1 << crt;
      this._actorSprites[crt].sprite = SPR_FALLEN;
    }
    ret = r + ret;
    this._actionsTxt.text = "$ret${this._actionsTxt.text}";
    if (sceneAct.status != 0) {
      final context = this.navigatorKey.currentState.overlay.context;
      showDialog<void>(context: context, barrierDismissible: false,
          builder: (final BuildContext context) {
            return AlertDialog(title: Text('Result'), content: Text(r.length > 0 ? r : ret),
              actions: <Widget>[
                FlatButton(child: Text("Ok"),
                  onPressed: () {
                    SystemChannels.platform.invokeMethod('SystemNavigator.pop');
                  }
                )
              ]);
          });
    } else {
      if (this._automatic || players[(crt = sceneAct.current)].automatic != 0) {
        this._execAI();
      } else {
        this.activeBtn = true;
        this.setState(() {
          final Actor crActor = this._crActor = players[crt];
          final List<Performance> skills = crActor.availableSkills;
          this._crSkills = this._prepareAbilities(skills, null, crt);
          this._crSkill = skills[0];
          final Map<int, List<Performance>> items = sceneAct.crItems;
          final List<DropdownMenuItem<Performance>> crItemsView =
          this._crItems =
          (items == null || items[crt] == null || items[crt].length == 0)
              ? this.emptyAbilities : this._prepareAbilities(
              items[crt], crActor.items, crt);
          this._crItem = crItemsView[0].value;
          this._setCrAutoSkill();
        });
      }
    }
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
        final Performance crSkill = this._crSkill;
        if (this._activeBtn && this._canPerform(crSkill)) {
          this._execSkill(crSkill);
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

  ArenaState(final List<Actor> party, final List<Actor> enemy, this._arenaImg, final String arenaSnd, final int surprise) {
    for (int i = 0; i < enemy.length; i++) {
      enemy[i].automatic = 2;
    }
    this._surprise = surprise;
    final SceneAct sceneAct = this._sceneAct = SceneAct(party, enemy, surprise, null, null);
    final List<Actor> players = sceneAct.players;
    final int current = sceneAct.current;
    final Actor crActor = this._crActor = players[current];
    this._crSkills = this._prepareAbilities(crActor.availableSkills, null, current);
    final Map<int, List<Performance>> items = this._sceneAct.crItems;
    this._crItems = (items == null || items[current] == null || items[current].length == 0)
        ? this.emptyAbilities : this._prepareAbilities(items[current], crActor.items, current);
    this._crSkill = this._crSkills[0].value;
    this._crItem = this._crItems[0].value;
    this._players = this._preparePlayers(players);
    final int enemyIndex = this._target = sceneAct.enemyIndex;
    final List<SpriteState> actorSprites = new List(8);
    for (int i = 0; i < 4; i++) {
      if (i < party.length) {
        actorSprites[i] = SpriteState(players[i], i, (surprise < 0 ? "r" : "l"), true, this._getOnClick(i));
      } else {
        actorSprites[i] = SpriteState(null, -1, null, false, null);
      }
    }
    for (int i = 0, j = enemyIndex; i < 4; i++, j++) {
      if (i < enemy.length) {
        actorSprites[i + 4] = SpriteState(players[j], j, (surprise < 0 ? "l" : "r"), true, this._getOnClick(j));
      } else {
        actorSprites[i + 4] = SpriteState(null, -1, null, false, null);
      }
    }
    this._actorSprites = actorSprites;
    /*if (arenaSnd != null && arenaSnd.length > 0) {
      this.flutterSound.loop(arenaSnd);
    }*/
    if (crActor.automatic == 0) {
      this._setCrAutoSkill();
    } else {
      SchedulerBinding.instance.addPostFrameCallback((_) {
        this._execAI();
      });
    }
  }

}