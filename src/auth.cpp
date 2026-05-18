#include "../header/auth.h"
#include "../header/fileHandler.h"
#include "../header/picosha2.h"
#include "../header/types.h"
#include "../header/utils.h"
#include <algorithm>
#include <iostream>
#include <limits>
#include <optional>
#include <vector>
using namespace std;

std::optional<User> loginAccount(vector<User> &user) {
  clearScreen();
  string name, pass, salt = "s$ltsh4#@", checkHash;
  cout << "Input Name : ";
  getline(cin, name);
  cout << "Input Pass : ";
  getline(cin, pass);
  auto pos = find_if(user.begin(), user.end(),
                     [&name](const User &a) { return a.username == name; });
  if (pos != user.end() && pos->username == name) {
    if (pos->isActive == false) {
      cout << "This Account Not Active\n";
      return std::nullopt;
    }
    checkHash = picosha2::hash256_hex_string(pass + salt);
    if (!(pos->password == checkHash)) {
      clearScreen();
      cout << "Wrong Password, Try Again!\n";
      return std::nullopt;
    }
    return *pos;
  } else {
    clearScreen();
    cout << "Account Not Found, Regist First\n";
    return std::nullopt;
  }
}

void registerAccount(vector<User> &user) {
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

  pass = picosha2::hash256_hex_string(pass + salt);
  user.emplace_back(generateIdUser(user), name, pass, true, CUSTOMER);
  saveUserFile(user);
  cout << "Registration Succes\n";
}
