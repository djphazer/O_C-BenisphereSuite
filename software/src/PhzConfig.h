#include <unordered_map>

namespace PhzConfig {
  enum KEY : uint32_t {
    POWER_CYCLE_COUNT,
    AUDIO_CONFIG,
  };

  using VALUE = uint64_t;
  using ConfigMap = std::unordered_map<uint32_t, uint64_t>;

  const char * const CONFIG_FILENAME = "PEWPEW.CFG";

  // Forward Decl
  void listFiles();
  void load_config(const char* filename = CONFIG_FILENAME);
  void save_config(const char* filename = CONFIG_FILENAME);
  void clear_config();

  void setValue(KEY key, VALUE value);
  bool getValue(KEY key, VALUE &value);

  void printDirectory(FS &fs);
  void printDirectory(File dir, int numSpaces);
  void printSpaces(int num);
  void setup();
  void eraseFiles();

}
