import QtQml 2.15
import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import Qt.labs.platform 1.1
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
                action: scanDocument
            }

            ComboBox {
                id: scannerPicker
                model: scanner.scannerService.scannerNames
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

    Menu {
        id: pageContextMenu

        property int boundToIndex: -1

        MenuItem {
            text: qsTr("Delete Page")
            shortcut: StandardKey.Delete
            icon.name: "delete"
            onTriggered: {
                scanner.scannedPages.detach(pageContextMenu.boundToIndex)
            }
        }

        MenuItem {
            text: qsTr("Move Right >")
            icon.name: "arrow-right"
            onTriggered: {
                scanner.scannedPages.move(pageContextMenu.boundToIndex, pageContextMenu.boundToIndex + 1);
            }
        }

        MenuItem {
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
        id: scanDocument
        icon.name: "document-scan"
        text: "Scan"
        onTriggered: scanner.scan(selectedScanner)
    }

    Connections {
        target: scanner

        function onScanFinished(image) {
            console.log("Scan finished from QML\n");
        }
    }
}
