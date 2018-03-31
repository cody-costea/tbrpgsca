import QtQuick.Controls 2.2
import QtQuick 2.2

Rectangle {
    id: mainRct

    width: 640
    height: 360
    color: "#000000"
    property alias btR4Selector: btR4Selector
    property alias btR2Selector: btR2Selector
    property alias btR3Selector: btR3Selector
    property alias btL4Selector: btL4Selector
    property alias btR1Selector: btR1Selector
    property alias btL2Selector: btL2Selector
    property alias btL1Selector: btL1Selector
    property alias btL3Selector: btL3Selector
    property alias btR4Mouse: btR4Mouse
    property alias btR4Ability: btR4Ability
    property alias btR2Mouse: btR2Mouse
    property alias btR2Ability: btR2Ability
    property alias btR3Ability: btR3Ability
    property alias btR3Mouse: btR3Mouse
    property alias btR1Ability: btR1Ability
    property alias btR1Mouse: btR1Mouse
    property alias btL4Ability: btL4Ability
    property alias btL4Mouse: btL4Mouse
    property alias btL2Mouse: btL2Mouse
    property alias btL2Ability: btL2Ability
    property alias btL1Mouse: btL1Mouse
    property alias btL1Ability: btL1Ability
    property alias btL3Ability: btL3Ability
    property alias btL3Mouse: btL3Mouse
    property alias btR3: btR3
    property alias btR2: btR2
    property alias btL3: btL3
    property alias btL2: btL2
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
    property alias btR1: btR1
    property alias btL1: btL1
    property alias btL4: btL4

    Rectangle {
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.Bottom
        }
        id: controlsRct
        x: 0
        y: 0
        width: 128
        height: parent.height
        color: "#ffffff"

        ComboBox {
            id: skillBox
            anchors {
                bottom: skillBtn.top
                margins: 5
            }
            x: 8
            y: 46
            width: 112
            height: 30
            font.pointSize: 10
        }

        Button {
            id: skillBtn
            anchors {
                bottom: targetBox.top
                margins: 5
            }
            x: 8
            y: 82
            width: 112
            height: 25
            text: "Execute"
            font.pointSize: 10
        }

        ComboBox {
            id: targetBox
            anchors {
                centerIn: parent.Center
                margins: 5
            }
            x: 8
            y: 112
            width: 112
            height: 30
            font.pointSize: 10
        }

        Button {
            id: itemBtn
            anchors {
                top: targetBox.bottom
                margins: 5
            }
            x: 8
            y: 148
            width: 112
            height: 25
            text: "Use"
            font.pointSize: 10
        }

        ComboBox {
            id: itemBox
            anchors {
                top: itemBtn.bottom
                margins: 5
            }
            x: 8
            y: 179
            width: 112
            height: 30
            font.pointSize: 10
        }

        Button {
            id: runBtn
            anchors {
                bottom: parent.bottom
                margins: 5
            }
            x: 8
            y: 330
            width: 112
            height: 25
            text: "Run"
            font.pointSize: 10
        }

        Button {
            id: autoBtn
            anchors {
                bottom: runBtn.top
                margins: 5
            }
            x: 8
            y: 299
            width: 112
            height: 25
            text: "Auto"
            font.pointSize: 10
        }

        Text {
            id: skillText
            anchors {
                bottom: skillBox.top
                margins: 5
            }

            x: 8
            y: 6
            width: 112
            height: 34
            text: qsTr("")
            wrapMode: Text.WrapAnywhere
            font.pixelSize: 10
        }

        Text {
            anchors {
                top: itemBox.bottom
                bottom: actorText.top
                margins: 5
            }

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
            anchors {
                bottom: autoBtn.top
                margins: 5
            }

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
        anchors {
            top: parent.top
            right: parent.right
            left: controlsRct.right
        }

        x: 129
        y: 0
        width: 511
        height: 297
        fillMode: Image.PreserveAspectFit

        Image {
            id: btR2Selector
            anchors {
                left: btR1Selector.right
                bottom: parent.bottom
            }
            x: 383
            y: 168
            width: 128
            height: 128
            source: ""
            fillMode: Image.PreserveAspectFit

            AnimatedImage {
                id: btR2
                anchors.fill: parent
                x: 383
                y: 168
                width: 128
                height: 128
                source: ""
                fillMode: Image.PreserveAspectFit

                AnimatedImage {
                    id: btR2Ability
                    anchors.fill: parent
                    x: 0
                    y: 0
                    width: 128
                    height: 128
                    source: ""
                    fillMode: Image.PreserveAspectFit
                }

                MouseArea {
                    id: btR2Mouse
                    anchors.fill: parent
                    x: 0
                    y: 0
                    width: 128
                    height: 128
                }
            }
        }

        Image {
            id: btR4Selector
            anchors {
                left: parent.horizontalCenter
                top: parent.top
            }
            x: 255
            y: 0
            width: 128
            height: 128
            source: ""
            fillMode: Image.PreserveAspectFit

            AnimatedImage {
                id: btR4
                anchors.fill: parent
                x: 255
                y: 0
                width: 128
                height: 128
                source: ""
                fillMode: Image.PreserveAspectFit

                AnimatedImage {
                    id: btR4Ability
                    anchors.fill: parent
                    x: 0
                    y: 0
                    width: 128
                    height: 128
                    source: ""
                    fillMode: Image.PreserveAspectFit
                }

                MouseArea {
                    id: btR4Mouse
                    anchors.fill: parent
                    x: 0
                    y: 0
                    width: 128
                    height: 128
                }
            }
        }

        Image {
            id: btR3Selector
            anchors {
                left: btR4Selector.right
                bottom: btR2Selector.top
            }

            x: 383
            y: 40
            width: 128
            height: 128
            source: ""
            fillMode: Image.PreserveAspectFit

            AnimatedImage {
                id: btR3
                anchors.fill: parent
                x: 383
                y: 40
                width: 128
                height: 128
                source: ""
                fillMode: Image.PreserveAspectFit

                AnimatedImage {
                    id: btR3Ability
                    anchors.fill: parent
                    x: 0
                    y: 0
                    width: 128
                    height: 128
                    source: ""
                    fillMode: Image.PreserveAspectFit
                }

                MouseArea {
                    id: btR3Mouse
                    anchors.fill: parent
                    x: 0
                    y: 0
                    width: 128
                    height: 128
                }
            }
        }

        Image {
            id: btR1Selector
            anchors {
                left: parent.horizontalCenter
                top: btR4Selector.bottom
            }

            x: 255
            y: 128
            width: 128
            height: 128
            source: ""
            fillMode: Image.PreserveAspectFit

            AnimatedImage {
                id: btR1
                anchors.fill: parent
                x: 255
                y: 128
                width: 128
                height: 128
                source: ""
                fillMode: Image.PreserveAspectFit

                AnimatedImage {
                    id: btR1Ability
                    anchors.fill: parent
                    x: 0
                    y: 0
                    width: 128
                    height: 128
                    source: ""
                    fillMode: Image.PreserveAspectFit
                }

                MouseArea {
                    id: btR1Mouse
                    anchors.fill: parent
                    x: 0
                    y: 0
                    width: 128
                    height: 128
                }
            }
        }

        Image {
            id: btL4Selector
            anchors {
                right: parent.horizontalCenter
                bottom: parent.bottom
            }

            x: 127
            y: 168
            width: 128
            height: 128
            source: ""
            fillMode: Image.PreserveAspectFit

            AnimatedImage {
                id: btL4
                anchors.fill: parent
                x: 127
                y: 168
                width: 128
                height: 128
                source: ""
                fillMode: Image.PreserveAspectFit

                AnimatedImage {
                    id: btL4Ability
                    anchors.fill: parent
                    x: 0
                    y: 0
                    width: 128
                    height: 128
                    source: ""
                    fillMode: Image.PreserveAspectFit
                }

                MouseArea {
                    id: btL4Mouse
                    anchors.fill: parent
                    x: 0
                    y: 0
                    width: 128
                    height: 128
                }
            }
        }

        Image {
            id: btL3Selector
            anchors {
                top: btL2Selector.bottom
                right: btL4Selector.left
            }

            x: 0
            y: 128
            width: 128
            height: 128
            source: ""
            fillMode: Image.PreserveAspectFit

            AnimatedImage {
                id: btL3
                anchors.fill: parent
                x: 0
                y: 128
                width: 128
                height: 128
                currentFrame: 0
                playing: false
                source: ""
                fillMode: Image.PreserveAspectFit

                AnimatedImage {
                    id: btL3Ability
                    anchors.fill: parent
                    x: 0
                    y: 0
                    width: 128
                    height: 128
                    source: ""
                    fillMode: Image.PreserveAspectFit
                }

                MouseArea {
                    id: btL3Mouse
                    anchors.fill: parent
                    x: 0
                    y: 0
                    width: 128
                    height: 128
                }
            }
        }

        Image {
            id: btL1Selector
            anchors {
                right: parent.horizontalCenter
                bottom: btL4Selector.top
            }

            x: 127
            y: 40
            width: 128
            height: 128
            source: ""
            fillMode: Image.PreserveAspectFit

            AnimatedImage {
                id: btL1
                anchors.fill: parent
                x: 127
                y: 40
                width: 128
                height: 128
                source: ""
                fillMode: Image.PreserveAspectFit

                AnimatedImage {
                    id: btL1Ability
                    anchors.fill: parent
                    x: 0
                    y: 0
                    width: 128
                    height: 128
                    source: ""
                    fillMode: Image.PreserveAspectFit
                }

                MouseArea {
                    id: btL1Mouse
                    anchors.fill: parent
                    x: 0
                    y: 0
                    width: 128
                    height: 128
                }
            }
        }

        Image {
            id: btL2Selector
            anchors {
                top: parent.top
                right: btL1Selector.left
            }

            x: 0
            y: 0
            width: 128
            height: 128
            source: ""
            fillMode: Image.PreserveAspectFit

            AnimatedImage {
                id: btL2
                anchors.fill: parent
                x: 0
                y: 0
                width: 128
                height: 128
                source: ""
                fillMode: Image.PreserveAspectFit

                AnimatedImage {
                    id: btL2Ability
                    anchors.fill: parent
                    x: 0
                    y: 0
                    width: 128
                    height: 128
                    source: ""
                    fillMode: Image.PreserveAspectFit
                }

                MouseArea {
                    id: btL2Mouse
                    anchors.fill: parent
                    x: 0
                    y: 0
                    width: 128
                    height: 128
                }
            }
        }
    }

    ScrollView {
        id: textScroll
        anchors {
            bottom: parent.bottom
            top: arenaImg.bottom
            left: controlsRct.right
            right: parent.right
        }

        x: 129
        y: 297
        width: 511
        height: 63
        font.pixelSize: 10

        TextArea {
            id: textArea
            anchors.fill: parent
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
