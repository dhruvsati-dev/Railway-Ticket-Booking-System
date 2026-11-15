/*
    =====================================================================
      RAILWAY TICKET BOOKING SYSTEM  -  FINAL VERSION (Windows GUI)
      Team: TECH CODER
    =====================================================================

    FEATURES
      1. View Trains            4. Cancel Ticket (waiting list promotion)
      2. Book Ticket            5. View All Bookings
      3. View Ticket (by PNR)   6. Data saved in text files
      7. Notification bar at the top of the window (auto hides)

    OOP USED : class, object, constructor, destructor, encapsulation,
               composition (Booking HAS-A Passenger, Train HAS-A WaitingList)
    DSA USED : linked list of trains, linked list of bookings,
               queue (linked list) for waiting list, array for seats,
               linear search (train by number, booking by PNR)
    FILES    : bookings.txt -> current data of every ticket (loaded at start)
               history.txt  -> log of every action with date and time

*/

#include <windows.h>
#include <string>
#include <fstream>     
#include <ctime>      
using namespace std;

const char* BOOKING_FILE = "bookings.txt";
const char* HISTORY_FILE = "history.txt";

const int NOTE_SUCCESS = 1;    // green  :ticket confirmed
const int NOTE_WAIT    = 2;    // orange :added to waiting list
const int NOTE_INFO    = 3;    // blue   :information (WL -> confirmed, loaded)
const int NOTE_ERROR   = 4;    // red    :cancelled / wrong input

// =====================================================================
//                       PART 1: HELPER FUNCTIONS
// =====================================================================

// Number to text: 1001 -> "1001" (digit by digit from the right)
string toText(int n) {
    if (n == 0)
        return "0";
    string s = "";
    while (n > 0) {
        s = char('0' + n % 10) + s;
        n = n / 10;
    }
    return s;
}

// Text to number. Returns -1 if empty or not only digits ("abc" -> -1)
int toNumber(string s) {
    if (s == "" || s.length() > 6)
        return -1;
    int n = 0;
    for (int i = 0; i < (int)s.length(); i++) {
        if (s[i] < '0' || s[i] > '9')
            return -1;
        n = n * 10 + (s[i] - '0');
    }
    return n;
}

// Remove spaces from start and end
string trim(string s) {
    int start = 0;
    int end = (int)s.length() - 1;
    while (start <= end && s[start] == ' ')
        start++;
    while (end >= start && s[end] == ' ')
        end--;
    return s.substr(start, end - start + 1);
}

// Compare two words ignoring capital letters ("delhi" == "Delhi")
bool sameText(string a, string b) {
    if (a.length() != b.length())
        return false;
    for (int i = 0; i < (int)a.length(); i++) {
        char x = a[i], y = b[i];
        if (x >= 'A' && x <= 'Z') x = x + 32;
        if (y >= 'A' && y <= 'Z') y = y + 32;
        if (x != y)
            return false;
    }
    return true;
}

// Phone number must be exactly 10 digits
bool isPhone(string s) {
    if (s.length() != 10)
        return false;
    for (int i = 0; i < 10; i++)
        if (s[i] < '0' || s[i] > '9')
            return false;
    return true;
}

// Current date and time as text, e.g. "14-11-2026 10:35:20"
string getTime() {
    time_t now = time(0);
    tm* t = localtime(&now);
    char buffer[30];
    strftime(buffer, 30, "%d-%m-%Y %H:%M:%S", t);
    return string(buffer);
}

// Split "1001|102|1|650" into parts using '|' as separator.
// Returns how many parts were found.
int splitLine(string line, string part[], int maxParts) {
    int count = 0;
    string current = "";
    for (int i = 0; i < (int)line.length(); i++) {
        if (line[i] == '|') {
            if (count < maxParts) part[count] = current;
            count++;
            current = "";
        } else if (line[i] != '\r') {
            current += line[i];
        }
    }
    if (count < maxParts) part[count] = current;
    count++;
    return count;
}

// Add one line at the END of history.txt (append mode)
void addHistory(string text) {
    ofstream file(HISTORY_FILE, ios::app);
    if (file)
        file << getTime() << "  " << text << "\n";
}

// =====================================================================
//                  PART 2: PROJECT LOGIC (OOP + DSA)
//   
// =====================================================================

// ================= PASSENGER CLASS =================
class Passenger {
public:
    string name;
    int age;
    char gender;
    string contact;

    string getDetails() {
        string text = "";
        text += "Name    : " + name + "\r\n";
        text += "Age     : " + toText(age) + "\r\n";
        text += "Gender  : ";
        text += gender;
        text += "\r\n";
        text += "Contact : " + contact + "\r\n";
        return text;
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

    ~WaitingList() {                 // free all nodes
        while (!isEmpty())
            pop();
    }

    bool isEmpty() {
        return front == NULL;
    }

    void push(int pnr) {             // add at rear
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

    int pop() {                      // remove from front
        Node* temp = front;
        int pnr = temp->pnr;
        front = front->next;
        if (front == NULL)
            rear = NULL;
        delete temp;
        return pnr;
    }

    void remove(int pnr) {           // remove from anywhere (WL cancel)
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

    int position(int pnr) {          // WL 1, WL 2 ... (0 = not found)
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

    string getText() {
        if (front == NULL)
            return "Empty";
        string text = "";
        Node* temp = front;
        while (temp != NULL) {
            text += toText(temp->pnr) + " ";
            temp = temp->next;
        }
        return text;
    }
};

// ================= TRAIN CLASS =================
class Train {
private:
    int seat[10];                    // 0 = free, 1 = booked
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

    int bookSeat() {                 // give first free seat
        for (int i = 0; i < totalSeats; i++) {
            if (seat[i] == 0) {
                seat[i] = 1;
                return i + 1;
            }
        }
        return 0;
    }

    bool occupySeat(int seatNo) {
        if (seatNo < 1 || seatNo > totalSeats || seat[seatNo - 1] == 1)
            return false;
        seat[seatNo - 1] = 1;
        return true;
    }

    void releaseSeat(int seatNo) {
        seat[seatNo - 1] = 0;
    }

    string getDetails() {
        string text = "";
        text += "Train No    : " + toText(number) + "\r\n";
        text += "Train Name  : " + name + "\r\n";
        text += "Route       : " + source + " -> " + destination + "\r\n";
        text += "Fare        : Rs. " + toText(fare) + "\r\n";
        text += "Free Seats  : " + toText(freeSeats()) + " / " + toText(totalSeats) + "\r\n";
        text += "Waiting PNR : " + wl.getText() + "\r\n";
        return text;
    }
};

// ================= BOOKING CLASS =================
class Booking {
public:
    int pnr;
    Passenger p;
    int trainNo;
    int seatNo;                      // 0 = no seat
    int fare;
    string status;                   // CONFIRMED / WL / CANCELLED
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

    string getDetails() {
        string text = "";
        text += "----------- TICKET -----------\r\n";
        text += "PNR     : " + toText(pnr) + "\r\n";
        text += p.getDetails();
        text += "Train   : " + toText(trainNo) + "\r\n";
        text += "Fare    : Rs. " + toText(fare) + "\r\n";
        text += "Status  : " + status + "\r\n";
        if (status == "CONFIRMED")
            text += "Seat No : " + toText(seatNo) + "\r\n";
        text += "------------------------------\r\n";
        return text;
    }

    // One line for bookings.txt, fields separated by '|'
    string getFileLine() {
        return toText(pnr) + "|" + toText(trainNo) + "|" + toText(seatNo) + "|" +
               toText(fare) + "|" + status + "|" + p.name + "|" + toText(p.age) + "|" +
               p.gender + "|" + p.contact;
    }
};

// ================= RAILWAY SYSTEM CLASS =================
class RailwaySystem {
private:
    Train* trainHead;
    Booking* bookingHead;
    int nextPnr;

public:
    string notice;
    int noticeType;
    int loadedCount;                 
    bool saveFailed;                 

    RailwaySystem() {
        trainHead = NULL;
        bookingHead = NULL;
        nextPnr = 1001;
        notice = "";
        noticeType = NOTE_INFO;
        loadedCount = 0;
        saveFailed = false;

        // Sample trains (small seats so waiting list is easy to test)
        addTrain(101, "DoonExpress", "Dehradun", "Delhi", 3, 500);
        addTrain(102, "GangaExpress", "Haridwar", "Lucknow", 2, 650);
        addTrain(103, "MussoorieMail", "Dehradun", "Chandigarh", 2, 400);

        // Bring back bookings saved last time
        loadBookings();
    }

    ~RailwaySystem() {               
        while (trainHead != NULL) {
            Train* temp = trainHead;
            trainHead = trainHead->next;
            delete temp;
        }
        while (bookingHead != NULL) {
            Booking* temp = bookingHead;
            bookingHead = bookingHead->next;
            delete temp;
        }
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

    Train* findTrain(int no) {       // linear search
        Train* temp = trainHead;
        while (temp != NULL) {
            if (temp->number == no)
                return temp;
            temp = temp->next;
        }
        return NULL;
    }

    void addBooking(Booking* b) {    // add at end of booking list
        if (bookingHead == NULL) {
            bookingHead = b;
            return;
        }
        Booking* temp = bookingHead;
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = b;
    }

    Booking* findBooking(int pnr) {  // linear search
        Booking* temp = bookingHead;
        while (temp != NULL) {
            if (temp->pnr == pnr)
                return temp;
            temp = temp->next;
        }
        return NULL;
    }

    // ---------------- FILE: SAVE ----------------
    void saveBookings() {
        ofstream file(BOOKING_FILE);
        if (!file) {
            saveFailed = true;
            return;
        }
        saveFailed = false;
        file << "# PNR|Train|Seat|Fare|Status|Name|Age|Gender|Contact\n";
        Booking* temp = bookingHead;
        while (temp != NULL) {
            file << temp->getFileLine() << "\n";
            temp = temp->next;
        }
    }

    // ---------------- FILE: LOAD ----------------
    void loadBookings() {
        ifstream file(BOOKING_FILE);
        if (!file)                  
            return;

        string line;
        while (getline(file, line)) {
            if (line == "" || line[0] == '#')       
                continue;

            string part[9];
            if (splitLine(line, part, 9) != 9)      
                continue;

            int pnr = toNumber(part[0]);
            int trainNo = toNumber(part[1]);
            int seatNo = toNumber(part[2]);
            int fare = toNumber(part[3]);
            string status = part[4];
            Train* t = findTrain(trainNo);
            if (pnr == -1 || fare == -1 || t == NULL || findBooking(pnr) != NULL)
                continue;

            Passenger p;
            p.name = part[5];
            p.age = toNumber(part[6]);
            p.gender = (part[7] == "") ? 'M' : part[7][0];
            p.contact = part[8];

            Booking* b = new Booking(pnr, p, trainNo, fare);
            if (status == "CONFIRMED") {
                if (!t->occupySeat(seatNo)) {      
                    delete b;
                    continue;
                }
                b->seatNo = seatNo;
                b->status = "CONFIRMED";
            } else if (status == "WL") {
                t->wl.push(pnr);
                b->status = "WL";
            } else {
                b->status = "CANCELLED";
            }

            addBooking(b);
            loadedCount++;
            if (pnr >= nextPnr)              
                nextPnr = pnr + 1;
        }
    }

    // ---------- 1. VIEW TRAINS ----------
    string viewTrains() {
        string text = "========== AVAILABLE TRAINS ==========\r\n\r\n";
        Train* temp = trainHead;
        while (temp != NULL) {
            text += temp->getDetails() + "\r\n";
            temp = temp->next;
        }
        return text;
    }

    // ---------- 2. BOOK TICKET ----------
    string bookTicket(int no, string src, string dest, Passenger p) {
        Train* t = findTrain(no);
        if (t == NULL) {
            notice = "Train " + toText(no) + " not found";
            noticeType = NOTE_ERROR;
            return "Train not found!\r\nClick 'View Trains' to see train numbers.";
        }

        if (!sameText(src, t->source) || !sameText(dest, t->destination)) {
            notice = "Wrong route for train " + toText(no);
            noticeType = NOTE_ERROR;
            return "This train does not run on this route!\r\nTrain " + toText(no) +
                   " runs from " + t->source + " to " + t->destination + ".";
        }

        int fare = t->fare;
        if (p.age >= 60)                     // senior citizen 50% off (sample rule)
            fare = fare / 2;

        Booking* b = new Booking(nextPnr, p, no, fare);
        nextPnr++;

        string message = "";
        if (t->freeSeats() > 0) {
            b->seatNo = t->bookSeat();
            b->status = "CONFIRMED";
            message = "Booking done! Seat confirmed.\r\n\r\n";
            notice = "Your ticket is CONFIRMED!   PNR " + toText(b->pnr) +
                     ",  Seat " + toText(b->seatNo);
            noticeType = NOTE_SUCCESS;
        } else {
            t->wl.push(b->pnr);
            b->status = "WL";
            message = "No seat free. Added to waiting list.\r\n\r\n";
            notice = "Train full - PNR " + toText(b->pnr) + " added to waiting list (WL " +
                     toText(t->wl.position(b->pnr)) + ")";
            noticeType = NOTE_WAIT;
        }

        addBooking(b);

        // Save: memory first, then files
        saveBookings();
        string statusText = (b->status == "CONFIRMED") ? "CONFIRMED Seat " + toText(b->seatNo)
                                                       : "WAITING WL " + toText(t->wl.position(b->pnr));
        addHistory("BOOKED     PNR " + toText(b->pnr) + " | " + p.name + " | Train " +
                   toText(no) + " " + t->source + "->" + t->destination + " | " +
                   statusText + " | Rs. " + toText(fare));

        message += b->getDetails();
        if (b->status == "WL")
            message += "Waiting number: WL " + toText(t->wl.position(b->pnr)) + "\r\n";
        return message;
    }

    // ---------- 3. VIEW TICKET ----------
    string viewTicket(int pnr) {
        Booking* b = findBooking(pnr);
        if (b == NULL) {
            notice = "PNR " + toText(pnr) + " not found";
            noticeType = NOTE_ERROR;
            return "PNR not found!";
        }
        notice = "";                         // no notification needed
        string text = b->getDetails();
        if (b->status == "WL") {
            Train* t = findTrain(b->trainNo);
            text += "Waiting number: WL " + toText(t->wl.position(pnr)) + "\r\n";
        }
        return text;
    }

    // ---------- 4. CANCEL TICKET ----------
    string cancelTicket(int pnr) {
        Booking* b = findBooking(pnr);
        if (b == NULL) {
            notice = "PNR " + toText(pnr) + " not found";
            noticeType = NOTE_ERROR;
            return "PNR not found!";
        }
        if (b->status == "CANCELLED") {
            notice = "PNR " + toText(pnr) + " is already cancelled";
            noticeType = NOTE_ERROR;
            return "Ticket already cancelled!";
        }

        Train* t = findTrain(b->trainNo);
        string message = "";

        if (b->status == "CONFIRMED") {
            int seat = b->seatNo;
            t->releaseSeat(seat);
            b->status = "CANCELLED";
            b->seatNo = 0;
            message = "PNR " + toText(pnr) + " cancelled. Seat " + toText(seat) + " is free now.\r\n";
            addHistory("CANCELLED  PNR " + toText(pnr) + " | " + b->p.name + " | Seat " +
                       toText(seat) + " freed");
            notice = "PNR " + toText(pnr) + " cancelled.  Seat " + toText(seat) + " is free";
            noticeType = NOTE_ERROR;

            
            if (!t->wl.isEmpty()) {
                int wPnr = t->wl.pop();
                Booking* w = findBooking(wPnr);
                w->seatNo = t->bookSeat();
                w->status = "CONFIRMED";
                message += "\r\nPNR " + toText(wPnr) + " moved from waiting list to CONFIRMED (Seat " +
                           toText(w->seatNo) + ").\r\n";
                addHistory("PROMOTED   PNR " + toText(wPnr) + " | " + w->p.name +
                           " | WL -> CONFIRMED Seat " + toText(w->seatNo));
                notice = "PNR " + toText(pnr) + " cancelled.   PNR " + toText(wPnr) +
                         " is now CONFIRMED (Seat " + toText(w->seatNo) + ")";
                noticeType = NOTE_INFO;
            }
        } else {
            t->wl.remove(pnr);
            b->status = "CANCELLED";
            message = "Waiting ticket " + toText(pnr) + " cancelled and removed from waiting list.\r\n";
            addHistory("CANCELLED  PNR " + toText(pnr) + " | " + b->p.name +
                       " | removed from waiting list");
            notice = "Waiting ticket " + toText(pnr) + " cancelled";
            noticeType = NOTE_ERROR;
        }

        saveBookings();
        return message;
    }

    // ---------- 5. VIEW ALL BOOKINGS (traverse booking list) ----------
    string viewAllBookings() {
        if (bookingHead == NULL)
            return "No bookings yet.";
        string text = "PNR   Train  Seat  Status      Name\r\n";
        text += "----------------------------------------------\r\n";
        Booking* temp = bookingHead;
        while (temp != NULL) {
            string seatText = (temp->status == "CONFIRMED") ? toText(temp->seatNo) : "-";
            string row = toText(temp->pnr) + "  " + toText(temp->trainNo) + "    " + seatText;
            while (row.length() < 18) row += " ";       
            row += temp->status;
            while (row.length() < 30) row += " ";
            row += temp->p.name;
            text += row + "\r\n";
            temp = temp->next;
        }
        return text;
    }
};

// =====================================================================
//                     PART 3: GUI (WINDOWS API)
// =====================================================================

#define ID_BTN_BOOK         101
#define ID_BTN_VIEWTICKET   102
#define ID_BTN_CANCEL       103
#define ID_BTN_VIEWTRAINS   104
#define ID_BTN_CLEAR        105
#define ID_BTN_ALLBOOKINGS  106
#define ID_TIMER_NOTICE     201      

const int TOP = 60;                  
const char* TITLE_TEXT = "RAILWAY TICKET BOOKING SYSTEM   |   TEAM TECH CODER";
const COLORREF TITLE_COLOR = RGB(31, 78, 121);

RailwaySystem rs;                    // the whole booking system
HINSTANCE hInst;
HFONT normalFont, codeFont, bannerFont;

HWND editTrainNo, editSource, editDest, editName, editAge,
     editGender, editContact, editPnr, editOutput;

// Notification bar
HWND banner;
COLORREF bannerColor = TITLE_COLOR;
HBRUSH bannerBrush = NULL;

// ---------- Helpers to create controls ----------
void setFont(HWND control, HFONT font) {
    SendMessageA(control, WM_SETFONT, (WPARAM)font, TRUE);
}

void makeLabel(HWND parent, const char* text, int x, int y, int w, int h) {
    HWND label = CreateWindowExA(0, "STATIC", text, WS_CHILD | WS_VISIBLE,
                                 x, y, w, h, parent, NULL, hInst, NULL);
    setFont(label, normalFont);
}

HWND makeEdit(HWND parent, int x, int y, int w, int h) {
    HWND box = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "",
                               WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
                               x, y, w, h, parent, NULL, hInst, NULL);
    setFont(box, normalFont);
    return box;
}

void makeButton(HWND parent, const char* text, int id, int x, int y, int w, int h) {
    HWND button = CreateWindowExA(0, "BUTTON", text,
                                  WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
                                  x, y, w, h, parent, (HMENU)(INT_PTR)id, hInst, NULL);
    setFont(button, normalFont);
}

void makeGroup(HWND parent, const char* text, int x, int y, int w, int h) {
    HWND group = CreateWindowExA(0, "BUTTON", text, WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
                                 x, y, w, h, parent, NULL, hInst, NULL);
    setFont(group, normalFont);
}

string getText(HWND box) {
    char buffer[256];
    GetWindowTextA(box, buffer, 256);
    return trim(string(buffer));
}

void showOutput(string text) {
    SetWindowTextA(editOutput, text.c_str());
}

// ---------- Notification bar ----------
void setBanner(string text, COLORREF color) {
    bannerColor = color;
    if (bannerBrush != NULL)
        DeleteObject(bannerBrush);       
    bannerBrush = CreateSolidBrush(color);
    SetWindowTextA(banner, text.c_str());
    InvalidateRect(banner, NULL, TRUE);     
}

COLORREF colorFor(int type) {
    if (type == NOTE_SUCCESS) return RGB(46, 125, 50);    // green
    if (type == NOTE_WAIT)    return RGB(230, 126, 0);    // orange
    if (type == NOTE_INFO)    return RGB(21, 101, 192);   // blue
    return RGB(198, 40, 40);                              // red
}

// Show a notification, then a timer brings back the title after 3.5 s
void showNotice(HWND hwnd, string text, int type) {
    setBanner(text, colorFor(type));
    SetTimer(hwnd, ID_TIMER_NOTICE, 3500, NULL);   

// ---------- Create all controls ----------
void createControls(HWND hwnd) {
    normalFont = CreateFontA(-15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                             OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                             DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");
    codeFont = CreateFontA(-15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                           OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                           FIXED_PITCH | FF_MODERN, "Consolas");
    bannerFont = CreateFontA(-19, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                             OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                             DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");

    // ----- Notification bar across the full width -----
    RECT area;
    GetClientRect(hwnd, &area);
    banner = CreateWindowExA(0, "STATIC", TITLE_TEXT,
                             WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE,
                             0, 0, area.right, 48, hwnd, NULL, hInst, NULL);
    setFont(banner, bannerFont);
    setBanner(TITLE_TEXT, TITLE_COLOR);

    // ----- Book Ticket section -----
    makeGroup(hwnd, "Book Ticket", 15, TOP + 10, 340, 335);
    makeLabel(hwnd, "Train No:",     30, TOP + 42, 110, 22);  editTrainNo = makeEdit(hwnd, 150, TOP + 40, 190, 26);
    makeLabel(hwnd, "Source:",       30, TOP + 77, 110, 22);  editSource  = makeEdit(hwnd, 150, TOP + 75, 190, 26);
    makeLabel(hwnd, "Destination:",  30, TOP + 112, 110, 22); editDest    = makeEdit(hwnd, 150, TOP + 110, 190, 26);
    makeLabel(hwnd, "Name:",         30, TOP + 147, 110, 22); editName    = makeEdit(hwnd, 150, TOP + 145, 190, 26);
    makeLabel(hwnd, "Age:",          30, TOP + 182, 110, 22); editAge     = makeEdit(hwnd, 150, TOP + 180, 190, 26);
    makeLabel(hwnd, "Gender (M/F):", 30, TOP + 217, 110, 22); editGender  = makeEdit(hwnd, 150, TOP + 215, 190, 26);
    makeLabel(hwnd, "Contact:",      30, TOP + 252, 110, 22); editContact = makeEdit(hwnd, 150, TOP + 250, 190, 26);
    makeButton(hwnd, "Book Ticket", ID_BTN_BOOK, 30, TOP + 295, 310, 34);

    // ----- PNR section -----
    makeGroup(hwnd, "View / Cancel Ticket", 15, TOP + 355, 340, 120);
    makeLabel(hwnd, "PNR:", 30, TOP + 387, 110, 22);
    editPnr = makeEdit(hwnd, 150, TOP + 385, 190, 26);
    makeButton(hwnd, "View Ticket",   ID_BTN_VIEWTICKET, 30, TOP + 425, 150, 34);
    makeButton(hwnd, "Cancel Ticket", ID_BTN_CANCEL,     190, TOP + 425, 150, 34);

    // ----- Other buttons -----
    makeButton(hwnd, "View Trains",  ID_BTN_VIEWTRAINS,  15,  TOP + 490, 108, 36);
    makeButton(hwnd, "All Bookings", ID_BTN_ALLBOOKINGS, 131, TOP + 490, 108, 36);
    makeButton(hwnd, "Clear Output", ID_BTN_CLEAR,       247, TOP + 490, 108, 36);

    // ----- Output box -----
    makeLabel(hwnd, "Output:", 375, TOP + 12, 200, 22);
    editOutput = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "",
                                 WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE |
                                 ES_READONLY | ES_AUTOVSCROLL,
                                 375, TOP + 38, 500, 488, hwnd, NULL, hInst, NULL);
    setFont(editOutput, codeFont);

    showOutput(rs.viewTrains());
    if (rs.loadedCount > 0)
        showNotice(hwnd, "Welcome back!  " + toText(rs.loadedCount) +
                   " saved bookings loaded from file", NOTE_INFO);
}

// Show result + notification after an operation
void showResult(HWND hwnd, string output) {
    if (rs.saveFailed)
        output += "\r\nWARNING: could not save bookings.txt (check folder permission).\r\n";
    showOutput(output);
    if (rs.notice != "")
        showNotice(hwnd, rs.notice, rs.noticeType);
}

// ---------- Button handlers ----------
void handleBook(HWND hwnd) {
    int no = toNumber(getText(editTrainNo));
    string src = getText(editSource);
    string dest = getText(editDest);
    string gender = getText(editGender);

    Passenger p;
    p.name = getText(editName);
    p.age = toNumber(getText(editAge));
    p.contact = getText(editContact);

    // Check every input before booking
    string error = "";
    if (no == -1)
        error = "Enter a valid train number (digits only)";
    else if (src == "" || dest == "" || p.name == "")
        error = "Please fill Source, Destination and Name";
    else if (p.name.find('|') != string::npos)
        error = "Name cannot contain the | symbol";
    else if (p.age < 1 || p.age > 120)
        error = "Enter a valid age (1 to 120)";
    else if (gender != "M" && gender != "m" && gender != "F" && gender != "f")
        error = "Gender must be M or F";
    else if (!isPhone(p.contact))
        error = "Contact number must be exactly 10 digits";

    if (error != "") {
        showNotice(hwnd, error, NOTE_ERROR);
        return;
    }

    if (gender == "M" || gender == "m")
        p.gender = 'M';
    else
        p.gender = 'F';

    showResult(hwnd, rs.bookTicket(no, src, dest, p));

    // Clear passenger boxes for the next booking (keep train and route)
    SetWindowTextA(editName, "");
    SetWindowTextA(editAge, "");
    SetWindowTextA(editGender, "");
    SetWindowTextA(editContact, "");
}

void handleViewTicket(HWND hwnd) {
    int pnr = toNumber(getText(editPnr));
    if (pnr == -1) {
        showNotice(hwnd, "Enter a valid PNR (digits only)", NOTE_ERROR);
        return;
    }
    showResult(hwnd, rs.viewTicket(pnr));
}

void handleCancel(HWND hwnd) {
    int pnr = toNumber(getText(editPnr));
    if (pnr == -1) {
        showNotice(hwnd, "Enter a valid PNR (digits only)", NOTE_ERROR);
        return;
    }
    int answer = MessageBoxA(hwnd, "Do you really want to cancel this ticket?",
                             "Confirm Cancel", MB_YESNO | MB_ICONQUESTION);
    if (answer == IDYES)
        showResult(hwnd, rs.cancelTicket(pnr));
}

// ---------- WindowProc: receives every message from Windows ----------
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            createControls(hwnd);
            return 0;

        case WM_COMMAND: {                     
            int id = LOWORD(wParam);
            if (id == ID_BTN_BOOK)        handleBook(hwnd);
            if (id == ID_BTN_VIEWTICKET)  handleViewTicket(hwnd);
            if (id == ID_BTN_CANCEL)      handleCancel(hwnd);
            if (id == ID_BTN_VIEWTRAINS)  showOutput(rs.viewTrains());
            if (id == ID_BTN_ALLBOOKINGS) showOutput(rs.viewAllBookings());
            if (id == ID_BTN_CLEAR)       showOutput("");
            return 0;
        }

        case WM_TIMER:                           
            if (wParam == ID_TIMER_NOTICE) {
                KillTimer(hwnd, ID_TIMER_NOTICE);
                setBanner(TITLE_TEXT, TITLE_COLOR);   
            }
            return 0;

        case WM_CTLCOLORSTATIC:                   
            if ((HWND)lParam == banner) {
                HDC hdc = (HDC)wParam;
                SetTextColor(hdc, RGB(255, 255, 255));
                SetBkColor(hdc, bannerColor);
                return (LRESULT)bannerBrush;          
            }
            break;                                

        case WM_DESTROY:
            KillTimer(hwnd, ID_TIMER_NOTICE);
            DeleteObject(normalFont);
            DeleteObject(codeFont);
            DeleteObject(bannerFont);
            if (bannerBrush != NULL)
                DeleteObject(bannerBrush);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

// ---------- WinMain: starting point of a Windows program ----------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    hInst = hInstance;

    WNDCLASSA wc = {};                            
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "RailwayWindow";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    RegisterClassA(&wc);

    HWND hwnd = CreateWindowExA(0, "RailwayWindow",
                                "Railway Ticket Booking System - TECH CODER",
                                WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                                CW_USEDEFAULT, CW_USEDEFAULT, 910, 655,
                                NULL, NULL, hInstance, NULL);
    ShowWindow(hwnd, nCmdShow);

    MSG msg;                                      
    while (GetMessageA(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    return 0;
}
