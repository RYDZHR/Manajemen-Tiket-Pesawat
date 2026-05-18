#include "../header/airline.h"
#include "../header/fileHandler.h"
#include "../header/flight.h"
#include "../header/types.h"
#include "../header/utils.h"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <limits>
#include <string>
#include <thread>
#include <vector>
using namespace std;
using namespace chrono;

void airlineMenu(const User &userlogged, vector<User> &user,
                 vector<Flight> &flights, vector<Ticket> &ticket) {
  string inputUser;
  while (true) {
    this_thread::sleep_for(seconds(2));
    clearScreen();
    cout << "1. View Transaction" << endl;
    cout << "2. Add New Flight Schedule" << endl;
    cout << "3. Remove Flight" << endl;
    cout << "4. Edit Flight" << endl;
    cout << "5. View Flight" << endl;
    cout << "6. Exit" << endl;
    cout << "Input User : ";
    cin >> inputUser;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    sortVectorData(user, flights, ticket);

    refreshPendingTicket(ticket, flights);
    if (inputUser == "1") {
      viewTotalTransaction(flights, ticket, userlogged);
      cout << "Press Enter To Continue...";
      cin.get();
    } else if (inputUser == "2") {
      addFlightData(flights, userlogged);
    } else if (inputUser == "3") {
      deleteFlightData(flights, user, ticket, userlogged);
    } else if (inputUser == "4") {
      editFlightData(flights, user, userlogged);
    } else if (inputUser == "5") {
      viewFlight(flights, userlogged, user);
      cout << "Press Enter To Continue...";
      cin.get();
    } else if (inputUser == "6") {
      return;
    }
  }
}

void viewTotalTransaction(vector<Flight> &flights, vector<Ticket> &ticket,
                          const User &userlogged) {
  clearScreen();
  int sum = 0;
  for (const auto &f : flights) {
    if (f.airlineUserID == userlogged.userId) {
      int count = 0;

      for (const auto &t : ticket) {
        if (t.flightID == f.flightID && t.bookingStatus == "Paid") {
          sum += f.price;
          count++;
        }
      }
      cout << "Flight " << f.flightID << " | " << f.origin << " -> "
           << f.destination << " | Tickets Sold: " << count
           << " | Revenue: " << (count * f.price) << "\n";
    }
  }

  cout << "Total Revenue : " << sum << endl;
}

void addFlightData(vector<Flight> &flights, const User &userlogged) {
  clearScreen();
  string origin, dest, date_time;
  char inputUser;
  int price, capacity;
  cout << "Input Origin : ";
  getline(cin, origin);
  if (haveSymbol(origin) || checkIsDigit(origin)) {
    cout << "Origin Cannot Containt Symbol or Digit\n";
    return;
  }
  cout << "Input Destination : ";
  getline(cin, dest);
  if (haveSymbol(dest) || checkIsDigit(dest)) {
    cout << "Destination Cannot Containt Symbol or Digit\n";
    return;
  }
  cout << "Input Date Time (YYYY-MM-DD H:M): ";
  getline(cin, date_time);
  if (!(isValidDateTime(date_time))) {
    cout << "Failed, Wrong Date Time Format Or Date Is In The Past, Example "
            "(2026-03-10 23:59)\n";
    return;
  }
  cout << "Input Price : ";
  cin >> price;
  if (cin.fail()) {
    cout << "Failed, Price Must Integer\n";
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return;
  } else if (price < 1) {
    cout << "Failed, Price Cannot Empty Or Mines\n";
    return;
  }
  cout << "Input Capacity : ";
  cin >> capacity;
  if (cin.fail()) {
    cout << "Failed, Capacity Must Integer\n";
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return;
  } else if (capacity < 1) {
    cout << "Failed, Capacity Cannot Empty Or Mines\n";
    return;
  }
  cout << "Added New Flight(y/n)? : ";
  cin >> inputUser;
  if (inputUser == 'Y' || inputUser == 'y') {

    string id = generateId(flights);
    flights.emplace_back(id, userlogged.userId, origin, dest, date_time, price,
                         capacity);
    cout << "Added Flight Succesfull\n";
    saveFlightFile(flights);
  } else {
    cout << "Added Flight Canceled\n";
  }
}

void deleteFlightData(vector<Flight> &flights, const vector<User> &user,
                      const vector<Ticket> &ticket, const User &userlogged) {
  clearScreen();
  string inputId;
  char inputUser;
  if (userlogged.role == AIRLINE) {
    viewFlight(flights, userlogged, user);
    cout << "Input Flight Id : ";
    cin >> inputId;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    auto pos = lower_bound(flights.begin(), flights.end(), inputId,
                           [&](const Flight &a, const string &targetId) {
                             return getNum(a.flightID) < getNum(targetId);
                           });
    if (pos != flights.end() && pos->flightID == inputId &&
        pos->airlineUserID == userlogged.userId) {
      cout << "Flight Found : " << pos->flightID << " | " << pos->airlineUserID
           << endl;
      auto it = lower_bound(ticket.begin(), ticket.end(), inputId,
                            [](const Ticket &id, const string &targetId) {
                              return getNum(id.flightID) < getNum(targetId);
                            });

      if (it != ticket.end() && it->flightID == pos->flightID &&
          it->bookingStatus == "Paid") {
        cout << "Sorry, there is already a customer who has paid for the "
                "ticket\n";
        return;
      }
      cout << "Delete (y/n): ";
      cin >> inputUser;
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      if (inputUser == 'y' || inputUser == 'Y') {
        cout << "Delete Flight Succesfull\n";
        flights.erase(pos);
        saveFlightFile(flights);
      } else {
        cout << "Delete Flight Canceled\n";
      }
    } else {
      cout << "Flight Id Not Found\n";
    }
  }
}

void editFlightData(vector<Flight> &flights, const vector<User> &user,
                    const User &userlogged) {
  clearScreen();
  string inputId;
  int price, capacity;
  char inputUser;
  viewFlight(flights, userlogged, user);
  cout << "Input Flight Id : ";
  cin >> inputId;
  cin.ignore(numeric_limits<streamsize>::max(), '\n');
  auto pos = lower_bound(flights.begin(), flights.end(), inputId,
                         [&](const Flight &a, const string &targetId) {
                           return getNum(a.flightID) < getNum(targetId);
                         });
  if (pos != flights.end() && pos->flightID == inputId &&
      pos->airlineUserID == userlogged.userId) {
    cout << "Flight Found : " << pos->flightID << " | " << pos->airlineUserID
         << endl;
    cout << "Edit? (y/n): ";
    cin >> inputUser;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (inputUser == 'y' || inputUser == 'Y') {
      cout << "Input Capacity : ";
      cin >> capacity;
      if (cin.fail()) {
        cout << "Failed, Capacity Must Integer\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
      } else if (pos->capacity < 1) {
        cout << "Failed, Capacity Cannot Empty Or Mines\n";
        return;
      }
      pos->capacity = capacity;
      cout << "Edit Flight Succescull\n";
      saveFlightFile(flights);
    } else {
      cout << "Edit Flight Canceled\n";
    }
  } else {
    cout << "Flight Id Not Found\n";
  }
}
