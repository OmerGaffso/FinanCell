import QtQuick
import QtTest
import "../../src/ui/qt/qml" as FinanCell

/**
 * @brief Exercises every application route with injected controller doubles.
 */
TestCase {
    id: testCase
    name: "Navigation"
    when: windowShown

    property var applicationWindow: null
    property var stack: null

    Component {
        id: applicationComponent
        FinanCell.Main {}
    }

    function expectPage(name) {
        tryCompare(stack.currentItem, "objectName", name)
        wait(0)
        compare(testDiagnostics.errors.length, 0,
                testDiagnostics.errors.join("\n"))
    }

    function init() {
        userController.setLoggedIn(false)
        transactionController.setHasSelectedTransaction(false)
        testDiagnostics.clear()
        applicationWindow = applicationComponent.createObject(null)
        verify(applicationWindow !== null)
        stack = findChild(applicationWindow, "mainStackView")
        verify(stack !== null)
    }

    function cleanup() {
        if (applicationWindow !== null) {
            applicationWindow.visible = false
            applicationWindow.destroy()
        }
        applicationWindow = null
        stack = null
        wait(0)
    }

    function test_allRoutesInstantiateWithoutQmlErrors() {
        expectPage("loginPage")

        stack.currentItem.registrationRequested()
        expectPage("registerPage")
        stack.currentItem.backRequested()
        expectPage("loginPage")

        userController.setLoggedIn(true)
        expectPage("homePage")

        stack.currentItem.userLookupRequested()
        expectPage("userLookupPage")
        stack.currentItem.backRequested()
        expectPage("homePage")

        stack.currentItem.financialCellsRequested()
        expectPage("cellListPage")

        stack.currentItem.createRequested()
        expectPage("createCellPage")
        stack.currentItem.backRequested()
        expectPage("cellListPage")

        stack.currentItem.cellSelected()
        expectPage("cellDashboardPage")

        stack.currentItem.membersRequested()
        expectPage("memberPage")
        stack.currentItem.addRequested()
        expectPage("memberLookupPage")
        stack.currentItem.backRequested()
        expectPage("memberPage")
        stack.currentItem.backRequested()
        expectPage("cellDashboardPage")

        stack.currentItem.categoriesRequested()
        expectPage("categoryPage")
        stack.currentItem.backRequested()
        expectPage("cellDashboardPage")

        stack.currentItem.transactionsRequested()
        expectPage("transactionPage")
        stack.currentItem.addRequested()
        expectPage("transactionFormPage")
        stack.currentItem.backRequested()
        expectPage("transactionPage")

        transactionController.setHasSelectedTransaction(true)
        stack.currentItem.editRequested()
        expectPage("transactionFormPage")
        stack.currentItem.backRequested()
        transactionController.setHasSelectedTransaction(false)
        expectPage("transactionPage")
        stack.currentItem.backRequested()
        expectPage("cellDashboardPage")

        stack.currentItem.reportRequested()
        expectPage("monthlyReportPage")
        stack.currentItem.backRequested()
        expectPage("cellDashboardPage")

        stack.currentItem.settingsRequested()
        expectPage("cellSettingsPage")
        stack.currentItem.backRequested()
        expectPage("cellDashboardPage")

        stack.currentItem.backRequested()
        expectPage("cellListPage")
        stack.currentItem.backRequested()
        expectPage("homePage")

        userController.logout()
        expectPage("loginPage")
    }
}
