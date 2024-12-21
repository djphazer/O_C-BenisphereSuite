// Copyright (c) 2023, Nicholas J. Michalek
// GUI updates copyright (C) 2024, Beau Sterling
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#define CAL8_PRECISION 10000

class Calibr8 : public HemisphereApplet {
public:
    enum CalCursor {
        SCALEFACTOR_A,
        TRANS_A,
        OFFSET_A,
        SCALEFACTOR_B,
        TRANS_B,
        OFFSET_B,

        MAX_CURSOR = OFFSET_B
    };

    const char* applet_name() {
        return "Calibr8";
    }
    const uint8_t* applet_icon() { return PhzIcons::calibr8; }

    void Start() {
        clocked_mode = false;
        AllowRestart();
    }

    void Controller() {
        bool clocked = Clock(0);
        if (clocked) clocked_mode = true;

        ForEachChannel(ch) {
            uint8_t input_note = MIDIQuantizer::NoteNumber(In(ch), 0);

            // clocked transpose
            if (!clocked_mode || clocked)
                transpose_active[ch] = transpose[ch];

            input_note += transpose_active[ch];

            int output_cv = MIDIQuantizer::CV(input_note) * (CAL8_PRECISION + scale_factor[ch]) / CAL8_PRECISION;
            output_cv += offset[ch];

            Out(ch, output_cv);
        }
    }

    void View() {
        DrawInterface();
    }

    //void OnButtonPress() { }

    void OnEncoderMove(int direction) {
        if (!EditMode()) {
            MoveCursor(cursor, direction, MAX_CURSOR);
            return;
        }

        bool ch = (cursor > OFFSET_A);
        switch (cursor) {
        case OFFSET_A:
        case OFFSET_B:
            offset[ch] = constrain(offset[ch] + direction, -100, 100);
            break;

        case SCALEFACTOR_A:
        case SCALEFACTOR_B:
            scale_factor[ch] = constrain(scale_factor[ch] + direction, -500, 500);
            break;

        case TRANS_A:
        case TRANS_B:
            transpose[ch] = constrain(transpose[ch] + direction, -36, 60);
            break;

        default: break;
        }
    }

    uint64_t OnDataRequest() {
        uint64_t data = 0;
        Pack(data, PackLocation { 0,10}, scale_factor[0] + 500);
        Pack(data, PackLocation {10,10}, scale_factor[1] + 500);
        Pack(data, PackLocation {20, 8}, offset[0] + 100);
        Pack(data, PackLocation {28, 8}, offset[1] + 100);
        Pack(data, PackLocation {36, 7}, transpose[0] + 36);
        Pack(data, PackLocation {43, 7}, transpose[1] + 36);
        return data;
    }

    void OnDataReceive(uint64_t data) {
        scale_factor[0] = Unpack(data, PackLocation { 0,10}) - 500;
        scale_factor[1] = Unpack(data, PackLocation {10,10}) - 500;
        offset[0]       = Unpack(data, PackLocation {20, 8}) - 100;
        offset[1]       = Unpack(data, PackLocation {28, 8}) - 100;
        transpose[0]    = Unpack(data, PackLocation {36, 7}) - 36;
        transpose[1]    = Unpack(data, PackLocation {43, 7}) - 36;
    }

protected:
  void SetHelp() {
    //                    "-------" <-- Label size guide
    help[HELP_DIGITAL1] = "Clock";
    help[HELP_DIGITAL2] = "";
    help[HELP_CV1]      = "Input1";
    help[HELP_CV2]      = "Input2";
    help[HELP_OUT1]     = "Pitch1";
    help[HELP_OUT2]     = "Pitch2";
    help[HELP_EXTRA1] = "";
    help[HELP_EXTRA2] = "";
    //                  "---------------------" <-- Extra text size guide
  }

private:
    int cursor;
    bool clocked_mode = false;
    int scale_factor[2] = {0,0}; // precision of 0.01% as an offset from 100%
    int offset[2] = {0,0}; // fine-tuning offset
    int transpose[2] = {0,0}; // in semitones
    int transpose_active[2] = {0,0}; // held value while waiting for trigger

    void DrawInterface() {
        int y_shift = 27; // B+D section y offset
        ForEachChannel(ch) {
            int y = 13 + (ch * y_shift);
            gfxPrint(0, y, OutputLabel(ch));

            int whole = (scale_factor[ch] + CAL8_PRECISION) / 100;
            int decimal = (scale_factor[ch] + CAL8_PRECISION) % 100;
            gfxPrint(12 + pad(100, whole), y, whole);
            gfxPrint(".");
            if (decimal < 10) gfxPrint("0");
            gfxPrint(decimal);
            gfxPrint("%");

            // second line
            y += 10;
            gfxIcon(0, y, BEND_ICON);
            gfxPrint(8, y, transpose[ch]);
            gfxIcon(32, y, UP_DOWN_ICON);
            gfxPrint(40, y, offset[ch]);
        }

        bool ch = (cursor > OFFSET_A);
        int param = (cursor % 3);
        if (param == 0) // Scaling
            gfxCursor(12, 21 + (ch * y_shift), 40);
        else // Transpose or Fine Tune
            gfxCursor(8 + (param-1)*32, 31 + (ch * y_shift), 20);

        // resize cv meter bars at the bottom of each section:
        ForEachChannel(ch) {
            int length;
            int max_length = 60; // max transpose value from above
            int out_bar_y = 33 + (ch * y_shift);
            int in_bar_y = 36 + (ch * y_shift);

            // positve values extend bars from left side of screen to the right
            // negative values go from right side to left
            length = ProportionCV(abs(In(ch)), max_length);
            if (In(ch) < 0)
                gfxFrame(max_length - length, in_bar_y, length, 1);
            else
                gfxFrame(1, in_bar_y, length, 1);

            length = ProportionCV(abs(ViewOut(ch)), max_length);
            if (ViewOut(ch) < 0)
                gfxFrame(max_length - length, out_bar_y, length, 2);
            else
                gfxFrame(1, out_bar_y, length, 2);
        }
    }
};
