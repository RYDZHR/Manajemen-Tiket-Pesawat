#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#define NOMCX
#define NOIME
#define NOMINMAX
#define byte win_byte
#include <windows.h>
#undef byte
#endif

#include "header/admin.h"
#include "header/airline.h"
#include "header/auth.h"
#include "header/customer.h"
#include "header/fileHandler.h"
#include "header/types.h"
#include "header/utils.h"
#include <algorithm>
#include <iostream>
#include <limits>
#include <optional>
#include <vector>
using namespace std;

void mainMenu(vector<User> &user, vector<Flight> &flights,
              vector<Ticket> &ticket) {
#ifdef _WIN32
  putenv("LC_ALL=C");
#endif
  setlocale(LC_ALL, "C");
  try {
    std::locale::global(std::locale("C"));
  } catch (...) {
  }
  string input;
  while (true) {
    cout << "1. Login\n";
    cout << "2. Register\n";
    cout << "3. Exit\n";
    cout << "Input : ";
    cin >> input;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    sortVectorData(user, flights, ticket);
    if (input == "1") {
      std::optional<User> userLogged = loginAccount(user);
      if (userLogged) {
        if (userLogged->role == ADMIN)
          adminMenu(*userLogged, user, flights, ticket);
        if (userLogged->role == CUSTOMER)
          customerMenu(*userLogged, user, flights, ticket);
        if (userLogged->role == AIRLINE)
          airlineMenu(*userLogged, user, flights, ticket);
      }
    } else if (input == "2") {
      registerAccount(user);
    } else if (input == "3") {
      exit(0);
    }
  }
}

int main() {
  vector<User> user;
  vector<Flight> flights;
  vector<Ticket> ticket;
  int file = loadFile(user, ticket, flights);
  if (file == -1) {
    cout << "Failed To Load database\n";
    return -1;
  }
  mainMenu(user, flights, ticket);
  return 0;
}
