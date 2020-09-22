//
//  Project 1 Starter Code - GerryMandering
//  Author: Shawn Klein
//  NetID:  SKlein8
//  Course: CS251[42499] Fall2020
//  Description: Application reads and parses accompanying
//      files to give Gerrymandering information on
//      particular voting districts.
//  Creative Component: The "voted" command will iterate
//      through ourvector and print out the percentage of
//      eligible voters who actually voted.
//
//

#include <string.h>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <cctype>
#include <regex>
#include <fstream>
#include "ourvector.h"

using namespace std;

struct StateVoterData {
    string state;
    int districts;
    int voters;
    int wastedDems;
    int wastedReps;
    int totalDems;
    int totalReps;
    string gerrymanAgainst;
    string districtsData;

    // method plots 100 D's or R's with 
    // respect to its district's votes
    void plot() {
        size_t foundIndx;
        string distData = districtsData;

        while (!distData.empty()) {
            // removes first ","
            distData.erase(0, 1);

            // removes dist number
            foundIndx = distData.find(',');
            string tempNum = distData.substr(0, foundIndx);
            int distNum;
            if (tempNum == "AL") {
                distNum = 1;
            } else {
                distNum = stoi(tempNum);
            }
            distData.erase(0, foundIndx + 1);

            foundIndx = distData.find(',');
            int demVotes = stoi(distData.substr(0, foundIndx));
            distData.erase(0, foundIndx + 1);

            foundIndx = distData.find(',');
            int repVotes = stoi(distData.substr(0, foundIndx));
            distData.erase(0, foundIndx);

            cout << "District: " << distNum << endl;

            if (demVotes + repVotes == 0) {
                cout << endl;
            } else {
                int numOfD = (double)(demVotes)/(demVotes+repVotes) * 100;
                int numOfR = 100 - numOfD;

                cout << string(numOfD, 'D') << string(numOfR, 'R') << endl;
            }
        }
    }

    // returns the percentage of a state's wasted votes
    double calcGerrymandering() {
        double effGap = 0.0;
        generateValues();
        // cout << "able to generateValues" << endl;
        int totalVotes = totalDems + totalReps;

        if (wastedDems > wastedReps) {
            gerrymanAgainst = "Democrats";
            effGap = ((double)(wastedDems - wastedReps) / totalVotes) * 100;
        } else if (wastedDems < wastedReps) {
            gerrymanAgainst = "Republicans";
            effGap = ((double)(wastedReps - wastedDems) / totalVotes) * 100;
        } else {
            effGap = 0.0;
        }
        return effGap;
    }

    // prints out state name and the percentage
    // of eligible voters who voted
    void voterPct() {
        generateValues();
        double voterPct = (double)(totalDems + totalReps)/voters;
        cout << state << ":" << string(20 - state.size(), '.')
             << setprecision(3) << voterPct * 100 << "%" << endl;
    }

 private:
    // Function parses districtsData string and
    // populates wasted and total vote values
    void generateValues() {
        size_t foundIndx;
        string distData = districtsData;
        wastedDems = 0;
        wastedReps = 0;
        totalDems = 0;
        totalReps = 0;

        while (!distData.empty()) {
            distData.erase(0, 1);

            foundIndx = distData.find(',');
            distData.erase(0, foundIndx + 1);

            foundIndx = distData.find(',');
            int demVotes = stoi(distData.substr(0, foundIndx));
            distData.erase(0, foundIndx + 1);

            foundIndx = distData.find(',');
            int repVotes = stoi(distData.substr(0, foundIndx));
            distData.erase(0, foundIndx);

            totalDems += demVotes;
            totalReps += repVotes;

            if (repVotes > demVotes) {
                int totalVotes = demVotes + repVotes;

                wastedDems += demVotes;
                wastedReps += repVotes - (floor( (double)(totalVotes)/2 ) + 1);
            } else if (repVotes < demVotes) {
                int totalVotes = demVotes + repVotes;

                wastedReps += repVotes;
                wastedDems += demVotes - (floor( (double)(totalVotes)/2 ) + 1);
            }
        }
    }
};

// Display methods
void welcomeMessage();
void userPrompt(string isLoaded, string state);
void borderSeparator();
void printDistLoad(StateVoterData &data);
void printVoteLoad(StateVoterData &data);
void printWastedPartyVotes(int wastedDeVotes, int wastedReVotes);
void noStateError();
void noDataError();
void previousDataLoadedError();
void stateExistError();
void printInvalidFile(int fileNum);

// loading file methods
void formatInput(string &input);
void loadDistFile(string fileName, ourvector<StateVoterData> &voterStates);
void loadVoteFile(string fileName, ourvector<StateVoterData> &voterStates);

// data extraction methods
string getStateName(string &line);
int getVoterNum(string &line);
int getTotalDistricts(string &line);
string getDistrictsData(string &line);

// user command execution methods
void loadFiles(string &isLoaded, ourvector<StateVoterData> &statesData);
void runStats(string &isLoaded, StateVoterData *loadedState, string &loadedStateName);
void runSearch(string &isLoaded, ourvector<StateVoterData> &statesData, StateVoterData **loadedState, string &loadedStateName);
void runPlot(string &isLoaded, StateVoterData *loadedState, string &loadedStateName);
void runVoted(string &isLoaded, ourvector<StateVoterData> &statesData);



// Entry point of program
int main() {
    bool isRunning = true;
    ourvector<StateVoterData> statesData;
    string isLoaded = "No";
    StateVoterData* loadedState;
    string loadedStateName = "N/A";
    string userInput;
    
    welcomeMessage();

    while (isRunning) {
        userPrompt(isLoaded, loadedStateName);
        cin >> userInput;

        borderSeparator();

        if (userInput == "exit") {
            isRunning = false;
        } else if (userInput == "load") {
            loadFiles(isLoaded, statesData);
        } else if (userInput == "search") {
            runSearch(isLoaded, statesData, &loadedState, loadedStateName);
        } else if (userInput == "stats") {
            runStats(isLoaded, loadedState, loadedStateName);

        } else if (userInput == "plot") {
            runPlot(isLoaded, loadedState, loadedStateName);

        } else if (userInput == "voted") {
            runVoted(isLoaded, statesData);
        }
    }

    return 0;
}

// searches for a state upon user request
void runSearch(string &isLoaded, ourvector<StateVoterData> &statesData, StateVoterData **loadedState, string &loadedStateName) {
    if (isLoaded == "No") {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        noDataError();
    } else {
        bool notFound = true;
        string searchState;
        getline(cin, searchState);

        formatInput(searchState);

        for (StateVoterData &e : statesData) {
            if (e.state == searchState) {
                notFound = false;
                loadedStateName = e.state;
                *loadedState = &e;
                break;
            }
        }

        if (notFound) {
            stateExistError();
        }
    }
}

// displays stats for a state upon user request
void runStats(string &isLoaded, StateVoterData *loadedState, string &loadedStateName) {
    double gerrymanPctThres = 7.0;
    int gerrymanDistLimit = 3;
    
    if (isLoaded == "No" ) {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        noDataError();
    } else if (loadedStateName == "N/A") {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        noStateError();
    } else {
        double gmPct = (*loadedState).calcGerrymandering();

        if ( (*loadedState).districts >= gerrymanDistLimit && gmPct > gerrymanPctThres ) {
            cout << "Gerrymandered: Yes" << endl;
            cout << "Gerrymandered against: " << (*loadedState).gerrymanAgainst << endl;
            cout << "Efficiency Factor: " << gmPct << "%" << endl;

        } else {
            cout << "Gerrymandered: No" << endl;
        }

        printWastedPartyVotes((*loadedState).wastedDems, (*loadedState).wastedReps);

        cout << "Eligible voters: " << (*loadedState).voters << endl;
        cout << endl;
    }
}

// displays district plot for a state upon user request
void runPlot(string &isLoaded, StateVoterData *loadedState, string &loadedStateName) {
    if (isLoaded == "No") {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        noDataError();
    } else if (loadedStateName == "N/A") {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        noStateError();
    } else {
        (*loadedState).plot();
    }
}

// displays voter percentage for all states upon user request
void runVoted(string &isLoaded, ourvector<StateVoterData> &statesData) {
    if (isLoaded == "No") {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        noDataError();
    } else {
        cout << "Percentage of eligible voters who voted:" << endl;
        for (StateVoterData &e : statesData) {
            e.voterPct();
        }
        cout << endl;
    }
}

// takes a string and formats it to a proper filename
void formatInput(string &input) {
    transform(input.begin(), input.end(), input.begin(), ::tolower);
    input.replace(0, 1, "");
    input[0] = toupper(input[0]);

    int spaceIndx = input.find(' ');
    if (spaceIndx > 0) {
        input[spaceIndx + 1] = toupper(input[spaceIndx + 1]);
    }

    spaceIndx = input.rfind(' ');
    if (spaceIndx > 0) {
        input[spaceIndx + 1] = toupper(input[spaceIndx + 1]);
    }
}

// take fileName and loads the Districts file where
// StateVoterData stucts are created and populate ourvector
void loadDistFile(string fileName, ourvector<StateVoterData> &voterStates) {
    ifstream infile(fileName);
    string line;

    while ( !infile.eof() ) {
        getline(infile, line, '\n');
        if (line != "") {
            StateVoterData data;

            data.state = getStateName(line);
            data.districtsData = getDistrictsData(line);
            data.districts = getTotalDistricts(line);

            printDistLoad(data);

            voterStates.push_back(data);
        }
    }
}

// takes string fileName and loads the Vote file to
// the existing data set
void loadVoteFile(string fileName, ourvector<StateVoterData> &voterStates) {
    ifstream infile(fileName);
    string line;

    while ( !infile.eof() ) {
        getline(infile, line, '\n');

        if (line != "") {
            StateVoterData data;
            string voterState = getStateName(line);

            for (StateVoterData &e : voterStates) {
                if (e.state == voterState) {
                    e.voters = getVoterNum(line);
                    printVoteLoad(e);
                    break;
                }
            }
        }
    }
}

// Function loads file from input buffer. It takes and sets a string
// named isLoaded if data is loaded. It also takes an ourvector and 
// attempts to populate it with the data loaded from files.
void loadFiles(string &isLoaded, ourvector<StateVoterData> &statesData) {
    if (isLoaded == "No") {
        regex distExpr("(districts)(.*)");
        regex voterExpr("(voters)(.*)");
        string filesToLoad;
        int fileNum = 0;

        getline(cin, filesToLoad, '\n');
        filesToLoad.replace(0, 1, "");
        stringstream ss;
        string loadFile;

        ss << filesToLoad;

        while (!ss.eof()) {
            fileNum++;
            string loadFile;
            ss >> loadFile;

            if (regex_search(loadFile, distExpr)) {
                cout << "Reading: " << loadFile << endl;
                loadDistFile(loadFile, statesData);

                cout << endl;
            } else if (regex_search(loadFile, voterExpr)) {
                cout << "Reading: " << loadFile << endl;
                loadVoteFile(loadFile, statesData);

                cout << endl;
            } else {
                isLoaded = "No";
                statesData.clear();

                printInvalidFile(fileNum);
                break;
            }
        }

        if (statesData.size() > 0) {
            isLoaded = "Yes";
        }
    } else {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        previousDataLoadedError();
    }
}

// prints invalid file by file order
void printInvalidFile(int fileNum) {
    if (fileNum == 1) {
        cout << "Invalid first file, try again.\n" << endl;
    } else if (fileNum == 2) {
        cout << "Invalid second file, try again.\n" << endl;
    }
}


// Fuction takes a string, and uses regex to return the name
// of the state. 
string getStateName(string &line) {
    smatch match;
    regex re("^[a-zA-Z ]+");

    regex_search(line, match, re);

    return  match.str(0);
}

// Function take a string and returns the integer 
// of that string. Used in eligible voters parsing.
int getVoterNum(string &line) {
    smatch match;
    regex re("[0-9]+");

    regex_search(line, match, re);
    string toConvert = match.str(0);

    return  stoi(toConvert);
}

// Function takes string, and uses regex to return the
// position of where district number is located.
int getTotalDistricts(string &line) {
    smatch match;
    regex re(",([0-9]+),[0-9]+,[0-9]+$");

    regex_search(line, match, re);
    string toConvert = match.str(1);
    if (toConvert == "") {
        return 1;
    } else {
        return  stoi(toConvert);
    }
}

// Takes a string and returns everything after a ,
string getDistrictsData(string &line) {
    int firstDemim = line.find(",");

    return line.substr(firstDemim);
}

// prints welcome message upon start of app
void welcomeMessage() {
    cout << "Welcome to the Gerrymandering App!\n" << endl;
}

// prints user prompt with basic info and requesting command
void userPrompt(string isLoaded, string state) {
    cout << "Data loaded? " << isLoaded << endl;
    cout << "State: " << state << '\n' << endl;

    cout << "Enter command: ";
}

// prints border separator between sections of output
void borderSeparator() {
    cout << "\n-----------------------------\n" << endl;
}

// takes a StateVoter Struct and prints name and number
// of districts. used when loading file
void printDistLoad(StateVoterData &data) {
    cout << "..." << data.state << "..."
         << data.districts << " districts total" << endl;
}

// takes a StateVoter Struct and prints name and number
// of eligible voters. used when loading file
void printVoteLoad(StateVoterData &data) {
    cout << "..." << data.state << "..."
         << data.voters << " eligible voters" << endl;
}

// takes integers of wasted votes from both parties and prints
// it out
void printWastedPartyVotes(int wastedDeVotes, int wastedReVotes) {
    cout << "Wasted Democratic votes: " << wastedDeVotes << endl;
    cout << "Wasted Republican votes: " << wastedReVotes << endl;
}

// print message indicating no state was found
void noStateError() {
    cout << "No state indicated, please search for state first." << endl;
}

// print message indicating no data is loaded
void noDataError() {
    cout << "No data loaded, please load data first.\n" << endl;
}

// print message indicating data is already loaded
void previousDataLoadedError() {
    cout << "Already read data in, exit and start over.\n" << endl;
}

// print message indicating state not found
void stateExistError() {
    cout << "State does not exist, search again.\n" << endl;
}
