#pragma once
#include "types.h"
#include <string>
#include <vector>
using namespace std;

bool viewFlight(const vector<Flight> &flights, const User &userlogged,
                const vector<User> &user);
