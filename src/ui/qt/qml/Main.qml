import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: window

    width: 420
    height: 720
    minimumWidth: 320
    minimumHeight: 480
    visible: true
    title: qsTr("FinanCell")

    readonly property bool backendReady: startupError.length === 0
                                         && userController !== null

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: window.backendReady ? loginPageComponent : startupFailureComponent
    }

    Connections {
        target: window.backendReady ? userController : null

        function onLoggedInChanged() {
            if (userController.loggedIn)
                stackView.replace(homePageComponent)
            else
                stackView.replace(loginPageComponent)
        }
    }

    Component {
        id: loginPageComponent

        LoginPage {
            controller: userController
            onRegistrationRequested: stackView.push(registerPageComponent)
        }
    }

    Component {
        id: registerPageComponent

        RegisterPage {
            controller: userController
            onBackRequested: stackView.pop()
            onRegistrationCompleted: {
                stackView.pop()
                stackView.currentItem.notice = qsTr("Account created. You can sign in now.")
            }
        }
    }

    Component {
        id: homePageComponent

        HomePage {
            controller: userController
        }
    }

    Component {
        id: startupFailureComponent

        Page {
            ColumnLayout {
                anchors.centerIn: parent
                width: Math.min(parent.width - 48, 440)
                spacing: 16

                Label {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    text: qsTr("FinanCell could not start")
                    font.pixelSize: 24
                    font.bold: true
                    wrapMode: Text.WordWrap
                }

                Label {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    text: startupError
                    wrapMode: Text.WordWrap
                }
            }
        }
    }
}
