import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page

    readonly property real pageMargin: width < 480 ? 16 : 24

    required property var controller
    property string notice: ""
    signal registrationRequested()

    background: Rectangle {
        color: brand.background
    }

    BrandPalette {
        id: brand
    }

    Component.onCompleted: {
        controller.clearError()
        usernameField.forceActiveFocus()
    }

    ScrollView {
        id: formScroll
        anchors.fill: parent
        contentWidth: availableWidth

        ColumnLayout {
            anchors.horizontalCenter: parent.horizontalCenter
            width: Math.max(0, Math.min(
                formScroll.availableWidth - (2 * page.pageMargin), 420))
            spacing: 14

            Item {
                Layout.preferredHeight: 36
            }

            Image {
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: page.width >= 600 ? Math.min(page.width - 96, 520) : 112
                Layout.preferredHeight: page.width >= 600 ? 180 : 112
                source: page.width >= 600 ? brand.lightBannerSource : brand.iconSource
                fillMode: Image.PreserveAspectFit
                mipmap: true
            }

            Label {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                text: qsTr("Welcome to FinanCell")
                font.pixelSize: 26
                font.bold: true
                wrapMode: Text.WordWrap
                color: brand.navy
            }

            Label {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                text: qsTr("Sign in to continue")
                color: brand.mutedText
            }

            Label {
                Layout.fillWidth: true
                visible: page.notice.length > 0
                text: page.notice
                color: brand.greenDark
                wrapMode: Text.WordWrap
            }

            TextField {
                id: usernameField
                Layout.fillWidth: true
                placeholderText: qsTr("Username")
                Accessible.name: qsTr("Username")
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
                Accessible.name: qsTr("Password")
                echoMode: TextInput.Password
                onTextEdited: controller.clearError()
                onAccepted: loginButton.clicked()
            }

            Label {
                Layout.fillWidth: true
                visible: controller.errorMessage.length > 0
                text: controller.errorMessage
                color: brand.danger
                wrapMode: Text.WordWrap
            }

            Button {
                id: loginButton
                Layout.fillWidth: true
                text: qsTr("Sign in")
                palette.button: brand.green
                palette.buttonText: brand.navyDeep
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
                palette.buttonText: brand.greenDark
                onClicked: {
                    controller.clearError()
                    page.registrationRequested()
                }
            }
        }
    }
}
