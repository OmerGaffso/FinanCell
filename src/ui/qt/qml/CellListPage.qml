import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page

    required property var controller
    signal backRequested()
    signal createRequested()
    signal cellSelected()

    background: Rectangle {
        color: brand.background
    }

    BrandPalette {
        id: brand
    }

    Component.onCompleted: controller.loadCells()

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 14

        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            Button {
                text: qsTr("Back")
                flat: true
                palette.buttonText: brand.greenDark
                onClicked: {
                    controller.clearError()
                    page.backRequested()
                }
            }

            Label {
                Layout.fillWidth: true
                text: qsTr("My financial cells")
                font.pixelSize: 24
                font.bold: true
                color: brand.navy
            }

            Image {
                Layout.preferredWidth: 44
                Layout.preferredHeight: 44
                source: brand.iconSource
                fillMode: Image.PreserveAspectFit
                mipmap: true
            }
        }

        Label {
            Layout.fillWidth: true
            text: qsTr("Your owned and shared financial spaces appear here.")
            wrapMode: Text.WordWrap
            color: brand.mutedText
        }

        Button {
            Layout.fillWidth: true
            text: qsTr("Create a financial cell")
            palette.button: brand.green
            palette.buttonText: brand.navyDeep
            onClicked: {
                controller.clearError()
                page.createRequested()
            }
        }

        Label {
            Layout.fillWidth: true
            visible: controller.errorMessage.length > 0
            text: controller.errorMessage
            color: brand.danger
            wrapMode: Text.WordWrap
        }

        ListView {
            id: cellList

            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 10
            model: controller.cells

            delegate: Rectangle {
                required property var modelData

                width: cellList.width
                height: details.implicitHeight + 28
                radius: 10
                color: brand.surface
                border.color: brand.border
                border.width: 1

                Column {
                    id: details

                    x: 14
                    y: 14
                    width: parent.width - 28
                    spacing: 4

                    Label {
                        width: parent.width
                        text: modelData.name
                        font.pixelSize: 18
                        font.bold: true
                        elide: Text.ElideRight
                        color: brand.navy
                    }

                    Label {
                        width: parent.width
                        visible: text.length > 0
                        text: modelData.description
                        wrapMode: Text.WordWrap
                        color: brand.mutedText
                    }

                    Label {
                        width: parent.width
                        text: qsTr("Currency: %1").arg(modelData.currency)
                        color: brand.greenDark
                    }

                    Item {
                        width: parent.width
                        height: openButton.implicitHeight

                        Button {
                            id: openButton

                            anchors.right: parent.right
                            text: qsTr("Open")
                            flat: true
                            palette.buttonText: brand.greenDark
                            onClicked: {
                                if (controller.selectCell(modelData.cellId))
                                    page.cellSelected()
                            }
                        }
                    }
                }
            }

            Label {
                anchors.centerIn: parent
                width: Math.min(parent.width, 320)
                visible: cellList.count === 0 && controller.errorMessage.length === 0
                horizontalAlignment: Text.AlignHCenter
                text: qsTr("You do not have any financial cells yet.")
                wrapMode: Text.WordWrap
                color: brand.mutedText
            }
        }
    }
}
