#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

using namespace std;

struct pair_hash {
    template <class T1, class T2>
    size_t operator()(const pair<T1, T2>& p) const {
        auto hash1 = hash<T1>{}(p.first);
        auto hash2 = hash<T2>{}(p.second);
        return hash1 ^ hash2;
    }
};

class grammar {
public:
    vector<pair<string, vector<string>>> rules = {};
    vector<pair<pair<string, string>, string>> predictTable = {};
    unordered_map<pair<string, string>, string, pair_hash> predictTable_temp = {};
    // put start symbol here
    string start_symbol = "E";
    // put all terminal and non-terminal characters here
    // vector<string> terminal_chars = {"i", "+", "*", "(", ")"};
    vector<string> terminal_chars = {"a", "b", "c", "d"};
    // vector<string> nonTerminal_chars = {"E", "E'", "T", "T'", "F"};
    vector<string> nonTerminal_chars = {"E", "A", "B", "C"};
    // put all rules here
    grammar() {
        /*
        rules.push_back({"E", {"TE'"}});
        rules.push_back({"E'", {"+TE'", "ε"}});
        rules.push_back({"T", {"FT'"}});
        rules.push_back({"T'", {"*FT'", "ε"}});
        rules.push_back({"F", {"(E)", "i"}});
         */
        rules.push_back({"E", {"BA"}});
        rules.push_back({"A", {"BE", "d"}});
        rules.push_back({"B", {"aA", "bE", "c"}});
        // importPredictTable();
        initPredictTable();
        printPredictTable();
    }

    // return the first character of a non-terminal
    vector<string> getFirstCharacter(string non_terminal) {
        vector<string> result = {};
        for (pair<string, vector<string>> rule: rules) {
            if (rule.first == non_terminal) {
                for (string terminal: rule.second) {
                    // check if the first character contains a ' like E'
                    if (terminal[1] == '\'') {
                        result.push_back(terminal.substr(0, 2));
                    } else {
                        result.push_back(terminal.substr(0, 1));
                    }
                }
            }
        }
        return result;
    }

    // calculate predict table via rules
    void initPredictTable() {
        // calculate first set
        vector<pair<string, vector<string>>> firstSet = {};
        for (string nonTerminal: nonTerminal_chars) {
            vector<string> this_firstSet = getFirstCharacter(nonTerminal);
            int this_firstSet_Done = 0;
            // if there are non-terminal in the first set, iteration until first set only contains terminal
            while (1) {
                // if all chars in first set is terminal, break
                if (this_firstSet_Done) {
                    firstSet.push_back({nonTerminal, this_firstSet});
                    break;
                }
                this_firstSet_Done = 1;
                for (string item: this_firstSet) {
                    // if the item is terminal, skip
                    if (find(terminal_chars.begin(), terminal_chars.end(), item) != terminal_chars.end() ||
                        item == "\xce" /*epsilon*/) {
                        continue;
                    } else {    // non-terminal found, set done to 0
                        this_firstSet_Done = 0;
                        // if the item is non-terminal, replace with its first 'terminals'
                        vector<string> firstChars = getFirstCharacter(item);
                        this_firstSet.insert(this_firstSet.end(), firstChars.begin(), firstChars.end());
                        this_firstSet.erase(remove(this_firstSet.begin(), this_firstSet.end(), item),
                                            this_firstSet.end());
                    }
                }
            }
        }
        // end of calculate first set
        // epsilon would be represented as \xce
        // calculate follow set
        vector<pair<string, vector<string>>> lastFollowSet = {};
        vector<pair<string, vector<string>>> thisFollowSet = {};
        // for principle 2.2, pick out all terminal who can -> epsilon
        vector<string> nonTerminal_has_epsilon = {};
        for (pair<string, vector<string>> rule: rules) {
            for (string terminal: rule.second) {
                if (terminal == "ε") {
                    nonTerminal_has_epsilon.push_back(rule.first);
                }
            }
        }
        // principle3: if <start symbol>, follow(<start symbol>) += "$"
        thisFollowSet.push_back({start_symbol, {"$"}});
        while (1) {
            // scan through all rules
            for (pair<string, vector<string>> rule: rules) {
                // principle1: if <non-terminal> <any>, follow(<non-terminal>) += first(<any>)
                // principle2: if <non-t left> -> <non-t> <end> || <non-t> <any who can -> epsilon>, follow(<non-t>) += follow(<non-t left>)
                for (string terminal: rule.second) {
                    int timer = 0;
                    string char1 = "";
                    string char2 = "";

                    while (1) { // the timer loop
                        // check "timer" and "timer + 1"
                        // check if the characters contains a ' like E'
                        // furthest reach is timer + 3
                        // code like this is not good, but I don't have time to fix it
                        if (timer + 3 < terminal.size()) {
                            if (terminal[timer + 1] == '\'') {
                                if (terminal[timer + 3] == '\'') {  // A' B'
                                    char1 = terminal.substr(timer, 2);
                                    char2 = terminal.substr(timer + 2, 2);
                                    timer += 2;
                                } else {    // A' B
                                    char1 = terminal.substr(timer, 2);
                                    char2 = terminal.substr(timer + 2, 1);
                                    timer += 2;
                                }
                            } else {
                                if (terminal[timer + 2] == '\'') {  // A B'
                                    char1 = terminal.substr(timer, 1);
                                    char2 = terminal.substr(timer + 1, 2);
                                    timer += 1;
                                } else {    // A B
                                    char1 = terminal.substr(timer, 1);
                                    char2 = terminal.substr(timer + 1, 1);
                                    timer += 1;
                                }
                            }
                        } else if (timer + 2 < terminal.size()) {
                            if (terminal[timer + 1] == '\'') {  // A' B
                                char1 = terminal.substr(timer, 2);
                                char2 = terminal.substr(timer + 2, 1);
                                timer += 2;
                            } else if (terminal[timer + 2] == '\'') {    // A B'
                                char1 = terminal.substr(timer, 1);
                                char2 = terminal.substr(timer + 1, 2);
                                timer += 1;
                            } else {    // A B
                                char1 = terminal.substr(timer, 1);
                                char2 = terminal.substr(timer + 1, 1);
                                timer += 1;
                            }
                        } else {    // A B
                            // specially, epsilon hase the length of 2
                            if (terminal[timer] == '\xce' && terminal[timer + 1] == '\xb5') {
                                char1 = "NULL";
                                char2 = "ε";
                            } else {
                                char1 = terminal.substr(timer, 1);
                                char2 = terminal.substr(timer + 1, 1);
                                timer += 1;
                            }
                        }

                        // principle1: if <non-terminal> <any>, follow(<non-terminal>) += first(<any>) except epsilon
                        // apply principle1
                        if (find(nonTerminal_chars.begin(), nonTerminal_chars.end(), char1) !=
                            nonTerminal_chars.end()) {
                            // cout << char1 << char2 << ": " << "follow(" << char1 << ") += first(" << char2 << ")" << endl;
                            if (find(terminal_chars.begin(), terminal_chars.end(), char2) != terminal_chars.end()) {
                                // if <char2> is terminal, follow(nonT aka char1) += char2
                                int found = 0;
                                for (pair<string, vector<string>> &followSetItem: thisFollowSet) {
                                    if (followSetItem.first == char1) {
                                        // duplicate check
                                        if (find(followSetItem.second.begin(), followSetItem.second.end(), char2) ==
                                            followSetItem.second.end()) {
                                            followSetItem.second.push_back(char2);
                                        }
                                        found = 1;
                                    }
                                }
                                if (!found) {
                                    thisFollowSet.push_back({char1, {char2}});
                                }
                            } else {
                                // if <char2> is non-terminal, follow(nonT aka char1) += first(char2)
                                int found = 0;
                                for (auto &followSetItem: thisFollowSet) {
                                    if (followSetItem.first == char1) {
                                        for (auto &firstSetItem: firstSet) {
                                            if (firstSetItem.first == char2) {
                                                // get rid of epsilon
                                                // followSetItem.second.insert(followSetItem.second.end(),firstSetItem.second.begin(),firstSetItem.second.end());
                                                for (string &firstSetItem2: firstSetItem.second) {
                                                    // duplicate check
                                                    if (firstSetItem2 != "\xce" && find(followSetItem.second.begin(),
                                                                                        followSetItem.second.end(),
                                                                                        firstSetItem2) ==
                                                                                   followSetItem.second.end()) {
                                                        followSetItem.second.push_back(firstSetItem2);
                                                    }
                                                }
                                            }
                                        }
                                        found = 1;
                                    }
                                }
                                if (!found) {
                                    for (auto firstSetItem: firstSet) {
                                        if (firstSetItem.first == char2) {
                                            // thisFollowSet.push_back({char1, firstSetItem.second});
                                            // get rid of epsilon
                                            vector<string> temp = {};
                                            for (string firstSetItem2: firstSetItem.second) {
                                                // duplicate check
                                                if (firstSetItem2 != "\xce" &&
                                                    find(temp.begin(), temp.end(), firstSetItem2) == temp.end()) {
                                                    temp.push_back(firstSetItem2);
                                                }
                                            }
                                            thisFollowSet.push_back({char1, temp});
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                        // principle2: if <non-t left> -> <non-t> <end> || <non-t> <any who can -> epsilon>, follow(<non-t>) += follow(<non-t left>)
                        // do principle 2.2 first because timer 2.1 involves breaking the timer loop
                        // apply principle2.2
                        // if char 1 is non-terminal and char 2 is non-terminal who can -> epsilon
                        if (find(nonTerminal_chars.begin(), nonTerminal_chars.end(), char1) !=
                            nonTerminal_chars.end() &&
                            find(nonTerminal_has_epsilon.begin(), nonTerminal_has_epsilon.end(), char2) !=
                            nonTerminal_has_epsilon.end()) {
                            // follow (nonT aka char2) += follow(nonT left aka rule.first)
                            // cout << char1 << char2 << "(has ε): " << "follow(" << char1 << ") += follow(" << rule.first << ")" << endl;
                            int found = 0;
                            for (pair<string, vector<string>> &followSetItem: thisFollowSet) {
                                if (followSetItem.first == char1) {
                                    vector<string> nonT_left_follow = {};
                                    for (pair<string, vector<string>> followSetItem2: thisFollowSet) {
                                        if (followSetItem2.first == rule.first) {
                                            nonT_left_follow = followSetItem2.second;
                                            break;
                                        }
                                    }
                                    // duplicate check
                                    for (const auto &follow: nonT_left_follow) {
                                        if (find(followSetItem.second.begin(), followSetItem.second.end(), follow) ==
                                            followSetItem.second.end()) {
                                            followSetItem.second.push_back(follow);
                                        }
                                    }
                                    found = 1;
                                    break;
                                }
                            }
                            if (!found) {
                                vector<string> nonT_left_follow = {};
                                for (pair<string, vector<string>> followSetItem: lastFollowSet) {
                                    if (followSetItem.first == rule.first) {
                                        nonT_left_follow = followSetItem.second;
                                        break;
                                    }
                                }
                                thisFollowSet.push_back({char2, nonT_left_follow});
                            }
                        }
                        // this if is the out of bound check
                        // condition1(A B'): if production like A B', timer at B, timer + 2 is out of bound
                        // condition2(A B): if production like A B, timer at B, timer + 1 is out of bound
                        if ((*char2.rbegin() == '\'' && timer + 2 >= terminal.size()) ||
                            (*char2.rbegin() != '\'' && timer + 1 >= terminal.size()) || terminal == "ε") {
                            // char 2 aka last terminal is non-terminal?
                            // ok I understand this expression is not good... It should be "last char in production is non-terminal?"
                            // if terminal not terminal, apply principle2.1
                            if (find(nonTerminal_chars.begin(), nonTerminal_chars.end(), char2) !=
                                nonTerminal_chars.end()) {
                                // cout << char2 << " is the last character in production, follow(" << char2 << ") += follow(" << rule.first << ")" << endl;
                                if (find(nonTerminal_chars.begin(), nonTerminal_chars.end(), char2) !=
                                    nonTerminal_chars.end()) {
                                    // follow (nonT aka char2) += follow(nonT left aka rule.first)
                                    int found = 0;
                                    for (pair<string, vector<string>> &followSetItem: thisFollowSet) {
                                        if (followSetItem.first == char2) {
                                            vector<string> nonT_left_follow = {};
                                            for (pair<string, vector<string>> &followSetItemLast: lastFollowSet) {
                                                if (followSetItemLast.first == rule.first) {
                                                    nonT_left_follow = followSetItemLast.second;
                                                    break;
                                                }
                                            }
                                            // check duplicate and don't add them to the follow set
                                            for (const auto &follow: nonT_left_follow) {
                                                if (find(followSetItem.second.begin(), followSetItem.second.end(),
                                                         follow) == followSetItem.second.end()) {
                                                    followSetItem.second.push_back(follow);
                                                }
                                            }
                                            found = 1;
                                        }
                                    }
                                    if (!found) {
                                        vector<string> nonT_left_follow = {};
                                        for (pair<string, vector<string>> followSetItem: lastFollowSet) {
                                            if (followSetItem.first == rule.first) {
                                                nonT_left_follow = followSetItem.second;
                                                break;
                                            }
                                        }
                                        thisFollowSet.push_back({char2, nonT_left_follow});
                                    }


                                }
                            }
                            // break the timer loop and go to the next rule
                            break;
                        }
                    }

                }

            }
            // if follow set not changed, calculation is done
            if (thisFollowSet == lastFollowSet) {
                break;
            }
            lastFollowSet = thisFollowSet;
        }
        // end of calculate follow set
        // this is so hard!
        // calculate predict table
        // principle1: if A -> B and a belongs to first(A), add {{A, a}, B} to predict table
        // principle2: if A -> B and epsilon belongs to first(A), for all t, add {{A, t}, ε} to predict table
        // principle3: remove all duplicates epsilons
        cout << "Start creating Predict Table..." << endl;
        for (auto rule: rules) {
            string nonTerminal_left = rule.first;
            vector<string> production = rule.second;
            // scan through first set
            for (auto firstSetItem: firstSet) {
                if (firstSetItem.first == nonTerminal_left) {
                    for (auto firstSetItem_production: firstSetItem.second) {
                        if (firstSetItem_production != "\xce") {
                            // choose production containing the terminal in first set
                            int pushed = 0;
                            for (auto productionItem: production) {
                                if (productionItem[0] == firstSetItem_production[0]) {
                                    // get rid of duplicates, push when not found
                                    pair<pair<string, string>, string> temp = {{nonTerminal_left, firstSetItem_production}, productionItem};
                                    predictTable_temp.insert(temp);
                                    pushed = 1;
                                }
                            }
                            // if no production containing the terminal in first set, choose the first production
                            if (!pushed) {
                                // get rid of duplicates, push with not found
                                pair<pair<string, string>, string> temp = {{nonTerminal_left, firstSetItem_production}, production[0]};
                                predictTable_temp.insert(temp);
                            }
                        }
                    }
                }
                // if production contains epsilon
                if (find(production.begin(), production.end(), "ε") != production.end()) {
                    for (auto followSetItem: lastFollowSet) {
                        if (followSetItem.first == nonTerminal_left) {
                            for (auto followSetItem_production: followSetItem.second) {
                                predictTable_temp.insert({{nonTerminal_left, followSetItem_production}, "ε"});
                            }
                            break;
                        }
                    }
                }
            }
        }
        // convert unordered_map to vector
        for (auto predict: predictTable_temp) {
            predictTable.push_back(predict);
        }
        cout << "Predict Table created complete." << endl;
    }

    void importPredictTable() {
                predictTable.push_back({{"E", "i"}, "TE'"});
                predictTable.push_back({{"E", "("}, "TE'"});
                predictTable.push_back({{"E'", "+"}, "+TE'"});
                predictTable.push_back({{"E'", ")"}, "ε"});
                predictTable.push_back({{"E'", "$"}, "ε"});
                predictTable.push_back({{"T", "i"}, "FT'"});
                predictTable.push_back({{"T", "("}, "FT'"});
                predictTable.push_back({{"T'", "+"}, "ε"});
                predictTable.push_back({{"T'", "*"}, "*FT'"});
                predictTable.push_back({{"T'", ")"}, "ε"});
                predictTable.push_back({{"T'", "$"}, "ε"});
                predictTable.push_back({{"F", "i"}, "i"});
                predictTable.push_back({{"F", "("}, "(E)"});
            }

    void printPredictTable(){
        cout << "Predict Table: " << endl;
        // print column names
        cout << left << setw(10) << " ";
        for (auto terminal: terminal_chars) {
            cout << left << setw(10) << terminal;
        }
        cout << endl;
        // print each row
        for (auto nonTerminal: nonTerminal_chars) {
            cout << left << setw(10) << nonTerminal;
            for (auto terminal: terminal_chars) {
                string production = "";
                if (predictTable_temp.find({nonTerminal, terminal}) != predictTable_temp.end()) {
                    production = predictTable_temp[{nonTerminal, terminal}];
                }
                if (production == "ε") production = "epsilon"; // make it align
                cout << left << setw(10) << production;
            }
            cout << endl;
        }
    }

    string getPredict(string stackTop, string buffer) {
        for (pair<pair<string, string>, string> predict: predictTable) {
            if (predict.first.first == stackTop && predict.first.second == buffer) {
                return predict.second;
            }
        }
        cout << "No predict found for " << stackTop << " " << buffer << endl;
        return "";
    }

    string printCurrentRule(string nonTerminal) {
        string output = "";
        for (pair<string, vector<string>> rule: rules) {
            if (rule.first == nonTerminal) {
                // cout << rule.first << " -> ";
                output += rule.first + " -> ";
                for (string terminals: rule.second) {
                    // cout << terminal_chars << " | ";
                    output += terminals + " | ";
                }
                // remove the last " | "
                output.erase(output.size() - 3, 3);
            }
        }
        return output;
    }
};

int main() {
    grammar G;
    string remainingString = "";
    cout << "Enter the input string: ";
    cin >> remainingString;
    // for testing
    // remainingString = "i+i*i";

    // append $ to the end of the string to make it a "stack"
    remainingString.append("$");
    // get 1 char from remainingString
    string buffer;
    buffer = remainingString[0];
    remainingString.erase(0, 1);
    string analyzeStack = "$" + G.start_symbol;

    vector<string> analyzeStackHistory = {};
    vector<string> currentBufferHistory = {};
    vector<string> remainingStringHistory = {};
    vector<string> actionHistory = {};

    while (1) {
        // get 1 char from analyzeStack
        // **you get it, you pop it**
        string top = "";
        // check if top contains a ' like E'
        // if it does, append the last 2 chars
        if (analyzeStack[analyzeStack.size() - 1] == '\'') {
            top.append(analyzeStack, analyzeStack.size() - 2, 2);
            analyzeStack.erase(analyzeStack.size() - 2, 2);
        } else {
            top.append(analyzeStack, analyzeStack.size() - 1, 1);
            analyzeStack.erase(analyzeStack.size() - 1, 1);
        }

        // if top is the terminal in buffer
        if (top == buffer) {
            // cout << "Accepted terminal " << top << endl;
            // finish of pop the top
            // update history
            analyzeStackHistory.push_back(analyzeStack);
            currentBufferHistory.push_back(buffer);
            remainingStringHistory.push_back(remainingString);
            actionHistory.push_back("Accepted " + top);
            // get 1 char from remainingString
            buffer = remainingString[0];
            remainingString.erase(0, 1);
            if (remainingString.empty() && analyzeStack == "\0") {
                cout << "String Accepted." << endl;
                break;
            }
            continue;
        }

        bool matched = 0;
        // if top is a non-terminal
        for (pair<string, vector<string>> rule: G.rules) {
            // find the rule matches the top
            if (rule.first == top) {
                // cout << "Applying rule " << G.printCurrentRule(top) << endl;
                // update history
                analyzeStackHistory.push_back(analyzeStack);
                currentBufferHistory.push_back(buffer);
                remainingStringHistory.push_back(remainingString);
                actionHistory.push_back("Applying rule " + G.printCurrentRule(top));
                // use predict table to determine the rule to push
                string predict = G.getPredict(top, buffer);
                // push rules in reversed order
                // if the rule is epsilon, don't push anything
                if (predict == "ε") {
                    matched = 1;
                    break;
                } else {    // if the top is not epsilon, push the rule in reversed order
                    // check if top contains a ' like E'
                    for (int timer = predict.size() - 1; timer >= 0; timer--) {
                        if (predict[timer] == '\'') {
                            analyzeStack.append(predict + '\'', timer - 1, 2);
                            timer--;
                        } else {
                            analyzeStack.append(predict, timer, 1);
                        }
                    }
                }
                matched = 1;
                break;
            }
        }

        // if all rules are checked and no match is found, reject
        if (!matched) {
            // epsilon rule when it can't match any rule
            for (pair<string, vector<string>> rule: G.rules) {
                if (rule.first == top && rule.second[0] == "ε") {
                    // cout << "Applying rule " << G.printCurrentRule(top) << endl;
                    // update history
                    analyzeStackHistory.push_back(analyzeStack);
                    currentBufferHistory.push_back(buffer);
                    remainingStringHistory.push_back(remainingString);
                    actionHistory.push_back("Applying rule " + G.printCurrentRule(top));
                    break;
                }
            }
            cout << "String Rejected." << endl;
            break;
        }

    }
    // print history
    cout << left << setw(20) << "Analyze Stack" << left << setw(20) << "Current Buffer" << left << setw(20)
         << "Remaining String" << left << setw(30) << "Action" << endl;
    for (int i = 0; i < analyzeStackHistory.size(); i++) {
        cout << left << setw(20) << analyzeStackHistory[i] << left << setw(20) << currentBufferHistory[i]
             << right << setw(20) << remainingStringHistory[i] << "    " << left << setw(30) << actionHistory[i]
             << endl;
    }

}