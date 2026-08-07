import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: window

    width: 1200
    height: 800
    minimumWidth: 700
    minimumHeight: 500
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
        objectName: "mainStackView"
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
            objectName: "loginPage"
            controller: userController
            onRegistrationRequested: stackView.push(registerPageComponent)
        }
    }

    Component {
        id: registerPageComponent

        RegisterPage {
            objectName: "registerPage"
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
            objectName: "homePage"
            controller: userController
            onFinancialCellsRequested: stackView.push(cellListPageComponent)
            onUserLookupRequested: stackView.push(userLookupPageComponent)
        }
    }

    Component {
        id: cellListPageComponent

        CellListPage {
            objectName: "cellListPage"
            controller: cellController
            onBackRequested: stackView.pop()
            onCreateRequested: stackView.push(createCellPageComponent)
            onCellSelected: stackView.push(cellDashboardPageComponent)
        }
    }

    Component {
        id: createCellPageComponent

        CreateCellPage {
            objectName: "createCellPage"
            controller: cellController
            onBackRequested: stackView.pop()
            onCellCreated: stackView.pop()
        }
    }

    Component {
        id: cellDashboardPageComponent

        CellDashboardPage {
            objectName: "cellDashboardPage"
            controller: cellController
            onMembersRequested: stackView.push(memberPageComponent)
            onCategoriesRequested: stackView.push(categoryPageComponent)
            onTransactionsRequested: stackView.push(transactionPageComponent)
            onReportRequested: stackView.push(monthlyReportPageComponent)
            onSettingsRequested: stackView.push(cellSettingsPageComponent)
            onBackRequested: {
                cellController.loadCells()
                stackView.pop()
                cellController.clearSelection()
            }
        }
    }

    Component {
        id: cellSettingsPageComponent

        CellSettingsPage {
            objectName: "cellSettingsPage"
            controller: cellController
            onBackRequested: stackView.pop()
            onCellDeleted: {
                const cellList = stackView.get(stackView.depth - 3)
                stackView.pop(cellList)
            }
        }
    }

    Component {
        id: monthlyReportPageComponent

        MonthlyReportPage {
            objectName: "monthlyReportPage"
            controller: reportController
            cellState: cellController
            onBackRequested: stackView.pop()
        }
    }

    Component {
        id: transactionPageComponent

        TransactionPage {
            objectName: "transactionPage"
            controller: transactionController
            cellState: cellController
            onBackRequested: stackView.pop()
            onAddRequested: stackView.push(transactionFormPageComponent)
            onEditRequested: stackView.push(transactionFormPageComponent)
        }
    }

    Component {
        id: transactionFormPageComponent

        TransactionFormPage {
            objectName: "transactionFormPage"
            controller: transactionController
            categoryState: categoryController
            cellState: cellController
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
            objectName: "categoryPage"
            controller: categoryController
            cellState: cellController
            onBackRequested: stackView.pop()
        }
    }

    Component {
        id: memberPageComponent

        MemberPage {
            objectName: "memberPage"
            controller: memberController
            cellState: cellController
            onBackRequested: stackView.pop()
            onAddRequested: stackView.push(memberLookupPageComponent)
        }
    }

    Component {
        id: memberLookupPageComponent

        UserLookupPage {
            objectName: "memberLookupPage"
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
            objectName: "userLookupPage"
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
