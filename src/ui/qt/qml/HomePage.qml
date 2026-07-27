import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page

    required property var controller

    background: Rectangle {
        color: brand.background
    }

    BrandPalette {
        id: brand
    }

    ColumnLayout {
        anchors.centerIn: parent
        width: Math.min(parent.width - 48, 440)
        spacing: 16

        Image {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 112
            Layout.preferredHeight: 112
            source: brand.iconSource
            fillMode: Image.PreserveAspectFit
            mipmap: true
        }

        Label {
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            text: qsTr("Welcome, %1").arg(controller.displayName)
            font.pixelSize: 26
            font.bold: true
            wrapMode: Text.WordWrap
            color: brand.navy
        }

        Label {
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            text: qsTr("Signed in as @%1").arg(controller.username)
            wrapMode: Text.WordWrap
            color: brand.mutedText
        }

        Button {
            Layout.fillWidth: true
            text: qsTr("Log out")
            palette.button: brand.green
            palette.buttonText: brand.navyDeep
            onClicked: controller.logout()
        }
    }
}
