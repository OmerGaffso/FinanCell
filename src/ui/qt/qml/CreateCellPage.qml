import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page
    focus: true

    readonly property real pageMargin: width < 480 ? 16 : 24

    required property var controller
    signal backRequested()
    signal cellCreated()

    background: Rectangle {
        color: brand.background
    }

    BrandPalette {
        id: brand
    }

    Keys.onEscapePressed: page.backRequested()

    Component.onCompleted: {
        controller.clearError()
        nameField.forceActiveFocus()
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

            Item {
                Layout.preferredHeight: 24
            }

            Image {
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: 88
                Layout.preferredHeight: 88
                source: brand.iconSource
                fillMode: Image.PreserveAspectFit
                mipmap: true
            }

            Label {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                text: qsTr("Create a financial cell")
                font.pixelSize: 26
                font.bold: true
                wrapMode: Text.WordWrap
                color: brand.navy
            }

            Label {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                text: qsTr("New cells use ILS as their currency.")
                wrapMode: Text.WordWrap
                color: brand.mutedText
            }

            TextField {
                id: nameField
                Layout.fillWidth: true
                placeholderText: qsTr("Cell name")
                Accessible.name: qsTr("Financial cell name")
                maximumLength: 50
                onTextEdited: controller.clearError()
            }

            Label {
                Layout.fillWidth: true
                visible: nameField.text.length > 0 && nameField.text.trim().length < 3
                text: qsTr("Use at least 3 characters for the cell name.")
                color: brand.danger
                wrapMode: Text.WordWrap
            }

            TextArea {
                id: descriptionField
                Layout.fillWidth: true
                Layout.preferredHeight: 120
                placeholderText: qsTr("Description (optional)")
                Accessible.name: qsTr("Financial cell description")
                wrapMode: TextEdit.Wrap
                selectByMouse: true
                onTextChanged: {
                    if (length > 200)
                        remove(200, length)
                    controller.clearError()
                }

                background: Rectangle {
                    radius: 4
                    color: brand.surface
                    border.color: descriptionField.activeFocus
                                  ? brand.green
                                  : brand.border
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
                text: qsTr("Create cell")
                enabled: nameField.text.trim().length >= 3
                palette.button: brand.green
                palette.buttonText: brand.navyDeep
                onClicked: {
                    if (controller.createCell(
                            nameField.text,
                            descriptionField.text))
                        page.cellCreated()
                }
            }

            Button {
                Layout.fillWidth: true
                text: qsTr("Back to my cells")
                flat: true
                palette.buttonText: brand.greenDark
                onClicked: {
                    controller.clearError()
                    page.backRequested()
                }
            }
        }
    }
}
