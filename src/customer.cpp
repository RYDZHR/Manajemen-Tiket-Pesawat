#include "../header/customer.h"
#include "../header/fileHandler.h"
#include "../header/flight.h"
#include "../header/tabulate.hpp"
#include "../header/ticket.h"
#include "../header/types.h"
#include "../header/utils.h"
#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <thread>
#include <vector>
using namespace std;
using namespace tabulate;
using namespace chrono_literals;
using namespace chrono;
void customerMenu(User &userlogged, vector<User> &user, vector<Flight> &flights,
                  vector<Ticket> &ticket) {
  string inputUser;
  clearScreen();
  while (true) {
    this_thread::sleep_for(seconds(2));
    clearScreen();
    cout << "Selamat Datang " << userlogged.username << endl;
    cout << "1. Booking Ticket\n";
    cout << "2. Paying Ticket\n";
    cout << "3. Cancel Ticket\n";
    cout << "4. View Ticket History\n";
    cout << "5. Exit\n";
    cout << "Input User : ";
    cin >> inputUser;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    sort(flights.begin(), flights.end(), [](const Flight &a, const Flight &b) {
      auto getNum = [](const string &s) {
        size_t pos = s.find_first_of("0123456789");
        return (pos != string::npos) ? stoi(s.substr(pos)) : 0;
      };
      return getNum(a.flightID) < getNum(b.flightID);
    });
    sort(ticket.begin(), ticket.end(), [](const Ticket &a, const Ticket &b) {
      auto getNum = [](const string &s) {
        size_t pos = s.find_first_of("0123456789");
        return (pos != string::npos) ? stoi(s.substr(pos)) : 0;
      };
      return getNum(a.ticketID) < getNum(b.ticketID);
    });

    refreshPendingTicket(ticket, flights);
    if (inputUser == "1") {
      bookingTicket(flights, ticket, user, userlogged);
    } else if (inputUser == "2") {
      payTicket(ticket, userlogged);
    } else if (inputUser == "3") {
      cancelTicket(flights, ticket, userlogged);
    } else if (inputUser == "4") {
      viewHistoryTicket(ticket, userlogged);
      cout << "Press Enter To Continue...";
      cin.get();
    } else if (inputUser == "5") {
      return;
    }
  }
}

void bookingTicket(vector<Flight> &flights, vector<Ticket> &ticket,
                   const vector<User> &user, const User &userlogged) {
  string name;
  clearScreen();
  viewFlight(flights, userlogged, user);
  string inputUser;
  auto timeNow = chrono::system_clock::now();
  time_t t = chrono::system_clock::to_time_t(timeNow + hours(1));
  ostringstream oss;
  oss << put_time(localtime(&t), "%Y-%m-%d %H:%M");
  string timeZoned = oss.str();
  cout << "Input Flight Id\n";
  cin >> inputUser;
  cin.ignore(numeric_limits<streamsize>::max(), '\n');

  auto pos = lower_bound(flights.begin(), flights.end(), inputUser,
                         [&](const Flight &a, const string &targetId) {
                           return getNum(a.flightID) < getNum(targetId);
                         });
  if (pos != flights.end() && pos->flightID == inputUser) {
    pos->displayInfo();
    if (pos->capacity >= 1) {
      cout << "Input Passenger Name : ";
      getline(cin, name);
      if (name.empty() || haveSymbol(name) || checkIsDigit(name) ||
          all_of(name.begin(), name.end(), ::isspace)) {
        cout << "Passenger Name Cannot Empty, and Contain Digit or Symbol\n";
        return;
      }
      cout << "Booking Ticket (y/n) : ";
      cin >> inputUser;
      if (inputUser == "Y" || inputUser == "y") {
        ticket.emplace_back(generateIdTicket(ticket), pos->flightID,
                            userlogged.userId, name, "Pending",
                            "A" + to_string(pos->capacity), timeZoned);
        (pos->capacity)--;
        saveTicketFile(ticket);
        saveFlightFile(flights);
      } else {
        cout << "Booking Canceled\n";
      }
    } else {
      cout << "No Available Seat, Flight Is Full\n";
    }
  } else {
    cout << "Flight Id Not Found\n";
  }
}

void payTicket(vector<Ticket> &ticket, const User &userlogged) {
  clearScreen();
  Table tableTicket;
  int i = 1;
  string inputUser;
  tableTicket.add_row({"No", "Ticket Id", "Flight Id", "User Id", "Username",
                       "Ticket Status", "Seat Number", "Pending Limit"});
  for (auto &data : ticket) {
    if (data.userID == userlogged.userId && data.bookingStatus == "Pending") {
      tableTicket.add_row({to_string(i), data.ticketID, data.flightID,
                           data.userID, data.customerName, data.bookingStatus,
                           data.seatNumber, data.bookingDate});
      i++;
    }
  }

  if (i != 1) {
    cout << tableTicket << endl;
    cout << "Input Ticket Id : ";
    cin >> inputUser;

    auto pos = lower_bound(ticket.begin(), ticket.end(), inputUser,
                           [&](const Ticket &a, const string &targetId) {
                             return getNum(a.ticketID) < getNum(targetId);
                           });
    if (pos != ticket.end() && pos->ticketID == inputUser &&
        pos->userID == userlogged.userId && pos->bookingStatus == "Pending") {
      cout << "Pay Ticket ? (y/n) : ";
      cin >> inputUser;
      if (inputUser == "y" || inputUser == "Y") {
        cout << "Process....\n";
        this_thread::sleep_for(4s);
        cout << "Success\n";
        pos->bookingStatus = "Paid";
        saveTicketFile(ticket);
      } else {
        cout << "Paying Canceled\n";
      }
    } else {
      cout << "Ticket Id Not Found\n";
    }
  } else {
    cout << "No Pending Ticket Found\n";
  }
}

void cancelTicket(vector<Flight> &flights, vector<Ticket> &ticket,
                  const User &userlogged) {
  clearScreen();
  Table tableTicket;
  int i = 1;
  string inputUser;

  tableTicket.add_row({"No", "Ticket Id", "Flight Id", "Username",
                       "Ticket Status", "Seat Number", "Pending Limit"});
  for (auto &data : ticket) {
    if (data.userID == userlogged.userId && data.bookingStatus == "Pending") {
      tableTicket.add_row({to_string(i), data.ticketID, data.flightID,
                           data.customerName, data.bookingStatus,
                           data.seatNumber, data.bookingDate});
      i++;
    }
  }

  if (i != 1) {
    cout << tableTicket << endl;
    cout << "Input Ticket Id : ";
    cin >> inputUser;

    auto posT = lower_bound(ticket.begin(), ticket.end(), inputUser,
                            [&](const Ticket &a, const string &targetId) {
                              return getNum(a.ticketID) < getNum(targetId);
                            });
    if (posT == ticket.end() || posT->ticketID != inputUser ||
        posT->userID != userlogged.userId || posT->bookingStatus != "Pending") {
      cout << "Ticket Id Not Found\n";
      return;
    }

    auto posF = lower_bound(flights.begin(), flights.end(), posT->flightID,
                            [](const Flight &id, const string &flightID) {
                              return getNum(id.flightID) < getNum(flightID);
                            });

    if (posF == flights.end() || posF->flightID != posT->flightID) {
      cout << "Associated Flight Not Found\n";
      return;
    }

    cout << "Cancel Ticket ? (y/n) : ";
    cin >> inputUser;
    if (inputUser == "y" || inputUser == "Y") {
      cout << "Process....\n";
      this_thread::sleep_for(4s);
      cout << "Success\n";
      posT->bookingStatus = "Canceled";
      (posF->capacity)++;
      saveTicketFile(ticket);
      saveFlightFile(flights);
    } else {
      cout << "Cancel Ticket Canceled\n";
    }
  } else {
    cout << "No Pending Ticket Found\n";
  }
}
