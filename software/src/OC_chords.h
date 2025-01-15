#ifndef OC_CHORDS_H_
#define OC_CHORDS_H_

#include "OC_chords_presets.h"

namespace OC {

using Chord = OC::Chord;

class Chords {
public:

  enum CHORD_SLOTS
  {
    CHORDS_USER_0_0,
    CHORDS_USER_1_0,
    CHORDS_USER_2_0,
    CHORDS_USER_3_0,
    CHORDS_USER_4_0,
    CHORDS_USER_5_0,
    CHORDS_USER_6_0,
    CHORDS_USER_7_0,
    CHORDS_USER_0_1,
    CHORDS_USER_1_1,
    CHORDS_USER_2_1,
    CHORDS_USER_3_1,
    CHORDS_USER_4_1,
    CHORDS_USER_5_1,
    CHORDS_USER_6_1,
    CHORDS_USER_7_1,
    CHORDS_USER_0_2,
    CHORDS_USER_1_2,
    CHORDS_USER_2_2,
    CHORDS_USER_3_2,
    CHORDS_USER_4_2,
    CHORDS_USER_5_2,
    CHORDS_USER_6_2,
    CHORDS_USER_7_2,
    CHORDS_USER_0_3,
    CHORDS_USER_1_3,
    CHORDS_USER_2_3,
    CHORDS_USER_3_3,
    CHORDS_USER_4_3,
    CHORDS_USER_5_3,
    CHORDS_USER_6_3,
    CHORDS_USER_7_3,
    CHORDS_USER_LAST
  };

  enum QUALITY 
  {
    CHORDS_FIFTH,
    CHORDS_TRIAD,
    CHORDS_SEVENTH,
    CHORDS_SUSPENDED,
    CHORDS_SUSPENDED_SEVENTH,
    CHORDS_SIXTH,
    CHORDS_ADDED_NINTH,
    CHORDS_ADDED_ELEVENTH,
    CHORDS_UNISONO,
    CHORDS_QUALITY_LAST
  };

  enum CHORDS_TYPE 
  {
    CHORDS_TYPE_MONAD,
    CHORDS_TYPE_DYAD,
    CHORDS_TYPE_TRIAD,
    CHORDS_TYPE_TETRAD,
    CHORDS_TYPE_LAST
  };

  enum VOICING 
  {
    CHORDS_CLOSE,
    CHORDS_DROP_1,
    CHORDS_DROP_2,
    CHORDS_DROP_3,
    CHORDS_SPREAD,
    CHORDS_VOICING_LAST
  };

  enum INVERSION
  {
    CHORDS_INVERSION_NONE,
    CHORDS_INVERSION_FIRST,
    CHORDS_INVERSION_SECOND,
    CHORDS_INVERSION_THIRD,
    CHORDS_INVERSION_LAST 
  };
  
  static void Init();
  static void Validate();
  static const Chord &GetChord(int index, int progression);

  static constexpr int NUM_CHORD_PROGRESSIONS = 0x4;
  static constexpr int NUM_CHORDS_TOTAL = CHORDS_USER_LAST; // = 8
  static constexpr int NUM_CHORDS_PROPERTIES = sizeof(Chord);
  static constexpr int NUM_CHORDS = NUM_CHORDS_TOTAL / NUM_CHORD_PROGRESSIONS;

};


extern Chord user_chords[OC::Chords::CHORDS_USER_LAST];
};

#endif // OC_CHORDS_H_
