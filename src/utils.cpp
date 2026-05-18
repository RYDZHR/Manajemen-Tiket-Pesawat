#include "../header/fileHandler.h"
#include "../header/types.h"
#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>
using namespace std;
using namespace chrono;

void clearScreen() {
#ifdef _WIN32
  std::system("cls");
#else
  std::system("clear");
#endif
}
void sortVectorData(vector<User> &user, vector<Flight> &flights,
                    vector<Ticket> &ticket) {
  sort(user.begin(), user.end(), [](const User &a, const User &b) {
    auto getNum = [](const string &s) {
      size_t pos = s.find_first_of("0123456789");
      return (pos != string::npos) ? stoi(s.substr(pos)) : 0;
    };
    return getNum(a.userId) < getNum(b.userId);
  });
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
}
void refreshPendingTicket(vector<User> &user, vector<Ticket> &ticket,
                          vector<Flight> &flights) {
  string statusP = "Pending";
  auto timeNow = chrono::system_clock::now();
  time_t t = chrono::system_clock::to_time_t(timeNow);
  ostringstream oss;
  oss << put_time(localtime(&t), "%Y-%m-%d %H:%M");
  string timeZone = oss.str();

  sort(ticket.begin(), ticket.end(), [](const Ticket &a, const Ticket &b) {
    return a.bookingStatus < b.bookingStatus;
  });

  auto pos = lower_bound(ticket.begin(), ticket.end(), statusP,
                         [](const Ticket &status, const string &statusP) {
                           return status.bookingStatus < statusP;
                         });

  bool changed = false;
  while (pos != ticket.end() && pos->bookingStatus == statusP) {
    if (pos->bookingDate < timeZone) {
      auto posF = lower_bound(flights.begin(), flights.end(), pos->flightID,
                              [](const Flight &id, const string &flightId) {
                                return id.flightID < flightId;
                              });
      if (posF != flights.end() && pos->flightID == posF->flightID) {
        (posF->capacity)++;
        pos->bookingStatus = "Canceled";
        changed = true;
      }
    }
    pos++;
  }

  sort(ticket.begin(), ticket.end(), [](const Ticket &a, const Ticket &b) {
    return a.ticketID < b.ticketID;
  });

  if (changed) {
    sortVectorData(user, flights, ticket);
    saveFlightFile(flights);
    saveTicketFile(ticket);
  }
  sortVectorData(user, flights, ticket);
}

string generateId(vector<Flight> &flights) {
  if (flights.empty())
    return "FL1";
  int maxId = 0;
  for (const auto &f : flights) {
    int id = stoi(f.flightID.substr(2));
    if (id > maxId)
      maxId = id;
  }
  return "FL" + to_string(maxId + 1);
}

string generateIdTicket(vector<Ticket> &ticket) {
  if (ticket.empty())
    return "TIX1";
  int maxId = 0;
  for (const auto &t : ticket) {
    int id = stoi(t.ticketID.substr(3));
    if (id > maxId)
      maxId = id;
  }
  return "TIX" + to_string(maxId + 1);
}

string generateIdUser(vector<User> &user) {
  if (user.empty())
    return "US1";
  int maxId = 0;
  for (const auto &u : user) {
    int id = stoi(u.userId.substr(2));
    if (id > maxId)
      maxId = id;
  }
  return "US" + to_string(maxId + 1);
}

string toStringRole(Role &role) {
  if (role == ADMIN) {
    return "ADMIN";
  } else if (role == AIRLINE) {
    return "AIRLINE";
  } else {
    return "CUSTOMER";
  }
}

bool isValidDateTime(const string &datetime) {
  tm t = {};
  istringstream ss(datetime);
  ss >> get_time(&t, "%Y-%m-%d %H:%M");
  if (ss.fail() || !ss.eof())
    return false;

  int orig_hour = t.tm_hour;
  int orig_min = t.tm_min;
  int orig_mday = t.tm_mday;
  int orig_mon = t.tm_mon;
  int orig_year = t.tm_year;

  time_t inputTime = mktime(&t);
  if (inputTime == -1 || t.tm_hour != orig_hour || t.tm_min != orig_min ||
      t.tm_mday != orig_mday || t.tm_mon != orig_mon ||
      t.tm_year != orig_year) {
    return false;
  }

  if (inputTime <= time(nullptr)) {
    return false;
  }

  return true;
}

bool haveSymbol(string username) {
  for (char c : username) {
    if (ispunct(c)) {
      return true;
    }
  }
  return false;
}

bool checkIsDigit(string &x) {
  for (char &c : x) {
    if (isdigit(c)) {
      return true;
    }
  }
  return false;
}
