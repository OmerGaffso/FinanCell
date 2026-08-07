import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page
    focus: true

    readonly property real pageMargin: width < 480 ? 16 : 24

    required property var controller
    property string localError: ""
    signal backRequested()
    signal registrationCompleted()

    background: Rectangle {
        color: brand.background
    }

    BrandPalette {
        id: brand
    }

    Keys.onEscapePressed: page.backRequested()

    Component.onCompleted: {
        controller.clearError()
        usernameField.forceActiveFocus()
    }

    function clearMessages() {
        localError = ""
        controller.clearError()
    }

    ScrollView {
        id: formScroll
        anchors.fill: parent
        contentWidth: availableWidth

        ColumnLayout {
            objectName: "formContent"
            anchors.horizontalCenter: parent.horizontalCenter
            width: Math.max(0, Math.min(
                formScroll.availableWidth - (2 * page.pageMargin), 420))
            spacing: 14

            Item {
                Layout.preferredHeight: 24
            }

            Image {
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: 88
                Layout.preferredHeight: 88
                source: brand.iconSource
                fillMode: Image.PreserveAspectFit
                mipmap: true
            }

            Label {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                text: qsTr("Create your account")
                font.pixelSize: 26
                font.bold: true
                wrapMode: Text.WordWrap
                color: brand.navy
            }

            TextField {
                id: usernameField
                Layout.fillWidth: true
                placeholderText: qsTr("Username")
                Accessible.name: qsTr("Username")
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                onTextEdited: page.clearMessages()
            }

            TextField {
                id: displayNameField
                Layout.fillWidth: true
                placeholderText: qsTr("Display name")
                Accessible.name: qsTr("Display name")
                onTextEdited: page.clearMessages()
            }

            TextField {
                id: passwordField
                Layout.fillWidth: true
                placeholderText: qsTr("Password")
                Accessible.name: qsTr("Password")
                echoMode: TextInput.Password
                onTextEdited: page.clearMessages()
            }

            TextField {
                id: confirmationField
                Layout.fillWidth: true
                placeholderText: qsTr("Confirm password")
                Accessible.name: qsTr("Confirm password")
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
                color: brand.danger
                wrapMode: Text.WordWrap
            }

            Button {
                id: registerButton
                Layout.fillWidth: true
                text: qsTr("Create account")
                palette.button: brand.green
                palette.buttonText: brand.navyDeep
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
                palette.buttonText: brand.greenDark
                onClicked: {
                    controller.clearError()
                    page.backRequested()
                }
            }
        }
    }
}
