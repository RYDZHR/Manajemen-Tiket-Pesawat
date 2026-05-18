#pragma once
#include "types.h"
#include <vector>

void adminMenu(const User &userlogged, vector<User> &user,
               vector<Flight> &flights, vector<Ticket> &ticket);
void banUserAccount(vector<User> &user);
void removeUserAccount(vector<User> &user);
void viewListAccount(const vector<User> &user);
void addNewAccount(vector<User> &user);
