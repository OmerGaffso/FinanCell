import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page
    focus: true

    readonly property real pageMargin: width < 480 ? 16 : 24

    required property var controller
    property bool selectionMode: false
    signal backRequested()
    signal userSelected(var user)

    background: Rectangle {
        color: brand.background
    }

    BrandPalette {
        id: brand
    }

    Keys.onEscapePressed: page.backRequested()

    Component.onCompleted: {
        controller.searchUsers("")
        searchField.forceActiveFocus()
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: page.pageMargin
        spacing: 14

        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            Button {
                text: qsTr("Back")
                flat: true
                palette.buttonText: brand.greenDark
                onClicked: {
                    controller.clearError()
                    page.backRequested()
                }
            }

            Label {
                Layout.fillWidth: true
                text: qsTr("Registered users")
                font.pixelSize: 24
                font.bold: true
                color: brand.navy
            }

            Image {
                Layout.preferredWidth: 44
                Layout.preferredHeight: 44
                source: brand.iconSource
                fillMode: Image.PreserveAspectFit
                mipmap: true
            }
        }

        Label {
            Layout.fillWidth: true
            text: qsTr("Find someone by username or display name. You can use their account when adding members to a financial cell.")
            wrapMode: Text.WordWrap
            color: brand.mutedText
        }

        GridLayout {
            Layout.fillWidth: true
            columns: page.width < 480 ? 1 : 2
            columnSpacing: 10
            rowSpacing: 10

            TextField {
                id: searchField
                Layout.fillWidth: true
                Layout.minimumWidth: 0
                placeholderText: qsTr("Search users")
                Accessible.name: qsTr("Search registered users")
                maximumLength: 50
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                onTextEdited: controller.clearError()
                onAccepted: searchButton.clicked()
            }

            Button {
                id: searchButton
                Layout.fillWidth: page.width < 480
                text: qsTr("Search")
                palette.button: brand.green
                palette.buttonText: brand.navyDeep
                onClicked: controller.searchUsers(searchField.text)
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
            text: qsTr("%1 user(s) found").arg(controller.users.length)
            color: brand.mutedText
        }

        ListView {
            id: userList

            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 10
            model: controller.users

            delegate: Rectangle {
                required property var modelData

                width: userList.width
                height: details.implicitHeight + 28
                radius: 10
                color: brand.surface
                border.color: brand.border
                border.width: 1

                ColumnLayout {
                    id: details

                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 3

                    Label {
                        Layout.fillWidth: true
                        text: modelData.displayName
                        font.pixelSize: 17
                        font.bold: true
                        elide: Text.ElideRight
                        color: brand.navy
                    }

                    Label {
                        Layout.fillWidth: true
                        text: qsTr("@%1").arg(modelData.username)
                        elide: Text.ElideRight
                        color: brand.greenDark
                    }

                    Label {
                        Layout.fillWidth: true
                        text: qsTr("User ID: %1").arg(modelData.userId)
                        color: brand.mutedText
                    }

                    Button {
                        Layout.alignment: Qt.AlignRight
                        visible: page.selectionMode
                        text: qsTr("Add")
                        flat: true
                        palette.buttonText: brand.greenDark
                        onClicked: page.userSelected(modelData)
                    }
                }
            }

            Label {
                anchors.centerIn: parent
                width: Math.min(parent.width, 320)
                visible: userList.count === 0 && controller.errorMessage.length === 0
                horizontalAlignment: Text.AlignHCenter
                text: qsTr("No registered users match this search.")
                wrapMode: Text.WordWrap
                color: brand.mutedText
            }
        }
    }
}
