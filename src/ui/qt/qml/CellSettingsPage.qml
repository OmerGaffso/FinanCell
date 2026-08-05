import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page
    focus: true
    required property var controller
    signal backRequested()
    signal cellDeleted()

    background: Rectangle { color: brand.background }
    BrandPalette { id: brand }
    Keys.onEscapePressed: page.backRequested()

    ScrollView {
        anchors.fill: parent
        contentWidth: availableWidth
        ColumnLayout {
            width: Math.min(page.width - 48, 440)
            x: (page.width - width) / 2
            spacing: 14

            Item { Layout.preferredHeight: 24 }
            Label {
                Layout.fillWidth: true
                text: qsTr("Cell settings")
                font.pixelSize: 26
                font.bold: true
                color: brand.navy
            }
            TextField {
                id: nameField
                Layout.fillWidth: true
                text: controller.hasSelectedCell ? controller.selectedCell.name : ""
                placeholderText: qsTr("Cell name")
                Accessible.name: qsTr("Financial cell name")
                maximumLength: 50
                onTextEdited: controller.clearError()
            }
            TextArea {
                id: descriptionField
                Layout.fillWidth: true
                Layout.preferredHeight: 120
                text: controller.hasSelectedCell ? controller.selectedCell.description : ""
                placeholderText: qsTr("Description")
                Accessible.name: qsTr("Financial cell description")
                wrapMode: TextEdit.Wrap
                onTextChanged: {
                    if (length > 200) remove(200, length)
                    controller.clearError()
                }
                background: Rectangle {
                    radius: 4
                    color: brand.surface
                    border.color: descriptionField.activeFocus ? brand.green : brand.border
                }
            }
            Label {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
                text: qsTr("%1 / 200").arg(descriptionField.length)
                color: brand.mutedText
            }
            Label {
                Layout.fillWidth: true
                visible: controller.errorMessage.length > 0
                text: controller.errorMessage
                color: brand.danger
                wrapMode: Text.WordWrap
            }
            Button {
                Layout.fillWidth: true
                text: qsTr("Save cell details")
                enabled: nameField.text.trim().length >= 3
                palette.button: brand.green
                palette.buttonText: brand.navyDeep
                onClicked: controller.updateSelectedCell(
                    nameField.text, descriptionField.text)
            }
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 1
                color: brand.border
            }
            Label {
                Layout.fillWidth: true
                text: qsTr("Danger zone")
                font.pixelSize: 18
                font.bold: true
                color: brand.danger
            }
            Button {
                Layout.fillWidth: true
                text: qsTr("Delete this financial cell")
                Accessible.description: qsTr("Permanently delete this cell and all of its records")
                palette.button: brand.danger
                palette.buttonText: "white"
                onClicked: deleteDialog.open()
            }
            Button {
                Layout.fillWidth: true
                text: qsTr("Back")
                flat: true
                palette.buttonText: brand.greenDark
                onClicked: page.backRequested()
            }
        }
    }

    Dialog {
        id: deleteDialog
        anchors.centerIn: parent
        modal: true
        title: qsTr("Delete financial cell?")
        Accessible.name: title
        standardButtons: Dialog.Ok | Dialog.Cancel
        Label {
            width: Math.min(300, implicitWidth)
            text: qsTr("This permanently deletes the cell, its memberships, categories, and transactions.")
            wrapMode: Text.WordWrap
        }
        onAccepted: {
            if (controller.deleteSelectedCell())
                page.cellDeleted()
        }
    }
}
