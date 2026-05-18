#pragma once
#include "constants.h"
#include "types.h"
#include <string>
#include <vector>
using namespace std;
inline int getNum(const std::string &s) {
  size_t pos = s.find_first_of("0123456789");
  return (pos != std::string::npos) ? std::stoi(s.substr(pos)) : 0;
}
void clearScreen();
void refreshPendingTicket(vector<Ticket> &ticket, vector<Flight> &flights);
string generateId(vector<Flight> &flights);
string generateIdTicket(vector<Ticket> &ticket);
string generateIdUser(vector<User> &user);
string toStringRole(Role &role);
bool haveSymbol(string username);
bool checkIsDigit(string &x);
bool isValidDateTime(const string &dateTime);
void sortVectorData(vector<User> &user, vector<Flight> &flights,
                    vector<Ticket> &ticket);
