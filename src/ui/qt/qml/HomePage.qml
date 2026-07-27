import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page

    required property var controller

    ColumnLayout {
        anchors.centerIn: parent
        width: Math.min(parent.width - 48, 440)
        spacing: 16

        Label {
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            text: qsTr("Welcome, %1").arg(controller.displayName)
            font.pixelSize: 26
            font.bold: true
            wrapMode: Text.WordWrap
        }

        Label {
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            text: qsTr("Signed in as @%1").arg(controller.username)
            wrapMode: Text.WordWrap
        }

        Button {
            Layout.fillWidth: true
            text: qsTr("Log out")
            onClicked: controller.logout()
        }
    }
}
