/*
Mastalgia - a (soon to be) Sega Master System emulator.
Lisenced under the GPLv3 license.
@author: Peter Savory
 */

class Memory {
public:
  Memory();
  ~Memory();
  unsigned char read(unsigned short location);
  void write(unsigned short location, unsigned char value);
private:
};
