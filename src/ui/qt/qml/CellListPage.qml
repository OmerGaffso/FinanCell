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

        GridView {
            id: cellGrid

            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: controller.cells
            readonly property real targetCellSize: 180
            readonly property int columnCount: Math.max(
                1, Math.floor(width / targetCellSize))
            cellWidth: width / columnCount
            cellHeight: cellWidth

            delegate: Item {
                required property var modelData

                width: cellGrid.cellWidth
                height: cellGrid.cellHeight

                Button {
                    id: cellButton

                    anchors.fill: parent
                    anchors.margins: 6
                    padding: 16
                    text: modelData.name
                    onClicked: {
                        if (controller.selectCell(modelData.cellId))
                            page.cellSelected()
                    }

                    background: Rectangle {
                        radius: 18
                        color: cellButton.down
                               ? brand.border
                               : cellButton.hovered
                                 ? "#eaf3ed"
                                 : brand.surface
                        border.color: cellButton.activeFocus
                                      ? brand.green
                                      : brand.border
                        border.width: cellButton.activeFocus ? 2 : 1
                    }

                    contentItem: Item {
                        Label {
                            anchors.top: parent.top
                            anchors.left: parent.left
                            anchors.right: parent.right
                            text: cellButton.text
                            font.pixelSize: 18
                            font.bold: true
                            maximumLineCount: 2
                            elide: Text.ElideRight
                            wrapMode: Text.WordWrap
                            color: brand.navy
                        }

                        Label {
                            id: balanceCaption

                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.bottom: balanceValue.top
                            anchors.bottomMargin: 4
                            text: qsTr("Current balance")
                            horizontalAlignment: Text.AlignHCenter
                            color: brand.mutedText
                        }

                        Label {
                            id: balanceValue

                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom
                            text: modelData.balanceText
                            horizontalAlignment: Text.AlignHCenter
                            font.pixelSize: 20
                            font.bold: true
                            color: modelData.balanceInMinorUnits < 0
                                   ? brand.danger
                                   : modelData.balanceInMinorUnits > 0
                                     ? brand.greenDark
                                     : brand.mutedText
                            elide: Text.ElideRight
                        }
                    }
                }
            }

            Label {
                anchors.centerIn: parent
                width: Math.min(parent.width, 320)
                visible: cellGrid.count === 0 && controller.errorMessage.length === 0
                horizontalAlignment: Text.AlignHCenter
                text: qsTr("You do not have any financial cells yet.")
                wrapMode: Text.WordWrap
                color: brand.mutedText
            }
        }
    }
}
