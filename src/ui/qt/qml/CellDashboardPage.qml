import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page

    required property var controller
    signal backRequested()
    signal membersRequested()

    background: Rectangle {
        color: brand.background
    }

    BrandPalette {
        id: brand
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16

        RowLayout {
            Layout.fillWidth: true

            Button {
                text: qsTr("Back")
                flat: true
                palette.buttonText: brand.greenDark
                onClicked: page.backRequested()
            }

            Item {
                Layout.fillWidth: true
            }

            Image {
                Layout.preferredWidth: 52
                Layout.preferredHeight: 52
                source: brand.iconSource
                fillMode: Image.PreserveAspectFit
                mipmap: true
            }
        }

        Label {
            Layout.fillWidth: true
            text: controller.hasSelectedCell
                  ? controller.selectedCell.name
                  : qsTr("Financial cell")
            font.pixelSize: 28
            font.bold: true
            wrapMode: Text.WordWrap
            color: brand.navy
        }

        Label {
            Layout.fillWidth: true
            visible: controller.hasSelectedCell
                     && controller.selectedCell.description.length > 0
            text: controller.hasSelectedCell
                  ? controller.selectedCell.description
                  : ""
            wrapMode: Text.WordWrap
            color: brand.mutedText
        }

        Rectangle {
            Layout.fillWidth: true
            implicitHeight: summary.implicitHeight + 32
            radius: 12
            color: brand.surface
            border.color: brand.border

            Column {
                id: summary

                x: 16
                y: 16
                width: parent.width - 32
                spacing: 8

                Label {
                    text: qsTr("Cell workspace ready")
                    font.pixelSize: 18
                    font.bold: true
                    color: brand.navy
                }

                Label {
                    width: parent.width
                    text: qsTr("Currency: %1").arg(
                              controller.hasSelectedCell
                              ? controller.selectedCell.currency
                              : "")
                    color: brand.greenDark
                }

                Label {
                    width: parent.width
                    text: qsTr("Transactions, categories, members, and reports will be added to this dashboard in the next UI stages.")
                    wrapMode: Text.WordWrap
                    color: brand.mutedText
                }
            }
        }

        Button {
            Layout.fillWidth: true
            text: qsTr("Members")
            palette.button: brand.green
            palette.buttonText: brand.navyDeep
            onClicked: page.membersRequested()
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
