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
        minimumWidth: 640
        minimumHeight: 360
        title: qsTr("Turn-Based RPG Simple Combat Arena")

        MainForm {
            id: arenaForm
            anchors.fill: parent

            property var actorImg: []
            property string sprType: ".gif";
            property int sprCount: 0;
            property int crActor: 0;

            function beginTurn() {
                arenaForm.crActor = arena.current;
                if (arena.status > 0) {
                    arenaForm.textArea.append("The party has won!");
                }
                else if (arena.status < 0) {
                    arenaForm.textArea.append("The party has been defeated!");
                }
                else if (arena.checkIfAI()) {
                    execAI();
                }
                else {
                    arenaForm.autoBtn.enabled = true;
                    arenaForm.skillBox.enabled = true;
                    arenaForm.itemBox.enabled = true
                    arenaForm.targetBox.enabled = true;;
                    arenaForm.runBtn.enabled = true;
                    arenaForm.skillBox.model = arena.crSkillsNames;
                    arenaForm.itemBox.model = arena.crItemsNames;
                    arenaForm.targetBox.model = arena.battlerNames;
                    arenaForm.actorText.text = arena.getBattlerDesc(arenaForm.crActor);
                    checkCrSkillAct();
                    checkCrItemUse();
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
                arenaForm.textArea.append(arena.endTurn(""));
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
                arenaForm.textArea.append(arena.executeAI(""));
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

            function checkCrSkillAct() {
                var trg = arenaForm.targetBox.currentIndex;
                var skill = arenaForm.skillBox.currentIndex;
                arenaForm.skillBtn.enabled = arena.checkCrSkill(skill) && (!arena.checkIfKO(trg) || arena.checkIfSkillHeals(skill))
                        && arena.getGuardianVsSkill(trg, skill) === trg;
            }

            function checkCrItemUse() {
                var trg = arenaForm.targetBox.currentIndex;
                var item = arenaForm.itemBox.currentIndex;
                arenaForm.itemBtn.enabled = arena.checkCrItem(item) && (!arena.checkIfKO(trg) || arena.checkIfItemHeals(item))
                        && arena.getGuardianVsSkill(trg, item) === trg;
            }

            skillBox.onCurrentIndexChanged: {
                arenaForm.skillText.text = arena.getCrSkillDesc(arenaForm.skillBox.currentIndex);
                checkCrSkillAct();
            }

            itemBox.onCurrentIndexChanged: {
                arenaForm.itemText.text = arena.getCrItemDesc(arenaForm.itemBox.currentIndex);
                checkCrItemUse();
            }

            targetBox.onCurrentIndexChanged: {
                checkCrSkillAct();
                checkCrItemUse();
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
                arenaForm.targetBox.currentIndex = arena.enemyIndex;
            }

            skillBtn.onClicked: {
                arenaForm.textArea.append(
                            arena.performSkill(arenaForm.skillBox.currentIndex,
                                               arenaForm.targetBox.currentIndex, ""));
                arenaForm.endTurn();
            }

            itemBtn.onClicked: {
                arenaForm.textArea.append(
                            arena.useItem(arenaForm.itemBox.currentIndex,
                                               arenaForm.targetBox.currentIndex, ""));
                arenaForm.endTurn();
            }

            autoBtn.onClicked: {
                arenaForm.execAI();
            }

            btL1.onPlayingChanged: {
                if (!arenaForm.btL1.playing) {
                    arenaForm.sprCountMod();
                }
            }
            btL2.onPlayingChanged: {
                if (!arenaForm.btL2.playing) {
                    arenaForm.sprCountMod();
                }
            }
            btL3.onPlayingChanged: {
                if (!arenaForm.btL3.playing) {
                    arenaForm.sprCountMod();
                }
            }
            btL4.onPlayingChanged: {
                if (!arenaForm.btL4.playing) {
                    arenaForm.sprCountMod();
                }
            }
            btR1.onPlayingChanged: {
                if (!arenaForm.btR1.playing) {
                    arenaForm.sprCountMod();
                }
            }
            btR2.onPlayingChanged: {
                if (!arenaForm.btR2.playing) {
                    arenaForm.sprCountMod();
                }
            }
            btR3.onPlayingChanged: {
                if (!arenaForm.btR3.playing) {
                    arenaForm.sprCountMod();
                }
            }
            btR4.onPlayingChanged: {
                if (!arenaForm.btR4.playing) {
                    arenaForm.sprCountMod();
                }
            }
        }
    }
}
