#pragma once
#ifndef _HAS_STD_BYTE
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
#include <string>
#include <vector>
using namespace std;

enum Role { ADMIN = 1, AIRLINE = 2, CUSTOMER = 3 };

struct User {
  string userId;
  string username;
  string password;
  bool isActive;
  Role role;

  User() {}
  User(string uId, string u, string p, bool i, Role r)
      : userId(uId), username(u), password(p), isActive(i), role(r) {}
};

struct Flight {
  string flightID;
  string airlineUserID;
  string origin;
  string destination;
  string departureTime;
  int price;
  int capacity;

  Flight() {}
  Flight(string f, string aUID, string o, string d, string t, int p, int c)
      : flightID(f), airlineUserID(aUID), origin(o), destination(d),
        departureTime(t), price(p), capacity(c) {}

  void displayInfo() {
    cout << "Flight ID     : " << flightID << endl;
    cout << "Airline ID    : " << airlineUserID << endl;
    cout << "Origin        : " << origin << endl;
    cout << "Destination   : " << destination << endl;
    cout << "Departure     : " << departureTime << endl;
    cout << "Price         : " << price << endl;
    cout << "Capacity      : " << capacity << endl;
  }
};

struct Ticket {
  string ticketID;
  string flightID; // FK ke Flight
  string userID;   // FK ke User (role CUSTOMER)
  string customerName;
  string bookingStatus;
  string seatNumber;
  string bookingDate; // rename dari 'date' biar tidak ambigu

  Ticket() {}
  Ticket(string t, string f, string u, string c, string b, string s, string d)
      : ticketID(t), flightID(f), userID(u), customerName(c), bookingStatus(b),
        seatNumber(s), bookingDate(d) {}
};
