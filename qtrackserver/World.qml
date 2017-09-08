import QtQuick 2.0

Rectangle {
    property double world_x:0.0
    property double world_y:0.0

    property double last_distance_:0.0
    property double total_x_:0.0
    property double total_y_: 0.0
    Ground{
        id:ground_;
        anchors.fill: parent;
    }

    Fork{
        id:fork_
        scale:0.8
        anchors.horizontalCenter: parent.horizontalCenter
        y:840
    }

    Text{
        id: vel_txt_
        text:""
        font.pixelSize: 12
    }

    Connections{
        target:track_server_
        onTrajectoryUpdated:{
            //step1 degree
            var current_degree = track_server_.yaw()-90;
            fork_.rotation = current_degree
            //console.log("degree:"+current_degree)

            //step2.new position
            if(last_distance_==0.0){
                last_distance_ = track_server_.distance();
            }

            var dist = track_server_.distance()-last_distance_;
            var arc = current_degree*Math.PI/180.0;
            var dx = dist*Math.sin(arc)*track_server_.scaleMeterToPixel();
            var dy = dist*Math.cos(arc)*track_server_.scaleMeterToPixel();
            console.log("degree:"+current_degree+" dist:"+dist+" dx:"+dx+" dy:"+dy)


            total_x_ += dx;
            total_y_ += dy;
            var p = {x:total_x_,y:total_y_};
            ground_.pushTrackPoint(p);

            //update last distance
            last_distance_ = track_server_.distance();
            vel_txt_.x  = total_x_+width/2;
            vel_txt_.y = total_y_+height/2;

            var v = track_server_.velocity()*3.6;
            vel_txt_.text = ""+v.toFixed(4)+"km/h"
        }
    }
    Component.onCompleted: {
      var p = {x:0,y:0};
      ground_.pushTrackPoint(p);
    }
}
