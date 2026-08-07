import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page
    focus: true

    readonly property real pageMargin: width < 480 ? 16 : 24
    required property var controller
    required property var categoryState
    required property var cellState
    signal backRequested()
    signal saved()

    readonly property bool editMode: controller.hasSelectedTransaction
    readonly property bool amountValid: /^\d+(\.\d{1,2})?$/.test(
                                            amountField.text.trim())
    readonly property var selectedCell: cellState.selectedCell
    property var selectedCategoryId: 0
    property string selectedCategoryName: ""
    background: Rectangle { color: brand.background }
    BrandPalette { id: brand }
    Keys.onEscapePressed: page.backRequested()

    Component.onCompleted: {
        categoryState.loadCategories(selectedCell.cellId)
        if (editMode) {
            typeBox.currentIndex = controller.selectedTransaction.type === "EXPENSE" ? 1 : 0
            descriptionField.text = controller.selectedTransaction.description
            amountField.text = controller.selectedTransaction.amountInput
            dateSelector.setIsoValue(controller.selectedTransaction.dateInput)
            Qt.callLater(selectCurrentCategory)
        }
    }

    function selectCurrentCategory() {
        if (!editMode)
            return
        for (let index = 0; index < categoryState.categories.length; ++index) {
            if (categoryState.categories[index].categoryId ===
                    controller.selectedTransaction.categoryId) {
                selectedCategoryId = categoryState.categories[index].categoryId
                selectedCategoryName = categoryState.categories[index].name
                return
            }
        }
    }

    ScrollView {
        id: formScroll
        anchors.fill: parent
        contentWidth: availableWidth
        ColumnLayout {
            objectName: "formContent"
            anchors.horizontalCenter: parent.horizontalCenter
            width: Math.max(0, Math.min(
                formScroll.availableWidth - (2 * page.pageMargin), 440))
            spacing: 14

            Item { Layout.preferredHeight: 24 }
            Label {
                Layout.fillWidth: true
                text: page.editMode ? qsTr("Edit transaction") : qsTr("Add transaction")
                font.pixelSize: 26
                font.bold: true
                color: brand.navy
            }
            ComboBox {
                id: typeBox
                Layout.fillWidth: true
                model: [qsTr("Income"), qsTr("Expense")]
            }
            TextField {
                id: descriptionField
                Layout.fillWidth: true
                placeholderText: qsTr("Description")
                Accessible.name: qsTr("Transaction description")
                maximumLength: 200
                onTextEdited: controller.clearError()
            }
            TextField {
                id: amountField
                Layout.fillWidth: true
                placeholderText: qsTr("Amount, for example 12.34")
                Accessible.name: qsTr("Transaction amount")
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                onTextEdited: controller.clearError()
            }
            Label {
                Layout.fillWidth: true
                visible: amountField.text.length > 0 && !page.amountValid
                text: qsTr("Enter a positive amount with no more than two decimal places.")
                color: brand.danger
                wrapMode: Text.WordWrap
            }
            DateSelector {
                id: dateSelector
                Layout.fillWidth: true
                title: qsTr("Transaction date")
                onIsoValueChanged: controller.clearError()
            }
            Button {
                id: categoryButton
                Layout.fillWidth: true
                text: page.selectedCategoryId > 0
                      ? qsTr("Category: %1").arg(page.selectedCategoryName)
                      : qsTr("Choose a category")
                Accessible.description: qsTr("Opens a grid of transaction categories")
                onClicked: categoryPicker.open()
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
                text: page.editMode ? qsTr("Save changes") : qsTr("Add transaction")
                enabled: descriptionField.text.trim().length > 0 &&
                         page.amountValid &&
                         page.selectedCategoryId > 0
                palette.button: brand.green
                palette.buttonText: brand.navyDeep
                onClicked: {
                    const type = typeBox.currentIndex === 0 ? "INCOME" : "EXPENSE"
                    const successful = page.editMode
                        ? controller.updateTransaction(
                              page.selectedCell.cellId, type,
                              descriptionField.text, amountField.text,
                              dateSelector.isoValue, page.selectedCategoryId)
                        : controller.addTransaction(
                              page.selectedCell.cellId, type,
                              descriptionField.text, amountField.text,
                              dateSelector.isoValue, page.selectedCategoryId)
                    if (successful)
                        page.saved()
                }
            }
            Button {
                Layout.fillWidth: true
                text: qsTr("Cancel")
                flat: true
                palette.buttonText: brand.greenDark
                onClicked: page.backRequested()
            }
        }
    }

    CategoryPickerDialog {
        id: categoryPicker
        categories: categoryState.categories
        selectedCategoryId: page.selectedCategoryId
        selectedCategoryName: page.selectedCategoryName
        onCategorySelected: function(category) {
            page.selectedCategoryId = category.categoryId
            page.selectedCategoryName = category.name
            controller.clearError()
        }
    }
}
