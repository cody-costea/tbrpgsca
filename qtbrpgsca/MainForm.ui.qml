import QtQuick.Controls 2.3
import QtQuick 2.4

Rectangle {
    id: mainRct

    width: 640
    height: 360
    color: "#000000"
    property alias itemText: itemText
    property alias actorText: actorText
    property alias skillText: skillText
    property alias textScroll: textScroll
    property alias skillBox: skillBox
    property alias skillBtn: skillBtn
    property alias targetBox: targetBox
    property alias itemBtn: itemBtn
    property alias itemBox: itemBox
    property alias runBtn: runBtn
    property alias autoBtn: autoBtn
    property alias textArea: textArea
    property alias arenaImg: arenaImg
    property alias mainRct: mainRct
    property alias btR4: btR4
    property alias btR2: btR2
    property alias btR1: btR1
    property alias btR3: btR3
    property alias btL2: btL2
    property alias btL1: btL1
    property alias btL4: btL4
    property alias btL3: btL3

    Rectangle {
        id: controlsRct
        x: 0
        y: 0
        width: 128
        height: 360
        color: "#ffffff"

        ComboBox {
            id: skillBox
            x: 8
            y: 46
            width: 112
            height: 30
            font.pointSize: 10
        }

        Button {
            id: skillBtn
            x: 8
            y: 82
            width: 112
            height: 25
            text: "Execute"
            font.pointSize: 10
        }

        ComboBox {
            id: targetBox
            x: 8
            y: 112
            width: 112
            height: 30
            font.pointSize: 10
        }

        Button {
            id: itemBtn
            x: 8
            y: 148
            width: 112
            height: 25
            text: "Use"
            font.pointSize: 10
        }

        ComboBox {
            id: itemBox
            x: 8
            y: 179
            width: 112
            height: 30
            font.pointSize: 10
        }

        Button {
            id: runBtn
            x: 8
            y: 330
            width: 112
            height: 25
            text: "Run"
        }

        Button {
            id: autoBtn
            x: 8
            y: 299
            width: 112
            height: 25
            text: "Auto"
            font.pointSize: 10
        }

        Text {
            id: skillText
            x: 8
            y: 6
            width: 112
            height: 34
            text: qsTr("")
            wrapMode: Text.WrapAnywhere
            font.pixelSize: 10
        }

        Text {
            id: itemText
            x: 8
            y: 215
            width: 112
            height: 34
            text: qsTr("")
            font.pixelSize: 10
        }

        Text {
            id: actorText
            x: 8
            y: 255
            width: 112
            height: 38
            text: qsTr("")
            font.pixelSize: 10
        }
    }

    Image {
        id: arenaImg
        x: 129
        y: 0
        width: 511
        height: 297

        AnimatedImage {
            id: btR4
            x: 383
            y: 168
            width: 128
            height: 128
            source: ""
        }

        AnimatedImage {
            id: btR3
            x: 255
            y: 0
            width: 128
            height: 128
            source: ""
        }

        AnimatedImage {
            id: btR2
            x: 383
            y: 40
            width: 128
            height: 128
            source: ""
        }

        AnimatedImage {
            id: btR1
            x: 255
            y: 128
            width: 128
            height: 128
            source: ""
        }

        AnimatedImage {
            id: btL4
            x: 127
            y: 168
            width: 128
            height: 128
            source: ""
        }

        AnimatedImage {
            id: btL2
            x: 0
            y: 128
            width: 128
            height: 128
            currentFrame: 0
            playing: false
            source: ""
        }

        AnimatedImage {
            id: btL1
            x: 127
            y: 40
            width: 128
            height: 128
            source: ""
        }

        AnimatedImage {
            id: btL3
            x: 0
            y: 0
            width: 128
            height: 128
            source: ""
        }
    }

    ScrollView {
        id: textScroll
        x: 129
        y: 297
        width: 511
        height: 63
        font.pixelSize: 10

        TextArea {
            id: textArea
            x: -10
            y: -6
            width: 511
            height: 49
            font.capitalization: Font.MixedCase
            textFormat: Text.AutoText
            color: "white"
            enabled: false
            text: ""
            font.pixelSize: 10
            font.wordSpacing: 4
        }
    }
}
