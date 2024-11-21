// This is a hotel room reservation program of (5p)

#include <iostream>
#include <cstdlib>
#include <string>
#include <random>
#include <cmath>
#include <cctype>
#include <fstream>
#include <sstream>
using namespace std;

const double PRICESINGLE = 100;
const double PRICETWIN = 150;
const int MIN_ROOMS = 40;
const int MAX_ROOMS = 300;
const int MAX_NIGHTS = 7;


/* Randomly generate and return an even number of rooms
between MIN_ROOMS and MAX_ROOMS. Half of which are single bed and the
other half double bed rooms.*/
void RoomGenerator(int&, mt19937&);

// Reserve new rooms.
void ReserveNew(int[], int[], string[], string, int, mt19937&);

/* Search and remove pre-existing reservations by
the name of the reserver or the reservation ID. */
void SearchReserved(int[], int[], string[], string, int);

// Get the reservation status of a given room.
bool ReservationStatus(int[], int);

// Check if the file containing the reservation data exists and is not empty.
bool fileExists(const string&);

// Validates and manipulates a given full name.
bool IsValidFullname(string&);

// Ask the user to input either 1 or 2 and validate.
string MenuChoice();

/* Gives the user the option to either manually or automatically
choose the room number. Depending on whether we want to get a
single bed room number or a double bed room number,
input corresponding range numbers (ints).*/
int ChooseRoom(int[], mt19937&, string, int, int);



int main()
{
    // Random number generation (Mersenne twister).
    random_device rd;
    mt19937 generator(rd());

    int numberOfRooms;
    int lineCounter = 0; // Count number of lines in the file.
    bool error; // Menu error check.
    bool again = true; // Run program again.
    string menuNumber;
    string line; // Reading the lines in the file.

    /* Arrays to hold info about the rooms i.e.
    their reservation ID, room number and 
    the respective reserver name. */
    int *RoomsID = nullptr;
    int *RoomsNumber = nullptr;
    string *RoomsName = nullptr;

    // If the file exists and is not empty, open.
    fstream reservationData;
    if (fileExists("reservationData.txt")) 
    {
        reservationData.open("reservationData.txt", ios::in);
        
        // Count the number of lines.
        while (getline(reservationData, line))
            ++lineCounter;

        reservationData.clear();
        reservationData.seekg(0, ios::beg);

        /* The number of rooms is the number of lines in the file divided by three
        because the file contains the name, room number and ID on seperate line
        for each reservation. */
        numberOfRooms = lineCounter/3;

        RoomsID = new int[numberOfRooms];
        RoomsNumber = new int[numberOfRooms];
        RoomsName = new string[numberOfRooms];
        
        for (int i=0; i<numberOfRooms; ++i)
        {
            getline(reservationData, line);
            RoomsName[i] = line;

            getline(reservationData, line);
            RoomsNumber[i] = stoi(line);

            getline(reservationData, line);
            RoomsID[i] = stoi(line);
        }

        reservationData.close();
    }

    else
    {
        cout << "The reservation data text file appears to be empty/missing/doesn't exist.\n"
                "Would you like to generate a new text file? This will generate new rooms. (y/n)\n";

        do
        {
            error = false;
            cout << ">> ";
            cin >> menuNumber;
            if (menuNumber !="y" && menuNumber !="Y" && menuNumber !="n" && menuNumber !="N")
            {
                cout << "\nInvalid input, please re-enter.\n";
                error = true;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        } while (error);

        if (menuNumber == "n" || menuNumber == "N")
        {
            cout << "\nThe program cannot save reservation data\n"
                    "without the necessary text file.\n";
            cout << "Please make sure the text file is in the same folder \n"
                    "as the program or create a new file.\n";

            return 1;
        }

        RoomGenerator(numberOfRooms, generator);

        RoomsID = new int[numberOfRooms]();
        RoomsNumber = new int[numberOfRooms]();
        RoomsName = new string[numberOfRooms];

        for (int i=0; i<numberOfRooms; ++i)
            RoomsName[i] = "N/A";
    }
        
    do
    {
        cout << endl << "┌─────────────────────────┐\n"
                     << "│Welcome to the hotel room│\n"
                     << "│   reservation program!  │\n"
                     << "└─────────────────────────┘\n";

        cout << endl << "Do you want to reserve or search for a pre-existing reservation? (enter number)";
        cout << endl << "1. Reserve new";
        cout << endl << "2. Search for pre-existing reservation";
        cout << endl << "3. Exit program and save changes\n";
        
        do // Choose 1, 2 or 3.
        {
            error = false;
            cout << ">> ";
            cin >> menuNumber;
            if (cin.fail() || (menuNumber!="1" && menuNumber!="2" && menuNumber!="1." && menuNumber!="2." && menuNumber!="3" && menuNumber!="3."))
            {
                cout << "\nPlease enter a number corresponding to a menu item.\n" ;
                error = true;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        } while (error);
        
        // Reserve a new room.
        if (menuNumber == "1" || menuNumber == "1.")
            ReserveNew(RoomsID, RoomsNumber, RoomsName, menuNumber, numberOfRooms, generator);
        
        // Search for pre-existing reservation.
        if (menuNumber == "2" || menuNumber == "2.")
            SearchReserved(RoomsID, RoomsNumber, RoomsName, menuNumber, numberOfRooms);

        // Exit program.
        if (menuNumber == "3" || menuNumber == "3.")
            again = false;

    } while(again);

    reservationData.open("reservationData.txt", ios::out | ios::trunc);

    if (!reservationData)
        cout << "error";

    for (int i=0; i<numberOfRooms; ++i)
    {
        reservationData << RoomsName[i] << endl;
        reservationData << RoomsNumber[i] << endl;
        reservationData << RoomsID[i] << endl;
    }

    reservationData.close();
 
    delete[] RoomsID;
    delete[] RoomsNumber;
    delete[] RoomsName;

    return EXIT_SUCCESS;
}



void RoomGenerator(int &numberOfRooms, mt19937 &generator)
{
    // Generate even number of rooms using Mersenne Twister.
    uniform_int_distribution<int> RoomGenerator(MIN_ROOMS, MAX_ROOMS);

    do
    {
        numberOfRooms = RoomGenerator(generator);
    } while ((numberOfRooms % 2) != 0);
}



void ReserveNew(int RoomsID[], int RoomsNumber[], string RoomsName[], string menuNumber, int numberOfRooms, mt19937 &generator)
{
    bool allFull = true; // All rooms reserved.
    bool error; // Menu error check.
    int roomNumber; // Room number entered by reserver.
    int nights; // Amount of nights
    int reservationID; // Randomly generated unique ID
    double discount; // Randomly generated discount factor.
    double price; // The total price of the reservation.
    string fullName; // Full name of the reserver.

    // Test if there are any rooms available.
    for (int i=0; i<numberOfRooms; ++i)
        if (RoomsID[i] == 0)
        {
            allFull = false;
            break;
        }
    
    if (allFull)
    {
        cout << endl <<  "Unfortunately all rooms are reserverd at this time." << endl << endl;
        return;
    }

    // Choose room number manually or automatically.
    cout << endl << "There are " << numberOfRooms/2 << " single and double bed rooms.\n" 
                    "Which wone would you like? (enter number)\n"; 

    cout << "1. " << "Single bed room (" << PRICESINGLE << "€)" << endl;
    cout << "2. " << "Double bed room (" << PRICETWIN << "€)" << endl;

    // Choose 1 or 2.
    menuNumber = MenuChoice();

    if (menuNumber == "1" || menuNumber == "1.")
    {
        price = PRICESINGLE;
        roomNumber = ChooseRoom(RoomsID, generator, menuNumber, 1, numberOfRooms/2);
    }

    if (menuNumber == "2" || menuNumber == "2.")
    {
        price = PRICETWIN;
        roomNumber = ChooseRoom(RoomsID, generator, menuNumber, (numberOfRooms/2)+1, numberOfRooms);
    }

    cin.ignore();
    cout << "\nPlease provide a name for this reservation (case sensitive).\n";

    do // Full name.
    {
        error = false;
        cout << ">> ";
        getline(cin, fullName);

        if (!IsValidFullname(fullName))
        {
            error = true;
            cout << "\nThe name is invalid, please re-enter.\n";
        }
    } while (error);

    cout << "\nHow many nights do you wish to stay. Maximum is " << MAX_NIGHTS << " nights per reservation." << endl;

    do  // Number of nights.
    {
        error = false;
        cout << ">> ";
        cin >> nights;

        if ((cin.peek() != '\n') || cin.fail() || nights < 1 || nights > MAX_NIGHTS)
        {
            cout << "\nPlease enter a valid number of nights\n";
            error = true;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    } while (error);
    
    // Generate unique reservation ID for the reservation.
    uniform_int_distribution<int> ID(10000, 99999);
    do
    {
        error = false;
        reservationID = ID(generator);
        for (int i=0; i<numberOfRooms; ++i)
            if (RoomsID[i] == reservationID)
                error = true;
    } while (error);

    // Randomly choose the discount factor.
    uniform_int_distribution<int> DiscountGen(1,3);
    discount = DiscountGen(generator);
    if (discount == 1)
        discount = 1.0;
    else if (discount == 2)
        discount = 0.9;
    else if (discount == 3)
        discount = 0.8;
    
    cout << endl << "┌──────────────────────────┐\n"
                    "│Summary of the reservation│\n"
                    "└──────────────────────────┘\n";
    cout << "Name: "  << fullName << endl;
    cout << "Reservation ID: " << reservationID << endl;
    cout << "Room number: " << roomNumber << endl;
    cout << "Number of nights: " << nights << endl;
    cout << "Total price: " << price*nights*discount << "€ (" << 100.0-discount*100 << "% discount)" << endl;
    cout << "\nDo you wish to continue? (y/n)\n";

    do  // Finalize reservation.
    {
        error = false;
        cout << ">> ";
        cin >> menuNumber;
        if (menuNumber !="y" && menuNumber !="Y" && menuNumber !="n" && menuNumber !="N")
        {
            cout << "\nInvalid input, please re-enter.\n";
            error = true;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    } while (error);

    if (menuNumber == "y" || menuNumber == "Y")
    {
        RoomsID[roomNumber-1] = reservationID;
        RoomsNumber[roomNumber-1] = roomNumber;
        RoomsName[roomNumber-1] = fullName;
        cout << endl << "Reservation successfull." << endl << endl;
    }
}



void SearchReserved(int RoomsID[], int RoomsNumber[], string RoomsName[], string menuNumber, int numberOfRooms)
{
    string search; // Search prompt.
    int roomNumber; // Write down the room number if one is found
    int nameCount, nameCountPrevious; // Keep track of name conflicts.
    bool error; // Menu error check.
    bool resultFoundIsID, resultFound; // Results found from search.

    while (true)
    {
        do
        {   
            resultFoundIsID = false;
            resultFound = false;
            nameCount = 0;
            nameCountPrevious = 1;
            roomNumber = 0;

            cout << endl << "Please enter a reservation ID, room number or a name.\n"
                            "Optionally enter q to go back to the menu." << endl;

            cout << ">> ";
            cin >> search;

            if (search == "q" || search == "Q")
            {
                cout << endl;
                return;
            }

            for (int i=0; i<numberOfRooms; ++i)
            {
                // Search is a room ID.
                if (search == to_string(RoomsID[i]))
                {
                    resultFoundIsID = true;
                    resultFound = true;
                    roomNumber = i;
                }

                // Search is a room number.
                else if (search == to_string(RoomsNumber[i]))
                {
                    resultFound = true;
                    roomNumber = i;
                }

                // Search is a name.
                else if (search == RoomsName[i])
                {
                    resultFound = true;
                    ++nameCount;

                    if (nameCount == 2)
                    {
                        cout << endl << "There seems to be multiple reservations under the name " << search << endl;
                        cout << "Reserver: " << RoomsName[roomNumber] << endl;
                        cout << "Room number: " << RoomsNumber[roomNumber] << endl;
                        cout << "Reservation ID: " << RoomsID[roomNumber] << endl;
                        resultFound = false;
                    }

                    if (nameCount > nameCountPrevious)
                    {
                        cout << "Reserver: " << RoomsName[i] << endl;
                        cout << "Room number: " << RoomsNumber[i] << endl;
                        cout << "Reservation ID: " << RoomsID[i] << endl;
                    }

                    nameCountPrevious = nameCount;
                    roomNumber = i;
                }
            }
            
            if (resultFound && nameCount == 1)
            {
                cout << endl << "Reservation found under the name " << search << endl;
                cout << "The room number is " << RoomsNumber[roomNumber] << endl;
                cout << "The reservation ID is " << RoomsID[roomNumber] << endl;
            }

            else if (resultFound && resultFoundIsID)
            {
                cout << endl << "Reaservaiton found under the ID " << search << endl;
                cout << "The room number is " << RoomsNumber[roomNumber] << endl;
                cout << "The name of the reserver is " << RoomsName[roomNumber] << endl;
            }

            else if (resultFound && !resultFoundIsID)
            {
                cout << endl << "Reservation found under the room number " << search << endl;
                cout << "The reservation ID is " << RoomsID[roomNumber] << endl;
                cout << "The name of the reserver is " << RoomsName[roomNumber] << endl;
            }

            else if (!resultFound && nameCount == 0)
                cout << endl << "No results found with the provided query.";
                
        } while (!resultFound);

        cout << "\nDo you wish to remove this reservation? (y/n)\n";
        do  // Remove reservation.
        {
            error = false;
            cout << ">> ";
            cin >> menuNumber;
            if (menuNumber !="y" && menuNumber !="Y" && menuNumber !="n" && menuNumber !="N")
            {
                cout << "\nInvalid input, please re-enter.\n";
                error = true;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        } while (error);

        if (menuNumber == "y" || menuNumber == "Y")
        {
            RoomsID[roomNumber] = 0;
            RoomsNumber[roomNumber] = 0;
            RoomsName[roomNumber] = "N/A";
            cout << endl << "Reservation removed succesfully.";
        }
    }
}



bool ReservationStatus(int RoomsID[], int roomNumber)
{
    /* If the RoomID index corresponding to the given room
    is non-zero, it isn't available. */
    if (RoomsID[roomNumber-1] == 0)
        return false;
    else
        return true;
}



bool fileExists(const string &filename)
{
    ifstream file(filename);
    if (file.is_open() && file.peek() != EOF)
    {
        file.close();
        return true;
    }
    else
    {
        file.close();
        return false;
    }
    
}



string MenuChoice()
{
    string n; // Menu input.
    bool error; // Invalid input state.

    do  // Room type.
    {
        error = false;
        cout << ">> ";
        cin >> n;
        if (cin.fail() || (n!="1" && n!="2" && n!="1." && n!="2."))
        {
            cout << "\nPlease enter a number corresponding to a menu item.\n" ;
            error = true;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    } while (error);

    return n;
}



int ChooseRoom(int RoomsID[], mt19937 &generator, string menuNumber, int roomMin, int roomMax)
{
    int roomNumber; // Room number to be returned.
    bool error; // Invalid input state.

    cout << endl << "Do you want to manually or automatically choose an available room?";
    cout << endl << "1. Choose manually";
    cout << endl << "2. Choose automatically\n";

    // Choose 1 or 2.
    menuNumber = MenuChoice();

    // Choose room number manually.
    if (menuNumber == "1" || menuNumber == "1.")
    {
        cout << endl << "Plase provide the room number (" << roomMin << "-" << roomMax << ")\n";
        do
        {
            error = false;
            cout << ">> ";
            cin >> roomNumber;

            if ((cin.peek() != '\n') || cin.fail() || roomNumber < roomMin || roomNumber > roomMax)
            {
                cout << "\nPlease enter a valid room number.\n";
                error = true;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }

            // If the input is valid but the room isn't available.
            if (ReservationStatus(RoomsID, roomNumber) && !error)
            {
                cout << "\nThis room is not available, please choose another room.\n";
                error = true;
            }
        } while (error);
    }

    // Choose room number automatically.
    if (menuNumber == "2" || menuNumber == "2.")
    {
        uniform_int_distribution<int> RoomNumberGenerator(roomMin, roomMax);
        do
        {
            error = false;
            roomNumber = RoomNumberGenerator(generator);
            for (int i=0; i<roomMax; ++i)
                if (ReservationStatus(RoomsID, roomNumber))
                    error = true;
        } while (error); 
    }

    return roomNumber;
}



bool IsValidFullname(string &fullName)
{
    // Make sure there aren't multiple hyphen in a row.
    bool previousCharWasHyphen = false;
    // Make sure there arent't multiple spaces in a row.
    bool previousCharWasSpace = false;
    // If first letter make sure it's uppercase.
    bool isFirstLetter = true;

    // Remove leading and trailing spaces and tab.
    size_t start = fullName.find_first_not_of(" \t");
    size_t end = fullName.find_last_not_of(" \t");

    if (start == string::npos || end == string::npos)
        // If the whole string is spaces or tabs.
        return false;
    else
        // Create new string staring with first character.
        fullName = fullName.substr(start, end - start +1); // maybe also fullName.lenght();

    // Iterate through all characters of the string.
    for (size_t i=0; i < fullName.length(); ++i)
    {
        char ch = fullName[i];

        // Check if the character is a hyphen.
        if (ch == '-')
        {
            if (i == 0 || i == fullName.length() - 1 || previousCharWasHyphen || previousCharWasSpace)
                return false;

            previousCharWasHyphen = true;
            previousCharWasSpace = false;
            isFirstLetter = true;
        }

        // Check if the charcter is a space.
        else if (ch == ' ')
        {
            if (i == 0 || i == fullName.length() - 1 || previousCharWasHyphen || previousCharWasSpace)
                return false;

            previousCharWasHyphen = false;
            previousCharWasSpace = true;
            isFirstLetter = true;
        }
        
        // Check if the character is a letter and check capitalization.
        else if (isalpha(ch))
        {
            if (isFirstLetter && !isupper(ch))
                return false;
            
            if(!isFirstLetter && !islower(ch))
                return false;

            previousCharWasHyphen = false;
            previousCharWasSpace = false;
            isFirstLetter = false;
        }

        else
            return false;
    }

    return true;
}