---
title: App & Applet Index
nav_order: 7
---

# App and Applet Index

Jump to the lists of [Full Screen Apps](App-and-Applet-Index.md#full-screen-apps) or [Hemisphere Applets](App-and-Applet-Index#hemisphere-applets), also organized [by function](App-and-Applet-Index.md#apps-and-applets-by-function)

## Full Screen Apps
Full screen apps in Phazerville are mostly from the original Ornament and Crime firmware, with a few notable additions ([Calibr8or](_apps/Calibr8or.md), [Scenes](_apps/Scenes.md), and [Passencore](_apps/Passencore.md)). Each of the full screen apps takes advantage of all inputs and outputs in their own way, which is usually configurable.

Not all the apps can fit at once on Teensy 3.2 hardware, but you can use the [default set](https://github.com/djphazer/O_C-Phazerville/releases) or [choose your own selection](https://github.com/djphazer/O_C-Phazerville/discussions/38) with a custom build.
* [Acid Curds](_apps/Acid-Curds.md) - Quad 8-step chord progression sequencer
* [Automatonnetz](_apps/Automatonnetz.md) - Neo-Riemannian transformations on a 5x5 matrix sequence!
* [Calibr8or](_apps/Calibr8or.md) - Quad performance quantizer with pitch tracking calibration
* [Captain MIDI](_apps/Captain-MIDI.md) - Configurable CV-to-MIDI and MIDI-to-CV interface
* [CopierMaschine](_apps/CopierMaschine.md) - Quantizing Analogue Shift Register
* [The Darkest Timeline](_apps/The-Darkest-Timeline.md) - Parallel universe sequencer
* [Dialectic Ping Pong](_apps/Dialectic-Ping-Pong.md)
* [Enigma](_apps/Enigma.md) – Sequencer of shift registers (Turing Machines)
* [Harrington 1200](_apps/Harrington-1200.md) - Neo-Riemannian transformations for triad chord progressions
* Hemisphere - 2 [Applets](#hemisphere-applets) at a time
* [Low-rents](_apps/Low-rents.md) - Lorenz attractor
* [Meta-Q](_apps/Meta-Q.md) - Dual sequenced quantizer
* [Neural Net](_apps/Neural-Net.md) - 6 Neuron logic processor
* [Quadraturia](_apps/Quadraturia.md) - Quadrature wavetable LFO
* [Quantermain](_apps/Quantermain.md) - Quad quantizer
* [References](_apps/References.md) - Tuning utility
* [Scenes](_apps/Scenes.md) - Macro CV switch / crossfader
* [Passencore](_apps/Passencore.md) - Generate a chord progression from LFOs (from sixolet)
* [Piqued](_apps/Piqued.md) - Quad envelope generator
* [Pong](_apps/Pong.md) - It's Pong!
* [Scale Editor](_apps/Scale-Editor.md) - Edit and save microtonal scales
* [Sequins](_apps/Sequins.md) - Basic dual-channel sequencer
* [Waveform Editor](_apps/Waveform-Editor.md) - Edit and save vector waveforms (for [LFOs](_applets/VectorLFO.md), [envelopes](_applets/VectorEG.md), [one-shots](_applets/VectorMod.md), and [phase scrubbing](_applets/VectorMorph.md))
* [Viznutcracker, sweet!](_apps/Viznutcracker-sweet.md) - Quad Bytebeat generator
* [Backup / Restore](_apps/Backup-and-Restore.md) - Transfer app and calibration data as SysEx
* [Setup / About](_apps/Setup-About.md) - Check your version, change encoder directions, adjust display/DAC/ADC, screen off time


***

## Hemisphere Applets
Hemisphere splits the screen into two halves: each side available to load any one of a long list of applets. On o_C hardware with inputs and outputs arranged in 3 rows of 4 columns (i.e. most 8hp units), the I/O corresponding to an applet should be in line with that half of the display (i.e. paired into 1+2/A+B and 3+4/C+D).

If you're coming from any of the other Hemisphere forks, note that many of the applets have been upgraded for additional flexibility and functionality, and several are brand new.
* [ADSR](_applets/ADSR-EG.md) - Dual attack / decay / sustain / release envelope
* [AD EG](_applets/AD-EG.md) - Attack / decay envelope
* [ASR](_applets/ASR.md) - Analog Shift Register
* [AttenOff](_applets/AttenOff.md) - Attenu-vert, Offset, and Mix inputs (now with +/-200% range, mix control)
* [Binary Counter](_applets/Binary-Counter.md) - 1 bit per input, output as voltage
* [BootsNCat](_applets/BootsNCat.md) - Noisy percussion
* [Brancher](_applets/Brancher.md) - Bernoulli gate
* [BugCrack](_applets/BugCrack.md) - Sick drums, don't bug out
* [Burst](_applets/Burst.md) - Rapid trigger / ratchet generator
* [Button2](_applets/Button2.md) - 2 simple triggers or latching gates. Press the button!
* [Calculate](_applets/Calculate.md) - Dual Min, Max, Sum, Diff, Mean, Random, S&H
* [Calibr8](_applets/Calibr8.md) - 2-channel, mini Calibr8or for v/Oct correction
* [Carpeggio](_applets/Carpeggio.md) - X-Y table of pitches from a scale/chord
* [Chordinate](_applets/Chordinate.md) - Quantizer with scale mask, outputs root + scale degree (from qiemem)
* [ClockDivider](_applets/ClockDivider.md) - Dual complex clock pulse multiplier / divider.
* [ClockSkip](_applets/Clock-Skipper.md) - Randomly skip pulses
* [Compare](_applets/Compare.md) - Basic comparator
* [Cumulus](_applets/Cumulus.md) - Bit accumulator, inspired by Schlappi Nibbler
* [CVRec](_applets/CV-Recorder.md) - Record / smooth / playback CV up to 384 steps on 2 tracks
* [DivSeq](_applets/DivSeq.md) - Two sequences of clock dividers
* [DrumMap](_applets/DrumMap.md) - Clone of Mutable Instruments Grids
* [DualQuant](_applets/Dual-Quantizer.md) - Basic 2-channel quantizer with sample and hold
* [DualTM](_applets/DualTM.md) - Highly configurable pair of Turing Machine shift registers (replacement for ShiftReg/TM)
* [Ebb & LFO](_applets/Ebb-&-LFO.md) - clone of Mutable Instruments Tides; oscillator / LFO with CV-controllable waveshape, slope, V/Oct, folding
* [Enigma Jr.](_applets/Enigma-Jr.md) - compact player of curated shift registers
* [EnvFollow](_applets/Envelope-Follower.md) - follows or ducks based on incoming audio
  - added Speed control
* [EuclidX](_applets/EuclidX.md) - Euclidean pattern generator (replacement for [AnnularFusion](https://github.com/Chysn/O_C-HemisphereSuite/wiki/Annular-Fusion-Euclidean-Drummer))
* [GameOfLife](_applets/GameOfLife.md) - experimental cellular automaton modulation source
* [GateDelay](_applets/Gate-Delay.md) - simple gate delay
* [GatedVCA](_applets/Gated-VCA.md) - simple VCA
* [Dr. LoFi](_applets/Dr.-LoFi.md) - super crunchy PCM delay line with bitcrushing and rate reduction
  - based on [LoFi Tape](https://github.com/Chysn/O_C-HemisphereSuite/wiki/LoFi-Tape) and Dr. Crusher
* [Logic](_applets/Logic.md) - AND / OR / XOR / NAND / NOR / XNOR
* [LowerRenz](_applets/LowerRenz.md) - orbiting particles, chaotic modulation source
* [Metronome](_applets/Metronome.md) - internal clock tempo control + multiplier output
* [MIDI In](_applets/MIDI-Input.md) - from USB to CV
* [MIDI Out](_applets/MIDI-Out.md) - from CV to USB
* [MixerBal](_applets/Mixer-Balance.md) - basic CV mixer
* [MultiScale](_applets/MultiScale.md) - like ScaleDuet, but with 4 scale masks
* [Palimpsest](_applets/Palimpsest.md) - accent sequencer
* [Pigeons](_applets/Pigeons.md) - dual Fibonacci-style melody generator
* [PolyDiv](_applets/PolyDiv.md) - four concurrent clock dividers with assignable outputs
* [ProbDiv](_applets/ProbDiv.md) - stochastic trigger generator
* [ProbMeloD](_applets/ProbMeloD.md) - stochastic melody generator
* [ResetClk](_applets/Reset-Clock.md) - rapidly advance a sequencer to the desired step (from [pkyme](https://github.com/pkyme/O_C-HemisphereSuite/tree/reset-additions))
* [RndWalk](_applets/Random-Walk.md) - clocked random walk CV generator (from [adegani](https://github.com/adegani/O_C-HemisphereSuite))
* [RunglBook](_applets/RunglBook.md) - chaotic shift-register modulation
* [ScaleDuet](_applets/ScaleDuet.md) - 2 quantizers with independent scale masks
* [Schmitt](_applets/Schmitt-Trigger.md) - Dual comparator with low and high threshold
* [Scope](_applets/Scope.md) - tiny CV scope / voltmeter / BPM counter
  - expanded with X-Y view
* [Seq32](_applets/Seq32.md) - compact 32-step sequencer using Sequins pattern storage — record, play, and CV switch between 8 sequences!
* [SeqPlay7](_applets/SeqPlay7.md) - Make a playlist of Seq32 sequences, with quantizer override
* [SequenceX](_applets/SequenceX.md) - up to 8 steps of CV, quantized to semitones
* [ShiftGate](_applets/ShiftGate.md) - dual shift register-based gate/trigger sequencer
* [Shredder](_applets/Shredder.md) - clone of Mimetic Digitalis
* [Shuffle](_applets/Shuffle.md) - it don't mean a thing if it ain't got that swing
  - triplets added on 2nd output
* [Slew](_applets/Slew.md) - Dual channel slew: one linear, the other exponential
* [Squanch](_applets/Squanch.md) - advanced quantizer with transpose
* [Stairs](_applets/Stairs.md) - stepped CV
* [Strum](_applets/Strum.md) - the ultimate arpeggiator (pairs well with Rings)
* [Switch](_applets/Switch.md) - CV switch & toggle
* [SwitchSeq](_applets/Switch-Seq.md) - multiple Seq32 patterns running in parallel
* [TB-3PO](_applets/TB-3PO.md) - a brilliant 303-style sequencer
* [TL Neuron](_applets/Threshold-Logic-Neuron.md) - clever logic gate
* [Trending](_applets/Trending.md) - rising / falling / moving / steady / state change / value change
* [TrigSeq](_applets/TrigSeq.md) - two 8-step trigger sequences
* [TrigSeq16](_applets/TrigSeq16.md) - one 16-step trigger sequence
* [Tuner](_applets/Tuner.md) - oscillator frequency detector
* [VectorEG](_applets/VectorEG.md) - Dual envelopes from a library of bipolar and unipolar shapes (customizable with the [Waveform Editor](Waveform-Editor))
* [VectorLFO](_applets/VectorLFO.md) - Dual LFOs from a library of bipolar and unipolar shapes
* [VectorMod](_applets/VectorMod.md) - Dual One-shots from a library of bipolar and unipolar shapes
* [VectorMorph](_applets/VectorMorph.md) - Dual (or linked) phase scrubbing along a library of bipolar and unipolar shapes
* [Voltage](_applets/Voltage.md) - static output CV

***


## Apps and Applets by Function
| Function                 | Hemisphere Applets                                                                                                                                                      | Full Screen Apps                                                                         |
| ------------------------ | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------- |
| **Accent Sequencer**       | [Palimpsest](_applets/Palimpsest.md)                                                                                                                                                        |                                                                                          |
| **Analog Logic**            | [Calculate](_applets/Calculate.md)                                                                                                                                                         |                                                                                          |
| **Clock Modulator**          | [ClockDivider](_applets/ClockDivider.md), [ClockSkip](_applets/Clock-Skipper.md), [DivSeq](_applets/DivSeq.md), [Metronome](_applets/Metronome.md), [PolyDiv](_applets/PolyDiv.md), [ProbDiv](_applets/ProbDiv.md), [ResetClk](_applets/Reset-Clock.md), [Shuffle](_applets/Shuffle.md)                                         |                                                                                          |
| **CV Recorder**              | [ASR](_applets/ASR.md), [CVRec](_applets/CV-Recorder.md)                                                                                                                                                 |                                                                                          |
| **Digital Logic**            | [Binary Counter](_applets/Binary-Counter.md), [Compare](_applets/Compare.md), [Cumulus](_applets/Cumulus.md), [Logic](_applets/Logic.md), [Schmitt](_applets/Schmitt-Trigger.md), [TL Neuron](_applets/Threshold-Logic-Neuron.md), [Trending](_applets/Trending.md)                                                         | [Neural Net](_apps/Neural-Net.md)                                                                             |
| **Delay**                    | [GateDelay](_applets/Gate-Delay.md)                                                                                                                                                         |                                                                                          |
| **Drums / Synth Voice**                    | [BootsNCat](_applets/BootsNCat.md), [BugCrack](_applets/BugCrack.md)                                                                                                                                         | [Viznutcracker, sweet!](_apps/Viznutcracker-sweet.md)                                                                  |
| **Effect**                   | [Dr. LoFi](_applets/Dr.-LoFi.md)                                                                                                                                                          |                                                                                          |
| **Envelope Follower**        | [EnvFollow](_applets/Envelope-Follower.md), [Slew](_applets/Slew.md)                                                                                                                                             |                                                                                          |
| **Envelope Generator**       | [ADSR](_applets/ADSR-EG.md), [AD EG](_applets/AD-EG.md), [VectorEG](_applets/VectorEG.md)                                                                                                                                 | [Piqued](_apps/Piqued.md), [Dialectic Ping Pong](_apps/Dialectic-Ping-Pong.md)                                                          |
| **LFO**                      | [Ebb & LFO](_applets/Ebb-&-LFO.md), [LowerRenz](_applets/LowerRenz.md), [VectorLFO](_applets/VectorLFO.md)                                                                                                                       | [Quadraturia](_apps/Quadraturia.md)                                                                            |
| **MIDI**                     | [MIDI In](_applets/MIDI-Input.md), [MIDI Out](_applets/MIDI-Out.md) _(See also: [Auto MIDI Output](Hemisphere-General-Settings#auto-midi-output))_                                                                                                                                           | [Captain MIDI](_apps/Captain-MIDI.md)                                                                           |
| **Mixer**                    | [MixerBal](_applets/Mixer-Balance.md)                                                                                                                                                          |                                                                                          |
| **Modulation Source**        | [GameOfLife](_applets/GameOfLife.md), [Stairs](_applets/Stairs.md), [VectorMod](_applets/VectorMod.md), [VectorMorph](_applets/VectorMorph.md)                                                                                                                          | [Low-rents](_apps/Low-rents.md), [Pong](_apps/Pong.md)                                                                                   |
| **Performance Utility**      | [Button2](_applets/Button2.md)                                                                                                                                                           |  [Scenes](_apps/Scenes.md)                                                                                        |
| **Pitch Sequencer**          | [Carpeggio](_applets/Carpeggio.md), [DualTM](_applets/DualTM.md), [Enigma Jr.](_applets/Enigma-Jr.md), [Pigeons](_applets/Pigeons.md), [ProbMeloD](_applets/ProbMeloD.md), [Seq32](_applets/Seq32.md), [SeqPlay7](_applets/SeqPlay7.md), [SequenceX](_applets/SequenceX.md), [Shredder](_applets/Shredder.md), [Strum](_applets/Strum.md), [SwitchSeq](_applets/Switch-Seq.md), [TB-3PO](_applets/TB-3PO.md) | [Enigma](_apps/Enigma.md), [The Darkest Timeline](_apps/The-Darkest-Timeline.md), [Automatonnetz](_apps/Automatonnetz.md), [Sequins](_apps/Sequins.md), [Acid Curds](_apps/Acid-Curds.md), [Passencore](_apps/Passencore.md) | 
| **Quantizer**               | [Calibr8](_applets/Calibr8.md), [Chordinate](_applets/Chordinate.md), [DualQuant](_applets/Dual-Quantizer.md), [MultiScale](_applets/MultiScale.md), [ScaleDuet](_applets/ScaleDuet.md), [Squanch](_applets/Squanch.md)                                                                      | [Calibr8or](_apps/Calibr8or.md), [Harrington 1200](_apps/Harrington-1200.md), [Quantermain](_apps/Quantermain.md), [Meta-Q](_apps/Meta-Q.md)                                  |
| **Random / Chaos**           | [Brancher](_applets/Brancher.md), [LowerRenz](_applets/LowerRenz.md), [ProbDiv](_applets/ProbDiv.md), [ProbMeloD](_applets/ProbMeloD.md), [RndWalk](_applets/Random-Walk.md), [Shredder](_applets/Shredder.md)                                                        | [Low-rents](_apps/Low-rents.md)                                                                              |
| **Shift Register**           | [ASR](_applets/ASR.md), [DualTM](_applets/DualTM.md), [Enigma Jr.](_applets/Enigma-Jr.md), [RunglBook](_applets/RunglBook.md), [ShiftGate](_applets/ShiftGate.md)                                                                                               | [Enigma](_apps/Enigma.md), [CopierMaschine](_apps/CopierMaschine.md)                                                               |
| **Switch**                   | [Switch](_applets/Switch.md), [SwitchSeq](_applets/Switch-Seq.md)                                                                                                                                         | [Scenes](_apps/Scenes.md)                                                                                    |
| **Trigger / Gate Sequencer** | [DivSeq](_applets/DivSeq.md), [DrumMap](_applets/DrumMap.md), [EuclidX](_applets/EuclidX.md), [PolyDiv](_applets/PolyDiv.md), [ProbDiv](_applets/ProbDiv.md), [Seq32](_applets/Seq32.md), [ShiftGate](_applets/ShiftGate.md), [TrigSeq](_applets/TrigSeq.md), [TrigSeq16](_applets/TrigSeq16.md)                                 |                                                                                  |
| **VCA**                      | [GatedVCA](_applets/Gated-VCA.md)                                                                                                                                                          |                                                                                          |
| **Voltage Utility**          | [AttenOff](_applets/AttenOff.md), [Calculate](_applets/Calculate.md), [Calibr8](_applets/Calibr8.md), [Scope](_applets/Scope.md), [Slew](_applets/Slew.md), [Stairs](_applets/Stairs.md), [Switch](_applets/Switch.md), [Tuner](_applets/Tuner.md), [Trending](_applets/Trending.md), [Voltage](_applets/Voltage.md)                         | [Calibr8or](_apps/Calibr8or.md), [References](_apps/References.md)                
