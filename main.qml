import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtQml 2.15
import org.nbfalcon.documentScanner 1.0

ApplicationWindow {
//    required property DocumentScanner scanner

    property alias selectedScanner: scannerPicker.currentText
    property var scannedImages: []

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

    ListView {
        visible: scanner.scannedAPage

        orientation: ListView.Horizontal
        anchors.fill: parent
        spacing: 16

        model: scannedImages
        delegate: ColumnLayout {
            ScannedPage {
                image: modelData
                width: 400
                height: 400
            }

            Label {
                text: number
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
            console.log(image);
            scannedImages.push(image);
            scannedImages = scannedImages;
        }
    }
}
