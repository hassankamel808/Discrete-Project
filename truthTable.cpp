#include <iostream>
#include <vector>
#include <cmath>
#include <set>
#include <stack>
using namespace std;

// object
struct Gate {
    char name;
    vector<int> values; 
};

// truth table
vector<string> truthTable(int bits)
{

    int rows = pow(2, bits);     
    vector<string> combination(rows);

    for (int i = 0; i < rows; i++)
    {
        string s = "";
        for (int j = bits - 1; j >= 0; j--)
        {
            int bit = (i >> j) & 1; 
            s += char(bit + '0');  
        }
        combination[i] = s; 
    }

    return combination;
}


//return value to gate depend on row
int getValue(char ch, vector<Gate>& gates, int row) {
    for (int i = 0; i < gates.size(); i++) {
        if (gates[i].name == ch) return gates[i].values[row];
    }
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
    }
}

// pariority  
int precedence(char op) {
    if (op == '!' || op =='~') return 3;
    if (op == '&') return 2;
    if (op == '|') return 1;
    return 0;
}

//evaluation
int evaluate(string exp, vector<Gate>& gates, int row) {
    stack<int> s;
    stack<char> ops;

    for (int i = 0; i < exp.length(); i++) {
        char ch = exp[i];

        if (ch == ' ') continue;

        if (ch == '(') {
            ops.push(ch);
        } else if (ch == ')') {
            while (!ops.empty() && ops.top() != '(') {
                applyOp(s, ops.top());
                ops.pop();
            }
            ops.pop(); 
        } else if (isalpha(ch)) {
            s.push(getValue(ch, gates, row));
        } else { 
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


string removeSpace(string exp){
    string newExp="";
    for(int i=0;i<exp.length();i++){
       if(exp[i] !=' '){
           newExp+=exp[i];
       }
    }
    return newExp;
}
int main()
{
  


    // get no of gates    
    int n;
    cout << "Enter number of gates: ";
    cin >> n;

    // get wire name
    vector<char> gateNames;
    for(int i = 0; i < n; i++) {
        char name;
        cout << "Enter name of gate " << i+1 << ": ";
        cin >> name;
        gateNames.push_back(name);   
    }

    // generate truth table
    vector<string> t = truthTable(n);

    // print truth table
    cout<<"truth table"<<endl;

    for(string row :t){
        cout << row<<endl;
    }

    // convert each string to  (int) bit
    vector<vector<int>> varsTable;
    for(int i = 0; i < t.size(); i++) {
        vector<int> row;
        for(int k = 0; k < n; k++) {
            row.push_back(t[i][k] - '0');
        }
        varsTable.push_back(row);
    }



    //   assign all combination  
       vector<Gate> gates;
        for(int i = 0; i < n; i++) {
        Gate g;
        g.name = gateNames[i];
        for(int j = 0; j < varsTable.size(); j++){
        g.values.push_back(varsTable[j][i]);
    }
        gates.push_back(g);
    }



   
    // enter expression
    string exp = "";
    cout<<"enter the exp:";
    cin.ignore();
    getline(cin, exp);

    // remove internal space
    exp=removeSpace(exp);
    cout<<"no space"<<exp<<endl;


    // print result 
    for(int row = 0; row < varsTable.size(); row++){
        int result = evaluate(exp, gates, row);
        cout << "Row " << row << ": ";
        for(int k = 0; k < n; k++) cout << varsTable[row][k] << " ";
        cout << " -> Result: " << result << endl;
    }

    }
  
    


 

//     for(int i = 0; i < gates.size(); i++) {
//     cout << gates[i].name << ": ";
//     for(int val : gates[i].values) {
//         cout << val << " ";
//     }
//     cout << endl;
// }