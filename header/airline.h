#pragma once
#include "types.h"
#include <string>
#include <vector>
using namespace std;

void airlineMenu(const User &userlogged, vector<User> &user,
                 vector<Flight> &flights, vector<Ticket> &ticket);
void viewTotalTransaction(vector<Flight> &flights, vector<Ticket> &ticket,
                          const User &userlogged);
void addFlightData(vector<Flight> &flights, const User &userlogged);
void deleteFlightData(vector<Flight> &flights, const vector<User> &user,
                      const vector<Ticket> &ticket, const User &userlogged);
void editFlightData(vector<Flight> &flights, const vector<User> &user,
                    const User &userlogged);
