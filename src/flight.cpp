#include "../header/flight.h"
#include "../header/tabulate.hpp"
#include "../header/types.h"
#include "../header/utils.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>
using namespace std;
using namespace tabulate;

void viewFlight(const vector<Flight> &flights, const User &userlogged,
                const vector<User> &user) {
  clearScreen();
  Table tableData;
  int i = 1;
  string inputDest, inputOri, inputUser;
  auto timeNow = chrono::system_clock::now();
  time_t t = chrono::system_clock::to_time_t(timeNow);
  ostringstream oss;
  oss << put_time(localtime(&t), "%Y-%m-%d %H:%M");
  string timeZoned = oss.str();

  cout << "1. View All Flight\n";
  cout << "2. Search Flight\n";
  cout << "Input User : ";
  cin >> inputUser;
  clearScreen();

  if (inputUser == "2") {
    cout << "Input Origin : ";
    cin >> inputOri;
    cout << "Input Destination : ";
    cin >> inputDest;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
  }

  tableData.add_row({"No", "Flight Id", "Airline Name", "Origin", "Destination",
                     "Date Time", "Price", "Capacity"});

  for (const auto &data : flights) {
    bool passSearch = (inputUser != "2") || (data.origin == inputOri &&
                                             data.destination == inputDest);
    bool passRole = false;
    string airlineName = data.airlineUserID;

    if (userlogged.role == ADMIN) {
      passRole = true;
    } else if (userlogged.role == AIRLINE) {
      passRole = (data.airlineUserID == userlogged.userId);
    } else {
      passRole = (data.departureTime > timeZoned);
      auto it = lower_bound(user.begin(), user.end(), data.airlineUserID,
                            [](const User &a, const string &b) {
                              return getNum(a.userId) < getNum(b);
                            });
      if (it != user.end() && it->userId == data.airlineUserID)
        airlineName = it->username;
    }

    if (passRole && passSearch) {
      tableData.add_row({to_string(i), data.flightID, airlineName, data.origin,
                         data.destination, data.departureTime,
                         to_string(data.price), to_string(data.capacity)});
      i++;
    }
  }

  if (i == 1)
    cout << "Flight Not Found\n";
  else
    cout << tableData << endl;
}
