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
import QtMultimedia 5.8

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
        property var abilityImg: []
        property var selectorImg: []

        property int koActors: 0

        property string sprType: ".gif";
        property int sprCount: 0;
        property int crActor: 0;
        property bool automatic: false;

        property alias arenaSong: arenaSong
        property alias abilitySnd: abilitySnd

        function beginTurn(target) {
            arenaForm.crActor = arena.current;
            if (arena.status > 0) {
                arenaForm.textArea.append("The party has won!");
            }
            else if (arena.status < 0) {
                arenaForm.textArea.append("The party has been defeated!");
            }
            else if (arenaForm.automatic || arena.checkIfAI()) {
                arenaForm.execAI();
            }
            else {
                arenaForm.skillBox.enabled = true;
                arenaForm.itemBox.enabled = true
                arenaForm.targetBox.enabled = true;
                arenaForm.runBtn.enabled = true;
                arenaForm.autoBtn.enabled = true;
                arenaForm.skillBox.model = arena.crSkillsNames;
                arenaForm.itemBox.model = arena.crItemsNames;
                arenaForm.targetBox.model = arena.battlerNames;
                arenaForm.actorText.text = arena.getBattlerDesc(arenaForm.crActor);
                arenaForm.skillBox.currentIndex = arena.getAIskill(
                            target === undefined ? (arenaForm.targetBox.currentIndex >= arena.enemyIndex ? 0 : 1) : target);
                arenaForm.checkCrSkillAct();
                arenaForm.checkCrItemUse();
                arenaForm.selectorImg[arenaForm.crActor].source = 'qrc:/sprites/current.png';
            }
        }

        function sprCallback() {
            if (arena.checkIfKO(arenaForm.crActor)) {
                var crActor = arenaForm.crActor;
                arenaForm.actorImg[crActor].source =
                        'qrc:/sprites/' + arena.sprites[crActor] + '/bt_' +
                        (crActor < arena.enemyIndex ? 'l' : 'r') +
                        '_fallen' + arenaForm.sprType;
                arenaForm.koActors += 1 << (crActor + 1);
                arenaForm.actorImg[crActor].playing = true;
            }
            arenaForm.beginTurn(undefined);
        }

        function endTurn() {
            arenaForm.selectorImg[arenaForm.crActor].source = '';
            arenaForm.skillBtn.enabled = false;
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
                    'qrc:/sprites/' + arena.sprites[arenaForm.crActor] + '/bt_' +
                    (arenaForm.crActor < arena.enemyIndex ? 'l' : 'r') +
                    arena.getLastAbilityActorSpr() + arenaForm.sprType;
            arenaForm.actorImg[arenaForm.crActor].playing = true;
            for (var i = arena.firstTarget; i <= arena.lastTarget; i++) {
                var s = arena.getLastAbilityAnim();
                if (s !== "") {
                    arenaForm.sprCount++;
                    arenaForm.abilityImg[i].source = 'qrc:/sprites/abilities/' + s + arenaForm.sprType;
                    arenaForm.abilityImg[i].playing = true;
                }
                s = arena.getLastAbilityAudio();
                if (s !== "") {
                    arenaForm.sprCount++;
                    arenaForm.abilitySnd.source = 'qrc:/audio/' + s;
                    arenaForm.abilitySnd.play();
                }
                if (i !== arenaForm.crActor) {
                    var koActors = arenaForm.koActors;
                    var crStatus = arena.checkIfKO(i);
                    var koBit = 1 << i;
                    var ko = (koActors & koBit) === koBit;
                    if (!(crStatus && ko)) {
                        arenaForm.sprCount++;
                        var sprName;
                        if (ko) {
                            sprName = "_restored";
                            arenaForm.koActors = koActors - koBit;
                        } else if (crStatus) {
                            sprName = "_fallen";
                            arenaForm.koActors = koActors + koBit;
                        } else {
                            sprName = "_hit";
                        }
                        arenaForm.actorImg[i].source =
                                'qrc:/sprites/' + arena.sprites[i] + '/bt_' +
                                (i < arena.enemyIndex ? 'l' : 'r') +
                                sprName + arenaForm.sprType;
                        arenaForm.actorImg[i].playing = true;
                    }
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
                    && arena.getGuardianVsItem(trg, item) === trg;
        }

        function execCrSkill() {
            arenaForm.autoBtn.enabled = false;
            arenaForm.textArea.append(
                        arena.performSkill(arenaForm.skillBox.currentIndex,
                                           arenaForm.targetBox.currentIndex, ""));
            arenaForm.endTurn();
        }

        skillBox.onCurrentIndexChanged: {
            arenaForm.skillText.text = arena.getCrSkillDesc(arenaForm.skillBox.currentIndex);
            arenaForm.checkCrSkillAct();
        }

        itemBox.onCurrentIndexChanged: {
            arenaForm.itemText.text = arena.getCrItemDesc(arenaForm.itemBox.currentIndex);
            arenaForm.checkCrItemUse();
        }

        targetBox.onCurrentIndexChanged: {
            arenaForm.checkCrSkillAct();
            arenaForm.checkCrItemUse();
        }

        runBtn.onClicked: {
            if (arena.escape()) {
                arenaForm.skillBtn.enabled = false;
                arenaForm.itemBtn.enabled = false;
                arenaForm.skillBox.enabled = false;
                arenaForm.itemBox.enabled = false;
                arenaForm.targetBox.enabled = false;
                arenaForm.runBtn.enabled = false;
                arenaForm.autoBtn.enabled = false;
                arenaForm.textArea.append("The party has fled.");
            }
        }

        btL1Mouse.onClicked: {
            if (arenaForm.targetBox.enabled) {
                if (arenaForm.targetBox.currentIndex === 0) {
                    if (arenaForm.skillBtn.enabled) {
                        arenaForm.execCrSkill();
                    }
                }
                else {
                    arenaForm.targetBox.currentIndex = 0;
                    arenaForm.skillBox.currentIndex = arena.getAIskill(1);
                }
            }
        }

        btL2Mouse.onClicked: {
            if (arenaForm.targetBox.enabled && arena.enemyIndex > 1) {
                if (arenaForm.targetBox.currentIndex === 1) {
                    if (arenaForm.skillBtn.enabled) {
                        arenaForm.execCrSkill();
                    }
                }
                else {
                    arenaForm.targetBox.currentIndex = 1;
                    arenaForm.skillBox.currentIndex = arena.getAIskill(1);
                }
            }
        }

        btL3Mouse.onClicked: {
            if (arenaForm.targetBox.enabled && arena.enemyIndex > 2) {
                if (arenaForm.targetBox.currentIndex === 2) {
                    if (arenaForm.skillBtn.enabled) {
                        arenaForm.execCrSkill();
                    }
                }
                else {
                    arenaForm.targetBox.currentIndex = 2;
                    arenaForm.skillBox.currentIndex = arena.getAIskill(1);
                }
            }
        }

        btL4Mouse.onClicked: {
            if (arenaForm.targetBox.enabled && arena.enemyIndex > 3) {
                if (arenaForm.targetBox.currentIndex === 3) {
                    if (arenaForm.skillBtn.enabled) {
                        arenaForm.execCrSkill();
                    }
                }
                else {
                    arenaForm.targetBox.currentIndex = 3;
                    arenaForm.skillBox.currentIndex = arena.getAIskill(1);
                }
            }
        }

        btR1Mouse.onClicked: {
            if (arenaForm.targetBox.enabled) {
                if (arenaForm.targetBox.currentIndex === arena.enemyIndex) {
                    if (arenaForm.skillBtn.enabled) {
                        arenaForm.execCrSkill();
                    }
                }
                else {
                    arenaForm.targetBox.currentIndex = arena.enemyIndex;
                    arenaForm.skillBox.currentIndex = arena.getAIskill(0);
                }
            }
        }

        btR2Mouse.onClicked: {
            if (arenaForm.targetBox.enabled && arena.enemyIndex + 1 < arena.battlerNr) {
                if (arenaForm.targetBox.currentIndex === arena.enemyIndex + 1) {
                    if (arenaForm.skillBtn.enabled) {
                        arenaForm.execCrSkill();
                    }
                }
                else {
                    arenaForm.targetBox.currentIndex = arena.enemyIndex + 1;
                    arenaForm.skillBox.currentIndex = arena.getAIskill(0);
                }
            }
        }

        btR3Mouse.onClicked: {
            if (arenaForm.targetBox.enabled && arena.enemyIndex + 2 < arena.battlerNr) {
                if (arenaForm.targetBox.currentIndex === arena.enemyIndex + 2) {
                    if (arenaForm.skillBtn.enabled) {
                        arenaForm.execCrSkill();
                    }
                }
                else {
                    arenaForm.targetBox.currentIndex = arena.enemyIndex + 2;
                    arenaForm.skillBox.currentIndex = arena.getAIskill(0);
                }
            }
        }

        btR4Mouse.onClicked: {
            if (arenaForm.targetBox.enabled && arena.enemyIndex + 3 < arena.battlerNr) {
                if (arenaForm.targetBox.currentIndex === arena.enemyIndex + 3) {
                    if (arenaForm.skillBtn.enabled) {
                        arenaForm.execCrSkill();
                    }
                }
                else {
                    arenaForm.targetBox.currentIndex = arena.enemyIndex + 3;
                    arenaForm.skillBox.currentIndex = arena.getAIskill(0);
                }
            }
        }

        Audio {
            id: arenaSong
            autoPlay: true;
            loops: Audio.Infinite;
        }

        Audio {
            id: abilitySnd
            autoPlay: false
            loops: 0
            onPlaybackStateChanged: function() {
                if (abilitySnd.playbackState === Audio.StoppedState) {
                    arenaForm.sprCountMod();
                }
            }
        }

        Component.onCompleted: {
            var s = arena.getArenaSongFile();
            if (s !== "") {
                arenaForm.arenaSong.source = 'qrc:/audio/' + s;
                arenaForm.arenaSong.play();
            }
            s = arena.getArenaImageFile();
            if (s !== "") {
                arenaForm.arenaImg.source = 'qrc:/sprites/arena/' + s;
            }

            var i, j, koBit;
            for (i = 0; i < 4 && i < arena.enemyIndex; i++) {
                koBit = 1 << i;
                if (arena.checkIfKO(i)) {
                    arenaForm.koActors += koBit;
                }
                arenaForm.abilityImg.push(arenaForm.mainRct["btL" + (i + 1) + "Ability"]);
                arenaForm.selectorImg.push(arenaForm.mainRct["btL" + (i + 1) + "Selector"]);
                arenaForm.actorImg.push(arenaForm.mainRct["btL" + (i + 1)]);
                arenaForm.actorImg[i].source =
                        'qrc:/sprites/' + arena.sprites[i] + '/bt_l'
                        + ((arenaForm.koActors & koBit) === koBit ? '_ko' : '_idle')
                        + arenaForm.sprType;
                arenaForm.actorImg[i].playing = true;
            }
            //TODO: de revizuit
            for (i = arena.enemyIndex, j = 1; j <= 4 && i < arena.battlerNr; i++, j++) {
                koBit = 1 << i;
                if (arena.checkIfKO(i)) {
                    arenaForm.koActors += (1 << i);
                }
                arenaForm.abilityImg.push(arenaForm.mainRct["btR" + j + "Ability"]);
                arenaForm.selectorImg.push(arenaForm.mainRct["btR" + j + "Selector"]);
                arenaForm.actorImg.push(arenaForm.mainRct["btR" + j]);
                arenaForm.actorImg[i].source =
                        'qrc:/sprites/' + arena.sprites[i] + '/bt_r'
                        + ((arenaForm.koActors & koBit) === koBit ? '_ko' : '_idle')
                        + arenaForm.sprType;
                arenaForm.actorImg[i].playing = true;
            }

            arenaForm.beginTurn(arena.enemyIndex);
            arenaForm.targetBox.currentIndex = arena.enemyIndex;
        }

        skillBtn.onClicked: {
            arenaForm.execCrSkill();
        }

        itemBtn.onClicked: {
            arenaForm.autoBtn.enabled = false;
            arenaForm.textArea.append(
                        arena.useItem(arenaForm.itemBox.currentIndex,
                                           arenaForm.targetBox.currentIndex, ""));
            arenaForm.endTurn();
        }

        autoBtn.onClicked: {
            if ((arenaForm.automatic = !arenaForm.automatic)) {
                if (!arena.checkIfAI()) {
                    arenaForm.execAI();
                }
                arenaForm.autoBtn.text = "Manual";
            }
            else {
                arenaForm.autoBtn.text = "Auto";
                arenaForm.autoBtn.enabled = false;
            }
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

        btL1Ability.onPlayingChanged: {
            if (!arenaForm.btL1Ability.playing) {
                arenaForm.sprCountMod();
            }
        }
        btL2Ability.onPlayingChanged: {
            if (!arenaForm.btL2Ability.playing) {
                arenaForm.sprCountMod();
            }
        }
        btL3Ability.onPlayingChanged: {
            if (!arenaForm.btL3Ability.playing) {
                arenaForm.sprCountMod();
            }
        }
        btL4Ability.onPlayingChanged: {
            if (!arenaForm.btL4Ability.playing) {
                arenaForm.sprCountMod();
            }
        }
        btR1Ability.onPlayingChanged: {
            if (!arenaForm.btR1Ability.playing) {
                arenaForm.sprCountMod();
            }
        }
        btR2Ability.onPlayingChanged: {
            if (!arenaForm.btR2Ability.playing) {
                arenaForm.sprCountMod();
            }
        }
        btR3Ability.onPlayingChanged: {
            if (!arenaForm.btR3Ability.playing) {
                arenaForm.sprCountMod();
            }
        }
        btR4Ability.onPlayingChanged: {
            if (!arenaForm.btR4Ability.playing) {
                arenaForm.sprCountMod();
            }
        }
    }
}
