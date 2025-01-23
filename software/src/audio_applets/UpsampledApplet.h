#pragma once

#include "HSicons.h"
#include "HemisphereAudioApplet.h"
#include "Audio/InterpolatingStream.h"
#include <Audio.h>

template <AudioChannels Channels>
class UpsampledApplet : public HemisphereAudioApplet {
public:
  const char* applet_name() override {
    return "Upsampled";
  }

  void Start() override {
    interp_stream.Acquire();
    interp_stream.Method(static_cast<InterpolationMethod>(method));

    for (int c = 0; c < Channels; c++) {
      interp_conn[c].connect(interp_stream, 0, mixer[c], 0);
      in_conn[c].connect(input_stream, c, mixer[c], 1);
      out_conn[c].connect(mixer[c], 0, output_stream, c);
      mixer[c].gain(0, 1.0f);
      mixer[c].gain(1, 1.0f);
    }
  }

  void Unload() override {
    interp_stream.Release();
    AllowRestart();
  }

  void Controller() override {
    int16_t in = input.In();
    // 0.001 should put cutoff at ~2.7Hz for SR of 16666
    ONE_POLE(lp, in, 0.001f);

    if (ac_couple) in -= lp;
    interp_stream.Push(Clip16(0.01f * gain * scalar * in));
  }

  void View() override {
    gfxPrint(1, 15, "Source:");
    gfxStartCursor();
    gfxPrintIcon(input.Icon());
    gfxEndCursor(cursor == 0);

    gfxPrint(1, 25, "Interp:");
    gfxStartCursor();
    switch (method) {
      case INTERPOLATION_ZOH:
        gfxPrint("ZOH");
        break;
      case INTERPOLATION_LINEAR:
        gfxPrint("Lin");
        break;
      case INTERPOLATION_HERMITE:
        gfxPrint("Spl");
        break;
    }
    gfxEndCursor(cursor == 1);

    gfxPrint(1, 35, "Gain:");
    gfxStartCursor();
    graphics.printf("%4d%%", gain);
    gfxEndCursor(cursor == 2);

    gfxPrint(1, 45, "AC:    ");
    gfxStartCursor();
    gfxPrintIcon(ac_couple ? CHECK_ON_ICON : CHECK_OFF_ICON);
    gfxEndCursor(cursor == 3);
  }

  void OnButtonPress() override {
    if (cursor == 3) {
      ac_couple = !ac_couple;
    } else {
      CursorToggle();
    }
  }

  void OnEncoderMove(int direction) override {
    if (!EditMode()) {
      MoveCursor(cursor, direction, 3);
      return;
    }

    switch (cursor) {
      case 0:
        input.ChangeSource(direction);
        break;
      case 1:
        method += direction;
        CONSTRAIN(method, 0, 2);
        interp_stream.Method(static_cast<InterpolationMethod>(method));
        break;
      case 2:
        gain += direction;
        CONSTRAIN(direction, -999, 999);
        break;
    }
  }
  void OnDataRequest(std::array<uint64_t, CONFIG_SIZE>& data) override {
    data[0] = PackPackables(pack(gain), pack<1>(ac_couple), pack<2>(method));
    data[1] = PackPackables(input);
  }

  void OnDataReceive(const std::array<uint64_t, CONFIG_SIZE>& data) override {
    UnpackPackables(data[0], pack(gain), pack<1>(ac_couple), pack<2>(method));
    UnpackPackables(data[1], input);
  }

  AudioStream* InputStream() override {
    return &input_stream;
  }
  AudioStream* OutputStream() override {
    return &output_stream;
  };

protected:
  void SetHelp() override {}

private:
  AudioPassthrough<Channels> input_stream;
  InterpolatingStream<> interp_stream;
  AudioMixer<2> mixer[Channels];
  AudioPassthrough<Channels> output_stream;

  AudioConnection in_conn[Channels];
  AudioConnection interp_conn[Channels];
  AudioConnection out_conn[Channels];

  CVInputMap input;
  float lp = 0.0f;
  static constexpr float scalar = -31267.0f / HEMISPHERE_MAX_CV;
  int cursor = 0;
  int8_t method = INTERPOLATION_HERMITE;
  int16_t gain = 90;
  boolean ac_couple = 0;
};
