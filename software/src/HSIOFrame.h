/* Copyright (c) 2023-2024 Nicholas J. Michalek & Beau Sterling
 *
 * IOFrame & friends
 *   attempts at making applet I/O more flexible and portable
 *
 * Some processing logic adapted from the MIDI In applet
 *
 */

#pragma once

#include <vector>
#include "HSMIDI.h"

#ifdef ARDUINO_TEENSY41
namespace OC {
    namespace AudioDSP {
        extern void Process(const int *values);
    }
}
#endif

namespace HS {

static constexpr int GATE_THRESHOLD = 15 << 7; // 1.25 volts
static constexpr int TRIGMAP_MAX = OC::DIGITAL_INPUT_LAST + ADC_CHANNEL_LAST + DAC_CHANNEL_LAST;
static constexpr int CVMAP_MAX = ADC_CHANNEL_LAST + DAC_CHANNEL_LAST;

typedef struct MIDILogEntry {
    int message;
    int data1;
    int data2;
} MIDILogEntry;

struct MIDINoteData {
    int note; // data1
    int vel;  // data2
};

// shared IO Frame, updated every tick
// this will allow chaining applets together, multiple stages of processing
typedef struct IOFrame {
    bool autoMIDIOut = false;
    bool clocked[OC::DIGITAL_INPUT_LAST + ADC_CHANNEL_LAST];
    bool gate_high[OC::DIGITAL_INPUT_LAST + ADC_CHANNEL_LAST];
    int inputs[ADC_CHANNEL_LAST];
    int outputs[DAC_CHANNEL_LAST];
    int output_diff[DAC_CHANNEL_LAST];
    int outputs_smooth[DAC_CHANNEL_LAST];
    int clock_countdown[DAC_CHANNEL_LAST];
    uint8_t clockskip[DAC_CHANNEL_LAST] = {0};
    bool clockout_q[DAC_CHANNEL_LAST]; // for loopback
    int adc_lag_countdown[ADC_CHANNEL_LAST]; // Time between a clock event and an ADC read event
    uint32_t last_clock[ADC_CHANNEL_LAST]; // Tick number of the last clock observed by the child class
    uint32_t cycle_ticks[ADC_CHANNEL_LAST]; // Number of ticks between last two clocks
    bool changed_cv[ADC_CHANNEL_LAST]; // Has the input changed by more than 1/8 semitone since the last read?
    int last_cv[ADC_CHANNEL_LAST]; // For change detection

    /* MIDI message queue/cache */
    struct {
        int channel[ADC_CHANNEL_LAST]; // MIDI channel number
        int function[ADC_CHANNEL_LAST]; // Function for each channel
        int function_cc[ADC_CHANNEL_LAST]; // CC# for each channel
        uint16_t semitone_mask[ADC_CHANNEL_LAST]; // which notes are currently on

        // MIDI input stuff handled by MIDIIn applet
        std::vector<MIDINoteData> note_buffer[16]; // note buffer for polyphonic input. one for each MIDI channel
        int outputs[DAC_CHANNEL_LAST]; // translated CV values
        bool trigout_q[DAC_CHANNEL_LAST];
        int last_midi_channel = 0; // for MIDI In activity monitor
        uint16_t sustain_latch; // each bit is a MIDI channel's sustain state

        void RemoveNoteData(std::vector<MIDINoteData> &buffer, int note) {
            buffer.erase(
                std::remove_if(buffer.begin(), buffer.end(), [&](MIDINoteData const &data) {
                    return data.note == note;
                }),
                buffer.end()
            );
        }

        void NoteStackPush(const int midi_chan, const int data1, const int data2) {
            int c = midi_chan - 1;
            uint8_t note = (uint8_t)data1;
            uint8_t vel = (uint8_t)data2;
            RemoveNoteData(note_buffer[c], note); // if new note is already in buffer, promote to latest and update velocity
            note_buffer[c].push_back({note, vel}); // else just append to the end
        }

        void NoteStackPop(const int midi_chan, const int data1) {
            int c = midi_chan - 1;
            uint8_t note = (uint8_t)data1;
            RemoveNoteData(note_buffer[c], note);
            if (note_buffer[c].size() == 0) note_buffer[c].shrink_to_fit(); // free up memory when MIDI is not used
        }

        void ClearNoteStack(int ch = -1) {
            if (ch > 0) {
                note_buffer[ch].clear();
                note_buffer[ch].shrink_to_fit();
            } else { // clear on all channels if no args passed
                for (int c = 0; c < 16; ++c) {
                    note_buffer[c].clear();
                    note_buffer[c].shrink_to_fit();
                }
            }
        }

        int GetNote(std::vector<MIDINoteData> &buffer, int n) {
            return buffer.at(buffer.size()-n).note;
        }

        int GetNoteFirst(std::vector<MIDINoteData> &buffer) {
            return buffer.front().note;
        }

        int GetNoteLast(std::vector<MIDINoteData> &buffer) {
            return buffer.back().note;
        }

        int GetNoteLastInv(std::vector<MIDINoteData> &buffer) {
            return 127 - buffer.back().note;
        }

        int GetNoteMin(std::vector<MIDINoteData> &buffer) {
            int m = 127;
            std::for_each (buffer.begin(), buffer.end(), [&](MIDINoteData const &data) {
                if (data.note < m) m = data.note;
            });
            return m;
        }

        int GetNoteMax(std::vector<MIDINoteData> &buffer) {
            int m = 0;
            std::for_each (buffer.begin(), buffer.end(), [&](MIDINoteData const &data) {
                if (data.note > m) m = data.note;
            });
            return m;
        }

        int GetVel(std::vector<MIDINoteData> &buffer, int n) {
            return buffer.at(buffer.size()-n).vel;
        }

        void ClearSustainLatch(int m_ch = -1) {
            if (m_ch > 0) sustain_latch &= ~(1 << m_ch);
            else { // clear on all channels if no args passed
                for (int c = 0; c < 16; ++c)
                    sustain_latch &= ~(1 << c);
            }
        }

        void SetSustainLatch(int m_ch) {
            sustain_latch |= (1 << m_ch);
        }

        bool CheckSustainLatch(int m_ch) {
            return sustain_latch & (1 << m_ch);
        }

        // Clock/Start/Stop are handled by ClockSetup applet
        bool clock_run = 0;
        bool clock_q;
        bool start_q;
        bool stop_q;
        uint8_t clock_count; // MIDI clock counter (24ppqn)
        int last_msg_tick; // Tick of last received message

        // MIDI output stuff
        int outchan[DAC_CHANNEL_LAST] = {
            0, 0, 1, 1,
#ifdef ARDUINO_TEENSY41
            2, 2, 3, 3,
#endif
        };
        int outchan_last[DAC_CHANNEL_LAST] = {
            0, 0, 1, 1,
#ifdef ARDUINO_TEENSY41
            2, 2, 3, 3,
#endif
        };
        int outfn[DAC_CHANNEL_LAST] = {
            HEM_MIDI_NOTE_OUT, HEM_MIDI_GATE_OUT,
            HEM_MIDI_NOTE_OUT, HEM_MIDI_GATE_OUT,
#ifdef ARDUINO_TEENSY41
            HEM_MIDI_NOTE_OUT, HEM_MIDI_GATE_OUT,
            HEM_MIDI_NOTE_OUT, HEM_MIDI_GATE_OUT,
#endif
        };
        uint8_t outccnum[DAC_CHANNEL_LAST] = {
            1, 1, 1, 1,
#ifdef ARDUINO_TEENSY41
            5, 6, 7, 8,
#endif
        };
        uint8_t current_note[16]; // note number, per MIDI channel
        uint8_t current_ccval[DAC_CHANNEL_LAST]; // level 0 - 127, per DAC channel
        int note_countdown[DAC_CHANNEL_LAST];
        int inputs[DAC_CHANNEL_LAST]; // CV to be translated
        int last_cv[DAC_CHANNEL_LAST];
        bool clocked[DAC_CHANNEL_LAST];
        bool gate_high[DAC_CHANNEL_LAST];
        bool changed_cv[DAC_CHANNEL_LAST];

        // Logging
        MIDILogEntry log[7];
        int log_index;

        void UpdateLog(int message, int data1, int data2) {
            log[log_index++] = {message, data1, data2};
            if (log_index == 7) {
                for (int i = 0; i < 6; i++) {
                    memcpy(&log[i], &log[i+1], sizeof(log[i+1]));
                }
                log_index--;
            }
            last_msg_tick = OC::CORE::ticks;
        }

        void ProcessMIDIMsg(const int midi_chan, const int message, const int data1, const int data2) {
            switch (message) {
            case usbMIDI.Clock:
                if (++clock_count == 1) {
                    clock_q = 1;
                    for(int ch = 0; ch < ADC_CHANNEL_LAST; ++ch) {
                        if (function[ch] == HEM_MIDI_CLOCK_OUT) {
                            trigout_q[ch] = 1;
                        }
                    }
                }
                if (clock_count == HEM_MIDI_CLOCK_DIVISOR) clock_count = 0;
                return;
                break;

            case usbMIDI.Continue: // treat Continue like Start
            case usbMIDI.Start:
                start_q = 1;
                clock_count = 0;
                clock_run = true;

                for(int ch = 0; ch < ADC_CHANNEL_LAST; ++ch) {
                    if (function[ch] == HEM_MIDI_START_OUT) {
                        trigout_q[ch] = 1;
                    }
                }

                // UpdateLog(message, data1, data2);
                return;
                break;

            case usbMIDI.SystemReset:
            case usbMIDI.Stop:
                stop_q = 1;
                clock_run = false;
                // a way to reset stuck notes
                ClearNoteStack();
                ClearSustainLatch();
                // for (int c = 0; c < 4; ++c) {
                //     outputs[c] = 0;
                //     trigout_q[c] = 0;
                // }
                return;
                break;

            case usbMIDI.NoteOn:
                NoteStackPush(midi_chan, data1, data2);
                break;
            case usbMIDI.NoteOff:
                NoteStackPop(midi_chan, data1);
                break;

            }

            bool log_skip = false;
            int m_ch_prev = -1;

            for(int ch = 0; ch < ADC_CHANNEL_LAST; ++ch) {
                if (function[ch] == HEM_MIDI_NOOP) continue;

                // skip unwanted MIDI Channels
                if (midi_chan - 1 != channel[ch]) continue;
                int m_ch = midi_chan - 1;

                last_midi_channel = m_ch;

                // prevent duplicate log entries
                if (m_ch == m_ch_prev) log_skip = true;
                else log_skip = false;
                m_ch_prev = m_ch;

                bool log_this = false;

                switch (message) {
                    case usbMIDI.NoteOn:
                        semitone_mask[ch] = semitone_mask[ch] | (1u << (data1 % 12));

                        // Should this message go out on this channel?
                        switch (function[ch]) { // note # output functions
                            case HEM_MIDI_NOTE_OUT:
                                outputs[ch] = MIDIQuantizer::CV(GetNoteLast(note_buffer[m_ch]));
                                break;

                            case HEM_MIDI_NOTE_POLY2_OUT:
                                if (note_buffer[m_ch].size() > 1)
                                    outputs[ch] = MIDIQuantizer::CV(GetNote(note_buffer[m_ch], 2));
                                else
                                    outputs[ch] = MIDIQuantizer::CV(GetNoteLast(note_buffer[m_ch]));
                                break;

                            case HEM_MIDI_NOTE_POLY3_OUT:
                                if (note_buffer[m_ch].size() > 2)
                                    outputs[ch] = MIDIQuantizer::CV(GetNote(note_buffer[m_ch], 3));
                                else {
                                    if (note_buffer[m_ch].size() == 2) // distribute notes evenly when only 2 are played
                                        outputs[ch] = MIDIQuantizer::CV(GetNoteLast(note_buffer[m_ch]));
                                    else
                                        outputs[ch] = MIDIQuantizer::CV(GetNoteFirst(note_buffer[m_ch]));
                                }
                                break;

                            case HEM_MIDI_NOTE_POLY4_OUT:
                                if (note_buffer[m_ch].size() > 3)
                                    outputs[ch] = MIDIQuantizer::CV(GetNote(note_buffer[m_ch], 4));
                                else
                                    outputs[ch] = MIDIQuantizer::CV(GetNoteFirst(note_buffer[m_ch]));
                                break;

                            case HEM_MIDI_NOTE_MIN_OUT:
                                outputs[ch] = MIDIQuantizer::CV(GetNoteMin(note_buffer[m_ch]));
                                break;

                            case HEM_MIDI_NOTE_MAX_OUT:
                                outputs[ch] = MIDIQuantizer::CV(GetNoteMax(note_buffer[m_ch]));
                                break;

                            case HEM_MIDI_NOTE_PEDAL_OUT:
                                outputs[ch] = MIDIQuantizer::CV(GetNoteFirst(note_buffer[m_ch]));
                                break;

                            case HEM_MIDI_NOTE_INV_OUT:
                                outputs[ch] = MIDIQuantizer::CV(GetNoteLastInv(note_buffer[m_ch]));
                                break;
                        }

                        if ((function[ch] == HEM_MIDI_TRIG_OUT)
                        || (function[ch] == HEM_MIDI_TRIG_ALWAYS_OUT)
                        || ((function[ch] == HEM_MIDI_TRIG_1ST_OUT) && (note_buffer[m_ch].size() == 1)))
                            trigout_q[ch] = 1;

                        if (function[ch] == HEM_MIDI_GATE_OUT)
                            outputs[ch] = PULSE_VOLTAGE * (12 << 7);
                        if (function[ch] == HEM_MIDI_GATE_INV_OUT)
                            outputs[ch] = 0;

                        switch (function[ch]) {
                            case HEM_MIDI_VEL_OUT:
                                outputs[ch] = (note_buffer[m_ch].size() > 0) ? Proportion(GetVel(note_buffer[m_ch], 1), 127, HEMISPHERE_MAX_CV) : 0;
                                break;
                            case HEM_MIDI_VEL2_OUT:
                                outputs[ch] = (note_buffer[m_ch].size() > 1) ? Proportion(GetVel(note_buffer[m_ch], 2), 127, HEMISPHERE_MAX_CV) : 0;
                                break;
                            case HEM_MIDI_VEL3_OUT:
                                outputs[ch] = (note_buffer[m_ch].size() > 2) ? Proportion(GetVel(note_buffer[m_ch], 3), 127, HEMISPHERE_MAX_CV) : 0;
                                break;
                            case HEM_MIDI_VEL4_OUT:
                                outputs[ch] = (note_buffer[m_ch].size() > 3) ? Proportion(GetVel(note_buffer[m_ch], 4), 127, HEMISPHERE_MAX_CV) : 0;
                                break;
                        }

                        if (!log_skip) log_this = 1; // Log all MIDI notes. Other stuff is conditional.
                        break;

                    case usbMIDI.NoteOff:
                        semitone_mask[ch] = semitone_mask[ch] & ~(1u << (data1 % 12));

                        if (note_buffer[m_ch].size() > 0) { // don't update output when last note is released
                            switch(function[ch]) { // note # output functions
                                case HEM_MIDI_NOTE_OUT:
                                    if (CheckSustainLatch(m_ch)) break;
                                    outputs[ch] = MIDIQuantizer::CV(GetNoteLast(note_buffer[m_ch]));
                                    break;

                                case HEM_MIDI_NOTE_POLY2_OUT:
                                    if (CheckSustainLatch(m_ch)) break;
                                    if (note_buffer[m_ch].size() < 2)
                                        outputs[ch] = MIDIQuantizer::CV(GetNoteLast(note_buffer[m_ch]));
                                    else
                                        outputs[ch] = MIDIQuantizer::CV(GetNote(note_buffer[m_ch], 2));
                                    break;

                                case HEM_MIDI_NOTE_POLY3_OUT:
                                    if (CheckSustainLatch(m_ch)) break;
                                    if (note_buffer[m_ch].size() < 3) {
                                        if (note_buffer[m_ch].size() == 2) // distribute notes evenly when only 2 are played
                                            outputs[ch] = MIDIQuantizer::CV(GetNoteLast(note_buffer[m_ch]));
                                        else
                                            outputs[ch] = MIDIQuantizer::CV(GetNoteFirst(note_buffer[m_ch]));
                                    } else
                                        outputs[ch] = MIDIQuantizer::CV(GetNote(note_buffer[m_ch], 3));
                                    break;

                                case HEM_MIDI_NOTE_POLY4_OUT:
                                    if (CheckSustainLatch(m_ch)) break;
                                    if (note_buffer[m_ch].size() < 4)
                                        outputs[ch] = MIDIQuantizer::CV(GetNoteFirst(note_buffer[m_ch]));
                                    else
                                        outputs[ch] = MIDIQuantizer::CV(GetNote(note_buffer[m_ch], 4));
                                    break;

                                case HEM_MIDI_NOTE_MIN_OUT:
                                    if (CheckSustainLatch(m_ch)) break;
                                    outputs[ch] = MIDIQuantizer::CV(GetNoteMin(note_buffer[m_ch]));
                                    break;

                                case HEM_MIDI_NOTE_MAX_OUT:
                                    if (CheckSustainLatch(m_ch)) break;
                                    outputs[ch] = MIDIQuantizer::CV(GetNoteMax(note_buffer[m_ch]));
                                    break;

                                case HEM_MIDI_NOTE_PEDAL_OUT:
                                    if (CheckSustainLatch(m_ch)) break;
                                    outputs[ch] = MIDIQuantizer::CV(GetNoteFirst(note_buffer[m_ch]));
                                    break;

                                case HEM_MIDI_NOTE_INV_OUT:
                                    if (CheckSustainLatch(m_ch)) break;
                                    outputs[ch] = MIDIQuantizer::CV(GetNoteLastInv(note_buffer[m_ch]));
                                    break;
                            }
                        }

                        if (function[ch] == HEM_MIDI_TRIG_ALWAYS_OUT) trigout_q[ch] = 1;

                        // turn gate off only when all notes are off, and sustain pedal up
                        if (!(note_buffer[m_ch].size() > 0) && !CheckSustainLatch(m_ch)) {
                            if (function[ch] == HEM_MIDI_GATE_OUT) {
                                outputs[ch] = 0;
                            }
                            if (function[ch] == HEM_MIDI_GATE_INV_OUT) {
                                outputs[ch] = PULSE_VOLTAGE * (12 << 7);
                            }
                        }

                        switch (function[ch]) {
                            case HEM_MIDI_VEL_OUT:
                                outputs[ch] = (note_buffer[m_ch].size() > 0) ? Proportion(GetVel(note_buffer[m_ch], 1), 127, HEMISPHERE_MAX_CV) : 0;
                                break;
                            case HEM_MIDI_VEL2_OUT:
                                outputs[ch] = (note_buffer[m_ch].size() > 1) ? Proportion(GetVel(note_buffer[m_ch], 2), 127, HEMISPHERE_MAX_CV) : 0;
                                break;
                            case HEM_MIDI_VEL3_OUT:
                                outputs[ch] = (note_buffer[m_ch].size() > 2) ? Proportion(GetVel(note_buffer[m_ch], 3), 127, HEMISPHERE_MAX_CV) : 0;
                                break;
                            case HEM_MIDI_VEL4_OUT:
                                outputs[ch] = (note_buffer[m_ch].size() > 3) ? Proportion(GetVel(note_buffer[m_ch], 4), 127, HEMISPHERE_MAX_CV) : 0;
                                break;
                        }

                        if (!log_skip) log_this = 1;
                        break;

                    case usbMIDI.ControlChange: // Modulation wheel or other CC
                        // handle sustain pedal
                        if (data1 == 64) {
                            if (data2 > 63) {
                                if (!CheckSustainLatch(m_ch)) SetSustainLatch(m_ch);
                            } else {
                                ClearSustainLatch(m_ch);
                                if (!(note_buffer[m_ch].size() > 0)) {
                                    if (function[ch] == HEM_MIDI_GATE_OUT) {
                                        outputs[ch] = 0;
                                    }
                                    if (function[ch] == HEM_MIDI_GATE_INV_OUT) {
                                        outputs[ch] = PULSE_VOLTAGE * (12 << 7);
                                    }
                                }
                            }
                        }

                        if (function[ch] == HEM_MIDI_CC_OUT) {
                            if (function_cc[ch] < 0) function_cc[ch] = data1;

                            if (function_cc[ch] == data1) {
                                outputs[ch] = Proportion(data2, 127, HEMISPHERE_MAX_CV);
                                if (!log_skip) log_this = 1;
                            }
                        }
                        break;

                    case usbMIDI.AfterTouchPoly:
                        switch (function[ch]) {
                            case HEM_MIDI_AT_KEY1_OUT:
                                if (note_buffer[m_ch].size() > 0) {
                                    if (GetNote(note_buffer[m_ch], 1) == data1) outputs[ch] = Proportion(data2, 127, HEMISPHERE_MAX_CV);
                                    if (!log_skip) log_this = 1;
                                } else outputs[ch] = 0;
                                break;
                            case HEM_MIDI_AT_KEY2_OUT:
                                if (note_buffer[m_ch].size() > 1) {
                                    if (GetNote(note_buffer[m_ch], 2) == data1) outputs[ch] = Proportion(data2, 127, HEMISPHERE_MAX_CV);
                                    if (!log_skip) log_this = 1;
                                } else outputs[ch] = 0;
                                break;
                            case HEM_MIDI_AT_KEY3_OUT:
                                if (note_buffer[m_ch].size() > 2) {
                                    if (GetNote(note_buffer[m_ch], 3) == data1) outputs[ch] = Proportion(data2, 127, HEMISPHERE_MAX_CV);
                                    if (!log_skip) log_this = 1;
                                } else outputs[ch] = 0;
                                break;
                            case HEM_MIDI_AT_KEY4_OUT:
                                if (note_buffer[m_ch].size() > 3) {
                                    if (GetNote(note_buffer[m_ch], 4) == data1) outputs[ch] = Proportion(data2, 127, HEMISPHERE_MAX_CV);
                                    if (!log_skip) log_this = 1;
                                } else outputs[ch] = 0;
                                break;
                        }
                        break;

                    case usbMIDI.AfterTouchChannel:
                        if (function[ch] == HEM_MIDI_AT_CHAN_OUT) {
                            outputs[ch] = Proportion(data1, 127, HEMISPHERE_MAX_CV);
                            if (!log_skip) log_this = 1;
                        }
                        break;

                    case usbMIDI.PitchBend:
                        if (function[ch] == HEM_MIDI_PB_OUT) {
                            int data = (data2 << 7) + data1 - 8192;
                            outputs[ch] = Proportion(data, 8192, HEMISPHERE_3V_CV);
                            if (!log_skip) log_this = 1;
                        }
                        break;

                }
                if (log_this) UpdateLog(message, data1, data2);
            }
        }

        void Send(const int *outvals) {

            // first pass - calculate things and turn off notes
            for (int i = 0; i < DAC_CHANNEL_LAST; ++i) {
                const int midi_ch = outchan[i];

                inputs[i] = outvals[i];
                gate_high[i] = inputs[i] > (12 << 7);
                clocked[i] = (gate_high[i] && last_cv[i] < (12 << 7));
                if (abs(inputs[i] - last_cv[i]) > HEMISPHERE_CHANGE_THRESHOLD) {
                    changed_cv[i] = 1;
                    last_cv[i] = inputs[i];
                } else changed_cv[i] = 0;

                switch (outfn[i]) {
                    case HEM_MIDI_NOTE_OUT:
                        if (changed_cv[i]) {
                            // a note has changed, turn the last one off first
                            SendNoteOff(outchan_last[i]);
                            current_note[midi_ch] = MIDIQuantizer::NoteNumber( inputs[i] );
                        }
                        break;

                    case HEM_MIDI_GATE_OUT:
                        if (!gate_high[i] && changed_cv[i])
                            SendNoteOff(midi_ch);
                        break;

                    case HEM_MIDI_CC_OUT:
                    {
                        const uint8_t newccval = ProportionCV(abs(inputs[i]), 127);
                        if (newccval != current_ccval[i])
                            SendCC(midi_ch, outccnum[i], newccval);
                        current_ccval[i] = newccval;
                        break;
                    }
                }

                // Handle clock pulse timing
                if (note_countdown[i] > 0) {
                    if (--note_countdown[i] == 0) SendNoteOff(outchan_last[i]);
                }
            }

            // 2nd pass - send eligible notes
            for (int i = 0; i < 2; ++i) {
                const int chA = i*2;
                const int chB = chA + 1;

                if (outfn[chB] == HEM_MIDI_GATE_OUT) {
                    if (clocked[chB]) {
                        SendNoteOn(outchan[chB]);
                        // no countdown
                        outchan_last[chB] = outchan[chB];
                    }
                } else if (outfn[chA] == HEM_MIDI_NOTE_OUT) {
                    if (changed_cv[chA]) {
                        SendNoteOn(outchan[chA]);
                        note_countdown[chA] = HEMISPHERE_CLOCK_TICKS * HS::trig_length;
                        outchan_last[chA] = outchan[chA];
                    }
                }
            }

            // I think this can cause the UI to lag and miss input
            //usbMIDI.send_now();
        }

        void SendAfterTouch(const int midi_ch, uint8_t val) {
            usbMIDI.sendAfterTouch(val, midi_ch + 1);
#ifdef ARDUINO_TEENSY41
            usbHostMIDI.sendAfterTouch(val, midi_ch + 1);
            MIDI1.sendAfterTouch(val, midi_ch + 1);
#endif
        }
        void SendPitchBend(const int midi_ch, uint16_t bend) {
            usbMIDI.sendPitchBend(bend, midi_ch + 1);
#ifdef ARDUINO_TEENSY41
            usbHostMIDI.sendPitchBend(bend, midi_ch + 1);
            MIDI1.sendPitchBend(bend, midi_ch + 1);
#endif
        }

        void SendCC(const int midi_ch, int ccnum, uint8_t val) {
            usbMIDI.sendControlChange(ccnum, val, midi_ch + 1);
#ifdef ARDUINO_TEENSY41
            usbHostMIDI.sendControlChange(ccnum, val, midi_ch + 1);
            MIDI1.sendControlChange(ccnum, val, midi_ch + 1);
#endif
        }
        void SendNoteOn(const int midi_ch, int note = -1, uint8_t vel = 100) {
            if (note < 0) note = current_note[midi_ch];
            else current_note[midi_ch] = note;

            usbMIDI.sendNoteOn(note, vel, midi_ch + 1);
#ifdef ARDUINO_TEENSY41
            usbHostMIDI.sendNoteOn(note, vel, midi_ch + 1);
            MIDI1.sendNoteOn(note, vel, midi_ch + 1);
#endif
        }
        void SendNoteOff(const int midi_ch, int note = -1, uint8_t vel = 0) {
            if (note < 0) note = current_note[midi_ch];
            usbMIDI.sendNoteOff(note, vel, midi_ch + 1);
#ifdef ARDUINO_TEENSY41
            usbHostMIDI.sendNoteOff(note, vel, midi_ch + 1);
            MIDI1.sendNoteOff(note, vel, midi_ch + 1);
#endif
        }

    } MIDIState;

    // --- Soft IO ---
    void Out(DAC_CHANNEL channel, int value) {
        // rising edge detection for trigger loopback
        if (value > GATE_THRESHOLD && outputs[channel] < GATE_THRESHOLD)
            clockout_q[channel] = true;

        output_diff[channel] = value - outputs[channel];
        outputs[channel] = value;
    }
    void ClockOut(DAC_CHANNEL ch, const int pulselength = HEMISPHERE_CLOCK_TICKS * HS::trig_length) {
        // short circuit if skip probability is zero to avoid consuming random numbers
        if (0 == clockskip[ch] || random(100) >= clockskip[ch]) {
            clock_countdown[ch] = pulselength;
            outputs[ch] = PULSE_VOLTAGE * (12 << 7);
            clockout_q[ch] = true;
        }
    }
    void NudgeSkip(int ch, int dir) {
        clockskip[ch] = constrain(clockskip[ch] + dir, 0, 100);
    }

    // TODO: Hardware IO should be extracted
    // --- Hard IO ---
    void Load() {
        clocked[0] = OC::DigitalInputs::clocked<OC::DIGITAL_INPUT_1>();
        clocked[1] = OC::DigitalInputs::clocked<OC::DIGITAL_INPUT_2>();
        clocked[2] = OC::DigitalInputs::clocked<OC::DIGITAL_INPUT_3>();
        clocked[3] = OC::DigitalInputs::clocked<OC::DIGITAL_INPUT_4>();
        gate_high[0] = OC::DigitalInputs::read_immediate<OC::DIGITAL_INPUT_1>();
        gate_high[1] = OC::DigitalInputs::read_immediate<OC::DIGITAL_INPUT_2>();
        gate_high[2] = OC::DigitalInputs::read_immediate<OC::DIGITAL_INPUT_3>();
        gate_high[3] = OC::DigitalInputs::read_immediate<OC::DIGITAL_INPUT_4>();
        for (int i = 0; i < ADC_CHANNEL_LAST; ++i) {
            // Set CV inputs
            inputs[i] = OC::ADC::raw_pitch_value(ADC_CHANNEL(i));

            // calculate gates/clocks for all ADC inputs as well
            gate_high[OC::DIGITAL_INPUT_LAST + i] = inputs[i] > GATE_THRESHOLD;
            clocked[OC::DIGITAL_INPUT_LAST + i] = (gate_high[OC::DIGITAL_INPUT_LAST + i] && last_cv[i] < GATE_THRESHOLD);

            if (abs(inputs[i] - last_cv[i]) > HEMISPHERE_CHANGE_THRESHOLD) {
                changed_cv[i] = 1;
                last_cv[i] = inputs[i];
            } else changed_cv[i] = 0;

            // Handle clock pulse timing
            if (clock_countdown[i] > 0) {
                if (--clock_countdown[i] == 0) outputs[i] = 0;
            }
        }
    }

    void Send() {
        for (int i = 0; i < DAC_CHANNEL_LAST; ++i) {
            OC::DAC::set_pitch_scaled(DAC_CHANNEL(i), outputs[i], 0);
        }
        if (autoMIDIOut) MIDIState.Send(outputs);

#ifdef ARDUINO_TEENSY41
        // this relies on the inputs and outputs arrays being contiguous...
        OC::AudioDSP::Process(inputs);
#endif
    }

} IOFrame;

} // namespace HS

