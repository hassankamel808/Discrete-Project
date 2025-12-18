#include <iostream>
#include <stack>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <cctype>
using namespace std;

// Logical implication
bool implies(bool A, bool B) { return !A || B; }

// Token struct
struct Token {
    string value;
    bool isOperator;
};

// Operator precedence
int precedence(const string &op) {
    if (op == "~") return 4;
    if (op == "&") return 3;
    if (op == "|") return 2;
    if (op == "->") return 1;
    if (op == "<->") return 0;
    return -1;
}

// Tokenize expression
vector<Token> tokenize(const string &expr) {
    vector<Token> tokens;
    for (size_t i = 0; i < expr.size(); ++i) {
        if (isspace(expr[i])) continue;

        if (expr[i] == '-' && i + 1 < expr.size() && expr[i+1] == '>') {
            tokens.push_back({"->", true});
            i++;
        } else if (expr[i] == '<' && i + 2 < expr.size() && expr[i+1] == '-' && expr[i+2] == '>') {
            tokens.push_back({"<->", true});
            i += 2;
        } else if (expr[i] == '~' || expr[i] == '&' || expr[i] == '|') {
            tokens.push_back({string(1, expr[i]), true});
        } else if (isalpha(expr[i])) {
            tokens.push_back({string(1, expr[i]), false});
        } else if (expr[i] == '(' || expr[i] == ')') {
            tokens.push_back({string(1, expr[i]), false});
        } else {
            throw runtime_error(string("Unknown character: ") + expr[i]);
        }
    }
    return tokens;
}

// Infix to postfix
vector<Token> infixToPostfix(const string &expr) {
    vector<Token> tokens = tokenize(expr);
    vector<Token> output;
    stack<Token> ops;

    for (auto &t : tokens) {
        if (!t.isOperator) {
            if (t.value != "(" && t.value != ")")
                output.push_back(t);
            else if (t.value == "(")
                ops.push(t);
            else {
                while (!ops.empty() && ops.top().value != "(") {
                    output.push_back(ops.top());
                    ops.pop();
                }
                if (!ops.empty()) ops.pop();
            }
        } else {
            while (!ops.empty() && ops.top().isOperator &&
                   ((precedence(t.value) < precedence(ops.top().value)) ||
                   (precedence(t.value) == precedence(ops.top().value) && t.value != "~"))) {
                output.push_back(ops.top());
                ops.pop();
            }
            ops.push(t);
        }
    }
    while (!ops.empty()) {
        output.push_back(ops.top());
        ops.pop();
    }
    return output;
}

// Evaluate postfix expression
bool evaluatePostfix(const vector<Token> &postfix, map<char,bool> &vars) {
    stack<bool> st;
    for (auto &t : postfix) {
        if (!t.isOperator) {
            if (t.value != "(" && t.value != ")")
                st.push(vars[t.value[0]]);
        } else {
            if (t.value == "~") {
                bool val = st.top(); 
                st.pop();
                st.push(!val);
            } else {
                bool right = st.top(); st.pop();
                bool left = st.top(); st.pop();
                if (t.value == "&") st.push(left && right);
                else if (t.value == "|") st.push(left || right);
                else if (t.value == "->") st.push(implies(left,right));
                else if (t.value == "<->") st.push(left == right);
            }
        }
    }
    return st.top();
}

// Extract unique variables
set<char> extractVariables(const vector<string> &premises, const string &conclusion) {
    set<char> vars;
    for (auto &s : premises)
        for (char c : s)
            if (isalpha(c)) vars.insert(c);

    for (char c : conclusion)
        if (isalpha(c)) vars.insert(c);

    return vars;
}

int main() {
    cout << "\nLogical Expression Truth Table Analyzer\n\n";
    cout <<"Terms of Use:\n"
        << "1. Ensure logical expressions are correctly formatted.\n"
        << "2. The program supports variables (letters), logical operators (~, &, |, ->, <->), and parentheses.\n"
        << "\n";
    int n;
    cout << "Enter number of premises (max 2): ";
    cin >> n;
    cin.ignore();

    vector<string> premises(n);
    vector<vector<Token>> postfixPrem(n);

    for (int i = 0; i < n; i++) {
        cout << "Enter premise " << i+1 << ": ";
        getline(cin, premises[i]);
        postfixPrem[i] = infixToPostfix(premises[i]);
    }

    string conclusion;
    cout << "Enter conclusion: ";
    getline(cin, conclusion);
    vector<Token> postfixConcl = infixToPostfix(conclusion);

    set<char> varsSet = extractVariables(premises, conclusion);
    vector<char> vars(varsSet.begin(), varsSet.end());
    int varCount = vars.size();

    bool satisfiable = false;
    bool valid = true;

    cout << "\nTruth Table:\n";
    for (char v : vars) cout << v << " ";
    cout << "| ";
    for (int i = 0; i < n; i++) cout << "P" << i+1 << " ";
    cout << "| Conclusion\n";

    int totalRows = 1 << varCount;
    for (int mask = 0; mask < totalRows; mask++) {
        map<char,bool> varValues;
        for (int i = 0; i < varCount; i++)
            varValues[vars[i]] = (mask >> (varCount-1-i)) & 1;

        vector<bool> premValues(n);
        bool allPremisesTrue = true;

        for (int i = 0; i < n; i++) {
            premValues[i] = evaluatePostfix(postfixPrem[i], varValues);
            if (!premValues[i]) allPremisesTrue = false;
        }

        bool conclValue = evaluatePostfix(postfixConcl, varValues);

        for (char v : vars) cout << varValues[v] << " ";
        cout << "| ";
        for (bool val : premValues) cout << val << " ";
        cout << "| " << conclValue << endl;

        if (allPremisesTrue) satisfiable = true;
        if (allPremisesTrue && !conclValue) valid = false;
    }

    cout << "\nResult:\n";
    cout << (satisfiable ? "The set of premises is Satisfiable.\n"
                         : "The set of premises is NOT Satisfiable.\n");
    cout << (valid ? "The argument is Valid.\n"
                   : "The argument is Falsifiable.\n");

    return 0;
}
