// load Pybind11 API
#include <pybind11/pybind11.h>

// C++ class
class Calculator {
public:
    
    Calculator(int value) : value(value) {}

    int add(int x) { return valude + x; }

private:
    int value;
}

Calculator calc1(10);
int result = calc.add(5); //sdf