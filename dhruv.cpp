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
        text += "Gelonder  : ";
        text += gender;
        text += "\r\n";
        text += "Contact : " + contact + "\r\n";
        return text;
    }
};

// ================= TRAIN CLASS =================
class Train {
private:
    // Encapsulation: seats change only through bookSeat(), occupySeat()
    // and releaseSeat(), so two passengers never get the same seat.
    int seat[10];                    // 0 = free, 1 = booked
    int totalSeats;

public:
    int number;
    string name;
    string source;
    string destination;
    int fare;
    WaitingList wl;                  // each train has its own waiting list
    Train* next;                     // link to next train

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

    // Book a SPECIFIC seat (used when loading saved data from file).
    // Returns false if the seat number is wrong or already booked.
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
