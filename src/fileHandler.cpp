#include "../header/fileHandler.h"
#include "../header/types.h"
#include "../header/utils.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;
static void stripCR(string &s) {
  if (!s.empty() && s.back() == '\r')
    s.pop_back();
}
int loadFile(vector<User> &user, vector<Ticket> &ticket,
             vector<Flight> &flights) {
  ifstream accountFile("data/account.csv");
  ifstream flightFile("data/flight.csv");
  ifstream ticketFile("data/ticket.csv");
  string line, word, header;
  bool isActive;
  Role role;
  vector<string> temp;
  if (!(accountFile.is_open() && flightFile.is_open() &&
        ticketFile.is_open())) {
    return -1;
  } else {
    if (getline(accountFile, header)) {
    }
    while (getline(accountFile, line)) {
      stripCR(line);
      if (!(line.empty())) {
        stringstream ss(line);
        while (getline(ss, word, ',')) {
          temp.emplace_back(word);
        }
        if (temp[3] == "true") {
          isActive = true;
        } else {
          isActive = false;
        }
        if (temp[4] == "ADMIN") {
          role = ADMIN;
        } else if (temp[4] == "AIRLINE") {
          role = AIRLINE;
        } else {
          role = CUSTOMER;
        }
        user.emplace_back(temp[0], temp[1], temp[2], isActive, role);
        temp.clear();
      }
    }

    if (getline(flightFile, header)) {
    }
    while (getline(flightFile, line)) {
      stripCR(line);
      if (!(line.empty())) {
        stringstream ss(line);
        string word;
        while (getline(ss, word, ',')) {
          temp.emplace_back(word);
        }
        flights.emplace_back(temp[0], temp[1], temp[2], temp[3], temp[4],
                             stoi(temp[5]), stoi(temp[6]));
        temp.clear();
      }
    }
    if (getline(ticketFile, header)) {
    }
    while (getline(ticketFile, line)) {
      stripCR(line);
      if (!(line.empty())) {
        stringstream ss(line);
        string word;
        while (getline(ss, word, ',')) {
          temp.emplace_back(word);
        }
        ticket.emplace_back(temp[0], temp[1], temp[2], temp[3], temp[4],
                            temp[5], temp[6]);
        temp.clear();
      }
    }
  }
  accountFile.close();
  flightFile.close();
  ticketFile.close();
  return 0;
}

void saveUserFile(vector<User> &user) {
  ofstream userFile("data/account.csv");
  string role;
  if (!(userFile.is_open()))
    return;
  else {
    userFile << "userId,username,password,isActive,role\n";
    for (auto &dataUser : user) {
      role = toStringRole(dataUser.role);
      userFile << dataUser.userId << ',' << dataUser.username << ','
               << dataUser.password << ','
               << (dataUser.isActive ? "true" : "false") << ',' << (role)
               << '\n';
    }
    userFile.close();
  }
}

void saveFlightFile(vector<Flight> &flight) {
  ofstream flightFile("data/flight.csv");
  if (!(flightFile.is_open()))
    return;
  else {
    flightFile << "flightId,airlineUserId,origin,destination,datetime,price,"
                  "capacity\n";
    for (const auto &dataFlight : flight) {
      flightFile << dataFlight.flightID << ',' << dataFlight.airlineUserID
                 << ',' << dataFlight.origin << ',' << dataFlight.destination
                 << ',' << dataFlight.departureTime << ',' << dataFlight.price
                 << ',' << dataFlight.capacity << '\n';
    }
    flightFile.close();
  }
}

void saveTicketFile(vector<Ticket> &ticket) {
  ofstream ticketFile("data/ticket.csv");
  if (!(ticketFile.is_open()))
    return;
  else {
    ticketFile
        << "ticketId,flightId,UserId,customerName,bookingStatus,seatNumber,"
           "pendingLimit\n";
    for (const auto &dataTicket : ticket) {
      ticketFile << dataTicket.ticketID << ',' << dataTicket.flightID << ','
                 << dataTicket.userID << ',' << dataTicket.customerName << ','
                 << dataTicket.bookingStatus << ',' << dataTicket.seatNumber
                 << ',' << dataTicket.bookingDate << '\n';
    }
    ticketFile.close();
  }
}
