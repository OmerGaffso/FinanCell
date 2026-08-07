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
    Component.onCompleted: controller.clearReport()
    Keys.onEscapePressed: page.backRequested()

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: page.pageMargin
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

        ColumnLayout {
            Layout.fillWidth: true
            Layout.maximumWidth: 520
            spacing: 10
            DateSelector {
                id: monthSelector
                Layout.fillWidth: true
                title: qsTr("Report month")
                monthOnly: true
                onIsoValueChanged: controller.clearError()
            }
            Button {
                id: generateButton
                Layout.fillWidth: true
                text: qsTr("Generate")
                palette.button: brand.green
                palette.buttonText: brand.navyDeep
                onClicked: controller.generateReport(
                    page.selectedCell.cellId, monthSelector.isoValue)
            }
        }

        Label {
            Layout.fillWidth: true
            visible: controller.errorMessage.length > 0
            text: controller.errorMessage
            color: brand.danger
            wrapMode: Text.WordWrap
        }

        Label {
            Layout.fillWidth: true
            visible: !controller.hasReport && controller.errorMessage.length === 0
            text: qsTr("Choose a month and generate a report to see income, expenses, balance, and category totals.")
            wrapMode: Text.WordWrap
            color: brand.mutedText
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
                height: reportContent.implicitHeight + 24
                radius: 12
                color: brand.surface
                border.color: modelData.overBudget ? brand.danger : brand.border
                border.width: modelData.overBudget ? 2 : 1
                ColumnLayout {
                    id: reportContent
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 4
                    Label {
                        Layout.fillWidth: true
                        text: modelData.categoryName
                        font.bold: true
                        color: brand.navy
                    }
                    Label {
                        Layout.fillWidth: true
                        text: qsTr("Income %1 · Expenses %2")
                              .arg(modelData.incomeText).arg(modelData.expensesText)
                        color: brand.mutedText
                        wrapMode: Text.WordWrap
                    }
                    Label {
                        Layout.fillWidth: true
                        visible: modelData.hasBudget
                        text: qsTr("Monthly budget: %1").arg(modelData.budgetText)
                        color: brand.navy
                    }
                    ProgressBar {
                        Layout.fillWidth: true
                        visible: modelData.hasBudget
                        from: 0
                        to: 1
                        value: modelData.budgetProgress
                        Accessible.name: qsTr("%1 budget usage: %2")
                                         .arg(modelData.categoryName)
                                         .arg(modelData.budgetUsageText)
                    }
                    Label {
                        Layout.fillWidth: true
                        visible: modelData.hasBudget
                        text: qsTr("%1 · %2")
                              .arg(modelData.budgetUsageText)
                              .arg(modelData.budgetStatusText)
                        color: modelData.overBudget ? brand.danger : brand.greenDark
                        font.bold: modelData.overBudget
                        wrapMode: Text.WordWrap
                    }
                }
            }
        }
    }
}
