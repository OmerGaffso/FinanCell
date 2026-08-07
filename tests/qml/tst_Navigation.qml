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

    function expectResponsiveForm(maximumWidth) {
        const form = findChild(stack.currentItem, "formContent")
        verify(form !== null)
        verify(form.width > 0)
        verify(form.width <= maximumWidth)
        verify(form.x >= 0)
        verify(form.x + form.width <= form.parent.width + 1)
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
        const addPicker = findChild(stack.currentItem, "categoryPickerDialog")
        verify(addPicker !== null)
        addPicker.open()
        tryCompare(addPicker, "visible", true)
        compare(testDiagnostics.errors.length, 0,
                testDiagnostics.errors.join("\n"))
        addPicker.close()
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

    function test_defaultWindowGeometry() {
        compare(applicationWindow.width, 1200)
        compare(applicationWindow.height, 800)
        compare(applicationWindow.minimumWidth, 700)
        compare(applicationWindow.minimumHeight, 500)
    }

    function test_commonWindowSizes_data() {
        return [
            {tag: "narrow", windowWidth: 400, windowHeight: 700, columns: 1},
            {tag: "compact", windowWidth: 700, windowHeight: 600, columns: 2},
            {tag: "desktop", windowWidth: 1200, windowHeight: 800, columns: 4},
            {tag: "wide", windowWidth: 1600, windowHeight: 900, columns: 6}
        ]
    }

    function test_commonWindowSizes(data) {
        applicationWindow.minimumWidth = 0
        applicationWindow.minimumHeight = 0
        applicationWindow.width = data.windowWidth
        applicationWindow.height = data.windowHeight
        tryCompare(applicationWindow, "width", data.windowWidth)
        tryCompare(applicationWindow, "height", data.windowHeight)

        expectPage("loginPage")
        expectResponsiveForm(420)
        stack.currentItem.registrationRequested()
        expectPage("registerPage")
        expectResponsiveForm(420)
        stack.currentItem.backRequested()

        userController.setLoggedIn(true)
        expectPage("homePage")
        stack.currentItem.userLookupRequested()
        expectPage("userLookupPage")
        stack.currentItem.backRequested()
        expectPage("homePage")
        stack.currentItem.financialCellsRequested()
        expectPage("cellListPage")
        const grid = findChild(stack.currentItem, "financialCellGrid")
        verify(grid !== null)
        tryCompare(grid, "columnCount", data.columns)

        stack.currentItem.createRequested()
        expectPage("createCellPage")
        expectResponsiveForm(440)
        stack.currentItem.backRequested()
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
        expectResponsiveForm(440)
        stack.currentItem.backRequested()
        expectPage("transactionPage")
        stack.currentItem.backRequested()
        expectPage("cellDashboardPage")

        stack.currentItem.reportRequested()
        expectPage("monthlyReportPage")
        stack.currentItem.backRequested()
        expectPage("cellDashboardPage")

        stack.currentItem.settingsRequested()
        expectPage("cellSettingsPage")
        expectResponsiveForm(440)
        compare(testDiagnostics.errors.length, 0,
                testDiagnostics.errors.join("\n"))
    }
}
