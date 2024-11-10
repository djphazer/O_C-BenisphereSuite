#include <unordered_map>

namespace PhzConfig {
  enum KEY : uint32_t {
    POWER_CYCLE_COUNT,
    AUDIO_CONFIG,
  };

  typedef uint64_t VALUE;
  typedef std::unordered_map<uint32_t, uint64_t> ConfigMap;

  // Forward Decl
  void listFiles();
  void load_config();
  void save_config();
  void clear_config();

  void setValue(KEY key, VALUE value);
  bool getValue(KEY key, VALUE &value);

  void printDirectory(FS &fs);
  void printDirectory(File dir, int numSpaces);
  void printSpaces(int num);
  void setup();
  void close();
  void eraseFiles();

}
