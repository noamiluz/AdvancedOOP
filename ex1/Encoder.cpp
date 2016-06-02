#include "Encoder.h"

#include <stdlib.h>
#include <string>

bool Encoder::encode(const string& imagesString, const string& videoOutput)
{
	// to DELETE : original cmd should be : string ffmpegCmd = "ffmpeg -y -loglevel fatal -i " + imagesString + " " + videoOutput;
	string ffmpegCmd = "ffmpeg -y -i " + imagesString + " " + videoOutput + " > /dev/null 2>&1";
  int ret = system(ffmpegCmd.c_str());
  if (ret == -1)
  {
	  return false;
  }
  /*
  string cmd = "rm -rf log.txt";
  int ret2 = system(cmd.c_str());
  if (ret2 == -1)
  {
	  //handle error
  }*/

  return true;
}

