#include "AudioPassthrough.h"
#include "HemisphereAudioApplet.h"

class DelayApplet : public HemisphereAudioApplet {
public:
  const char* applet_name() override {
    return "Delay";
  }
  void Start() override {}
  void Controller() override {}
  void View() override {}
  uint64_t OnDataRequest() override {
    return 0;
  }
  void OnDataReceive(uint64_t data) override {}
  void OnEncoderMove(int direction) override {}

  AudioStream* InputStream() override {
    return &passthru;
  }
  AudioStream* OutputStream() override {
    return &passthru;
  }
  AudioChannels NumChannels() override {
    return MONO;
  }

protected:
  void SetHelp() override {}

private:
  AudioPassthrough<1> passthru;
};
