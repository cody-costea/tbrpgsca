/*
Copyright (C) AD 2018 Claudiu-Stefan Costea

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

import QtQuick 2.2
import QtQuick.Window 2.2

Item {

    Window {
        visible: true
        width: 640
        height: 360
        maximumWidth: 640
        maximumHeight: 360
        minimumWidth: 640
        minimumHeight: 360
        title: qsTr("Turn-Based RPG Simple Combat Arena")

        MainForm {
            id: arenaForm
            anchors.fill: parent

            property var actorImg: []
            property string sprType: ".mng";
            property int sprCount: 0;
            property int crActor: 0;

            function beginTurn() {
                arenaForm.crActor = arena.current;
                if (arena.status > 0) {
                    arenaForm.mainRct.textArea.append("The party has won!");
                }
                else if (arena.status < 0) {
                    arenaForm.mainRct.textArea.append("The party has been defeated!");
                }
                else if (arena.checkIfAI()) {
                    execAI();
                }
                else {
                    arenaForm.skillBtn.enabled = true;
                    arenaForm.autoBtn.enabled = true;
                    arenaForm.itemBtn.enabled = true;
                    arenaForm.skillBox.enabled = true;
                    arenaForm.itemBox.enabled = true
                    arenaForm.targetBox.enabled = true;;
                    arenaForm.runBtn.enabled = true;
                    arenaForm.mainRct.skillBox.model = arena.crSkillsNames;
                    arenaForm.mainRct.itemBox.model = arena.crItemsNames;
                    arenaForm.mainRct.targetBox.model = arena.battlerNames;
                    arenaForm.mainRct.actorText.text = arena.battlerNames[arenaForm.crActor];
                }
            }

            function sprCallback() {
                if (arena.checkIfKO(arenaForm.crActor)) {
                    arenaForm.actorImg[arenaForm.crActor].source =
                            'qrc:/' + arena.sprites[arenaForm.crActor] +
                            (arenaForm.crActor < arena.enemyIndex ? '_l_' : '_r_') +
                            'fallen' + arenaForm.sprType;
                    arenaForm.actorImg[arenaForm.crActor].playing = true;
                }
                arenaForm.beginTurn();
            }

            function endTurn() {
                arenaForm.skillBtn.enabled = false;
                arenaForm.autoBtn.enabled = false;
                arenaForm.itemBtn.enabled = false;
                arenaForm.skillBox.enabled = false;
                arenaForm.itemBox.enabled = false;
                arenaForm.targetBox.enabled = false;
                arenaForm.runBtn.enabled = false;
                arenaForm.mainRct.textArea.append(arena.endTurn(""));
                arenaForm.playAnim();
            }

            function playAnim() {
                arenaForm.sprCount++;
                arenaForm.actorImg[arenaForm.crActor].source =
                        'qrc:/' + arena.sprites[arenaForm.crActor] +
                        (arenaForm.crActor < arena.enemyIndex ? '_l_' : '_r_') +
                        'act' + arenaForm.sprType;
                arenaForm.actorImg[arenaForm.crActor].playing = true;
                for (var i = arena.firstTarget; i <= arena.lastTarget; i++) {
                    if (i != arenaForm.crActor) {
                        arenaForm.sprCount++;
                        arenaForm.actorImg[i].source =
                                'qrc:/' + arena.sprites[i] +
                                (i < arena.enemyIndex ? '_l_' : '_r_') +
                                (arena.checkIfKO(i) ? 'fallen' : 'hit') +
                                arenaForm.sprType;
                        arenaForm.actorImg[i].playing = true;
                    }
                }
            }

            function execAI() {
                arenaForm.mainRct.textArea.append(arena.executeAI(""));
                arenaForm.endTurn();
            }

            function sprCountMod() {
                if (arenaForm.sprCount > 0) {
                    arenaForm.sprCount--;
                    if (arenaForm.sprCount < 1) {
                        arenaForm.sprCallback();
                    }
                }
            }

            skillBox.onCurrentIndexChanged: {
                arenaForm.mainRct.skillText.text = arena.crSkillsNames[arenaForm.mainRct.skillBox.currentIndex];
            }

            itemBox.onCurrentIndexChanged: {
                arenaForm.mainRct.itemText.text = arena.crItemsNames[arenaForm.mainRct.itemBox.currentIndex];
            }

            Component.onCompleted: {
                var i, j = 1;
                for (i = 0; i < 4 && i < arena.enemyIndex; i++) {
                    arenaForm.actorImg.push(arenaForm.mainRct["btL" + (i + 1)]);
                    arenaForm.actorImg[i].source =
                            'qrc:/' + arena.sprites[i] + '_l_idle' + arenaForm.sprType;
                    arenaForm.actorImg[i].playing = true;
                }
                //TODO: de revizuit
                for (i = arena.enemyIndex; j <= 4 && i < arena.battlerNr; i++) {
                    arenaForm.actorImg.push(arenaForm.mainRct["btR" + j]);
                    arenaForm.actorImg[i].source =
                            'qrc:/' + arena.sprites[i] + '_r_idle' + arenaForm.sprType;
                    arenaForm.actorImg[i].playing = true;
                    j++;
                }

                arenaForm.beginTurn();
            }

            skillBtn.onClicked: {
                arenaForm.mainRct.textArea.append(
                            arena.performSkill(arenaForm.mainRct.skillBox.currentIndex,
                                               arenaForm.mainRct.targetBox.currentIndex, ""));
                arenaForm.endTurn();
            }

            itemBtn.onClicked: {
                arenaForm.mainRct.textArea.append(
                            arena.useItem(arenaForm.mainRct.itemBox.currentIndex,
                                               arenaForm.mainRct.targetBox.currentIndex, ""));
                arenaForm.endTurn();
            }

            autoBtn.onClicked: {
                arenaForm.execAI();
            }

            btL1.onPlayingChanged: {
                if (!arenaForm.mainRct.btL1.playing) {
                    arenaForm.sprCountMod();
                }
            }
            btL2.onPlayingChanged: {
                if (!arenaForm.mainRct.btL2.playing) {
                    arenaForm.sprCountMod();
                }
            }
            btL3.onPlayingChanged: {
                if (!arenaForm.mainRct.btL3.playing) {
                    arenaForm.sprCountMod();
                }
            }
            btL4.onPlayingChanged: {
                if (!arenaForm.mainRct.btL4.playing) {
                    arenaForm.sprCountMod();
                }
            }
            btR1.onPlayingChanged: {
                if (!arenaForm.mainRct.btR1.playing) {
                    arenaForm.sprCountMod();
                }
            }
            btR2.onPlayingChanged: {
                if (!arenaForm.mainRct.btR2.playing) {
                    arenaForm.sprCountMod();
                }
            }
            btR3.onPlayingChanged: {
                if (!arenaForm.mainRct.btR3.playing) {
                    arenaForm.sprCountMod();
                }
            }
            btR4.onPlayingChanged: {
                if (!arenaForm.mainRct.btR4.playing) {
                    arenaForm.sprCountMod();
                }
            }
        }
    }
}
