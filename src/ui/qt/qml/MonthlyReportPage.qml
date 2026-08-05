import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page
    required property var controller
    required property var cellController
    signal backRequested()

    readonly property var selectedCell: cellController.selectedCell
    background: Rectangle { color: brand.background }
    BrandPalette { id: brand }
    Component.onCompleted: controller.clearReport()

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
                text: qsTr("Monthly report")
                font.pixelSize: 24
                font.bold: true
                color: brand.navy
            }
        }

        RowLayout {
            Layout.fillWidth: true
            TextField {
                id: monthField
                Layout.fillWidth: true
                placeholderText: qsTr("Month YYYY-MM")
                maximumLength: 7
                onTextEdited: controller.clearError()
                onAccepted: generateButton.clicked()
            }
            Button {
                id: generateButton
                text: qsTr("Generate")
                enabled: monthField.text.length === 7
                palette.button: brand.green
                palette.buttonText: brand.navyDeep
                onClicked: controller.generateReport(
                    page.selectedCell.cellId, monthField.text)
            }
        }

        Label {
            Layout.fillWidth: true
            visible: controller.errorMessage.length > 0
            text: controller.errorMessage
            color: brand.danger
            wrapMode: Text.WordWrap
        }

        GridLayout {
            Layout.fillWidth: true
            visible: controller.hasReport
            columns: page.width >= 600 ? 3 : 1
            columnSpacing: 10
            rowSpacing: 10

            Repeater {
                model: [
                    {"label": qsTr("Income"), "value": controller.totalIncomeText,
                     "color": brand.greenDark},
                    {"label": qsTr("Expenses"), "value": controller.totalExpensesText,
                     "color": brand.danger},
                    {"label": qsTr("Balance"), "value": controller.balanceText,
                     "color": controller.balanceText.startsWith("-")
                              ? brand.danger : brand.greenDark}
                ]
                delegate: Rectangle {
                    required property var modelData
                    Layout.fillWidth: true
                    implicitHeight: 82
                    radius: 12
                    color: brand.surface
                    border.color: brand.border
                    Column {
                        anchors.centerIn: parent
                        spacing: 4
                        Label { text: modelData.label; color: brand.mutedText }
                        Label {
                            text: modelData.value
                            font.pixelSize: 18
                            font.bold: true
                            color: modelData.color
                        }
                    }
                }
            }
        }

        Label {
            Layout.fillWidth: true
            visible: controller.hasReport
            text: qsTr("Breakdown by category")
            font.pixelSize: 18
            font.bold: true
            color: brand.navy
        }

        ListView {
            id: reportList
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: controller.hasReport
            clip: true
            spacing: 8
            model: controller.categoryLines
            delegate: Rectangle {
                required property var modelData
                width: reportList.width
                height: 82
                radius: 12
                color: brand.surface
                border.color: brand.border
                Column {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 4
                    Label { text: modelData.categoryName; font.bold: true; color: brand.navy }
                    Label {
                        text: qsTr("Income %1 · Expenses %2")
                              .arg(modelData.incomeText).arg(modelData.expensesText)
                        color: brand.mutedText
                    }
                }
            }
        }
    }
}
