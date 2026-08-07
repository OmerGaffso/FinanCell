import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page
    focus: true
    required property var controller
    required property var cellState
    signal backRequested()

    readonly property var selectedCell: cellState.selectedCell
    background: Rectangle { color: brand.background }
    BrandPalette { id: brand }
    Component.onCompleted: controller.loadCategories(selectedCell.cellId)
    Keys.onEscapePressed: page.backRequested()

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 14

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
                text: qsTr("Categories")
                font.pixelSize: 24
                font.bold: true
                color: brand.navy
            }
        }

        RowLayout {
            Layout.fillWidth: true
            visible: controller.canCreate
            TextField {
                id: nameField
                Layout.fillWidth: true
                placeholderText: qsTr("New category")
                Accessible.name: qsTr("New category name")
                maximumLength: 50
                onTextEdited: controller.clearError()
                onAccepted: createButton.clicked()
            }
            Button {
                id: createButton
                text: qsTr("Add")
                enabled: nameField.text.trim().length > 0
                palette.button: brand.green
                palette.buttonText: brand.navyDeep
                onClicked: {
                    if (controller.createCategory(
                            page.selectedCell.cellId, nameField.text))
                        nameField.clear()
                }
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
            id: categoryList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 10
            model: controller.categories
            delegate: Rectangle {
                required property var modelData
                width: categoryList.width
                height: 58
                radius: 12
                color: brand.surface
                border.color: brand.border
                Label {
                    anchors.fill: parent
                    anchors.margins: 16
                    text: modelData.name
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 17
                    color: brand.navy
                }
            }

            Label {
                anchors.centerIn: parent
                visible: categoryList.count === 0 && controller.errorMessage.length === 0
                text: qsTr("No categories are available for this cell.")
                color: brand.mutedText
            }
        }
    }
}
