import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page

    required property var controller
    property string notice: ""
    signal registrationRequested()

    Component.onCompleted: controller.clearError()

    ScrollView {
        anchors.fill: parent
        contentWidth: availableWidth

        ColumnLayout {
            width: Math.min(page.width - 48, 420)
            x: (page.width - width) / 2
            spacing: 14

            Item {
                Layout.preferredHeight: 56
            }

            Label {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                text: qsTr("Welcome to FinanCell")
                font.pixelSize: 26
                font.bold: true
                wrapMode: Text.WordWrap
            }

            Label {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                text: qsTr("Sign in to continue")
            }

            Label {
                Layout.fillWidth: true
                visible: page.notice.length > 0
                text: page.notice
                color: "#287a3d"
                wrapMode: Text.WordWrap
            }

            TextField {
                id: usernameField
                Layout.fillWidth: true
                placeholderText: qsTr("Username")
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                onTextEdited: {
                    page.notice = ""
                    controller.clearError()
                }
            }

            TextField {
                id: passwordField
                Layout.fillWidth: true
                placeholderText: qsTr("Password")
                echoMode: TextInput.Password
                onTextEdited: controller.clearError()
                onAccepted: loginButton.clicked()
            }

            Label {
                Layout.fillWidth: true
                visible: controller.errorMessage.length > 0
                text: controller.errorMessage
                color: "#b3261e"
                wrapMode: Text.WordWrap
            }

            Button {
                id: loginButton
                Layout.fillWidth: true
                text: qsTr("Sign in")
                enabled: usernameField.text.length > 0 && passwordField.text.length > 0
                onClicked: {
                    const submittedPassword = passwordField.text
                    passwordField.clear()
                    controller.login(usernameField.text, submittedPassword)
                }
            }

            Button {
                Layout.fillWidth: true
                text: qsTr("Create an account")
                flat: true
                onClicked: {
                    controller.clearError()
                    page.registrationRequested()
                }
            }
        }
    }
}
