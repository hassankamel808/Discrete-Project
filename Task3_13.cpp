#include <iostream>
#include <map>
#include <string>
using namespace std;


string samoan = " AEIOUFGLMNPSTV";  

int modInverse(int a, int m) {
    for (int i = 1; i < m; i++) {
        if ((a * i) % m == 1)
            return i;
    }
    return -1; 
}

char safeToUpper(char ch) {
    if (ch >= 'a' && ch <= 'z') {
        char upper = ch - 32;
        for (int i = 0; i < samoan.size(); i++) {
            if (upper == samoan[i])
                return upper;
        }
    }
    return ch; 
}

string Affine_cipher(string original, int a, int b) {
    
    map<int, char> intToChar;
    int m = samoan.size(); 

    for (int i = 0; i < m; i++) {
        
        intToChar[i] = samoan[i];
    }

    string c = "";
    for (int i = 0; i < original.length(); i++) {
        char ch = safeToUpper(original[i]); 

        int index = -1;
        for (int j = 0; j < m; j++) {
            if (ch == samoan[j]) {
                index = j;
                break;
            }
        }

        if (index != -1) {
            int value = (a * index + b) % m;  
            c += intToChar[value];
        } else {
            c += original[i]; 
        }
    }
    return c;
}

string Affine_decipher(string cipher, int a, int b) {
    map<int, char> intToChar;
    int m = samoan.size(); 
    int a_inv = modInverse(a, m);
    for (int i = 0; i < m; i++) {
       
        intToChar[i] = samoan[i];
    }

    string p = "";
    for (int i = 0; i < cipher.length(); i++) {
        int index = -1;
        for (int j = 0; j < m; j++) {
            if (cipher[i] == samoan[j]) {
                index = j;
                break;
            }
        }

        if (index != -1) {
            int value = a_inv * (index - b);
            value = (value % m + m) % m;
            p += intToChar[value];
        } else {
            p += cipher[i]; 
        }
    }
    return p;
}

int main() {
    string message;
    int a, b;
    int a_inv;
    int m = samoan.size();
    /*
    //Format 1:"hardcoded"
    a=17;
    b=13;
    message="E TAUA LENEI GALUEGA";
    

*/
    

    // Loop until user enters a valid 'a'

    //Format 2 user input 
    
    do {
        cout << "Enter cipher key a: ";
        cin >> a;
        a_inv = modInverse(a, m);
        if (a_inv == -1) {
            cout << "This function has no inverse, so you will not be able to decrypt it. Choose another value for a." << endl;
        }
    } while (a_inv == -1);

    cout << "Enter cipher key b: ";
    cin >> b;

    cin.ignore(); 
    cout << "Enter message to cipher: ";
    getline(cin, message);
    

    string ciphered = Affine_cipher(message, a, b);
    cout << "Ciphered message: " << ciphered << endl;

    string deciphered = Affine_decipher(ciphered, a, b);
    cout << "Deciphered message: " << deciphered << endl;

    return 0;
}