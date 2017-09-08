import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

ApplicationWindow {
    visible: true
    maximumHeight: 900
    maximumWidth: 1440
    minimumHeight: 900
    minimumWidth: 1440
    title: qsTr("Hello World")


    World{
        id: world_
        anchors.fill:parent
    }

    Rectangle{
        id: info_rect_
        width:100
        height:25
        color:"Black"
        visible: true
        x:0
        y:100
        Label{
            id: info_lbl_
            anchors.fill: parent
            font.pixelSize: 12
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color:"White"
            text:"disconnected"
        }
        SpringAnimation{
            id:animate_scale
            target:info_rect_
            property: "x"
            spring: 3.0
            damping: 0.2
            epsilon: 0.2
            from:-info_rect_.width
            to:0
            duration:30
        }
    }
    Component.onCompleted: {
        var s={name:"hello",x:25,y:23}
        s.name="fuck";
        console.log("s:"+s.name+","+s.x+","+s.y);
    }
    Connections{
        target:track_server_
        onClientConnected:{
            if(connected){
                info_lbl_.text = "connected";
                info_rect_.color  = "Orange";
            }else{
                info_lbl_.text = "disconnected"
                info_rect_.color = "Black"
            }
            animate_scale.running = true;
        }
    }
}
