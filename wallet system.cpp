#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>
#include <limits>

using namespace std;


class Transaction {
public:
    string detail;

    Transaction(string d = "") {
        detail = d;
    }
};


class User {
private:
    vector<Transaction> history;

public:
    string name;
    string cnic;
    string phone;
    string address;
    int age;
    string pin;
    double balance;

    User() {
        balance = 0;
        age = 0;
    }

    void addTransaction(const string& msg) {
        history.push_back(Transaction(msg));
    }

    void showHistory() {
        if (history.empty()) {
            cout << "No transactions yet.\n";
            return;
        }

        for (auto &t : history) {
            cout << "- " << t.detail << endl;
        }
    }

    void deposit(double amt) {
        balance += amt;
        addTransaction("Deposited: " + to_string(amt));
    }

    bool withdraw(double amt) {
        if (amt > balance) return false;
        balance -= amt;
        addTransaction("Withdrawn: " + to_string(amt));
        return true;
    }

    void receive(double amt, const string& from) {
        balance += amt;
        addTransaction("Received " + to_string(amt) + " from " + from);
    }

    bool send(double amt, User &receiver) {
        if (amt > balance) return false;

        balance -= amt;
        receiver.receive(amt, cnic);

        addTransaction("Sent " + to_string(amt) + " to " + receiver.cnic);
        return true;
    }
};

class WalletSystem {
private:
    vector<User> users;

    int findUser(string cnic) {
        for (int i = 0; i < users.size(); i++) {
            if (users[i].cnic == cnic)
                return i;
        }
        return -1;
    }

public:

    WalletSystem() {
        loadUsers();
    }

    ~WalletSystem() {
        saveUsers();
    }

    
    int safeInt() {
        int x;
        while (!(cin >> x)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Enter valid number: ";
        }
        return x;
    }

    double safeDouble() {
        double x;
        while (!(cin >> x)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Enter valid amount: ";
        }
        return x;
    }

    bool validCNIC(string cnic) {
        regex pattern("[0-9]{5}-[0-9]{7}-[0-9]");
        return regex_match(cnic, pattern);
    }

    bool validPhone(string phone) {
        regex pattern("03[0-9]{9}");
        return regex_match(phone, pattern);
    }

  
    void saveUsers() {
        ofstream file("users.txt");

        for (auto &u : users) {
            file << u.name << "|"
                 << u.cnic << "|"
                 << u.phone << "|"
                 << u.address << "|"
                 << u.age << "|"
                 << u.pin << "|"
                 << u.balance << "\n";
        }
    }

    void loadUsers() {
        ifstream file("users.txt");
        string line;

        while (getline(file, line)) {
            stringstream ss(line);
            User u;
            string temp;

            getline(ss, u.name, '|');
            getline(ss, u.cnic, '|');
            getline(ss, u.phone, '|');
            getline(ss, u.address, '|');

            getline(ss, temp, '|');
            u.age = stoi(temp);

            getline(ss, u.pin, '|');

            getline(ss, temp, '|');
            u.balance = stod(temp);

            users.push_back(u);
        }
    }

    void registerUser() {
        User u;

        cin.ignore();
        cout << "Name: ";
        getline(cin, u.name);

        cout << "CNIC (12345-1234567-1): ";
        cin >> u.cnic;
        if (!validCNIC(u.cnic)) {
            cout << "Invalid CNIC\n";
            return;
        }

        cout << "Phone (03XXXXXXXXX): ";
        cin >> u.phone;
        if (!validPhone(u.phone)) {
            cout << "Invalid phone\n";
            return;
        }

        cout << "Age: ";
        u.age = safeInt();
        if (u.age < 18) {
            cout << "Must be 18+\n";
            return;
        }

        cin.ignore();
        cout << "Address: ";
        getline(cin, u.address);

        cout << "PIN: ";
        cin >> u.pin;

        if (findUser(u.cnic) != -1) {
            cout << "Account already exists\n";
            return;
        }

        users.push_back(u);
        cout << "Account created successfully\n";
    }

    int login() {
        string cnic, pin;

        cout << "CNIC: ";
        cin >> cnic;

        cout << "PIN: ";
        cin >> pin;

        int idx = findUser(cnic);

        if (idx != -1 && users[idx].pin == pin) {
            cout << "Welcome " << users[idx].name << "\n";
            return idx;
        }

        cout << "Invalid login\n";
        return -1;
    }

    void checkBalance(int idx) {
        cout << "Balance: " << users[idx].balance << endl;
    }

    void deposit(int idx) {
        cout << "Amount: ";
        double amt = safeDouble();

        if (amt <= 0) {
            cout << "Invalid amount\n";
            return;
        }

        users[idx].deposit(amt);
        cout << "Deposit successful\n";
    }

    void withdraw(int idx) {
        cout << "Amount: ";
        double amt = safeDouble();

        if (amt <= 0) {
            cout << "Invalid amount\n";
            return;
        }

        if (!users[idx].withdraw(amt)) {
            cout << "Insufficient balance\n";
            return;
        }

        cout << "Withdrawal successful\n";
    }

    void sendMoney(int senderIdx) {
        string receiverCnic;
        double amt;

        cout << "Receiver CNIC: ";
        cin >> receiverCnic;

        cout << "Amount: ";
        amt = safeDouble();

        if (amt <= 0) {
            cout << "Invalid amount\n";
            return;
        }

        int receiverIdx = findUser(receiverCnic);

        if (receiverIdx == -1) {
            cout << "Receiver not found\n";
            return;
        }

        if (!users[senderIdx].send(amt, users[receiverIdx])) {
            cout << "Insufficient balance\n";
            return;
        }

        cout << "Transfer successful\n";
    }

    void showHistory(int idx) {
        users[idx].showHistory();
    }

    void adminPanel() {
        cout << "\n--- ALL USERS ---\n";

        for (auto &u : users) {
            cout << u.name << " | " << u.cnic << " | " << u.balance << endl;
        }
    }
};

int main() {
    WalletSystem system;

    while (true) {
        cout << "\n1 Register\n2 Login\n3 Admin\n4 Exit\n";
        int choice;
        cin >> choice;

        if (choice == 1) {
            system.registerUser();
        }

        else if (choice == 2) {
            int idx = system.login();

            if (idx != -1) {
                while (true) {
                    cout << "\n1 Balance\n2 Deposit\n3 Withdraw\n4 Send\n5 History\n6 Logout\n";
                    int op;
                    cin >> op;

                    if (op == 1) system.checkBalance(idx);
                    else if (op == 2) system.deposit(idx);
                    else if (op == 3) system.withdraw(idx);
                    else if (op == 4) system.sendMoney(idx);
                    else if (op == 5) system.showHistory(idx);
                    else break;
                }
            }
        }

        else if (choice == 3) {
            system.adminPanel();
        }

        else break;
    }

    return 0;
}