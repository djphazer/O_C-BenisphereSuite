// Copyright (c) 2016 Patrick Dowling, Tim Churches
//
// Initial app implementation: Patrick Dowling (pld@gurkenkiste.com)
// Modifications by: Tim Churches (tim.churches@gmail.com)
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
//
// Quadrrature LFO app, based on the Mutable Instruments Frames Easter egg 
// quadrature wavetable LFO by Olivier Gillet (see frames_poly_lfo_.h/cpp)

#ifdef ENABLE_APP_POLYLFO

#include "OC_apps.h"
#include "OC_digital_inputs.h"
#include "OC_menus.h"

#include "util/util_math.h"
#include "util/util_settings.h"
#include "frames_poly_lfo.h"

#include "VBiasManager.h"

enum POLYLFO_SETTINGS {
  POLYLFO_SETTING_COARSE,
  POLYLFO_SETTING_FINE,
  POLYLFO_SETTING_TAP_TEMPO,
  POLYLFO_SETTING_SHAPE,
  POLYLFO_SETTING_SHAPE_SPREAD,
  POLYLFO_SETTING_SPREAD,
  POLYLFO_SETTING_COUPLING,
  POLYLFO_SETTING_ATTENUATION,
  POLYLFO_SETTING_OFFSET,
  POLYLFO_SETTING_FREQ_RANGE,
  POLYLFO_SETTING_FREQ_DIV_B,
  POLYLFO_SETTING_FREQ_DIV_C,
  POLYLFO_SETTING_FREQ_DIV_D,
  POLYLFO_SETTING_B_XOR_A,
  POLYLFO_SETTING_C_XOR_A,
  POLYLFO_SETTING_D_XOR_A,
  POLYLFO_SETTING_B_AM_BY_A,
  POLYLFO_SETTING_C_AM_BY_B,
  POLYLFO_SETTING_D_AM_BY_C,
  POLYLFO_SETTING_CV4,
  POLYLFO_SETTING_TR4_MULT,
#ifdef VOR
  POLYLFO_SETTING_VBIAS,
#endif
  POLYLFO_SETTING_LAST
};


const char* const freq_range_names[12] = {
  "cosm", "geol", "glacl", "snail", "sloth", "vlazy", "lazy", "vslow", "slow", "med", "fast", "vfast",
};

const char* const freq_div_names[frames::POLYLFO_FREQ_MULT_LAST] = {
   "16/1", "15/1", "14/1", "13/1", "12/1", "11/1", "10/1", "9/1", "8/1", "7/1", "6/1", "5/1", "4/1", "3/1", "5/2", "2/1", "5/3", "3/2", "5/4",
   "unity", 
   "4/5", "2/3", "3/5", "1/2", "2/5", "1/3", "1/4", "1/5", "1/6", "1/7", "1/8", "1/9", "1/10", "1/11", "1/12", "1/13", "1/14", "1/15", "1/16"
};

const char* const xor_levels[9] = {
  "off", "  1", "  2", "  3", "  4", "  5", "  6", "  7", "  8"
};

const char* const cv4_destinations[7] = {
  "cplg", "sprd", " rng", "offs", "a->b", "b->c", "c->d"
};

const char* const tr4_multipliers[6] = {
  "/8", "/4", "/2", "x2", "x4", "x8"
};

class PolyLfo : public settings::SettingsBase<PolyLfo, POLYLFO_SETTING_LAST> {
public:

  uint16_t get_coarse() const {
    return values_[POLYLFO_SETTING_COARSE];
  }

  int16_t get_fine() const {
    return values_[POLYLFO_SETTING_FINE];
  }

  bool get_tap_tempo() const {
    return static_cast<bool>(values_[POLYLFO_SETTING_TAP_TEMPO]);
  }

  uint16_t get_freq_range() const {
    return values_[POLYLFO_SETTING_FREQ_RANGE];
  }

  uint16_t get_shape() const {
    return values_[POLYLFO_SETTING_SHAPE];
  }

  int16_t get_shape_spread() const {
    return values_[POLYLFO_SETTING_SHAPE_SPREAD];
  }

  int16_t get_spread() const {
    return values_[POLYLFO_SETTING_SPREAD];
  }

  int16_t get_coupling() const {
    return values_[POLYLFO_SETTING_COUPLING];
  }

  uint16_t get_attenuation() const {
    return values_[POLYLFO_SETTING_ATTENUATION];
  }

  int16_t get_offset() const {
    return values_[POLYLFO_SETTING_OFFSET];
  }

  frames::PolyLfoFreqMultipliers get_freq_div_b() const {
    return static_cast<frames::PolyLfoFreqMultipliers>(values_[POLYLFO_SETTING_FREQ_DIV_B]);
  }

  frames::PolyLfoFreqMultipliers get_freq_div_c() const {
    return static_cast<frames::PolyLfoFreqMultipliers>(values_[POLYLFO_SETTING_FREQ_DIV_C]);
  }

  frames::PolyLfoFreqMultipliers get_freq_div_d() const {
    return static_cast<frames::PolyLfoFreqMultipliers>(values_[POLYLFO_SETTING_FREQ_DIV_D]);
  }

  uint8_t get_b_xor_a() const {
    return values_[POLYLFO_SETTING_B_XOR_A];
  }

  uint8_t get_c_xor_a() const {
    return values_[POLYLFO_SETTING_C_XOR_A];
  }

  uint8_t get_d_xor_a() const {
    return values_[POLYLFO_SETTING_D_XOR_A];
  }

  uint8_t get_b_am_by_a() const {
    return values_[POLYLFO_SETTING_B_AM_BY_A];
  }

  uint8_t get_c_am_by_b() const {
    return values_[POLYLFO_SETTING_C_AM_BY_B];
  }

  uint8_t get_d_am_by_c() const {
    return values_[POLYLFO_SETTING_D_AM_BY_C];
  }

  uint8_t cv4_destination() const {
    return values_[POLYLFO_SETTING_CV4];
  }

  uint8_t tr4_multiplier() const {
    return values_[POLYLFO_SETTING_TR4_MULT];
  }

#ifdef VOR
  void saveVbias() {
    VBiasManager *v = v->get();
    values_[POLYLFO_SETTING_VBIAS] = v->GetState();
  }
  void restoreVbias() {
    if (values_[POLYLFO_SETTING_VBIAS] <= 2)
    {
        VBiasManager *v = v->get();
        VBiasManager::VState bias_state = (VBiasManager::VState)values_[POLYLFO_SETTING_VBIAS];
        v->SetState( bias_state );
        //v->DrawPopupPerhaps();
    }
  }
#endif

  void Init();

  void freeze() {
    frozen_ = true;
  }

  void thaw() {
    frozen_ = false;
  }

  bool frozen() const {
    return frozen_;
  }

  uint8_t freq_mult() const {
    return freq_mult_;
  }

  void set_freq_mult(uint8_t freq_mult) {
    freq_mult_ = freq_mult;
  }

  frames::PolyLfo lfo;
  bool frozen_;
  uint8_t freq_mult_;

  // ISR update is at 16.666kHz, we don't need it that fast so smooth the values to ~1Khz
  static constexpr int32_t kSmoothing = 16;

  SmoothedValue<int32_t, kSmoothing> cv_freq;
  SmoothedValue<int32_t, kSmoothing> cv_shape;
  SmoothedValue<int32_t, kSmoothing> cv_spread;
  SmoothedValue<int32_t, kSmoothing> cv_mappable;

  // TOTAL EEPROM SIZE: 22 bytes
  SETTINGS_ARRAY_DECLARE() {{
    { 64, 0, 255, "C", NULL, settings::STORAGE_TYPE_U8 },
    { 0, -128, 127, "F", NULL, settings::STORAGE_TYPE_I16 },
    { 0, 0, 1, "Tap tempo", OC::Strings::off_on, settings::STORAGE_TYPE_U8 }, 
    { 0, 0, 255, "Shape", NULL, settings::STORAGE_TYPE_U8 },
    { 0, -128, 127, "Shape spread", NULL, settings::STORAGE_TYPE_I8 },
    { 0, -128, 127, "Phase/frq sprd", NULL, settings::STORAGE_TYPE_I8 },
    { 0, -128, 127, "Coupling", NULL, settings::STORAGE_TYPE_I8 },
    { 230, 0, 230, "Output range", NULL, settings::STORAGE_TYPE_U8 },
    { 0, -128, 127, "Offset", NULL, settings::STORAGE_TYPE_I8 },
    { 9, 0, 11, "Freq range", freq_range_names, settings::STORAGE_TYPE_U8 },
    { frames::POLYLFO_FREQ_MULT_NONE, frames::POLYLFO_FREQ_MULT_BY16, frames::POLYLFO_FREQ_MULT_LAST - 1, "B freq ratio", freq_div_names, settings::STORAGE_TYPE_U8 },
    { frames::POLYLFO_FREQ_MULT_NONE, frames::POLYLFO_FREQ_MULT_BY16, frames::POLYLFO_FREQ_MULT_LAST - 1, "C freq ratio", freq_div_names, settings::STORAGE_TYPE_U8 },
    { frames::POLYLFO_FREQ_MULT_NONE, frames::POLYLFO_FREQ_MULT_BY16, frames::POLYLFO_FREQ_MULT_LAST - 1, "D freq ratio", freq_div_names, settings::STORAGE_TYPE_U8 },
    { 0, 0, 8, "B XOR A", xor_levels, settings::STORAGE_TYPE_U8 },
    { 0, 0, 8, "C XOR A", xor_levels, settings::STORAGE_TYPE_U8 },
    { 0, 0, 8, "D XOR A", xor_levels, settings::STORAGE_TYPE_U8 }, 
    { 0, 0, 127, "B AM by A", NULL, settings::STORAGE_TYPE_U8 },
    { 0, 0, 127, "C AM by B", NULL, settings::STORAGE_TYPE_U8 },
    { 0, 0, 127, "D AM by C", NULL, settings::STORAGE_TYPE_U8 }, 
    { 0, 0, 6, "CV4: DEST", cv4_destinations, settings::STORAGE_TYPE_U8 },
    { 3, 0, 5, "TR4: MULT", tr4_multipliers, settings::STORAGE_TYPE_U4 }, 
#ifdef VOR
    { 0, 0, 2, "VBias", OC::Strings::VOR_offsets, settings::STORAGE_TYPE_U4 }, 
#endif
  }};
};
SETTINGS_ARRAY_DEFINE(PolyLfo);

void PolyLfo::Init() {
  InitDefaults();
  lfo.Init();
  frozen_= false;
  freq_mult_ = 0x3; // == x2 / default
}

namespace OC {

OC_APP_TRAITS(AppPolyLfo, TWOCCS("PL"), "Quadraturia", "Quadrature LFO");
class OC_APP_CLASS(AppPolyLfo) {
public:
  OC_APP_INTERFACE_DECLARE(AppPolyLfo);
  OC_APP_STORAGE_SIZE(PolyLfo::storageSize());

private:

  PolyLfo poly_lfo_;

  POLYLFO_SETTINGS left_edit_mode_;
  menu::ScreenCursor<menu::kScreenLines> cursor_;

};

void FASTRUN AppPolyLfo::Process(OC::IOFrame *ioframe) {

  bool reset_phase = ioframe->digital_inputs.triggered<OC::DIGITAL_INPUT_1>();
  bool freeze = ioframe->digital_inputs.raised<OC::DIGITAL_INPUT_2>();
  bool tempo_sync = ioframe->digital_inputs.triggered<OC::DIGITAL_INPUT_3>();
 
  // TODO[PLD] Do we need the excessive smoothing?
  poly_lfo_.cv_freq.push(ioframe->cv.values[ADC_CHANNEL_1]);
  poly_lfo_.cv_shape.push(ioframe->cv.values[ADC_CHANNEL_2]);
  poly_lfo_.cv_spread.push(ioframe->cv.values[ADC_CHANNEL_3]);
  poly_lfo_.cv_mappable.push(ioframe->cv.values[ADC_CHANNEL_4]);

  // Range in settings is (0-256] so this gets scaled to (0,65535]
  // CV value is 12 bit so also needs scaling

  int32_t freq = SCALE8_16(poly_lfo_.get_coarse()) + (poly_lfo_.cv_freq.value() * 16) + poly_lfo_.get_fine() * 2;
  freq = USAT16(freq);

  poly_lfo_.lfo.set_freq_range(poly_lfo_.get_freq_range());

  poly_lfo_.lfo.set_sync(poly_lfo_.get_tap_tempo());

  int32_t shape = SCALE8_16(poly_lfo_.get_shape()) + (poly_lfo_.cv_shape.value() * 16);
  poly_lfo_.lfo.set_shape(USAT16(shape));

  int32_t spread = SCALE8_16(poly_lfo_.get_spread() + 128) + (poly_lfo_.cv_spread.value() * 16);
  poly_lfo_.lfo.set_spread(USAT16(spread));

  int32_t coupling = 0;
  int32_t shape_spread = 0;
  int32_t attenuation = 0;
  int32_t offset = 0;
  int32_t b_am_by_a = 0;
  int32_t c_am_by_b = 0;
  int32_t d_am_by_c = 0;
  
  switch (poly_lfo_.cv4_destination()) {
    case 1:  // shape spread: -128, 127
    shape_spread = poly_lfo_.cv_mappable.value() << 4;
    break;
    case 2:  // attenuation: 0, 230
    attenuation = poly_lfo_.cv_mappable.value() << 4;
    break;
    case 3:  // offset: -128, 127
    offset = poly_lfo_.cv_mappable.value() << 4;
    break;
    case 4:  // "a->b", 0-127
    b_am_by_a = (poly_lfo_.cv_mappable.value() + 15) >> 5;
    break;
    case 5:  // "b->c", 0-127
    c_am_by_b = (poly_lfo_.cv_mappable.value() + 15) >> 5;
    break;
    case 6:  // "c->d", 0-127
    d_am_by_c = (poly_lfo_.cv_mappable.value() + 15) >> 5;
    break;
    case 0:  // coupling, -128, 127
    default: 
    coupling = poly_lfo_.cv_mappable.value() << 4;
    break;
  }
  
  coupling += SCALE8_16(poly_lfo_.get_coupling() + 127);
  poly_lfo_.lfo.set_coupling(USAT16(coupling));

  shape_spread += SCALE8_16(poly_lfo_.get_shape_spread() + 127);
  poly_lfo_.lfo.set_shape_spread(USAT16(shape_spread));

  attenuation += SCALE8_16(poly_lfo_.get_attenuation());
  poly_lfo_.lfo.set_attenuation(USAT16(attenuation));

  offset += SCALE8_16(poly_lfo_.get_offset());
  poly_lfo_.lfo.set_offset(USAT16(offset));

  poly_lfo_.lfo.set_freq_div_b(poly_lfo_.get_freq_div_b());
  poly_lfo_.lfo.set_freq_div_c(poly_lfo_.get_freq_div_c());
  poly_lfo_.lfo.set_freq_div_d(poly_lfo_.get_freq_div_d());

  poly_lfo_.lfo.set_b_xor_a(poly_lfo_.get_b_xor_a());
  poly_lfo_.lfo.set_c_xor_a(poly_lfo_.get_c_xor_a());
  poly_lfo_.lfo.set_d_xor_a(poly_lfo_.get_d_xor_a());

  b_am_by_a += poly_lfo_.get_b_am_by_a();
  CONSTRAIN(b_am_by_a, 0, 127);
  poly_lfo_.lfo.set_b_am_by_a(b_am_by_a);

  c_am_by_b += poly_lfo_.get_c_am_by_b();
  CONSTRAIN(c_am_by_b, 0, 127);
  poly_lfo_.lfo.set_c_am_by_b(c_am_by_b);

  d_am_by_c += poly_lfo_.get_d_am_by_c();
  CONSTRAIN(d_am_by_c, 0, 127);
  poly_lfo_.lfo.set_d_am_by_c(d_am_by_c);
  
  // div/multiply frequency if TR4 / gate high
  int8_t freq_mult = digitalReadFast(TR4) ? 0xFF : poly_lfo_.tr4_multiplier();
  poly_lfo_.set_freq_mult(freq_mult);

  if (!freeze && !poly_lfo_.frozen())
    poly_lfo_.lfo.Render(freq, reset_phase, tempo_sync, freq_mult);

  // TODO[PLD] Ranges & offsets
  ioframe->outputs.set_raw_value(DAC_CHANNEL_A, poly_lfo_.lfo.dac_code(0));
  ioframe->outputs.set_raw_value(DAC_CHANNEL_B, poly_lfo_.lfo.dac_code(1));
  ioframe->outputs.set_raw_value(DAC_CHANNEL_C, poly_lfo_.lfo.dac_code(2));
  ioframe->outputs.set_raw_value(DAC_CHANNEL_D, poly_lfo_.lfo.dac_code(3));
}

void AppPolyLfo::GetIOConfig(OC::IOConfig &ioconfig) const
{
  ioconfig.outputs[DAC_CHANNEL_A].set("CH1", OC::OUTPUT_MODE_RAW);
  ioconfig.outputs[DAC_CHANNEL_B].set("CH2", OC::OUTPUT_MODE_RAW);
  ioconfig.outputs[DAC_CHANNEL_C].set("CH3", OC::OUTPUT_MODE_RAW);
  ioconfig.outputs[DAC_CHANNEL_D].set("CH4", OC::OUTPUT_MODE_RAW);
}

void AppPolyLfo::Init() {
  left_edit_mode_ = POLYLFO_SETTING_COARSE;
  cursor_.Init(POLYLFO_SETTING_TAP_TEMPO, POLYLFO_SETTING_LAST - 1);
  poly_lfo_.Init();
}

size_t AppPolyLfo::SaveAppData(util::StreamBufferWriter &stream_buffer) const {
  poly_lfo_.Save(stream_buffer);
  return stream_buffer.written();
}

size_t AppPolyLfo::RestoreAppData(util::StreamBufferReader &stream_buffer) {
  poly_lfo_.Restore(stream_buffer);
  return stream_buffer.read();
}

void AppPolyLfo::Loop() {
}

static const size_t kSmallPreviewBufferSize = 32;
uint16_t preview_buffer[kSmallPreviewBufferSize];

void AppPolyLfo::DrawMenu() const {

  menu::DefaultTitleBar::Draw();
  if (poly_lfo_.get_tap_tempo()) {
    graphics.print("(T) Ch A: tap tempo") ;
  } else {
    float menu_freq_ = poly_lfo_.lfo.get_freq_ch1();

    if (poly_lfo_.freq_mult() < 0xFF) 
        graphics.drawBitmap8(122, menu::DefaultTitleBar::kTextY, 4, OC::bitmap_indicator_4x8); 
    if (menu_freq_ >= 0.1f) {
        const int f = int(floor(menu_freq_ * 100));
        const int value = f / 100;
        const int cents = f % 100;
        graphics.printf("(%s) Ch A: %3u.%02u Hz", PolyLfo::value_attributes(left_edit_mode_).name, value, cents);
    } else {
        const int f = int(floor(1.0f / menu_freq_ * 1000));
        const int value = f / 1000;
        const int cents = f % 1000;
        graphics.printf("(%s) Ch A: %6u.%03us", PolyLfo::value_attributes(left_edit_mode_).name, value, cents);
    }
  }
  menu::SettingsList<menu::kScreenLines, 0, menu::kDefaultValueX - 1> settings_list(cursor_);
  menu::SettingsListItem list_item;
  while (settings_list.available()) {
    const int current = settings_list.Next(list_item);
    const int value = poly_lfo_.get_value(current);
    if (POLYLFO_SETTING_SHAPE != current) {
      list_item.DrawDefault(value, PolyLfo::value_attributes(current));
    } else {
      poly_lfo_.lfo.RenderPreview(value << 8, preview_buffer, kSmallPreviewBufferSize);
      const uint16_t *preview = preview_buffer;
      uint16_t count = kSmallPreviewBufferSize;
      weegfx::coord_t x = list_item.valuex;
      while (count--)
        graphics.setPixel(x++, list_item.y + 8 - (*preview++ >> 13));

      list_item.endx = menu::kDefaultMenuEndX - 39;
      list_item.DrawDefault(value, PolyLfo::value_attributes(current));
    }
  }
}

void AppPolyLfo::DrawScreensaver() const {
  OC::scope_render();
}

void AppPolyLfo::HandleAppEvent(AppEvent event) {
  switch (event) {
    case APP_EVENT_RESUME:
      cursor_.set_editing(false);
#ifdef VOR
      poly_lfo_.restoreVbias();
#endif
      break;
    case APP_EVENT_SUSPEND:
#ifdef VOR
      poly_lfo_.saveVbias();
#endif
    case APP_EVENT_SCREENSAVER_ON:
    case APP_EVENT_SCREENSAVER_OFF:
      break;
  }
}

void AppPolyLfo::HandleButtonEvent(const UI::Event &event) {
  if (UI::EVENT_BUTTON_PRESS == event.type) {
    switch (event.control) {
      case CONTROL_BUTTON_UP:
        if (!poly_lfo_.get_tap_tempo()) poly_lfo_.change_value(POLYLFO_SETTING_COARSE, 32);
        break;
      case CONTROL_BUTTON_DOWN:
        if (!poly_lfo_.get_tap_tempo()) poly_lfo_.change_value(POLYLFO_SETTING_COARSE, -32);
        break;
      case CONTROL_BUTTON_L:
      if (!poly_lfo_.get_tap_tempo()) {
        if (POLYLFO_SETTING_COARSE == left_edit_mode_)
          left_edit_mode_ = POLYLFO_SETTING_FINE;
        else
          left_edit_mode_ = POLYLFO_SETTING_COARSE;
      }
      break;
      case CONTROL_BUTTON_R:
        cursor_.toggle_editing();
        break;
    }
  }
  
  if (UI::EVENT_BUTTON_LONG_PRESS == event.type) {
     switch (event.control) {
      case CONTROL_BUTTON_DOWN:
        poly_lfo_.lfo.set_phase_reset_flag(true);
        break;
      default:
        break;
     }
  } 
}

void AppPolyLfo::HandleEncoderEvent(const UI::Event &event) {
  if (CONTROL_ENCODER_L == event.control) {
    if (!poly_lfo_.get_tap_tempo()) poly_lfo_.change_value(left_edit_mode_, event.value);
  } else if (CONTROL_ENCODER_R == event.control) {
    if (cursor_.editing()) {
      poly_lfo_.change_value(cursor_.cursor_pos(), event.value);
    } else {
      cursor_.Scroll(event.value);
    }
  }
}

void AppPolyLfo::DrawDebugInfo() const {
#ifdef POLYLFO_DEBUG  
  graphics.setPrintPos(2, 12);
  graphics.print(poly_lfo_.cv_shape.value());
  graphics.print(" ");
  int32_t value = SCALE8_16(poly_lfo_.get_shape());
  graphics.print(value);
  graphics.print(" ");
  graphics.print(poly_lfo_.cv_shape.value() * 16);
  value += poly_lfo_.cv_shape.value() * 16;
  graphics.setPrintPos(2, 22);
  graphics.print(value); graphics.print(" ");
  value = USAT16(value);
#endif // POLYLFO_DEBUG
}

} // namespace OC

#endif // ENABLE_APP_POLYLFO
