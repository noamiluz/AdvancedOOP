#include "Encoder.h"

#include <stdlib.h>
#include <string>

void Encoder::encode(const string& imagesString, const string& videoOutput)
{
	// to DELETE : original cmd should be : string ffmpegCmd = "ffmpeg -y -loglevel fatal -i " + imagesString + " " + videoOutput;
	string ffmpegCmd = "ffmpeg  -y -i " + imagesString + " " + videoOutput +"< /dev/null";
  int ret = system(ffmpegCmd.c_str());
  if (ret == -1)
  {
    //handle error
  }
}

