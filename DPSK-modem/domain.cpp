#include "domain.h"

#include <random>

using namespace std;

void AddGausNoise(vector<double>& samples, double standard_deviation, double average) {
    static random_device rd;
    static mt19937 mt(rd());
    normal_distribution<double> dist(average, standard_deviation);
    for (double& sample : samples) {
        sample += dist(mt);
    }
}
