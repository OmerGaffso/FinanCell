import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

/**
 * @brief Modal grid used to choose a transaction category without a drop-down menu.
 */
Dialog {
    id: dialog
    objectName: "categoryPickerDialog"

    property var categories: []
    property var selectedCategoryId: 0
    property string selectedCategoryName: ""
    signal categorySelected(var category)

    function choose(category) {
        selectedCategoryId = category.categoryId
        selectedCategoryName = category.name
        categorySelected(category)
        close()
    }

    anchors.centerIn: parent
    width: Math.min(parent ? parent.width - 32 : 420, 520)
    height: Math.min(parent ? parent.height - 48 : 520, 520)
    modal: true
    title: qsTr("Choose a category")
    Accessible.name: title
    standardButtons: Dialog.Cancel

    BrandPalette { id: brand }

    contentItem: ColumnLayout {
        spacing: 12

        Label {
            Layout.fillWidth: true
            text: qsTr("Select the square that best matches this transaction.")
            wrapMode: Text.WordWrap
            color: brand.mutedText
        }

        GridView {
            id: categoryGrid
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: dialog.categories
            readonly property real targetSize: 130
            readonly property int columnCount: Math.max(
                1, Math.floor(width / targetSize))
            cellWidth: width / columnCount
            cellHeight: cellWidth

            delegate: Item {
                required property var modelData
                width: categoryGrid.cellWidth
                height: categoryGrid.cellHeight

                Button {
                    id: categoryButton
                    anchors.fill: parent
                    anchors.margins: 6
                    text: modelData.name
                    Accessible.name: qsTr("Choose %1 category").arg(modelData.name)
                    onClicked: dialog.choose(modelData)

                    background: Rectangle {
                        radius: 16
                        color: categoryButton.down
                               ? brand.border
                               : dialog.selectedCategoryId === modelData.categoryId
                                 ? "#e3f2e8"
                                 : categoryButton.hovered ? "#eaf3ed" : brand.surface
                        border.color: dialog.selectedCategoryId === modelData.categoryId ||
                                      categoryButton.activeFocus
                                      ? brand.green : brand.border
                        border.width: dialog.selectedCategoryId === modelData.categoryId ||
                                      categoryButton.activeFocus ? 2 : 1
                    }

                    contentItem: Label {
                        text: categoryButton.text
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        wrapMode: Text.WordWrap
                        maximumLineCount: 3
                        elide: Text.ElideRight
                        font.pixelSize: 16
                        font.bold: true
                        color: brand.navy
                    }
                }
            }

            Label {
                anchors.centerIn: parent
                visible: categoryGrid.count === 0
                text: qsTr("Create a category before adding a transaction.")
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
                color: brand.mutedText
            }
        }
    }
}
