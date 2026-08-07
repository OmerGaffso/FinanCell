import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page
    focus: true

    required property var controller
    required property var cellState
    signal backRequested()
    signal addRequested()

    readonly property var selectedCell: cellState.selectedCell

    background: Rectangle { color: brand.background }
    BrandPalette { id: brand }

    Component.onCompleted: controller.loadMembers(selectedCell.cellId)
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
                text: qsTr("Members")
                font.pixelSize: 24
                font.bold: true
                color: brand.navy
            }
        }

        Label {
            Layout.fillWidth: true
            text: qsTr("People who can access %1").arg(selectedCell.name)
            wrapMode: Text.WordWrap
            color: brand.mutedText
        }

        Button {
            Layout.fillWidth: true
            visible: controller.canManage
            text: qsTr("Add a registered user")
            palette.button: brand.green
            palette.buttonText: brand.navyDeep
            onClicked: page.addRequested()
        }

        Label {
            Layout.fillWidth: true
            visible: controller.errorMessage.length > 0
            text: controller.errorMessage
            color: brand.danger
            wrapMode: Text.WordWrap
        }

        ListView {
            id: memberList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 10
            model: controller.members

            delegate: Rectangle {
                required property var modelData
                width: memberList.width
                height: content.implicitHeight + 28
                radius: 12
                color: brand.surface
                border.color: brand.border

                Column {
                    id: content
                    x: 14
                    y: 14
                    width: parent.width - 28
                    spacing: 6

                    Label {
                        width: parent.width
                        text: modelData.displayName
                        font.pixelSize: 17
                        font.bold: true
                        color: brand.navy
                        elide: Text.ElideRight
                    }
                    Label {
                        width: parent.width
                        text: qsTr("@%1 · %2").arg(modelData.username).arg(modelData.role)
                        color: modelData.isOwner ? brand.greenDark : brand.mutedText
                    }
                    Row {
                        visible: controller.canManage && !modelData.isOwner
                        spacing: 8

                        ComboBox {
                            id: roleBox
                            model: [qsTr("Member"), qsTr("Guest")]
                            currentIndex: modelData.role === "GUEST" ? 1 : 0
                            onActivated: controller.updateMemberRole(
                                page.selectedCell.cellId,
                                modelData.userId,
                                currentIndex === 0 ? "MEMBER" : "GUEST")
                        }
                        Button {
                            text: qsTr("Remove")
                            flat: true
                            palette.buttonText: brand.danger
                            onClicked: {
                                removeDialog.userId = modelData.userId
                                removeDialog.displayName = modelData.displayName
                                removeDialog.open()
                            }
                        }
                    }
                }
            }

            Label {
                anchors.centerIn: parent
                visible: memberList.count === 0 && controller.errorMessage.length === 0
                text: qsTr("No members are available for this cell.")
                color: brand.mutedText
            }
        }
    }

    Dialog {
        id: removeDialog
        property var userId: 0
        property string displayName: ""
        anchors.centerIn: parent
        modal: true
        title: qsTr("Remove member?")
        Accessible.name: title
        standardButtons: Dialog.Ok | Dialog.Cancel
        Label {
            text: qsTr("Remove %1 from this financial cell?").arg(
                      removeDialog.displayName)
            wrapMode: Text.WordWrap
        }
        onAccepted: controller.removeMember(
            page.selectedCell.cellId, userId)
    }
}
