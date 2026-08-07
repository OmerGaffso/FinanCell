import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page
    focus: true
    readonly property real pageMargin: width < 480 ? 16 : 24
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
        anchors.margins: page.pageMargin
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

        Label {
            Layout.fillWidth: true
            text: qsTr("Set an optional monthly spending limit for each category.")
            wrapMode: Text.WordWrap
            color: brand.mutedText
        }

        GridLayout {
            Layout.fillWidth: true
            visible: controller.canCreate
            columns: page.width < 480 ? 1 : 2
            columnSpacing: 10
            rowSpacing: 10
            TextField {
                id: nameField
                Layout.fillWidth: true
                Layout.minimumWidth: 0
                placeholderText: qsTr("New category")
                Accessible.name: qsTr("New category name")
                maximumLength: 50
                onTextEdited: controller.clearError()
                onAccepted: createButton.clicked()
            }
            Button {
                id: createButton
                Layout.fillWidth: page.width < 480
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
                height: categoryContent.implicitHeight + 28
                radius: 12
                color: brand.surface
                border.color: brand.border

                GridLayout {
                    id: categoryContent
                    anchors.fill: parent
                    anchors.margins: 14
                    columns: page.width < 480 ? 1 : 2
                    columnSpacing: 12
                    rowSpacing: 8

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 3
                        Label {
                            Layout.fillWidth: true
                            text: modelData.name
                            font.pixelSize: 17
                            font.bold: true
                            color: brand.navy
                            elide: Text.ElideRight
                        }
                        Label {
                            Layout.fillWidth: true
                            text: modelData.budgetText
                            color: modelData.hasBudget ? brand.greenDark : brand.mutedText
                            elide: Text.ElideRight
                        }
                    }

                    Button {
                        Layout.fillWidth: page.width < 480
                        visible: controller.canCreate
                        text: modelData.hasBudget ? qsTr("Change") : qsTr("Set budget")
                        Accessible.name: qsTr("Set monthly budget for %1").arg(modelData.name)
                        onClicked: budgetDialog.editCategory(modelData)
                    }
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

    Dialog {
        id: budgetDialog
        property var categoryId: 0
        property string categoryName: ""
        property string currentAmount: ""
        readonly property bool amountValid: /^\d+(\.\d{1,2})?$/.test(
                                                amountField.text.trim()) &&
                                            Number(amountField.text) > 0

        function editCategory(category) {
            categoryId = category.categoryId
            categoryName = category.name
            currentAmount = category.budgetInput
            amountField.text = currentAmount
            controller.clearError()
            open()
            amountField.forceActiveFocus()
        }

        anchors.centerIn: parent
        width: Math.min(page.width - 40, 380)
        modal: true
        title: qsTr("Monthly budget for %1").arg(categoryName)
        Accessible.name: title
        standardButtons: Dialog.Cancel

        ColumnLayout {
            width: parent.width
            spacing: 10
            Label {
                Layout.fillWidth: true
                text: qsTr("This limit repeats every month and is stored in the cell currency.")
                wrapMode: Text.WordWrap
                color: brand.mutedText
            }
            TextField {
                id: amountField
                Layout.fillWidth: true
                placeholderText: qsTr("Amount, for example 500.00")
                Accessible.name: qsTr("Monthly category budget")
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                onAccepted: {
                    if (budgetDialog.amountValid)
                        budgetDialog.accept()
                }
            }
            Label {
                Layout.fillWidth: true
                visible: amountField.text.length > 0 && !budgetDialog.amountValid
                text: qsTr("Enter a positive amount with no more than two decimal places.")
                wrapMode: Text.WordWrap
                color: brand.danger
            }
            Button {
                Layout.fillWidth: true
                text: qsTr("Save monthly budget")
                enabled: budgetDialog.amountValid
                palette.button: brand.green
                palette.buttonText: brand.navyDeep
                onClicked: {
                    if (controller.setMonthlyBudget(
                            page.selectedCell.cellId, budgetDialog.categoryId,
                            amountField.text))
                        budgetDialog.close()
                }
            }
            Button {
                Layout.fillWidth: true
                visible: budgetDialog.currentAmount.length > 0
                text: qsTr("Clear budget")
                flat: true
                palette.buttonText: brand.danger
                onClicked: {
                    if (controller.clearMonthlyBudget(
                            page.selectedCell.cellId, budgetDialog.categoryId))
                        budgetDialog.close()
                }
            }
        }
    }
}
