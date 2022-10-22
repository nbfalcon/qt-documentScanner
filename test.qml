import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQml.Models 2.15

ApplicationWindow {
    visible: true
    width: 800
    height: 600

    component MyObj : Rectangle {
        required property color myColor;
    }

    ListModel {
        id: objectModel
        ListElement { obj: MyObj { myColor: "red" } }
        ListElement { obj: MyObj { myColor: "green" } }
        ListElement { obj: MyObj { myColor: "blue" } }
    }

    ListView {
        anchors.fill: parent

        model: objectModel

        delegate: Rectangle {
            width: 40
            height: 40
            color: obj.myColor
        }
    }
}
