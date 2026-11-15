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

// Add one line at the END of history.txt (ios::app = append mode,
// old lines are never deleted)
void addHistory(string text) {
    ofstream file(HISTORY_FILE, ios::app);
    if (file)
        file << getTime() << "  " << text << "\n";
}

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
