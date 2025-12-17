#include <iostream>
#include <vector>
#include <cmath>
#include <stack>
using namespace std;

// object
struct Gate {
    char name;
    vector<int> values; 
};

// generate truth table
vector<string> truthTable(int bits) {
    int rows = pow(2, bits);     
    vector<string> combination(rows);
    for (int i = 0; i < rows; i++) {
        string s = "";
        for (int j = bits - 1; j >= 0; j--) {
            int bit = (i >> j) & 1; 
            s += char(bit + '0');  
        }
        combination[i] = s; 
    }
    return combination;
}

// get value of a gate for a row
int getValue(char ch, vector<Gate>& gates, int row) {
    for (auto &g : gates)
        if (g.name == ch) return g.values[row];
    return 0; 
}

// apply operation
void applyOp(stack<int>& s, char op) {
    if (op == '!' || op=='~') {
        int a = s.top(); s.pop();
        s.push(!a);
    } else {
        int b = s.top(); s.pop();
        int a = s.top(); s.pop();
        if (op == '&') s.push(a & b);
        else if (op == '|') s.push(a | b);
        else if (op == '^') s.push(a ^ b);
    }
}

// precedence
int precedence(char op) {
    if (op == '!' || op =='~') return 3;
    if (op == '&') return 2;
    if (op == '|' || op == '^') return 1;   
    return 0;
}

// evaluate expression
int evaluate(string exp, vector<Gate>& gates, int row) {
    stack<int> s;
    stack<char> ops;

    for (int i = 0; i < exp.length(); i++) {
        char ch = exp[i];
        if (ch == ' ') continue;

        if (ch == '(') {
            ops.push(ch);
        } 
        else if (ch == ')') {
            while (!ops.empty() && ops.top() != '(') {
                applyOp(s, ops.top());
                ops.pop();
            }
            ops.pop(); // remove '('
            // APPLY NOT if exists before '('
            if(!ops.empty() && (ops.top() == '!' || ops.top() == '~')) {
                applyOp(s, ops.top());
                ops.pop();
            }
        } 
        else if (isalpha(ch)) {
            s.push(getValue(ch, gates, row));
            // APPLY NOT immediately if exists
            if(!ops.empty() && (ops.top() == '!' || ops.top() == '~')) {
                applyOp(s, ops.top());
                ops.pop();
            }
        } 
        else { 
            while (!ops.empty() && precedence(ops.top()) >= precedence(ch)) {
                applyOp(s, ops.top());
                ops.pop();
            }
            ops.push(ch);
        }
    }

    while (!ops.empty()) {
        applyOp(s, ops.top());
        ops.pop();
    }

    return s.top();
}

// remove spaces
string removeSpace(string exp){
    string newExp="";
    for(char c : exp)
        if(c !=' ') newExp += c;
    return newExp;
}

// print truth table
void printRes(vector<vector<int>>& varsTable, string exp, vector<Gate>& gates, int n){
    for(int row = 0; row < varsTable.size(); row++){
        int result = evaluate(exp, gates, row);
        cout << "Row " << row << ": ";
        for(int k = 0; k < n; k++) cout << varsTable[row][k] << " ";
        cout << "-> Result: " << result << endl;
    }
}

// print satisfying assignments
void   satisfy(vector<vector<int>>& varsTable, string exp, vector<Gate>& gates, vector<char>& gateNames, int n){
    for(int row = 0 ; row < varsTable.size(); row++){
        int result = evaluate(exp, gates, row);
        if(result == 1){
            for(int i = 0; i < n; i++)
                cout << gateNames[i] << "=" << varsTable[row][i] << " ";
            cout << endl;
        }
    }
}

// detect tautology or unsatisfiable
string detectSOrT(vector<vector<int>>& varsTable, string exp, vector<Gate>& gates){
    bool allZero = true;
    bool allOne = true;
    for(int row = 0 ; row < varsTable.size(); row++){
        int result = evaluate(exp, gates, row);
        if(result == 1) allZero = false;
        if(result == 0) allOne = false;
    }
    if(allZero) {
        cout << "All result is 0: Unsatisfiable" << endl;
        return "allZero";
    }
    else if(allOne){ 
        cout << "All result is 1: Tautology" << endl;
        return "allOne";
    }
    else {
        cout << "Satisfiable (has both 0 and 1)" << endl;
        return "Satisfiable";
    }
}

// compare expressions
void compare(vector<vector<int>>& varsTable, string original, string simplified, vector<Gate>& gates , string detectRes){
    bool equivalent = true;
   
if(detectRes=="allZero" && simplified=="0") equivalent = true;
else if(detectRes=="allOne" && simplified=="1") equivalent = true;

    else{
    for(int row = 0; row < varsTable.size(); row++) {
        int resOriginal = evaluate(original, gates, row);
        int resSimplified = evaluate(simplified, gates, row);
        if(resOriginal ^ resSimplified){  
            equivalent = false;
            break;
        }
    }
    }

    if(equivalent) cout << "Equivalent expressions!" << endl;
    else cout << "Not equivalent!" << endl;
}

int main() {
    int n;
    cout << "Enter number of input: ";
    cin >> n;

    // store gateName ex:[a,b]
    vector<char> gateNames(n);
    for(int i = 0; i < n; i++) {
        cout << "Enter name of input " << i+1 << ": ";
        cin >> gateNames[i];
    }

    // convert from string to sream of bits
    vector<string> truthTableString = truthTable(n);
    vector<vector<int>> varsTable;
    for(string& row : truthTableString){
        vector<int> temp;
        for(char c : row) temp.push_back(c - '0');
        varsTable.push_back(temp);
    }

    // ex:[{a:0 0 1 1},{b: 0 1 0 1}]
    vector<Gate> gates(n);
    for(int i = 0; i < n; i++){
        gates[i].name = gateNames[i];
        for(int row = 0; row < varsTable.size(); row++)
            gates[i].values.push_back(varsTable[row][i]);
    }

    string detectRes ;
    cin.ignore();
    string exp;

    cout << "Please enter an expression that must contain (~, !, &, |, ^):" << endl;

    cout << "Enter the exp: ";
    getline(cin, exp);
    exp = removeSpace(exp);
    while(true){
  

    cout << "\nTruth table:\n";
    printRes(varsTable, exp, gates, n);

    cout << "\nSatisfying assignments:\n";
    satisfy(varsTable, exp, gates, gateNames, n);

    cout << "\nDetect Tautology or Unsatisfiable:\n";
     detectRes = detectSOrT(varsTable, exp, gates);

    if(detectRes=="allZero" || detectRes=="allOne"){
        if(detectRes=="allZero"){
            cout << "This circuit is Unsatisfiable => may be needed to change one gate\n";
        }else{
            cout << "This circuit is Tautology => may be needed to change one gate\n";
        }

        char choice;
        cout << "Do you want to change the expression and continue? (y/n): ";
        cin >> choice;
        cin.ignore(); // clear newline

        if(choice=='y' || choice=='Y'){
            cout << "Enter new expression: ";
            getline(cin, exp);
            exp = removeSpace(exp);
            continue; // loop again with new expression
        } else {
            break; // exit loop, continue program
        }
    } else {
        break; // expression satisfiable, continue
    }
}


    string simplified;
   
    cout << "Enter simplified expression: ";
    getline(cin, simplified);
  
    simplified = removeSpace(simplified);
    
    
    cout << "\nComparison:\n";
    compare(varsTable, exp, simplified, gates ,detectRes);

    return 0;
}

