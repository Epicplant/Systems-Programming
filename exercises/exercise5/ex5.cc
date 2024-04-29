/*
 * Copyright @2023 Christopher Roy. All rights reserved. Permission is
 * hereby granted to students registered for Unversity of Washgnton
 * CSE 333 for use solely during Fall Quarter 2023 for purposes of
 * the course. No other use, copying, distributon, or modification
 * is permitted wthout prior written consent. Copyrights for
 * third-party components of this work must be honored. Instructors
 * interested in reusing these course materials should contact the
 * author.
 *
 * Name: Christopher Roy
 * Email: cr0y@uw.edu
 */

#include <iostream>
#include <cstdlib>

using std::cout;
using std::cin;
using std::cerr;
using std::endl;

int main(int argc, char** argv) {
    int num;

    // There should be no arguments here
    if (argc != 1) {
        cerr << "This executive takes no parameters" << endl;
        return EXIT_FAILURE;
    }

    // Asks for user input to populate num integer
    cout << "Which positive integer would you like me to factorize? ";
    cin >> num;

    // Checks to see if input was an integer or not (simply rounds
    // decimals down). If the string starts with a readable number,
    // the program will simply use the first readable number
    // and not use anything else. Additionally, if the integer overflows,
    // then an error is returned.
    if (!cin) {
        cerr << "Input could not be interprted as an integer" << endl;
        return EXIT_FAILURE;
    }

    // If num is less than 0 there are no factors. Bad input
    if (num <= 0) {
        cerr << "Input was not a positive integer or was too big" << endl;
        return EXIT_FAILURE;
    }

    // Every positive number has a factor of 1
    cout << 1;

    // Loop num-1 times to print out any other possible factors
    for (int i = 2; i <= num; i++) {
        if (num % i == 0) {
            cout << " " << i;
        }
     }

    cout << std::endl;
    return EXIT_SUCCESS;
}
