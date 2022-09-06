import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Dialog {
    id: terminalPickerView
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    ColumnLayout {
        Text {
            text: qsTr("Select the terminal for this junction.")
            Layout.alignment: Qt.AlignHCenter
        }

        ComboBox {
            id: terminalSelection
            model: sampleModel.terminals
            Layout.alignment: Qt.AlignHCenter
        }
    }

    onAccepted: sampleModel.selectedTerminal(terminalSelection.currentIndex);
}
