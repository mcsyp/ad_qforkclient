import QtQuick 2.0

Rectangle {
    id: ground_
    anchors.fill: parent
    property var track_queue_: new Array()
    property var offset_x:width/2
    property var offset_y:height/2

    Canvas{
        id:mycanvas
        anchors.fill: parent
        onPaint: {
            var ctx = getContext("2d")
            drawTrack(ctx)
        }
        function drawTrack(ctx){
            //ctx.strokeStyle = "#bab6b7"
            ctx.strokeStyle = "Orange"

            ctx.lineWidth=1
            ctx.beginPath()
            ctx.moveTo(offset_x+track_queue_[0].x,offset_y+track_queue_[0].y);
            for(var i=1;i<track_queue_.length;++i){
                ctx.lineTo(offset_x+track_queue_[i].x,offset_y+track_queue_[i].y)
            }
            ctx.stroke();
        }
    }
    Timer{
        id: update_timer_
        repeat: true
        interval: 25
        onTriggered: {
            mycanvas.requestPaint()
        }
    }

    function pushTrackPoint(point){
        track_queue_.push(point)
    }
    Component.onCompleted: {
        update_timer_.running=true
    }
}
