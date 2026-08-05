import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: window

    width: 420
    height: 720
    minimumWidth: 320
    minimumHeight: 480
    visible: true
    title: qsTr("FinanCell")
    color: brand.background
    palette.window: brand.background
    palette.windowText: brand.navy
    palette.base: brand.surface
    palette.text: brand.navy
    palette.button: brand.green
    palette.buttonText: brand.navyDeep
    palette.highlight: brand.green
    palette.highlightedText: brand.navyDeep
    palette.placeholderText: brand.mutedText

    BrandPalette {
        id: brand
    }

    readonly property bool backendReady: startupError.length === 0
                                         && userController !== null
                                         && cellController !== null
                                         && memberController !== null
                                         && categoryController !== null
                                         && transactionController !== null
                                         && reportController !== null

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: window.backendReady ? loginPageComponent : startupFailureComponent
    }

    Connections {
        target: window.backendReady ? userController : null

        function onLoggedInChanged() {
            if (userController.loggedIn)
                stackView.replace(homePageComponent)
            else
                stackView.replace(loginPageComponent)
        }
    }

    Component {
        id: loginPageComponent

        LoginPage {
            controller: userController
            onRegistrationRequested: stackView.push(registerPageComponent)
        }
    }

    Component {
        id: registerPageComponent

        RegisterPage {
            controller: userController
            onBackRequested: stackView.pop()
            onRegistrationCompleted: {
                stackView.pop()
                stackView.currentItem.notice = qsTr("Account created. You can sign in now.")
            }
        }
    }

    Component {
        id: homePageComponent

        HomePage {
            controller: userController
            onFinancialCellsRequested: stackView.push(cellListPageComponent)
            onUserLookupRequested: stackView.push(userLookupPageComponent)
        }
    }

    Component {
        id: cellListPageComponent

        CellListPage {
            controller: cellController
            onBackRequested: stackView.pop()
            onCreateRequested: stackView.push(createCellPageComponent)
            onCellSelected: stackView.push(cellDashboardPageComponent)
        }
    }

    Component {
        id: createCellPageComponent

        CreateCellPage {
            controller: cellController
            onBackRequested: stackView.pop()
            onCellCreated: stackView.pop()
        }
    }

    Component {
        id: cellDashboardPageComponent

        CellDashboardPage {
            controller: cellController
            onMembersRequested: stackView.push(memberPageComponent)
            onCategoriesRequested: stackView.push(categoryPageComponent)
            onTransactionsRequested: stackView.push(transactionPageComponent)
            onReportRequested: stackView.push(monthlyReportPageComponent)
            onBackRequested: {
                stackView.pop()
                cellController.clearSelection()
            }
        }
    }

    Component {
        id: monthlyReportPageComponent

        MonthlyReportPage {
            controller: reportController
            cellController: cellController
            onBackRequested: stackView.pop()
        }
    }

    Component {
        id: transactionPageComponent

        TransactionPage {
            controller: transactionController
            cellController: cellController
            onBackRequested: stackView.pop()
            onAddRequested: stackView.push(transactionFormPageComponent)
            onEditRequested: stackView.push(transactionFormPageComponent)
        }
    }

    Component {
        id: transactionFormPageComponent

        TransactionFormPage {
            controller: transactionController
            categoryController: categoryController
            cellController: cellController
            onBackRequested: {
                transactionController.clearSelection()
                stackView.pop()
            }
            onSaved: stackView.pop()
        }
    }

    Component {
        id: categoryPageComponent

        CategoryPage {
            controller: categoryController
            cellController: cellController
            onBackRequested: stackView.pop()
        }
    }

    Component {
        id: memberPageComponent

        MemberPage {
            controller: memberController
            cellController: cellController
            onBackRequested: stackView.pop()
            onAddRequested: stackView.push(memberLookupPageComponent)
        }
    }

    Component {
        id: memberLookupPageComponent

        UserLookupPage {
            controller: userController
            selectionMode: true
            onBackRequested: stackView.pop()
            onUserSelected: function(user) {
                memberController.addMember(
                    cellController.selectedCell.cellId,
                    user.userId,
                    "MEMBER")
                stackView.pop()
            }
        }
    }

    Component {
        id: userLookupPageComponent

        UserLookupPage {
            controller: userController
            onBackRequested: stackView.pop()
        }
    }

    Component {
        id: startupFailureComponent

        Page {
            background: Rectangle {
                color: brand.background
            }

            ColumnLayout {
                anchors.centerIn: parent
                width: Math.min(parent.width - 48, 440)
                spacing: 16

                Image {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.preferredWidth: 96
                    Layout.preferredHeight: 96
                    source: brand.iconSource
                    fillMode: Image.PreserveAspectFit
                    mipmap: true
                }

                Label {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    text: qsTr("FinanCell could not start")
                    font.pixelSize: 24
                    font.bold: true
                    wrapMode: Text.WordWrap
                    color: brand.navy
                }

                Label {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    text: startupError
                    wrapMode: Text.WordWrap
                    color: brand.mutedText
                }
            }
        }
    }
}
