// Copyright (c) 2024, Beau Sterling
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

// TODO:
//      check the rest of the wave generators, attach them to the menu
//      range limit cv2 input based on calibration data
//      connect cv1 input to OC quantizer
//      trig 1 input toggle quant on and off
//      trig 2 input toggle LFO / audio modes
//      root note / volt ref select

#include <arm_math.h>
#include "../tideslite.h"

class WTVCO : public HemisphereApplet {
public:

    static constexpr size_t WT_SIZE = 256;

    enum WTVCO_Cursor {
        WAVEFORM_OUT,
        WAVEFORM_A,
        WAVEFORM_B,
        WAVEFORM_C,

        LAST_SETTING = WAVEFORM_C
    };

    enum WaveTables { A, B, C, OUT };

    enum WaveForms {
        WAVE_SINE,
        WAVE_TRIANGLE,
        WAVE_SQUARE,
        WAVE_SAW,
        WAVE_RAMP,
        // WAVE_PULSE12,
        // WAVE_PULSE25,
        // WAVE_PULSE50,
        // WAVE_PULSE75,
        // WAVE_PULSE87,
        // add more waves here and generator functions at the bottom

        WAVEFORM_COUNT
    };
    static constexpr const char* const wavetable_names[WAVEFORM_COUNT] = {
      "Sine", "Triangle", "Square", "Saw", "Ramp", //"Pulse12", "Pulse25", "Pulse50", "Pulse75", "Pulse87",

    };

    const char* applet_name() {
        return "WTVCO";
    }

    void Start() {
        waveform[A] = WAVE_SINE;
        waveform[B] = WAVE_TRIANGLE;
        waveform[C] = WAVE_SQUARE;
        for (int w = 0; w < 3; ++w) GenerateWaveTable(w);
    }

    void Controller() {
        if (Clock(0)) pitch_range_shift = constrain(pitch_range_shift - 1, 0, 8);
        if (Clock(1)) pitch_range_shift = constrain(pitch_range_shift + 1, 0, 8);

        if (Changed(0)) pitch = In(0);
        phase_inc = ComputePhaseIncrement(pitch);
        phase += phase_inc;
        uint8_t phase_acc_msb = (uint8_t)(phase >> (24 - pitch_range_shift));

        if (Changed(1)) wt_pos = Proportion(In(1), 5 * HEMISPHERE_MAX_INPUT_CV / 6, 255); // wavetable morphs over a 5 volt range

        InterpolateSample(wt[OUT], phase_acc_msb);
        Out(0, wt[OUT][phase_acc_msb] * HEMISPHERE_MAX_CV / 255);
    }

    void View() {
        DrawInterface();
        DrawSelector();
        DrawWaveForm();
    }

    void OnEncoderMove(int direction) {
        if (!EditMode()) {
            MoveCursor(cursor, direction, LAST_SETTING);
            return;
        }

        switch((WTVCO_Cursor)cursor) {
            case WAVEFORM_A:
                waveform[A] = (WaveForms) constrain(((int)waveform[A]) + direction, 0, WAVEFORM_COUNT-1);
                GenerateWaveTable(A);
                break;
            case WAVEFORM_B:
                waveform[B] = (WaveForms) constrain(((int)waveform[B]) + direction, 0, WAVEFORM_COUNT-1);
                GenerateWaveTable(B);
                break;
            case WAVEFORM_C:
                waveform[C] = (WaveForms) constrain(((int)waveform[C]) + direction, 0, WAVEFORM_COUNT-1);
                GenerateWaveTable(C);
                break;
            case WAVEFORM_OUT:
            default: break;
        }
    }

    uint64_t OnDataRequest() {
        uint64_t data = 0;
        Pack(data, PackLocation { 0,8}, waveform[0]);
        Pack(data, PackLocation { 8,8}, waveform[1]);
        Pack(data, PackLocation {16,8}, waveform[2]);
        return data;
    }

    void OnDataReceive(uint64_t data) {
        waveform[0] = (WaveForms) Unpack(data, PackLocation { 0,8});
        waveform[1] = (WaveForms) Unpack(data, PackLocation { 8,8});
        waveform[2] = (WaveForms) Unpack(data, PackLocation {16,8});
        for (int w = 0; w < 3; ++w) GenerateWaveTable(w);
    }

protected:
    void SetHelp() {
      // TODO
    }

private:
    int cursor = 0; // WTVCO_Cursor

    int16_t pitch = 0;
    uint32_t phase_inc = 0;
    uint32_t phase;
    int pitch_range_shift = 4;

    WaveForms waveform[3];
    std::array<int8_t, WT_SIZE> wt[4];
    int wt_pos = 0;


// GRAPHIC STUFF:
    static constexpr int HEADER_HEIGHT = 11;
    static constexpr int X_DIV = 64 / 4;
    static constexpr int MENU_ROW = 14;

    void gfxRenderWave(int w) {
        for (size_t x = 0; x < WT_SIZE; x+=4) {
            int y = 44 - Proportion(wt[w][x], 127, 16);
            gfxPixel(x/4, y);
        }
    }

    void DrawInterface() {
        int x = 3;
        int y = MENU_ROW;

        if (!EditMode()
        || (WTVCO_Cursor)cursor == WAVEFORM_OUT) {
            gfxBitmap(x+1, y, 8, WAVEFORM_ICON);
            x += X_DIV;
            gfxPrint(x+2, y, "A");
            x += X_DIV;
            gfxPrint(x+2, y, "B");
            x += X_DIV;
            gfxPrint(x+2, y, "C");

        } else {
            switch((WTVCO_Cursor)cursor) {
                case WAVEFORM_A:
                    gfxPrint(3, MENU_ROW, "A:");
                    gfxPrint(wavetable_names[waveform[A]]);
                    break;
                case WAVEFORM_B:
                    gfxPrint(3, MENU_ROW, "B:");
                    gfxPrint(wavetable_names[waveform[B]]);
                    break;
                case WAVEFORM_C:
                    gfxPrint(3, MENU_ROW, "C:");
                    gfxPrint(wavetable_names[waveform[C]]);
                    break;
                default: break;
            }
        }

        gfxLine(0, y+11, 63, y+11);
        gfxLine(0, 63, 63, 63);
    }

    void DrawSelector() {
        if(!EditMode()) {
            gfxSpicyCursor((cursor * X_DIV), 23, X_DIV);
        }
    }

    void DrawWaveForm() {
        switch((WTVCO_Cursor)cursor) {
            case WAVEFORM_OUT:
                gfxRenderWave(OUT);
                break;
            case WAVEFORM_A:
                gfxRenderWave(A);
                break;
            case WAVEFORM_B:
                gfxRenderWave(B);
                break;
            case WAVEFORM_C:
                gfxRenderWave(C);
                break;
            default: break;
        }
    }


// WAVETABLE STUFF:
    void InterpolateSample(std::array<int8_t, WT_SIZE>& waveform, uint8_t sample) {
        waveform[sample] = (int8_t) ((((wt_pos <= 127) * ((127 - wt_pos) * (int)wt[A][sample] + wt_pos * (int)wt[B][sample]))
                                + ((wt_pos > 127) * ((255 - wt_pos) * (int)wt[B][sample] + (wt_pos - 128) * (int)wt[C][sample]))) / 127);
    }

    void GenerateWaveTable(int w) {
        switch(waveform[w]) {
            case WAVE_SINE:
                GenerateWaveForm_Sine(wt[w]);
                break;
            case WAVE_TRIANGLE:
                GenerateWaveForm_Triangle(wt[w]);
                break;
            case WAVE_SQUARE:
                GenerateWaveForm_Square(wt[w]);
                break;
            case WAVE_SAW:
                GenerateWaveForm_Sawtooth(wt[w]);
                break;
            case WAVE_RAMP:
                GenerateWaveForm_Ramp(wt[w]);
                break;
            // add waves here

            default: break;
        }
    }


// standard waves
    void GenerateWaveForm_Sine(std::array<int8_t, WT_SIZE>& waveform) {
        for (size_t i = 0; i < WT_SIZE; ++i) {
            q15_t t = static_cast<q15_t>(i * 32767 / WT_SIZE);
            waveform[i] = arm_sin_q15(t) >> 8;
        }
    }

    void GenerateWaveForm_Triangle(std::array<int8_t, WT_SIZE>& waveform) {
        for (size_t i = 0; i < WT_SIZE; ++i) {
            float t = std::fmod((static_cast<float>(i) / WT_SIZE) + 0.25, 1.0);
            float value = 4.0 * std::fabs(t - std::floor(t + 0.5)) - 1.0;
            waveform[i] = static_cast<int8_t>(127 * value);
        }
    }

    void GenerateWaveForm_Square(std::array<int8_t, WT_SIZE>& waveform) {
        for (size_t i = 0; i < WT_SIZE; ++i) {
            waveform[i] = (i < WT_SIZE / 2) ? 127 : -128;
        }
    }

    void GenerateWaveForm_Sawtooth(std::array<int8_t, WT_SIZE>& waveform) {
        for (size_t i = 0; i < WT_SIZE; ++i) {
            float t = static_cast<float>(i) / WT_SIZE;
            waveform[i] = static_cast<int8_t>(255 - (255 * (t - 0.5)));
        }
    }

    void GenerateWaveForm_Ramp(std::array<int8_t, WT_SIZE>& waveform) {
        for (size_t i = 0; i < WT_SIZE; ++i) {
            float t = static_cast<float>(i) / WT_SIZE;
            waveform[i] = static_cast<int8_t>(255 * (t - 0.5));
        }
    }



// q15 math
//     void GenerateWaveForm_Sine(std::array<int8_t, WT_SIZE>& waveform) {
//         for (size_t i = 0; i < WT_SIZE; ++i) {
//             q15_t t = static_cast<q15_t>(i * 32767 / WT_SIZE);
//             waveform[WT_SIZE-i-1] = arm_sin_q15(t) >> 8;
//         }
//     }

// void GenerateWaveForm_Triangle(std::array<int8_t, WT_SIZE>& waveform) {
//     for (size_t i = 0; i < WT_SIZE; ++i) {
//         q15_t t = static_cast<q15_t>((((int32_t)i << 15) / WT_SIZE) + (1 << 13)); // Fixed-point equivalent of (i / WT_SIZE + 0.25)
//         t &= 0x7FFF; // Wrap to [0, 1) in Q15 format
//         q15_t value = abs((t << 1) - 0x7FFF) - 0x7FFF; // Fixed-point equivalent of 4 * fabs(t - 0.5) - 1
//         waveform[WT_SIZE-i-1] = (int8_t)(value >> 8); // Scale down to 8-bit signed
//     }
// }

// void GenerateWaveForm_Triangle(std::array<int8_t, WT_SIZE>& waveform) { // half amplitude, half period
//     for (size_t i = 0; i < WT_SIZE; ++i) {
//         q15_t t = (q15_t)((i << 15) / WT_SIZE); // Calculate t in Q15 format as (i / WT_SIZE)
//         q15_t value = (t < 0x4000) ? (t << 1) : ((0x7FFF - t) << 1); // Calculate triangle waveform value in Q15
//         waveform[WT_SIZE-i-1] = (int8_t)(value >> 8); // Convert to 8-bit signed and assign
//     }
// }

// void GenerateWaveForm_Square(std::array<int8_t, WT_SIZE>& waveform) {
//     for (size_t i = 0; i < WT_SIZE; ++i) {
//         waveform[WT_SIZE-i-1] = (i < WT_SIZE / 2) ? 127 : -128;
//     }
// }

// void GenerateWaveForm_Sawtooth(std::array<int8_t, WT_SIZE>& waveform) {
//     for (size_t i = 0; i < WT_SIZE; ++i) {
//         q15_t t = (q15_t)((i << 15) / WT_SIZE); // Fixed-point equivalent of i / WT_SIZE
//         q15_t value = 0x7FFF - t; // Fixed-point equivalent of 1 - t
//         waveform[WT_SIZE-i-1] = (int8_t)(value >> 8); // Scale down to 8-bit signed
//     }
// }

// void GenerateWaveForm_Ramp(std::array<int8_t, WT_SIZE>& waveform) {
//     for (size_t i = 0; i < WT_SIZE; ++i) {
//         q15_t t = (q15_t)((i << 15) / WT_SIZE); // Fixed-point equivalent of i / WT_SIZE
//         q15_t value = t - 0x4000; // Fixed-point equivalent of t - 0.5
//         waveform[WT_SIZE-i-1] = (int8_t)(value >> 8); // Scale down to 8-bit signed
//     }
// }





// MOAR WAVEZ
// the wave generator functions below were created by ChatGPT
// and have not all been tested. its on the TODO list.

// // pulses

//     std::array<int8_t> GenerateWaveForm_Pulse(const size_t size, double dutyCycle = 0.1) {
//         std::array<int8_t, WT_SIZE> waveform;
//         size_t pulseWidth = static_cast<size_t>(dutyCycle * size);
//         for (size_t i = 0; i < size; ++i) {
//             waveform[i] = (i < pulseWidth) ? 127 : -128;
//         }
//         return waveform;
//     }

//     std::array<int8_t> GenerateWaveForm_Pulse25(const size_t size) {
//         std::array<int8_t, WT_SIZE> waveform;
//         for (size_t i = 0; i < size; ++i) {
//             waveform[i] = (i < size / 4) ? 127 : -128; // High for 25% of the period
//         }
//         return waveform;
//     }


// // ramps

//     std::array<int8_t> GenerateWaveForm_RisingRamp(const size_t size) {
//         std::array<int8_t, WT_SIZE> waveform;
//         for (size_t i = 0; i < size; ++i) {
//             waveform[i] = static_cast<int8_t>((255.0 * i / size) - 128);
//         }
//         return waveform;
//     }

//     std::array<int8_t> GenerateWaveForm_FallingRamp(const size_t size) {
//         std::array<int8_t, WT_SIZE> waveform;
//         for (size_t i = 0; i < size; ++i) {
//             waveform[i] = static_cast<int8_t>(127 - (255.0 * i / size));
//         }
//         return waveform;
//     }

//     std::array<int8_t> GenerateWaveForm_Sharkfin(const size_t size) {
//         std::array<int8_t, WT_SIZE> waveform;
//         for (size_t i = 0; i < size; ++i) {
//             double t = static_cast<double>(i) / size;
//             double value = (t < 0.5) ? 2 * t : 2 * (1 - t); // Rapid rise, slow fall
//             waveform[i] = static_cast<int8_t>(127 * value);
//         }
//         return waveform;
//     }



// // steps

//     std::array<int8_t> GenerateWaveForm_Step(const size_t size) {
//         std::array<int8_t, WT_SIZE> waveform;
//         for (size_t i = 0; i < size; ++i) {
//             waveform[i] = (i < size / 2) ? 127 : -128; // Half high, half low
//         }
//         return waveform;
//     }

//     std::array<int8_t> GenerateWaveForm_RandomStep(const size_t size) {
//         std::array<int8_t, WT_SIZE> waveform;
//         std::random_device rd;
//         std::mt19937 gen(rd());
//         std::uniform_int_distribution<int> dist(-128, 127);
//         for (size_t i = 0; i < size; i += 32) {
//             int value = dist(gen);
//             for (size_t j = i; j < i + 32 && j < size; ++j) {
//                 waveform[j] = static_cast<int8_t>(value);
//             }
//         }
//         return waveform;
//     }

//     std::array<int8_t> GenerateWaveForm_SinusoidalStep(const size_t size) {
//         std::array<int8_t, WT_SIZE> waveform;
//         for (size_t i = 0; i < size; ++i) {
//             waveform[i] = static_cast<int8_t>(127 * std::round(std::sin(TWO_PI * i / size)));
//         }
//         return waveform;
//     }

//     std::array<int8_t> GenerateWaveForm_WavyStaircase(const size_t size) {
//         std::array<int8_t, WT_SIZE> waveform;
//         size_t stepSize = size / 8;
//         for (size_t i = 0; i < size; ++i) {
//             size_t step = i / stepSize;
//             waveform[i] = static_cast<int8_t>(127 * std::sin(TWO_PI * step / 8));
//         }
//         return waveform;
//     }

//     std::array<int8_t> GenerateWaveForm_Stepped(const size_t size, size_t steps = 8) {
//         std::array<int8_t, WT_SIZE> waveform;
//         size_t stepSize = size / steps;
//         for (size_t i = 0; i < size; ++i) {
//             waveform[i] = static_cast<int8_t>(127 * ((i / stepSize) / static_cast<double>(steps - 1)) - 128);
//         }
//         return waveform;
//     }



// // noise

//     std::array<int8_t> GenerateWaveForm_Noise(const size_t size) {
//         std::array<int8_t, WT_SIZE> waveform;
//         for (size_t i = 0; i < size; ++i) {
//             waveform[i] = static_cast<int8_t>(rand() % 256 - 128); // Random int8_t values
//         }
//         return waveform;
//     }

//     std::array<int8_t> GenerateWaveForm_Noise2(const size_t size) {
//         std::array<int8_t, WT_SIZE> waveform;
//         std::random_device rd;
//         std::mt19937 gen(rd());
//         std::uniform_int_distribution<int> dist(-128, 127);
//         for (size_t i = 0; i < size; ++i) {
//             waveform[i] = static_cast<int8_t>(dist(gen));
//         }
//         return waveform;
//     }

//     std::array<int8_t> GenerateWaveForm_GaussianNoise(const size_t size) {
//         std::array<int8_t, WT_SIZE> waveform;
//         std::random_device rd;
//         std::mt19937 gen(rd());
//         std::normal_distribution<> dist(0, 50); // Mean 0, std deviation 50
//         for (size_t i = 0; i < size; ++i) {
//             waveform[i] = static_cast<int8_t>(std::clamp<int>(dist(gen), -128, 127));
//         }
//         return waveform;
//     }



// // exponential

//     std::array<int8_t> GenerateWaveForm_ExponentialGrowth(const size_t size) {
//         std::array<int8_t, WT_SIZE> waveform;
//         for (size_t i = 0; i < size; ++i) {
//             double t = static_cast<double>(i) / size;
//             waveform[i] = static_cast<int8_t>(127 * (std::exp(t) - 1) / (std::exp(1) - 1));
//         }
//         return waveform;
//     }

//     std::array<int8_t> GenerateWaveForm_ExponentialDecay(const size_t size) {
//         std::array<int8_t, WT_SIZE> waveform;
//         for (size_t i = 0; i < size; ++i) {
//             double t = static_cast<double>(i) / size;
//             double value = std::exp(-5 * t); // Exponential decay
//             waveform[i] = static_cast<int8_t>(127 * value);
//         }
//         return waveform;
//     }

//     std::array<int8_t> GenerateWaveForm_ExponentialDecay2(const size_t size) {
//         std::array<int8_t, WT_SIZE> waveform;
//         for (size_t i = 0; i < size; ++i) {
//             double t = static_cast<double>(i) / size;
//             waveform[i] = static_cast<int8_t>(127 * std::exp(-5 * t)); // Decays over the range
//         }
//         return waveform;
//     }

//     std::array<int8_t> GenerateWaveForm_InverseExponential(const size_t size) {
//         std::array<int8_t, WT_SIZE> waveform;
//         for (size_t i = 0; i < size; ++i) {
//             double t = static_cast<double>(i) / size;
//             double value = 1.0 - std::exp(-5 * t); // Inverse exponential growth
//             waveform[i] = static_cast<int8_t>(127 * value);
//         }
//         return waveform;
//     }



// // modified sinusoids

//     std::array<int8_t> GenerateWaveForm_OffsetSine(const size_t size, double offset = 0.5) {
//         std::array<int8_t, WT_SIZE> waveform;
//         for (size_t i = 0; i < size; ++i) {
//             double t = static_cast<double>(i) / size;
//             waveform[i] = static_cast<int8_t>(127 * (std::sin(TWO_PI * t) + offset));
//         }
//         return waveform;
//     }

//     std::array<int8_t> GenerateWaveForm_OffsetSine2(const size_t size) {
//         std::array<int8_t, WT_SIZE> waveform;
//         for (size_t i = 0; i < size; ++i) {
//             double t = static_cast<double>(i) / size;
//             double value = 0.5 + 0.5 * std::sin(2 * M_PI * t); // Sine wave offset to [0, 1]
//             waveform[i] = static_cast<int8_t>(127 * value - 64); // Offset to [-64, 63]
//         }
//         return waveform;
//     }

//     std::array<int8_t> GenerateWaveForm_Cosine(const size_t size) {
//         std::array<int8_t, WT_SIZE> waveform;
//         for (size_t i = 0; i < size; ++i) {
//             double t = static_cast<double>(i) / size;
//             waveform[i] = static_cast<int8_t>(127 * std::cos(TWO_PI * t));
//         }
//         return waveform;
//     }

//     std::array<int8_t> GenerateWaveForm_SinusoidalAM(const size_t size) {
//         std::array<int8_t, WT_SIZE> waveform;
//         for (size_t i = 0; i < size; ++i) {
//             double t = static_cast<double>(i) / size;
//             double value = std::sin(2 * M_PI * t) * (0.5 + 0.5 * std::sin(4 * M_PI * t)); // Sine modulated by sine
//             waveform[i] = static_cast<int8_t>(127 * value);
//         }
//         return waveform;
//     }

//     std::array<int8_t> GenerateWaveForm_ClippedSine(const size_t size, double clip = 0.5) {
//         std::array<int8_t, WT_SIZE> waveform;
//         for (size_t i = 0; i < size; ++i) {
//             double t = static_cast<double>(i) / size;
//             double sineValue = std::sin(TWO_PI * t);
//             sineValue = std::clamp(sineValue, -clip, clip);
//             waveform[i] = static_cast<int8_t>(127 * sineValue);
//         }
//         return waveform;
//     }

//     std::array<int8_t> GenerateWaveForm_StretchedSine(const size_t size, double frequency = 2.0) {
//         std::array<int8_t, WT_SIZE> waveform;
//         for (size_t i = 0; i < size; ++i) {
//             double t = static_cast<double>(i) / size;
//             waveform[i] = static_cast<int8_t>(127 * std::sin(TWO_PI * frequency * t));
//         }
//         return waveform;
//     }

//     std::array<int8_t> GenerateWaveForm_FullRectifiedSine(const size_t size) {
//         std::array<int8_t, WT_SIZE> waveform;
//         for (size_t i = 0; i < size; ++i) {
//             double t = static_cast<double>(i) / size;
//             waveform[i] = static_cast<int8_t>(127 * std::fabs(std::sin(TWO_PI * t)));
//         }
//         return waveform;
//     }

//     std::array<int8_t> GenerateWaveForm_HalfRectifiedSine(const size_t size) {
//         std::array<int8_t, WT_SIZE> waveform;
//         for (size_t i = 0; i < size; ++i) {
//             double t = static_cast<double>(i) / size;
//             double sineValue = std::sin(TWO_PI * t);
//             waveform[i] = static_cast<int8_t>(127 * std::max((int)sineValue, 0));
//         }
//         return waveform;
//     }

//     std::array<int8_t> GenerateWaveForm_HalfSine(const size_t size) {
//         std::array<int8_t, WT_SIZE> waveform;
//         for (size_t i = 0; i < size; ++i) {
//             double t = static_cast<double>(i) / size;
//             double value = std::sin(M_PI * t); // Half sine wave
//             waveform[i] = static_cast<int8_t>(127 * value);
//         }
//         return waveform;
//     }



// // geometric

//     std::array<int8_t> GenerateWaveForm_Parabolic(const size_t size) {
//         std::array<int8_t, WT_SIZE> waveform;
//         for (size_t i = 0; i < size; ++i) {
//             double t = static_cast<double>(i) / (size - 1);
//             double value = 4.0 * (t - 0.5) * (t - 0.5) - 1.0; // Parabola centered at 0
//             waveform[i] = static_cast<int8_t>(127 * value);
//         }
//         return waveform;
//     }

//     std::array<int8_t> GenerateWaveForm_Gaussian(const size_t size) {
//         std::array<int8_t, WT_SIZE> waveform;
//         for (size_t i = 0; i < size; ++i) {
//             double t = static_cast<double>(i) / (size - 1);
//             double value = std::exp(-50 * (t - 0.5) * (t - 0.5)); // Gaussian bell curve
//             waveform[i] = static_cast<int8_t>(127 * value);
//         }
//         return waveform;
//     }


// // probably harsh

//     std::array<int8_t> GenerateWaveForm_Ripple(const size_t size) {
//         std::array<int8_t, WT_SIZE> waveform;
//         for (size_t i = 0; i < size; ++i) {
//             double t = static_cast<double>(i) / size;
//             double value = std::sin(2 * M_PI * t) + 0.3 * std::sin(10 * M_PI * t);
//             waveform[i] = static_cast<int8_t>(127 * value);
//         }
//         return waveform;
//     }

//     std::array<int8_t> GenerateWaveForm_Zigzag(const size_t size) {
//         std::array<int8_t, WT_SIZE> waveform;
//         for (size_t i = 0; i < size; ++i) {
//             waveform[i] = static_cast<int8_t>((i % 64) - 32);
//         }
//         return waveform;
//     }

//     std::array<int8_t> GenerateWaveForm_Spike(const size_t size) {
//         std::array<int8_t> waveform(size, -128);
//         for (size_t i = 0; i < size; i += size / 8) {
//             waveform[i] = 127; // Set spikes periodically
//         }
//         return waveform;
//     }

//     std::array<int8_t> GenerateWaveForm_Alternating(const size_t size) {
//         std::array<int8_t, WT_SIZE> waveform;
//         for (size_t i = 0; i < size; ++i) {
//             waveform[i] = (i % 32 < 16) ? 127 : -128;
//         }
//         return waveform;
//     }

//     std::array<int8_t> GenerateWaveForm_SquarePulseTrain(const size_t size) {
//         std::array<int8_t, WT_SIZE> waveform;
//         for (size_t i = 0; i < size; ++i) {
//             waveform[i] = ((i / 16) % 2 == 0) ? 127 : -128;
//         }
//         return waveform;
//     }

    // add more wave generators here, and add to enum

};
