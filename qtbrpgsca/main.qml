/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

import QtQuick 2.2
import QtQuick.Window 2.2
import QtMultimedia 5.12

import com.codycostea.tbrpgsca 1.0

Window {
    visible: true
    width: 640
    height: 360
    minimumWidth: 640
    minimumHeight: 360
    title: qsTr("Turn-Based RPG Simple Combat Arena")

    property string arenaSong
    property bool automatic: false;
    property string sprType: ".gif";
    property int sprCount: 0;

    function sprCountMod() {
        if (sprCount > 0) {
            --sprCount;
        }
    }

    Scene {
        id: scene
    }

    Image {
        id: backImg
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
    }


    Audio {
        id: backSong
        autoPlay: true;
        loops: Audio.Infinite;
    }

    Audio {
        id: abilitySnd
        autoPlay: false
        onPlaybackStateChanged: function() {
            if (abilitySnd.playbackState === Audio.StoppedState) {
                sprCountMod();
            }
        }
        loops: 0
    }

    Component.onCompleted: {
        var s = scene.song;
        if (s && s.length > 0) {
            backSong.source = 'qrc:/audio/' + s;
            backSong.play();
        }
        s = scene.image;
        if (s && s.length > 0) {
            backImg.source = 'qrc:/sprites/arena/' + s;
        }
    }
}
