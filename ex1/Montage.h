#ifndef _MONTAGE__H_
#define _MONTAGE__H_

#include <vector>
#include <string>
using namespace std;

/**
* Montage class. contains static function
* that compose a given vector of images.
**/
class Montage
{
public:
  static bool compose(const vector<string> &images, int cols, int rows, const string& composedImagePath);
};

#endif //_MONTAGE__H_
