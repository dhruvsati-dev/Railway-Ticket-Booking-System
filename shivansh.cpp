// ================= RAILWAY SYSTEM CLASS =================
class RailwaySystem {
private:
    Train* trainHead;
    Booking* bookingHead;
    int nextPnr;

public:
    // Message for the notification bar after each operation
    string notice;
    int noticeType;
    int loadedCount;                 // bookings loaded from file at start
    bool saveFailed;                 // true if bookings.txt could not be written

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

    ~RailwaySystem() {               // free every train and booking
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
    // Traverse the booking list and write every booking to bookings.txt.
    // The file is written fresh each time, so it always shows the
    // CURRENT status of every ticket.
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
    // Read bookings.txt line by line and rebuild the booking list,
    // the seats and the waiting queues exactly as they were.
    // Lines are in PNR order, so WL tickets join the queue in the
    // same order as before.
    void loadBookings() {
        ifstream file(BOOKING_FILE);
        if (!file)                   // first run: file does not exist yet
            return;

        string line;
        while (getline(file, line)) {
            if (line == "" || line[0] == '#')       // skip empty / heading line
                continue;

            string part[9];
            if (splitLine(line, part, 9) != 9)      // broken line -> skip
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
                if (!t->occupySeat(seatNo)) {      // seat problem -> skip line
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
            if (pnr >= nextPnr)              // continue PNR numbers after the last one
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
            // Step 1: free the seat
            int seat = b->seatNo;
            t->releaseSeat(seat);
            b->status = "CANCELLED";
            b->seatNo = 0;
            message = "PNR " + toText(pnr) + " cancelled. Seat " + toText(seat) + " is free now.\r\n";
            addHistory("CANCELLED  PNR " + toText(pnr) + " | " + b->p.name + " | Seat " +
                       toText(seat) + " freed");
            notice = "PNR " + toText(pnr) + " cancelled.  Seat " + toText(seat) + " is free";
            noticeType = NOTE_ERROR;

            // Step 2: first person in waiting list gets the seat (FIFO)
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
            // Waiting ticket: only remove it from the queue
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
            while (row.length() < 18) row += " ";       // line up the columns
            row += temp->status;
            while (row.length() < 30) row += " ";
            row += temp->p.name;
            text += row + "\r\n";
            temp = temp->next;
        }
        return text;
    }
};
   