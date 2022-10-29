import QtQml 2.15
import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import Qt.labs.platform 1.1 as Platform
import org.nbfalcon.documentScanner 1.0

ApplicationWindow {
    required property DocumentScanner scanner

    property alias selectedScanner: scannerPicker.currentText

    title: qsTr("Document Scanner")

    width: 800
    height: 600
    visible: true

    header: ToolBar {
        RowLayout {
            ToolButton {
                action: scanDocumentAction
            }

            ComboBox {
                id: scannerPicker
                model: scanner.scannerService.scannerNames
            }

            ToolButton {
                action: saveDocumentAction
            }

            ToolButton {
                action: configureAction
            }
        }
    }

    Item {
        visible: !scanner.scannedAPage
        anchors.fill: parent

        Label {
            visible: !scanner.scannersLoaded
            text: qsTr("Loading scanners. Please wait...")
            anchors.centerIn: parent
        }

        Label {
            visible: scanner.scannersLoaded && scanner.scannerService.scannerCount === 0
            text: qsTr("No scanners found.")
            anchors.centerIn: parent
        }

        Label {
            visible: scanner.scannersLoaded && scanner.scannerService.scannerCount > 0
            text: qsTr("Ready to scan.")
            anchors.centerIn: parent
        }
    }

    Platform.Menu {
        id: pageContextMenu

        property int boundToIndex: -1

        Platform.MenuItem {
            text: qsTr("Delete Page")
            shortcut: StandardKey.Delete
            icon.name: "delete"
            onTriggered: {
                scanner.scannedPages.detach(pageContextMenu.boundToIndex)
            }
        }

        Platform.MenuItem {
            text: qsTr("Move Right >")
            icon.name: "arrow-right"
            onTriggered: {
                scanner.scannedPages.move(pageContextMenu.boundToIndex, pageContextMenu.boundToIndex + 1);
            }
        }

        Platform.MenuItem {
            text: qsTr("Move Left <")
            icon.name: "arrow-left"
            onTriggered: {
                if (pageContextMenu.boundToIndex > 0)
                    scanner.scannedPages.move(pageContextMenu.boundToIndex, pageContextMenu.boundToIndex - 1);
            }
        }
    }

    ListView {
        id: scannedPagesListView
        visible: scanner.scannedAPage

        anchors.fill: parent
        orientation: ListView.Horizontal
        spacing: 16

        model: scanner.scannedPages

        delegate: ColumnLayout {
            required property int index
            required property ScannedPage item

            height: ListView.view.height

            Item {
                Layout.fillHeight: true
                Layout.preferredWidth: item == null ? -1 : (item.imWidth / item.imHeight) * height

                BusyIndicator {
                    anchors.centerIn: parent
                    running: item == null ? false : !item.ready
                }

                QmlQImageView {
                    anchors.fill: parent
                    visible: item == null ? false : item.ready
                    image: item == null ? undefined : item.scannedPage
                }

                MouseArea {
                    anchors.fill: parent

                    // Context Menu
                    acceptedButtons: Qt.RightButton
                    onClicked: (event) => {
                        if (event.button === Qt.RightButton) {
                            pageContextMenu.boundToIndex = index
                            pageContextMenu.open()
                        }
                    }
                }
            }

            Label {
                text: index + 1
                Layout.alignment: Qt.AlignHCenter
            }
        }
    }

    Action {
        id: scanDocumentAction
        icon.name: "document-scan"
        text: "Scan"
        onTriggered: scanner.scan(selectedScanner)
    }

    Platform.FileDialog {
        id: saveDialog

        title: qsTr("Save")
        folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)

        fileMode: Platform.FileDialog.SaveFile
        nameFilters: [qsTr("PNG File (*.png)"), qsTr("JPEG File (*.jpg)"), qsTr("TIFF File (*.tiff)"), qsTr("PDF File (*.pdf)")]

        onAccepted: {
            console.log("Accepted <3", this.file)
            scanner.save(this.file, DocumentScanner.PNG)
        }
    }

    Action {
        id: saveDocumentAction
        icon.name: "document-save"
        shortcut: StandardKey.Save
        text: "Save"
        onTriggered: {
            console.log(saveDialog)
            saveDialog.open()
        }
    }

    Drawer {
        id: settingsDrawer
        edge: Qt.RightEdge

        ButtonGroup {
            buttons: sideButtons.children
        }

        GridLayout {
            columns: 2
            columnSpacing: 16

            Label {
                text: qsTr("Scanner Area:")
            }
            ComboBox {
                model: ["Auto", "A4", "A5"]
            }

            Label {
                text: qsTr("DPI:")
            }
            ComboBox {
                model: [300, 600, 900, 1200]
            }

            Label {
                text: qsTr("Double Sided:")
            }
            RowLayout {
                id: sideButtons
                spacing: 0

                Button {
                    checkable: true
                    checked: true
                    text: "Front"
                }
                Button {
                    checkable: true
                    text: "Back"
                }
                Button {
                    checkable: true
                    text: "Both"
                }
            }

            // FIXME: delay
            // FIXME: DPI in toolbar
            // FIXME: scan editor

            CheckBox {
                id: colorCheckBox
                text: qsTr("Color")
            }

            CheckBox {
                text: qsTr("Invert Colors")
            }

            CheckBox {
                text: qsTr("Color Profile:")
                enabled: colorCheckBox.checked
            }
            ComboBox {
                model: ["sRGB"]
                enabled: colorCheckBox.checked
            }

            Label {
                text: "Brightness:"
            }
            Slider {
                from: 0
                to: 1
            }

            Label {
                text: "Contrast:"
            }
            Slider {
                from: 0
                to: 1
            }

            Label {
                text: "Gamma:"
            }
            Slider {
                from: 0
                to: 1
            }

            Label {
                text: "Shadows:"
            }
            Slider {
                from: 0
                to: 1
            }

            CheckBox {
                text: "Auto-Rotate"
            }

            CheckBox {
                text: "Enable OCR"
            }

            Button {
                text: "Cancel"
            }

            Button {
                text: "Rescan"
            }
        }
    }

    Action {
        id: configureAction
        icon.name: "configure"
        text: "Configure"
        shortcut: "Ctrl+H"
        onTriggered: {
            settingsDrawer.open()
        }
    }

    Connections {
        target: scanner

        function onScanFinished(image) {
            console.log("Scan finished from QML\n");
        }
    }
}
