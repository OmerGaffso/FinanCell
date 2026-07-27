import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    width: 420
    height: 720
    minimumWidth: 320
    minimumHeight: 480
    visible: true
    title: qsTr("FinanCell")

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 12

        Label {
            Layout.alignment: Qt.AlignHCenter
            text: qsTr("FinanCell")
            font.pixelSize: 28
        }

        Label {
            Layout.alignment: Qt.AlignHCenter
            text: qsTr("Graphical application shell")
        }
    }
}
