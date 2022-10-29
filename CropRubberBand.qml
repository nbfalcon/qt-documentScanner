import QtQuick 2.0

Item {
    anchors.margins: 16

    Rectangle {
        id: rubberBand

        color: 'blue'
        x: 0
        y: 0
        width: parent.width
        height: parent.height

    //    MouseArea {
    //        id: resizeArea

    //        anchors.fill: parent
    //        enabled: rubberBand.cornerBeingDragged !== rubberBand.None

    //        onPositionChanged: (event) => {
    //            let area = event;

    //            let corner = rubberBand.cornerBeingDragged % 4;
    //            let lCoeff = [0, -1, 0, 1][corner];
    //            let tCoeff = [1, 0, -1, 0][corner];

    //            if (lCoeff < 0) {
    //                parent.anchors.rightMargin = resizeArea.width - area.x;
    //            } else if (lCoeff > 0) {
    //                parent.anchors.leftMargin = resizeArea.x;
    //            }

    //            if (tCoeff < 0) {
    //                parent.anchors.bottomMargin = resizeArea.height - area.y;
    //            } else if (tCoeff > 0) {
    //                parent.anchors.topMargin = area.y;
    //            }
    //        }
    //    }

        MouseArea {
            id: dragArea

            anchors.fill: parent
            cursorShape: Qt.SizeAllCursor

            visible: true
            drag.target: this
            Drag.active: true

            enum DragFlags {
                SHRINK_RHS = 1, SHRINK_BOT = 2
            }
            property int dragFlags: 0

            onMouseXChanged: {
                if (drag.active) {
                    if (dragFlags & this.SHRINK_RHS) {
                        rubberBand.width -= dragArea.width - mouseX;
                    } else {
                        rubberBand.x += mouseX;
                        rubberBand.width -= mouseX;
                    }
                }
            }
            onMouseYChanged: {
                if (drag.active) {
                    if (dragFlags & this.SHRINK_BOT) {
                        rubberBand.height -= dragArea.height - mouseY;
                    } else {
                        rubberBand.y += mouseY;
                        rubberBand.height -= mouseY;
                    }
                }
            }
        }

        component DragHandle : Rectangle {
            width: 16; height: 16
            color: 'red'

            required property int dragFlags
            required property int dragAxis

            MouseArea {
                anchors.fill: parent
                onPressed: {
                    console.log("Mewo")
                    parent.visible = false
                    dragArea.dragFlags = parent.dragFlags
                    dragArea.drag.startDrag()
                }
            }
        }

        DragHandle {
            anchors { top: parent.top; left: parent.left }
            dragFlags: 0
        }
        DragHandle {
            anchors { top: parent.top; right: parent.right; }
            dragFlags: dragArea.SHRINK_RHS
        }
        DragHandle {
            anchors { bottom: parent.bottom; left: parent.left }
            dragFlags: dragArea.SHRINK_BOT
        }
        DragHandle {
            anchors { bottom: parent.bottom; right: parent.right }
            dragFlags: dragArea.SHRINK_BOT | dragArea.SHRINK_RHS
        }
    }
}
