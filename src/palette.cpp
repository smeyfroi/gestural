#include "palette.hpp"
#include <string>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;
vector<string> split(string str, char sep) {
  vector<string> result;
  istringstream stream(str);
  string s;
  while (getline(stream, s, sep)) {
      result.push_back(s);
  }
  return result;
}

// eg https://coolors.co/780000-fdf0d5-003049
Palette::Palette(const std::string& coolerUrl) {
  size_t slashPos = coolerUrl.find_last_of("/");
  if (slashPos == string::npos) return;
  const auto& colorList = coolerUrl.substr(slashPos+1);
  const auto colorStrings = split(colorList, '-');
  transform(colorStrings.begin(), colorStrings.end(), back_inserter(colors), [](string cs) {
    return ofColor::fromHex(std::stoi(cs, nullptr, 16));
  });
}

ofColor Palette::getInterpolated(float amount) {
  if (colors.size() == 0) return ofColor::black;
  if (colors.size() == 1) return colors[0];
  size_t i1 = amount*(colors.size()-1);
  float lerpAmount = amount-1.0/(colors.size()-1)*i1;
  // |---|---|---|---|
  // 0   .25 .5  .75 1
//  cout<<amount<<"   "<<"   "<<i1<<"   "<<colors.size()<<"  "<<lerpAmount<<endl;
  return colors[i1].getLerped(colors[i1+1], lerpAmount);
}
