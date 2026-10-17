/*
    RAILWAY TICKET BOOKING SYSTEM
    Team: TECH CODER
*/

#include <iostream>
#include <string>
using namespace std;

// ================= PASSENGER CLASS =================

class Passenger {
public:
    string name;
    int age;
    char gender;
    string contact;

    void display() {
        cout << "Name    : " << name << endl;
        cout << "Age     : " << age << endl;
        cout << "Gender  : " << gender << endl;
        cout << "Contact : " << contact << endl;
    }
};

// ================= WAITING LIST (QUEUE) =================

class Node {
public:
    int pnr;
    Node* next;
};

class WaitingList {
private:
    Node* front;
    Node* rear;

public:
    WaitingList() {
        front = NULL;
        rear = NULL;
    }

    bool isEmpty() {
        return front == NULL;
    }

    
    void push(int pnr) {
        Node* temp = new Node;
        temp->pnr = pnr;
        temp->next = NULL;

        if (rear == NULL) {          
            front = temp;
            rear = temp;
        } else {                    
            rear->next = temp;
            rear = temp;
        }
    }

    int pop() {
        Node* temp = front;
        int pnr = temp->pnr;
        front = front->next;
        if (front == NULL)         
            rear = NULL;
        delete temp;
        return pnr;
    }

    void remove(int pnr) {
        Node* temp = front;
        Node* prev = NULL;

        while (temp != NULL && temp->pnr != pnr) {
            prev = temp;
            temp = temp->next;
        }
        if (temp == NULL)            
            return;

        if (prev == NULL)           
            front = temp->next;
        else
            prev->next = temp->next; 

        if (temp == rear)           
            rear = prev;

        delete temp;
    }

    int position(int pnr) {
        int pos = 1;
        Node* temp = front;
        while (temp != NULL) {
            if (temp->pnr == pnr)
                return pos;
            pos++;
            temp = temp->next;
        }
        return 0;
    }

    void display() {
        if (front == NULL) {
            cout << "Empty";
            return;
        }
        Node* temp = front;
        while (temp != NULL) {
            cout << temp->pnr << " ";
            temp = temp->next;
        }
    }
};

// ================= TRAIN CLASS =================

class Train {
private:
    int seat[10];                  
    int totalSeats;

public:
    int number;
    string name;
    string source;
    string destination;
    int fare;
    WaitingList wl;                 
    Train* next;                     

    Train(int no, string n, string src, string dest, int seats, int f) {
        number = no;
        name = n;
        source = src;
        destination = dest;
        totalSeats = seats;
        fare = f;
        next = NULL;
        for (int i = 0; i < totalSeats; i++)
            seat[i] = 0;             
    }

    int freeSeats() {
        int count = 0;
        for (int i = 0; i < totalSeats; i++)
            if (seat[i] == 0)
                count++;
        return count;
    }

    int bookSeat() {
        for (int i = 0; i < totalSeats; i++) {
            if (seat[i] == 0) {
                seat[i] = 1;        
                return i + 1;       
            }
        }
        return 0;                   
    }

    void releaseSeat(int seatNo) {
        seat[seatNo - 1] = 0;
    }

    void display() {
        cout << "\nTrain No    : " << number << endl;
        cout << "Train Name  : " << name << endl;
        cout << "Route       : " << source << " -> " << destination << endl;
        cout << "Fare        : Rs. " << fare << endl;
        cout << "Free Seats  : " << freeSeats() << " / " << totalSeats << endl;
        cout << "Waiting PNR : ";
        wl.display();
        cout << endl;
    }
};

// ================= BOOKING CLASS =================

class Booking {
public:
    int pnr;
    Passenger p;
    int trainNo;
    int seatNo;                   
    int fare;
    string status;                 
    Booking* next;                  

    Booking(int pnrNo, Passenger pass, int tNo, int f) {
        pnr = pnrNo;
        p = pass;
        trainNo = tNo;
        fare = f;
        seatNo = 0;
        status = "WL";
        next = NULL;
    }

    void display() {
        cout << "\n----------- TICKET -----------\n";
        cout << "PNR     : " << pnr << endl;
        p.display();
        cout << "Train   : " << trainNo << endl;
        cout << "Fare    : Rs. " << fare << endl;
        cout << "Status  : " << status << endl;
        if (status == "CONFIRMED")
            cout << "Seat No : " << seatNo << endl;
        cout << "------------------------------\n";
    }
};

// ================= RAILWAY SYSTEM CLASS =================

class RailwaySystem {
private:
    Train* trainHead;                
    Booking* bookingHead;           
    int nextPnr;                  

public:
    RailwaySystem() {
        trainHead = NULL;
        bookingHead = NULL;
        nextPnr = 1001;
        addTrain(101, "DoonExpress", "Dehradun", "Delhi", 3, 500);
        addTrain(102, "GangaExpress", "Haridwar", "Lucknow", 2, 650);
        addTrain(103, "MussoorieMail", "Dehradun", "Chandigarh", 2, 400);
    }

    void addTrain(int no, string n, string src, string dest, int seats, int f) {
        Train* t = new Train(no, n, src, dest, seats, f);

        if (trainHead == NULL) {
            trainHead = t;
            return;
        }
        Train* temp = trainHead;
        while (temp->next != NULL)   
            temp = temp->next;
        temp->next = t;
    }

    Train* findTrain(int no) {
        Train* temp = trainHead;
        while (temp != NULL) {
            if (temp->number == no)
                return temp;
            temp = temp->next;
        }
        return NULL;               
    }

    void addBooking(Booking* b) {
        if (bookingHead == NULL) {
            bookingHead = b;
            return;
        }
        Booking* temp = bookingHead;
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = b;
    }

    Booking* findBooking(int pnr) {
        Booking* temp = bookingHead;
        while (temp != NULL) {
            if (temp->pnr == pnr)
                return temp;
            temp = temp->next;
        }
        return NULL;
    }

    void viewTrains() {
        Train* temp = trainHead;
        while (temp != NULL) {
            temp->display();
            temp = temp->next;
        }
    }

    void bookTicket() {
        int no;
        cout << "Enter train number: ";
        cin >> no;

        Train* t = findTrain(no);
        if (t == NULL) {
            cout << "Train not found!\n";
            return;
        }

        string src, dest;
        cout << "Enter source: ";
        cin >> src;
        cout << "Enter destination: ";
        cin >> dest;
        if (src != t->source || dest != t->destination) {
            cout << "This train does not run on this route!\n";
            return;
        }

        Passenger p;
        cout << "Enter name: ";
        cin >> p.name;
        cout << "Enter age: ";
        cin >> p.age;
        cout << "Enter gender (M/F): ";
        cin >> p.gender;
        cout << "Enter contact number: ";
        cin >> p.contact;

        int fare = t->fare;
        if (p.age >= 60)
            fare = fare / 2;

        Booking* b = new Booking(nextPnr, p, no, fare);
        nextPnr++;

        if (t->freeSeats() > 0) {
            b->seatNo = t->bookSeat();
            b->status = "CONFIRMED";
        } else {
            t->wl.push(b->pnr);
            b->status = "WL";
            cout << "No seat free. Added to waiting list.\n";
        }

        addBooking(b);
        cout << "Booking done!\n";
        b->display();
        if (b->status == "WL")
            cout << "Waiting number: WL " << t->wl.position(b->pnr) << endl;
    }

    void viewTicket() {
        int pnr;
        cout << "Enter PNR: ";
        cin >> pnr;

        Booking* b = findBooking(pnr);
        if (b == NULL) {
            cout << "PNR not found!\n";
            return;
        }
        b->display();
        if (b->status == "WL") {
            Train* t = findTrain(b->trainNo);
            cout << "Waiting number: WL " << t->wl.position(pnr) << endl;
        }
    }
};

// ================= MAIN =================
int main() {
    RailwaySystem rs;
    int choice;

    do {
        cout << "\n===== RAILWAY TICKET BOOKING =====\n";
        cout << "1. View Trains\n";
        cout << "2. Book Ticket\n";
        cout << "3. View Ticket\n";
        cout << "4. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: rs.viewTrains(); break;
            case 2: rs.bookTicket(); break;
            case 3: rs.viewTicket(); break;
            case 4: cout << "Thank you!\n"; break;
            default: cout << "Wrong choice!\n";
        }
    } while (choice != 4);

    return 0;
}
