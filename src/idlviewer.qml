import QtQuick 1.0

Rectangle {
  width: 640
  height: 480
  anchors.fill: parent
  ListView {
    id: list_view1
    anchors.fill: parent
    delegate: Item {
      x: 5
      height: 220
      Image {
        source: "image://src/" + filename
        asynchronous: true
      }
      Image {
        source: "image://grad/" + filename
        asynchronous: true
        opacity: 0.6
      }
      Rectangle {
        x: rx
        y: ry
        width: rwidth
        height: rheight
        border.color: "#00FF00"
        color: "#3000FF00"
      }
    }
    model: files
  }
}
