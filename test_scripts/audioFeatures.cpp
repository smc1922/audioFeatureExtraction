//trying to test c++ pybind and python integration
#include <vector>
#include <cmath>
using namespace std;

int main() {
    vector<double> audioData = {0.0, -0.1, 0.1, -0.2};
    //left off here...
}

// declare c++ class named audioFeatures
class audioFeatures {
private:
    vector<double> signal;

public:
    // return constant reference
    const vector<double>& getAudio() const { return signal; }

    double zcr() const {}
    double rms() const {}
};