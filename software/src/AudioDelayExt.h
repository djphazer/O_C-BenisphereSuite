#pragma once

#include "AudioBuffer.h"
#include "AudioParam.h"
#include "dsputils.h"
#include "stmlib_utils_dsp.h"
#include <Audio.h>
#include <optional>

template <size_t BufferLength = static_cast<size_t>(AUDIO_SAMPLE_RATE),
          size_t Taps = 1>
class AudioDelayExt : public AudioStream {
public:
  AudioDelayExt() : AudioStream(1, input_queue_array) {
    delay_secs.fill(AudioParam(0.0f, 0.0002f));
  }

  void delay(size_t tap, float secs) {
    delay_secs[tap] = secs;
    if (delay_secs[tap].Read() == 0.0f)
      delay_secs[tap].Reset();
  }

  void cf_delay(size_t tap, float secs) {
    auto& t = target_delay[tap];
    if (t.phase == 0.0f) {
      t.phase = crossfade_dt;
      t.target = secs;
    }
  }

  void feedback(size_t tap, float fb) { this->fb[tap] = fb; }

  void update(void) {
    auto in_block = receiveReadOnly();
    if (in_block == NULL)
      return;

    audio_block_t *outs[Taps];
    for (size_t tap = 0; tap < Taps; tap++) {
      outs[tap] = allocate();
    }

    for (size_t i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
      int32_t in = in_block->data[i];
      for (size_t tap = 0; tap < Taps; tap++) {
        // outs[tap]->data[i] = buffer.ReadInterp(delay_secs[tap].ReadNext());
        outs[tap]->data[i] = ReadNext(tap);
        in += fb[tap].ReadNext() * outs[tap]->data[i];
      }
      buffer.WriteSample(Clip16(in));
    }
    release(in_block);
    for (size_t tap = 0; tap < Taps; tap++) {
      transmit(outs[tap], tap);
      release(outs[tap]);
    }
  }

  int16_t ReadNext(size_t tap) {
    float d = delay_secs[tap].ReadNext();
    auto& target = target_delay[tap];
    if (target.phase > 0.0f) {
      int16_t target_val = buffer.ReadSample(target.target * AUDIO_SAMPLE_RATE);
      int16_t source_val = buffer.ReadSample(d * AUDIO_SAMPLE_RATE);
      float t = target.phase;
      int16_t val = t * target_val + (1.0f - t) * source_val;
      target.phase += crossfade_dt;
      if (target.phase >= 1.0f) {
        target.phase = 0.0f;
        delay_secs[tap] = target.target;
        delay_secs[tap].Reset();
      }
      return val;
    } else {
      return buffer.ReadInterp(d);
    }
  }

private:
  static constexpr float crossfade_dt = 100.0f / AUDIO_SAMPLE_RATE;
  struct CrossfadeTarget {
    float target;
    float phase;
  };

  audio_block_t *input_queue_array[1];
  std::array<CrossfadeTarget, Taps> target_delay;
  std::array<AudioParam<float>, Taps> delay_secs;
  std::array<AudioParam<float>, Taps> fb;
  ExtAudioBuffer<BufferLength> buffer;
};
