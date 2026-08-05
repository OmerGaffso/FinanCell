import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page
    required property var controller
    required property var cellState
    signal backRequested()
    signal addRequested()
    signal editRequested()

    readonly property var selectedCell: cellState.selectedCell
    background: Rectangle { color: brand.background }
    BrandPalette { id: brand }
    Component.onCompleted: controller.loadTransactions(selectedCell.cellId)

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 12

        RowLayout {
            Layout.fillWidth: true
            Button {
                text: qsTr("Back")
                flat: true
                palette.buttonText: brand.greenDark
                onClicked: page.backRequested()
            }
            Label {
                Layout.fillWidth: true
                text: qsTr("Transactions")
                font.pixelSize: 24
                font.bold: true
                color: brand.navy
            }
        }

        Label {
            Layout.fillWidth: true
            text: qsTr("Current balance: %1").arg(controller.balanceText)
            font.pixelSize: 18
            font.bold: true
            color: brand.navy
        }

        Button {
            Layout.fillWidth: true
            visible: controller.canWrite
            text: qsTr("Add income or expense")
            palette.button: brand.green
            palette.buttonText: brand.navyDeep
            onClicked: {
                controller.clearSelection()
                page.addRequested()
            }
        }

        RowLayout {
            Layout.fillWidth: true
            TextField {
                id: fromDate
                Layout.fillWidth: true
                placeholderText: qsTr("From YYYY-MM-DD")
                maximumLength: 10
            }
            TextField {
                id: toDate
                Layout.fillWidth: true
                placeholderText: qsTr("To YYYY-MM-DD")
                maximumLength: 10
            }
            Button {
                text: qsTr("Apply")
                onClicked: controller.loadTransactions(
                    page.selectedCell.cellId, fromDate.text, toDate.text)
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
            id: transactionList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 10
            model: controller.transactions

            delegate: Rectangle {
                required property var modelData
                width: transactionList.width
                height: details.implicitHeight + 28
                radius: 12
                color: brand.surface
                border.color: brand.border

                Column {
                    id: details
                    x: 14
                    y: 14
                    width: parent.width - 28
                    spacing: 5

                    Row {
                        width: parent.width
                        spacing: 8
                        Label {
                            width: Math.max(0, parent.width - amountLabel.implicitWidth - 8)
                            text: modelData.description
                            font.bold: true
                            color: brand.navy
                            elide: Text.ElideRight
                        }
                        Label {
                            id: amountLabel
                            text: (modelData.type === "EXPENSE" ? "-" : "") + modelData.amountText
                            font.bold: true
                            color: modelData.type === "INCOME" ? brand.greenDark : brand.danger
                        }
                    }
                    Label {
                        width: parent.width
                        text: qsTr("%1 · %2 · %3")
                              .arg(modelData.categoryName)
                              .arg(modelData.type)
                              .arg(modelData.occurredAt)
                        color: brand.mutedText
                        elide: Text.ElideRight
                    }
                    Row {
                        visible: modelData.editable
                        spacing: 8
                        Button {
                            text: qsTr("Edit")
                            flat: true
                            palette.buttonText: brand.greenDark
                            onClicked: {
                                if (controller.selectTransaction(modelData.transactionId))
                                    page.editRequested()
                            }
                        }
                        Button {
                            text: qsTr("Delete")
                            flat: true
                            palette.buttonText: brand.danger
                            onClicked: {
                                deleteDialog.transactionId = modelData.transactionId
                                deleteDialog.open()
                            }
                        }
                    }
                }
            }

            Label {
                anchors.centerIn: parent
                visible: transactionList.count === 0 && controller.errorMessage.length === 0
                text: qsTr("No transactions have been recorded.")
                color: brand.mutedText
            }
        }
    }

    Dialog {
        id: deleteDialog
        property var transactionId: 0
        anchors.centerIn: parent
        modal: true
        title: qsTr("Delete transaction?")
        standardButtons: Dialog.Ok | Dialog.Cancel
        Label { text: qsTr("This transaction will be permanently deleted.") }
        onAccepted: controller.deleteTransaction(
            page.selectedCell.cellId, transactionId)
    }
}
