// Copyright (c) 2018, Jason Justian
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

/*
 * HSAppIO.h
 *
 * HSAppIO is a base class for full O_C apps that are designed to work (or act) like Hemisphere apps,
 * for consistency in development, or ease of porting apps or applets in either direction.
 */

// Icons
#ifndef HS_ICON_SET
#define HS_ICON_SET
const uint8_t CHECK_ICON[8]      = {0x00, 0xf0, 0x40, 0x20, 0x10, 0x08, 0x04, 0x00};
const uint8_t X_NOTE_ICON[8]     = {0x00, 0xa0, 0x40, 0xa0, 0x1f, 0x02, 0x0c, 0x00};
const uint8_t METER_ICON[8]      = {0x00, 0xff, 0x00, 0xfc, 0x00, 0xff, 0x00, 0xfc};
const uint8_t NOTE_ICON[8]       = {0xc0, 0xe0, 0xe0, 0xe0, 0x7f, 0x02, 0x14, 0x08};
const uint8_t CLOCK_ICON[8]      = {0x9c, 0xa2, 0xc1, 0xcf, 0xc9, 0xa2, 0x9c, 0x00};
const uint8_t MOD_ICON[8]        = {0x30, 0x08, 0x04, 0x08, 0x10, 0x20, 0x10, 0x0c};
const uint8_t BEND_ICON[8]       = {0x20, 0x70, 0x70, 0x3f, 0x20, 0x14, 0x0c, 0x1c};
const uint8_t AFTERTOUCH_ICON[8] = {0x00, 0x00, 0x20, 0x42, 0xf5, 0x48, 0x20, 0x00};
const uint8_t MIDI_ICON[8]       = {0x3c, 0x42, 0x91, 0x45, 0x45, 0x91, 0x42, 0x3c};
const uint8_t CV_ICON[8]         = {0x1f, 0x11, 0x11, 0x00, 0x07, 0x18, 0x07, 0x00};
const uint8_t SCALE_ICON[8]      = {0x81, 0x7f, 0x9f, 0x81, 0x7f, 0x9f, 0x81, 0x7f};
const uint8_t LOCK_ICON[8]       = {0x00, 0xf8, 0xfe, 0xf9, 0x89, 0xf9, 0xfe, 0xf8};
const uint8_t FAVORITE_ICON[8]   = {0x0e, 0x15, 0x31, 0x62, 0x62, 0x31, 0x15, 0x0e};
#endif // HS_ICON_SET

#ifndef int2simfloat
#define int2simfloat(x) (x << 14)
#define simfloat2int(x) (x >> 14)
typedef int32_t simfloat;
#endif

#ifndef HSAPPLICATION_H_
#define HSAPPLICATION_H_

#define HSAPPLICATION_CURSOR_TICKS 12000
#define HSAPPLICATION_SCREEN_BLANK_TICKS 30000000
#define HSAPPLICATION_5V 7680
#define HSAPPLICATION_3V 4608

class HSApplication {
public:
    virtual void Start();
    virtual void Controller();
    virtual void View();
    virtual void ScreensaverView();

    void BaseController() {
        for (uint8_t ch = 0; ch < 4; ch++)
        {
            // Set ADC input values
            inputs[ch] = OC::ADC::raw_pitch_value((ADC_CHANNEL)ch);

            if (clock_countdown[ch] > 0) {
                if (--clock_countdown[ch] == 0) Out(ch, 0);
            }
        }

        // Cursor countdowns. See CursorBlink(), ResetCursor(), gfxCursor()
        if (--cursor_countdown < -HSAPPLICATION_CURSOR_TICKS) cursor_countdown = HSAPPLICATION_CURSOR_TICKS;

        Controller();
    }

    void BaseStart() {
        screensaver_on = 0;

        // Initialize some things for startup
        for (uint8_t ch = 0; ch < 4; ch++)
        {
            clock_countdown[ch]  = 0;
            adc_lag_countdown[ch] = 0;
        }
        cursor_countdown = HSAPPLICATION_CURSOR_TICKS;

        Start();
    }

    void BaseView() {
        View();
        last_view_tick = OC::CORE::ticks;
        screensaver_on = 0;
    }

    void BaseScreensaverView() {
        screensaver_on = 1;
        if (OC::CORE::ticks - last_view_tick < HSAPPLICATION_SCREEN_BLANK_TICKS) ScreensaverView();
    }

    int Proportion(int numerator, int denominator, int max_value) {
        simfloat proportion = int2simfloat((int32_t)numerator) / (int32_t)denominator;
        int scaled = simfloat2int(proportion * max_value);
        return scaled;
    }

protected:
    /* Check cursor blink cycle. Suppress cursor when screensaver is on */
    bool CursorBlink() {
        return (cursor_countdown > 0 && !screensaver_on);
    }

    void ResetCursor() {
        cursor_countdown = HSAPPLICATION_CURSOR_TICKS;
    }

    //////////////// Hemisphere-like IO methods
    ////////////////////////////////////////////////////////////////////////////////
    void Out(int ch, int value, int octave = 0) {
        OC::DAC::set_pitch((DAC_CHANNEL)ch, value, octave);
        outputs[ch] = value + (octave * (12 << 7));
    }

    int In(int ch) {
        return inputs[ch];
    }

    // Apply small center detent to input, so it reads zero before a threshold
    int DetentedIn(int ch) {
        return (In(ch) > 64 || In(ch) < -64) ? In(ch) : 0;
    }

    bool Gate(int ch) {
        bool high = 0;
        if (ch == 0) high = OC::DigitalInputs::read_immediate<OC::DIGITAL_INPUT_1>();
        if (ch == 1) high = OC::DigitalInputs::read_immediate<OC::DIGITAL_INPUT_2>();
        if (ch == 2) high = OC::DigitalInputs::read_immediate<OC::DIGITAL_INPUT_3>();
        if (ch == 3) high = OC::DigitalInputs::read_immediate<OC::DIGITAL_INPUT_4>();
        return high;
    }

    void GateOut(int ch, bool high) {
        Out(ch, 0, (high ? 5 : 0));
    }

    bool Clock(int ch) {
        bool clocked = 0;

        if (ch == 0) clocked = OC::DigitalInputs::clocked<OC::DIGITAL_INPUT_1>();
        if (ch == 1) clocked = OC::DigitalInputs::clocked<OC::DIGITAL_INPUT_2>();
        if (ch == 2) clocked = OC::DigitalInputs::clocked<OC::DIGITAL_INPUT_3>();
        if (ch == 3) clocked = OC::DigitalInputs::clocked<OC::DIGITAL_INPUT_4>();

        if (clocked) last_clock[ch] = OC::CORE::ticks;
        return clocked;
    }

    void ClockOut(int ch, int ticks = 100) {
        clock_countdown[ch] = ticks;
        Out(ch, 0, 5);
    }

    // Buffered I/O functions for use in Views
    int ViewIn(int ch) {return inputs[ch];}
    int ViewOut(int ch) {return outputs[ch];}
    int TicksSinceClock(int ch) {return OC::CORE::ticks - last_clock[ch];} // in ticks
    int TimeSinceClock(int ch) {return TicksSinceClock(ch) / 17;} // in approx. ms

    /* ADC Lag: There is a small delay between when a digital input can be read and when an ADC can be
     * read. The ADC value lags behind a bit in time. So StartADCLag() and EndADCLag() are used to
     * determine when an ADC can be read. The pattern goes like this
     *
     * if (Clock(ch)) StartADCLag(ch);
     *
     * if (EndOfADCLog(ch)) {
     *     int cv = In(ch);
     *     // etc...
     * }
     */
    void StartADCLag(int ch) {adc_lag_countdown[ch] = 96;}
    bool EndOfADCLag(int ch) {return (--adc_lag_countdown[ch] == 0);}

    //////////////// Hemisphere-like graphics methods for easy porting
    ////////////////////////////////////////////////////////////////////////////////
    void gfxCursor(int x, int y, int w) {
        if (CursorBlink()) gfxLine(x, y, x + w - 1, y);
    }

    void gfxPos(int x, int y) {
        graphics.setPrintPos(x, y);
    }

    void gfxPrint(int x, int y, const char *str) {
        graphics.setPrintPos(x, y);
        graphics.print(str);
    }

    void gfxPrint(int x, int y, int num) {
        graphics.setPrintPos(x, y);
        graphics.print(num);
    }

    void gfxPrint(int x_adv, int num) { // Print number with character padding
        for (int c = 0; c < (x_adv / 6); c++) gfxPrint(" ");
        gfxPrint(num);
    }

    void gfxPrint(const char *str) {
        graphics.print(str);
    }

    void gfxPrint(int num) {
        graphics.print(num);
    }

    void gfxPixel(int x, int y) {
        graphics.setPixel(x, y);
    }

    void gfxFrame(int x, int y, int w, int h) {
        graphics.drawFrame(x, y, w, h);
    }

    void gfxRect(int x, int y, int w, int h) {
        graphics.drawRect(x, y, w, h);
    }

    void gfxInvert(int x, int y, int w, int h) {
        graphics.invertRect(x, y, w, h);
    }

    void gfxLine(int x, int y, int x2, int y2) {
        graphics.drawLine(x, y, x2, y2);
    }

    void gfxCircle(int x, int y, int r) {
        graphics.drawCircle(x, y, r);
    }

    void gfxBitmap(int x, int y, int w, const uint8_t *data) {
        graphics.drawBitmap8(x, y, w, data);
    }

    uint8_t pad(int range, int number) {
        uint8_t padding = 0;
        while (range > 1)
        {
            if (number < range) padding += 6;
            range = range / 10;
        }
        return padding;
    }

    void gfxHeader(const char *str) {
         gfxPrint(1, 2, str);
         gfxLine(0, 10, 127, 10);
         gfxLine(0, 12, 127, 12);
    }

private:
    int clock_countdown[4]; // For clock output timing
    int adc_lag_countdown[4]; // Lag countdown for each input channel
    int cursor_countdown; // Timer for cursor blinkin'
    bool screensaver_on; // Is the screensaver active?
    uint32_t last_view_tick; // Time since the last view, for activating screen blanking
    int inputs[4]; // Last ADC values
    int outputs[4]; // Last DAC values; inputs[] and outputs[] are used to allow access to values in Views
    uint32_t last_clock[4]; // Keeps time since last clock
};

#endif /* HSAPPLICATION_H_ */
