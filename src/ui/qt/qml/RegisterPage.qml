import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page

    required property var controller
    property string localError: ""
    signal backRequested()
    signal registrationCompleted()

    Component.onCompleted: controller.clearError()

    function clearMessages() {
        localError = ""
        controller.clearError()
    }

    ScrollView {
        anchors.fill: parent
        contentWidth: availableWidth

        ColumnLayout {
            width: Math.min(page.width - 48, 420)
            x: (page.width - width) / 2
            spacing: 14

            Item {
                Layout.preferredHeight: 32
            }

            Label {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                text: qsTr("Create your account")
                font.pixelSize: 26
                font.bold: true
                wrapMode: Text.WordWrap
            }

            TextField {
                id: usernameField
                Layout.fillWidth: true
                placeholderText: qsTr("Username")
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                onTextEdited: page.clearMessages()
            }

            TextField {
                id: displayNameField
                Layout.fillWidth: true
                placeholderText: qsTr("Display name")
                onTextEdited: page.clearMessages()
            }

            TextField {
                id: passwordField
                Layout.fillWidth: true
                placeholderText: qsTr("Password")
                echoMode: TextInput.Password
                onTextEdited: page.clearMessages()
            }

            TextField {
                id: confirmationField
                Layout.fillWidth: true
                placeholderText: qsTr("Confirm password")
                echoMode: TextInput.Password
                onTextEdited: page.clearMessages()
                onAccepted: registerButton.clicked()
            }

            Label {
                Layout.fillWidth: true
                visible: text.length > 0
                text: page.localError.length > 0
                      ? page.localError
                      : controller.errorMessage
                color: "#b3261e"
                wrapMode: Text.WordWrap
            }

            Button {
                id: registerButton
                Layout.fillWidth: true
                text: qsTr("Create account")
                enabled: usernameField.text.length > 0
                         && displayNameField.text.length > 0
                         && passwordField.text.length > 0
                         && confirmationField.text.length > 0
                onClicked: {
                    if (passwordField.text !== confirmationField.text) {
                        page.localError = qsTr("Passwords do not match.")
                        passwordField.clear()
                        confirmationField.clear()
                        return
                    }

                    const submittedPassword = passwordField.text
                    passwordField.clear()
                    confirmationField.clear()
                    if (controller.registerUser(
                            usernameField.text,
                            displayNameField.text,
                            submittedPassword)) {
                        page.registrationCompleted()
                    }
                }
            }

            Button {
                Layout.fillWidth: true
                text: qsTr("Back to sign in")
                flat: true
                onClicked: {
                    controller.clearError()
                    page.backRequested()
                }
            }
        }
    }
}
