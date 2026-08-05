import QtQuick.Controls
import QtQuick.Layouts

/**
 * @brief Button-based date selector that presents short dates while exposing ISO values.
 */
ColumnLayout {
    id: root

    property string title: qsTr("Date")
    property bool monthOnly: false
    readonly property date today: new Date()
    readonly property int fullYear: 2000 + yearSpin.value
    readonly property string isoValue: monthOnly
        ? "%1-%2".arg(fullYear).arg(pad(monthSpin.value))
        : "%1-%2-%3".arg(fullYear).arg(pad(monthSpin.value)).arg(pad(daySpin.value))
    readonly property string displayValue: monthOnly
        ? "%1/%2".arg(pad(monthSpin.value)).arg(pad(yearSpin.value))
        : "%1/%2/%3".arg(pad(daySpin.value)).arg(pad(monthSpin.value)).arg(pad(yearSpin.value))

    spacing: 5
    BrandPalette { id: selectorBrand }

    function pad(value) {
        return value < 10 ? "0" + value : value.toString()
    }

    function daysInMonth(year, month) {
        return new Date(year, month, 0).getDate()
    }

    function setIsoValue(value) {
        const parts = value.substring(0, 10).split("-")
        if (parts.length < 2)
            return

        const year = Number(parts[0])
        const month = Number(parts[1])
        const day = parts.length >= 3 ? Number(parts[2]) : 1
        if (year < 2000 || year > 2099 || month < 1 || month > 12 ||
                day < 1 || day > daysInMonth(year, month))
            return

        yearSpin.value = year - 2000
        monthSpin.value = month
        daySpin.value = day
    }

    Label {
        text: root.monthOnly
            ? qsTr("%1 (mm/yy)").arg(root.title)
            : qsTr("%1 (dd/mm/yy)").arg(root.title)
        color: selectorBrand.mutedText
    }

    RowLayout {
        Layout.fillWidth: true
        spacing: 5

        SpinBox {
            id: daySpin
            visible: !root.monthOnly
            Layout.fillWidth: true
            from: 1
            to: root.daysInMonth(root.fullYear, monthSpin.value)
            value: root.today.getDate()
            editable: false
            textFromValue: function(value) { return root.pad(value) }
        }
        Label {
            visible: !root.monthOnly
            text: "/"
            color: selectorBrand.mutedText
        }
        SpinBox {
            id: monthSpin
            Layout.fillWidth: true
            from: 1
            to: 12
            value: root.today.getMonth() + 1
            editable: false
            textFromValue: function(value) { return root.pad(value) }
        }
        Label {
            text: "/"
            color: selectorBrand.mutedText
        }
        SpinBox {
            id: yearSpin
            Layout.fillWidth: true
            from: 0
            to: 99
            value: root.today.getFullYear() - 2000
            editable: false
            textFromValue: function(value) { return root.pad(value) }
        }
    }
}
