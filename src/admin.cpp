#include "../header/admin.h"
#include "../header/fileHandler.h"
#include "../header/flight.h"
#include "../header/picosha2.h"
#include "../header/tabulate.hpp"
#include "../header/ticket.h"
#include "../header/types.h"
#include "../header/utils.h"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <limits>
#include <thread>
#include <vector>
using namespace std;
using namespace chrono;

void adminMenu(const User &userlogged, vector<User> &user,
               vector<Flight> &flights, vector<Ticket> &ticket) {
  int inputUser;
  while (true) {
    this_thread::sleep_for(seconds(2));
    clearScreen();
    cout << "Greeting " << userlogged.username << endl;
    cout << "1. View List Account\n";
    cout << "2. Add New Account\n";
    cout << "3. Ban User Account \n";
    cout << "4. Remove User Account\n";
    cout << "5. View List Flight\n";
    cout << "6. View List Ticket Order\n";
    cout << "7. Exit\n";
    cout << "Input : ";
    cin >> inputUser;

    sortVectorData(user, flights, ticket);
    refreshPendingTicket(ticket, flights);
    if (cin.fail()) {
      cin.clear();
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      continue;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    switch (inputUser) {
    case 1:
      viewListAccount(user);
      cout << "Press Enter To Continue...";
      cin.get();
      break;
    case 2:
      addNewAccount(user);
      break;
    case 3:
      banUserAccount(user);
      break;
    case 4:
      removeUserAccount(user);
      break;
    case 5:
      viewFlight(flights, userlogged, user);
      cout << "Press Enter To Continue...";
      cin.get();
      break;
    case 6:
      viewHistoryTicket(ticket, userlogged);
      cout << "Press Enter To Continue...";
      cin.get();
      break;
    case 7:
      return;
      break;
    default:
      cout << "Not In Option\n";
    }
  }
}

void viewListAccount(const vector<User> &user) {
  clearScreen();
  tabulate::Table tableAccount;
  tableAccount.add_row({"No", "User Id", "Username", "Password",
                        "Account Status", "Role Account"});
  int i = 1;
  string role;
  for (auto data : user) {
    tableAccount.add_row({to_string(i), data.userId, data.username,
                          data.password,
                          string(data.isActive ? "Active" : "Inactive"),
                          toStringRole(data.role)});
    i++;
  }
  cout << tableAccount << endl;
}

void addNewAccount(vector<User> &user) {
  clearScreen();
  string name, pass, salt = "s$ltsh4#@";
  cout << "Input Name : ";
  getline(cin, name);
  cout << "Input Pass : ";
  getline(cin, pass);
  auto it = find_if(user.begin(), user.end(),
                    [&name](const User &a) { return a.username == name; });
  if (it != user.end() && it->username == name) {
    cout << "Username Already Used By Another User\n";
    return;
  }
  if (haveSymbol(name) || checkIsDigit(name) ||
      all_of(name.begin(), name.end(), ::isspace)) {
    cout << "Name Cannot Contain Symbol, Digit or Empty\n";
    cout << "Failed To Add Account\n";
    return;
  }
  if (pass.size() <= 5 || !(haveSymbol(pass) || !(checkIsDigit(pass)))) {
    cout << "Password Must Be > 5 Length, Containt Symbol (#$$) and Digit "
            "(123)\n";
    return;
  }

  sort(user.begin(), user.end(),
       [](const User &a, const User &b) { return a.userId < b.userId; });

  pass = picosha2::hash256_hex_string(pass + salt);
  user.emplace_back(generateIdUser(user), name, pass, true, AIRLINE);
  saveUserFile(user);
  cout << "Registration Succes\n";
}

void banUserAccount(vector<User> &user) {
  clearScreen();
  string userId;
  char inputUser;
  viewListAccount(user);
  cout << "Input User Id : ";
  getline(cin, userId);

  auto pos = lower_bound(user.begin(), user.end(), userId,
                         [&](const User &a, const string &targetId) {
                           return getNum(a.userId) < getNum(targetId);
                         });
  if (pos != user.end() && pos->userId == userId) {
    cout << "Account Found : " << pos->userId << " | " << pos->username << endl;
    cout << "1. Ban Account\n";
    cout << "2. Unban Account\n";
    cout << "3. Exit\n";
    cout << "Input Menu : ";
    cin >> inputUser;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (inputUser == '1') {
      cout << "Ban Account (y/n) : ";
      cin >> inputUser;
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      if (inputUser == 'Y' || inputUser == 'y') {
        pos->isActive = false;
        cout << "Account Banned\n";
        saveUserFile(user);
      } else {
        cout << "Failed Banned\n";
      }
    } else if (inputUser == '2') {
      cout << "Unban Account (y/n) : ";
      cin >> inputUser;
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      if (inputUser == 'Y' || inputUser == 'y') {
        pos->isActive = true;
        cout << "Account Unbanned\n";
        saveUserFile(user);
      } else {
        cout << "Failed To Unban\n";
      }
    } else {
      return;
    }
  } else {
    cout << "Account Not Found\n";
  }
}

void removeUserAccount(vector<User> &user) {
  clearScreen();
  string userId;
  char inputUser;
  viewListAccount(user);
  cout << "Input User Id : ";
  getline(cin, userId);

  auto pos = lower_bound(user.begin(), user.end(), userId,
                         [&](const User &a, const string &targetId) {
                           return getNum(a.userId) < getNum(targetId);
                         });
  if (pos != user.end() && pos->userId == userId) {
    cout << "Account Found : " << pos->userId << " | " << pos->username << endl;
    cout << "Remove Account (y/n) ?\n";
    cin >> inputUser;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (inputUser == 'Y' || inputUser == 'y') {
      user.erase(pos);
      cout << "Account Removed\n";
      saveUserFile(user);
    } else {
      cout << "Failed To Remove\n";
    }
  } else {
    cout << "Account Not Found\n";
  }
}
