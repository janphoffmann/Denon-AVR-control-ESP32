#include "DenonVolume.h"

DenonVolume& DenonVolume::operator--() {
  if (is <= max) is -= 0.5;
  if(_VolCallback != NULL) _VolCallback(_VolCallback_arg, &is);
  return *this;
}
DenonVolume& DenonVolume::operator--(int){
  if (is <= max) is -= 0.5;
  if(_VolCallback != NULL) _VolCallback(_VolCallback_arg, &is);
  return *this;
}
DenonVolume& DenonVolume::operator++(int) {
  if (is <= max) is += 0.5;
  if(_VolCallback != NULL) _VolCallback(_VolCallback_arg, &is);
  return *this;
}
DenonVolume& DenonVolume::operator++() {
  if (is <= max) is += 0.5;
  if(_VolCallback != NULL) _VolCallback(_VolCallback_arg, &is);
  return *this;
}

DenonVolume& DenonVolume::operator=(const float& i) {
  if (i < max && i > min) is = i;
  if(_VolCallback != NULL) _VolCallback(_VolCallback_arg, &is);
  return *this;
}
DenonVolume& DenonVolume::operator=(const int& i) {
  if ((float)i < max && (float)i > min) is = (float)i;
  if(_VolCallback != NULL) _VolCallback(_VolCallback_arg, &is);
  return *this;
}
DenonVolume& DenonVolume::operator=(const double& i) {
  if (i < max && i > min) is = i;
  if(_VolCallback != NULL) _VolCallback(_VolCallback_arg, &is);
  return *this;
}

DenonVolume& DenonVolume::operator+(const int& i){
  if(is+i <=max) is+=i;
  else is = max;
  if(_VolCallback != NULL) _VolCallback(_VolCallback_arg, &is);
  return *this;
}
DenonVolume& DenonVolume::operator-(const int& i){
  if(is-i>=min)is-=i;
  else is = min;
  if(_VolCallback != NULL) _VolCallback(_VolCallback_arg, &is);
  return *this;
}
DenonVolume& DenonVolume::operator+(const float& i){
  if(is+i <=max) is+=i;
  else is = max;  
  if(_VolCallback != NULL) _VolCallback(_VolCallback_arg, &is);
  return *this;
}
DenonVolume& DenonVolume::operator-(const float& i){
  if(is-i>=min)is-=i;
  else is = min;
  if(_VolCallback != NULL) _VolCallback(_VolCallback_arg, &is);
  return *this;
}
DenonVolume& DenonVolume::operator+(const double& i){
  if(is+i <=max) is+=i;
  else is = max;
  if(_VolCallback != NULL) _VolCallback(_VolCallback_arg, &is);
  return *this;
}
DenonVolume& DenonVolume::operator-(const double& i){
  if(is-i>=min)is-=i;
  else is = min;
  if(_VolCallback != NULL) _VolCallback(_VolCallback_arg, &is);
  return *this;
}

void DenonVolume::onVolChange(VolumeHandler callbackFunc, void* arg) {
  _VolCallback = callbackFunc;
  _VolCallback_arg = arg;
}