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
