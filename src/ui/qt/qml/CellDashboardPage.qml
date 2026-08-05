import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page
    focus: true

    required property var controller
    signal backRequested()
    signal membersRequested()
    signal categoriesRequested()
    signal transactionsRequested()
    signal reportRequested()
    signal settingsRequested()

    background: Rectangle {
        color: brand.background
    }

    BrandPalette {
        id: brand
    }

    Keys.onEscapePressed: page.backRequested()

    ScrollView {
        anchors.fill: parent
        contentWidth: availableWidth

        ColumnLayout {
            width: Math.max(0, page.width - 48)
            x: 24
            spacing: 16

            Item { Layout.preferredHeight: 8 }

            RowLayout {
                Layout.fillWidth: true

                Button {
                    text: qsTr("Back")
                    flat: true
                    palette.buttonText: brand.greenDark
                    Accessible.name: qsTr("Back to financial cells")
                    onClicked: page.backRequested()
                }

                Item { Layout.fillWidth: true }

                Image {
                    Layout.preferredWidth: 52
                    Layout.preferredHeight: 52
                    source: brand.iconSource
                    fillMode: Image.PreserveAspectFit
                    mipmap: true
                    Accessible.name: qsTr("FinanCell logo")
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
                implicitHeight: balanceSummary.implicitHeight + 32
                radius: 14
                color: brand.surface
                border.color: brand.border

                Column {
                    id: balanceSummary

                    x: 16
                    y: 16
                    width: parent.width - 32
                    spacing: 6

                    Label {
                        width: parent.width
                        text: qsTr("Current balance")
                        color: brand.mutedText
                    }

                    Label {
                        width: parent.width
                        text: controller.hasSelectedCell
                              ? controller.selectedCell.balanceText
                              : qsTr("Unavailable")
                        font.pixelSize: 28
                        font.bold: true
                        color: !controller.hasSelectedCell
                               ? brand.mutedText
                               : controller.selectedCell.balanceInMinorUnits < 0
                                 ? brand.danger
                                 : controller.selectedCell.balanceInMinorUnits > 0
                                   ? brand.greenDark
                                   : brand.mutedText
                        Accessible.name: qsTr("Current balance: %1").arg(text)
                    }

                    Label {
                        width: parent.width
                        text: qsTr("Recorded income minus recorded expenses")
                        wrapMode: Text.WordWrap
                        color: brand.mutedText
                    }
                }
            }

            Label {
                Layout.fillWidth: true
                text: qsTr("Manage this cell")
                font.pixelSize: 18
                font.bold: true
                color: brand.navy
            }

            GridLayout {
                Layout.fillWidth: true
                columns: page.width >= 620 ? 2 : 1
                columnSpacing: 10
                rowSpacing: 10

                Button {
                    Layout.fillWidth: true
                    text: qsTr("Transactions")
                    palette.button: brand.green
                    palette.buttonText: brand.navyDeep
                    Accessible.description: qsTr("View, add, edit, and filter transactions")
                    onClicked: page.transactionsRequested()
                }

                Button {
                    Layout.fillWidth: true
                    text: qsTr("Monthly report")
                    flat: true
                    palette.buttonText: brand.greenDark
                    Accessible.description: qsTr("Generate a monthly financial summary")
                    onClicked: page.reportRequested()
                }

                Button {
                    Layout.fillWidth: true
                    text: qsTr("Members")
                    flat: true
                    palette.buttonText: brand.greenDark
                    Accessible.description: qsTr("View and manage cell members")
                    onClicked: page.membersRequested()
                }

                Button {
                    Layout.fillWidth: true
                    text: qsTr("Categories")
                    flat: true
                    palette.buttonText: brand.greenDark
                    Accessible.description: qsTr("View and create transaction categories")
                    onClicked: page.categoriesRequested()
                }

                Button {
                    Layout.fillWidth: true
                    visible: controller.canManageSelectedCell
                    text: qsTr("Cell settings")
                    flat: true
                    palette.buttonText: brand.danger
                    Accessible.description: qsTr("Edit or delete this financial cell")
                    onClicked: page.settingsRequested()
                }
            }

            Item { Layout.preferredHeight: 8 }
        }
    }
}
