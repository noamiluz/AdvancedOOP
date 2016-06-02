#ifndef _ENCODER__H_
#define _ENCODER__H_

#include <vector>
#include <string>
using namespace std;

/**
* Encoder class. contains static function
* that creates from a given string that represents
* images pattern - a video.
**/
class Encoder
{
public:
  static bool encode(const string& imagesString, const string& videoOutput);
};

#endif //_ENCODER__H_
