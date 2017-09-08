import QtQuick 2.0

Item {
    id:fork_item_

    Image{
        id:fork_img_
        source:"images/angle.png"
        anchors.centerIn: parent
    }

    Text{
        id: fork_id_txt_
        anchors.centerIn: parent
        text:""
        font.pixelSize: 12
    }

    SpringAnimation{
        id:animate_scale
        target:fork_img_
        property: "scale"
        spring: 3.0
        damping: 0.2
        epsilon: 0.05
        from:0.05
        to:1.0
        duration:50
    }
}
