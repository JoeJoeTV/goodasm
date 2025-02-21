import QtQuick
import QtQuick.Layouts

Window {
    id: win
    //width: 640
    //height: 480

    visible: true
    title: "GoodASM"


    onHeightChanged: update()
    onWidthChanged: update()

    // Inform the C++ world that the input changed.
    signal updateSignal(msg: string)

    function update(){
        //We do assembly here.
        textout.text=goodasm.updateSlot(edit.text);

        //Set the font.
        edit.font=fixedFont;
        textout.font=fixedFont;

        //Here we ensure that the input edit position is shown in both views.
        flickedit.ensureVisible(edit.cursorRectangle);
        flickout.ensureVisible(edit.cursorRectangle);
    }


    Flickable {
        id: flickout

        Layout.fillHeight: true
        Layout.fillWidth: true
        clip: true

        function ensureVisible(r) {
            if (contentX >= r.x)
                contentX = r.x;
            else if (contentX+width <= r.x+r.width)
                contentX = r.x+r.width-width;
            if (contentY >= r.y)
                contentY = r.y;
            else if (contentY+height <= r.y+r.height)
                contentY = r.y+r.height-height;
        }

        Text {
            id: textout
            Layout.fillHeight: true
            Layout.fillWidth: true
            color: "steelblue"
            text: "blue text\nblue text."

            /*
            font.family: "Courier" //macOS
            font.features: font.Monospace
            font.hintingPreference: font.Monospace
            font.styleName: font.Monospace
            //font.family: "DejaVu Sans Mono"
            font.kerning: false
            */

        }
    }

    Flickable {
        id: flickedit

        Layout.fillHeight: true
        Layout.fillWidth: true
        clip: true

        function ensureVisible(r) {
            if (contentX >= r.x)
                contentX = r.x;
            else if (contentX+width <= r.x+r.width)
                contentX = r.x+r.width-width;
            if (contentY >= r.y)
                contentY = r.y;
            else if (contentY+height <= r.y+r.height)
                contentY = r.y+r.height-height;
        }

        TextEdit {
            id: edit
            Layout.fillHeight: true
            Layout.fillWidth: true
            focus: true
            wrapMode: TextEdit.NoWrap
            onCursorRectangleChanged: win.update()
            onTextChanged: win.update()
            //font.family: "Courier"
            text: ".lang fcard\nloop:\n  lda #0x99   ; famous 99\n  jsr 0x1e7f  ; send byte\n  bra loop    ; tx forever\n\n  .ib 2d      ; Disass\n\n"
        }
    }

    GridLayout {
        id: editgrid
        anchors.fill: parent

        columns: 1


        //60 pixels at the top seems right for Android.
        //iOS?
        Rectangle {
            id: filltop
            Layout.preferredWidth: win.width
            Layout.preferredHeight: 30
            color: "#00414A"
        }

        LayoutItemProxy { target: flickedit }
        LayoutItemProxy { target: flickout }


        //Covers the Android keyboard.
        //iOS?  Windows?

        Rectangle {
            Layout.preferredWidth: win.width
            Layout.preferredHeight: win.height/3-40
            color: "#00414A"
        }


    }
}
