#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

class Words {
public:
    string word;
    int id;
};

vector<Words> wordTable;
void importTable() {
    ifstream wordTableFile("wordTable.csv");
    if (!wordTableFile.is_open()) {
        cout << "Error opening wordTable.csv" << endl;
        exit(114);
    }
    string line;
    while (getline(wordTableFile, line)) {
        if (line.empty() || line[0] == '#') continue; // Skip empty lines and comments
        string word;
        int id;
        stringstream ss(line);
        getline(ss, word, ',');
        ss >> id;
        Words temp;
        temp.word = word;
        temp.id = id;
        wordTable.push_back(temp);
    }
    cout << "Table imported successfully." << endl;
    wordTableFile.close();
}

void importCode(string &input) {
    ifstream codeFile("code.txt");
    if (!codeFile.is_open()) {
        cout << "Error opening code.txt" << endl;
        exit(114);
    }
    stringstream readFile;
    readFile << codeFile.rdbuf();
    input = readFile.str();
}

int main() {
    // import table from csv file
    cout << "Importing table from file." << endl;
    importTable();

    // for testing
    // cout << "Waiting for input." << endl;
    string input = "";
    // getline(cin, input);
    importCode(input);

    // variable init
    string buffer = "";
    char idMarker = 0;  // id marker would set to 1 if the next word is an id
    char stringMarker = 0;  // string marker would set to 1 if the next word is a string
    char digitMarker = 0;   // digit marker would set to 1 if the next word is a digit
    int idCounter = 101;    // "end of the id list"

    while (input.length() > 0) {
        // get rid of invalid characters
        while (input[0] == ' ' || input[0] == '\t' || input[0] == '\n') {
            input.erase(0, 1);
        }
        if (input[0] == '/') // for /, read one more character to specify
        {
            // if //, read until newline
            if (input[1] == '/') {
                while (input[0] != '\n') {
                    input.erase(0, 1);
                }
                continue;
            }
                // if /*, read until */
            else if (input[1] == '*') {
                while (input[0] != '*' || input[1] != '/') {
                    input.erase(0, 1);
                }
                input.erase(0, 2);
                continue;
            }
        }

        // version 0
        // buffer += input[0]; // buffer usually read one character at a time
        // input.erase(0, 1);
        // principle of buffer handling: when you read, you erase the input[0]
        // version 1
        // read 1 character at a time is not a good idea
        // read until space, tab, newline, or special characters
        // if a special character, read it

        // special character reader
        int specialCharacterInBuffer = 0;
        if (input[0] == '(' || input[0] == '[' || input[0] == '{' || input[0] == ')' || input[0] == ']' || input[0] == '}') {
            buffer.push_back(input[0]);
            input.erase(0, 1);
            specialCharacterInBuffer = 1;
        }
        // for + - * /, < > = !, check the next character to specify
        else if (input[0] == '+' || input[0] == '-' || input[0] == '*' || input[0] == '/') {
            buffer.push_back(input[0]);
            input.erase(0, 1);
            specialCharacterInBuffer = 1;
            if(input[0] == '=') {
                buffer.push_back(input[0]);
                buffer.push_back(input[1]);
                input.erase(0, 1);
            }
        }
        else if (input[0] == '<' || input[0] == '>' || input[0] == '=' || input[0] == '!') {
            buffer.push_back(input[0]);
            input.erase(0, 1);
            specialCharacterInBuffer = 1;
            if(input[0] == '=') {
                buffer.push_back(input[0]);
                buffer.push_back(input[1]);
                input.erase(0, 1);
            }
        }
        else if (input[0] == '.' || input[0] == ',' || input[0] == ';' || input[0] == ':') {
            buffer.push_back(input[0]);
            input.erase(0, 1);
            specialCharacterInBuffer = 1;
        }
        else if (input[0] == '\"') {
            buffer.push_back(input[0]);
            input.erase(0, 1);
            stringMarker = 1;
            specialCharacterInBuffer = 1;
        }
        // normal character reader
        while (!specialCharacterInBuffer) {
            // empty check should be in the while loop
            // if we check like 'while(empty_check)', the empty state would not be updated and the loop would not break
            if (input.empty()) break;
            if (input[0] == ' ' || input[0] == '\t' || input[0] == '\n') break;
            if (input[0] == '(' || input[0] == '[' || input[0] == '{' || input[0] == ')' || input[0] == ']' || input[0] == '}') break;
            // for + - * /, < > = !, check the next character to specify
            if (input[0] == '+' || input[0] == '-' || input[0] == '*' || input[0] == '/') break;
            if (input[0] == '<' || input[0] == '>' || input[0] == '=' || input[0] == '!') break;
            if (input[0] == '.' || input[0] == ',' || input[0] == ';' || input[0] == ':') break;
            // if detected number, read all following digits
            if (isdigit(input[0])){
                while (isdigit(input[0])) {
                    buffer.push_back(input[0]);
                    input.erase(0, 1);
                    digitMarker = 1;
                }
                break;
            }
            buffer.push_back(input[0]);
            input.erase(0, 1);
        }


        // if a word showed the first time after int, char, void, it is an id, even it contains numbers or keywords
        // if detected new id, idMarker would set to 1, read until = or ; or (
        if (idMarker == 1)
        {
            while (1) {
                while (input[0] == ' ' || input[0] == '\t') // remove spaces
                {
                    input.erase(0, 1);
                }
                if (input[0] == '=' || input[0] == ';' || input[0] == '(') break;  // detected end of id
                buffer += input[0];
                input.erase(0, 1);
            }
            // do not consider main as a new id
            if (buffer != "main") {
                // save id to list
                Words newID;
                newID.word = buffer;
                newID.id = idCounter;
                wordTable.push_back(newID);
                cout << "Added " << buffer << " / " << idCounter << " to the table." << endl;
                idCounter++;
            }
            // reset marker
            idMarker = 0;
        }
        // if detected string, stringMarker would set to 1, read until ", print it and continue
        if (stringMarker == 1)
        {
            // print the first "
            cout << "[\", 11]" << endl;
            // clear buffer for string
            buffer = "";
            while (input[0] != '\"') {
                buffer += input[0];
                input.erase(0, 1);
            }
            cout << "[<string> " << buffer << ", 50]" << endl;
            buffer = "";
            // remove the " at the end of string and print code
            input.erase(0, 1);
            cout << "[\", 11]" << endl;
            // reset marker
            stringMarker = 0;
            continue;
        }
        // if detected digits, digitMarker would set to 1, print it and continue
        if (digitMarker == 1)
        {
            cout << "[<digit> " << buffer << ", 51]" << endl;
            buffer = "";
            // reset marker
            digitMarker = 0;
            continue;
        }


        // buffer handling part below
        // scan the table to check if the buffer is a keyword
        for (int timer = 0; timer < wordTable.size(); timer++) {
            // id handler
            if (buffer == "int" || buffer == "char" || buffer == "void") idMarker = 1;
            // string handler
            if (buffer == "\"") stringMarker = 1;
            // default keywords
            if (buffer == wordTable[timer].word) // print the keyword and its id
            {
                cout << "[" << wordTable[timer].word << ", " << wordTable[timer].id << "]" << endl;
                buffer = "";    // clear buffer for next keyword
                break;
            }
        }
        // if not found, read until space, tab, or newline (continue)
        // still not found, print error message and exit the program
        if (buffer != "") {
            cout << "Error: " << buffer << " is not a valid keyword." << endl;
            exit(114);
        }

    }
}
