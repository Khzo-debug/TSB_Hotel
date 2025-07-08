#include <iostream>
#include <stack>
#include <queue>
#include <string>
#include <iomanip>
using namespace std;

/* ========== KONSTANTA DAN STRUKTUR DATA ========== */
const int STANDARD_ROOMS = 50;
const int DELUXE_ROOMS = 30;
const int SUITE_ROOMS = 15;
const int VIP_ROOMS = 5;
const int TOTAL_ROOMS = STANDARD_ROOMS + DELUXE_ROOMS + SUITE_ROOMS + VIP_ROOMS;
const int MAX_GUESTS = 200;
const int MAX_SERVICE_REQUESTS = 100;

// Pointer - Linked list untuk kamar
struct Room {
    int number;
    string type;
    int price;
    bool occupied;
    Room* next;
    
    Room(int num, string t, int p) : number(num), type(t), price(p), occupied(false), next(nullptr) {}
};

struct Guest {
    string name;
    string phone;
    int roomNumber;
};

// Tree - Binary Tree untuk tamu (bukan BST)
struct GuestNode {
    Guest* guest;
    GuestNode* left;
    GuestNode* right;
    GuestNode(Guest* g) : guest(g), left(nullptr), right(nullptr) {}
};

// Queue - Circular Queue dengan array untuk layanan
struct ServiceQueue {
    string requests[MAX_SERVICE_REQUESTS];
    int front;
    int rear;
    int count;
    
    ServiceQueue() : front(0), rear(-1), count(0) {}
};

/* ========== VARIABEL GLOBAL ========== */
Room* roomList = nullptr; // Pointer - Linked list head
stack<int> availableRooms; // Stack - STL stack untuk kamar tersedia
Guest* guestList[MAX_GUESTS] = {nullptr}; // Array untuk tamu
int guestCount = 0;
GuestNode* guestTree = nullptr; // Tree - Binary Tree root
ServiceQueue serviceQueue; // Queue - Implementasi circular queue

/* ========== FUNGSI UTILITY ========== */
string formatColumn(string text, int width) {
    while (text.length() < width) {
        text += " ";
    }
    return text;
}

/* ========== SORTING FUNCTIONS ========== */
// Sorting - Bubble Sort untuk mengurutkan tamu berdasarkan nama
void bubbleSortGuestsByName() {
    for (int i = 0; i < guestCount-1; i++) {
        for (int j = 0; j < guestCount-i-1; j++) {
            if (guestList[j]->name > guestList[j+1]->name) {
                swap(guestList[j], guestList[j+1]);
            }
        }
    }
}

// Sorting - Selection Sort untuk mengurutkan kamar berdasarkan nomor
void selectionSortRooms() {
    Room* current = roomList;
    while (current) {
        Room* min = current;
        Room* r = current->next;
        
        while (r) {
            if (r->number < min->number) {
                min = r;
            }
            r = r->next;
        }
        
        // Swap data
        swap(current->number, min->number);
        swap(current->type, min->type);
        swap(current->price, min->price);
        swap(current->occupied, min->occupied);
        
        current = current->next;
    }
}

/* ========== SEARCHING FUNCTIONS ========== */
// Searching - Binary Search untuk mencari tamu berdasarkan nama
Guest* binarySearchGuest(const string& name) {
    // Sort dulu untuk binary search
    bubbleSortGuestsByName();
    
    int left = 0, right = guestCount - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (guestList[mid]->name == name) {
            return guestList[mid];
        }
        if (guestList[mid]->name < name) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return nullptr;
}

// Searching - Linear Search untuk mencari tamu berdasarkan nomor kamar
Guest* linearSearchGuestByRoom(int roomNum) {
    for (int i = 0; i < guestCount; i++) {
        if (guestList[i]->roomNumber == roomNum) {
            return guestList[i];
        }
    }
    return nullptr;
}

/* ========== BINARY TREE FUNCTIONS ========== */
// Tree - Insert ke Binary Tree (level order insertion)
void insertToGuestTree(GuestNode* &root, Guest* guest) {
    if (!root) {
        root = new GuestNode(guest);
        return;
    }
    
    // Tree - Level order insertion menggunakan queue
    queue<GuestNode*> q;
    q.push(root);
    
    while (!q.empty()) {
        GuestNode* temp = q.front();
        q.pop();
        
        if (!temp->left) {
            temp->left = new GuestNode(guest);
            return;
        } else {
            q.push(temp->left);
        }
        
        if (!temp->right) {
            temp->right = new GuestNode(guest);
            return;
        } else {
            q.push(temp->right);
        }
    }
}

// Tree - Inorder traversal untuk menampilkan tamu
void displayGuestsInOrder(GuestNode* root) {
    if (root) {
        displayGuestsInOrder(root->left);
        cout << "- " << root->guest->name << " (Telp: " << root->guest->phone << ")";
        if (root->guest->roomNumber != 0) {
            cout << " - Kamar " << root->guest->roomNumber;
        }
        cout << "\n";
        displayGuestsInOrder(root->right);
    }
}

// Tree - Preorder traversal (tambahan)
void displayGuestsPreOrder(GuestNode* root) {
    if (root) {
        cout << "- " << root->guest->name << " (Telp: " << root->guest->phone << ")";
        if (root->guest->roomNumber != 0) {
            cout << " - Kamar " << root->guest->roomNumber;
        }
        cout << "\n";
        displayGuestsPreOrder(root->left);
        displayGuestsPreOrder(root->right);
    }
}

// Tree - Postorder traversal (tambahan)
void displayGuestsPostOrder(GuestNode* root) {
    if (root) {
        displayGuestsPostOrder(root->left);
        displayGuestsPostOrder(root->right);
        cout << "- " << root->guest->name << " (Telp: " << root->guest->phone << ")";
        if (root->guest->roomNumber != 0) {
            cout << " - Kamar " << root->guest->roomNumber;
        }
        cout << "\n";
    }
}

/* ========== INISIALISASI KAMAR ========== */
void initializeRooms() {
    // Pointer - Membuat linked list kamar
    for (int i = 1; i <= STANDARD_ROOMS; i++) {
        Room* newRoom = new Room(i, "Standard", 500000);
        if (!roomList) {
            roomList = newRoom;
        } else {
            Room* temp = roomList;
            while (temp->next) temp = temp->next;
            temp->next = newRoom;
        }
        availableRooms.push(i); // Stack - Push ke stack
    }
    
    for (int i = 51; i <= 80; i++) {
        Room* newRoom = new Room(i, "Deluxe", 750000);
        Room* temp = roomList;
        while (temp->next) temp = temp->next;
        temp->next = newRoom;
        availableRooms.push(i);
    }
    
    for (int i = 81; i <= 95; i++) {
        Room* newRoom = new Room(i, "Suite", 1000000);
        Room* temp = roomList;
        while (temp->next) temp = temp->next;
        temp->next = newRoom;
        availableRooms.push(i);
    }
    
    for (int i = 96; i <= 100; i++) {
        Room* newRoom = new Room(i, "VIP", 1500000);
        Room* temp = roomList;
        while (temp->next) temp = temp->next;
        temp->next = newRoom;
        availableRooms.push(i);
    }
    
    selectionSortRooms(); // Sorting - Urutkan kamar setelah inisialisasi
}

/* ========== FUNGSI MANAJEMEN KAMAR ========== */
// Pointer - Traversal linked list untuk mencari kamar
Room* findRoom(int roomNum) {
    Room* current = roomList;
    while (current) {
        if (current->number == roomNum) return current;
        current = current->next;
    }
    return nullptr;
}

void showAvailableRooms() {
    cout << "\nKAMAR TERSEDIA:\n";
    cout << formatColumn("No.", 8) << formatColumn("Tipe", 15) 
         << formatColumn("Harga", 15) << formatColumn("Status", 15) << "\n";
    cout << string(60, '-') << "\n";
    
    Room* current = roomList;
    while (current) {
        if (!current->occupied) {
            cout << formatColumn(to_string(current->number), 8)
                 << formatColumn(current->type, 15)
                 << formatColumn("Rp " + to_string(current->price), 15)
                 << formatColumn("Tersedia", 15) << "\n";
        }
        current = current->next;
    }
}

void showAllRooms() {
    cout << "\nSEMUA KAMAR:\n";
    cout << formatColumn("No.", 8) << formatColumn("Tipe", 15) 
         << formatColumn("Harga", 15) << formatColumn("Status", 15) << "\n";
    cout << string(60, '-') << "\n";
    
    Room* current = roomList;
    while (current) {
        cout << formatColumn(to_string(current->number), 8)
             << formatColumn(current->type, 15)
             << formatColumn("Rp " + to_string(current->price), 15)
             << formatColumn(current->occupied ? "Terisi" : "Tersedia", 15) << "\n";
        current = current->next;
    }
}

void registerGuest() {
    if (guestCount >= MAX_GUESTS) {
        cout << "Kapasitas tamu penuh!\n";
        return;
    }
    
    guestList[guestCount] = new Guest();
    cout << "Nama Tamu: ";
    getline(cin, guestList[guestCount]->name);
    cout << "Nomor Telepon: ";
    getline(cin, guestList[guestCount]->phone);
    guestList[guestCount]->roomNumber = 0;
    
    insertToGuestTree(guestTree, guestList[guestCount]); // Tree - Insert ke Binary Tree
    guestCount++;
    cout << "Tamu berhasil terdaftar!\n";
}

void bookRoom() {
    if (guestCount == 0) {
        cout << "Belum ada tamu terdaftar!\n";
        return;
    }
    
    showAvailableRooms();
    cout << "Masukkan nomor kamar: ";
    int roomNum;
    cin >> roomNum;
    
    Room* room = findRoom(roomNum); // Pointer - Cari kamar
    if (!room || room->occupied) {
        cout << "Kamar tidak tersedia!\n";
        return;
    }
    
    cout << "Pilih tamu:\n";
    bubbleSortGuestsByName(); // Sorting - Urutkan sebelum menampilkan
    for (int i = 0; i < guestCount; i++) {
        cout << i+1 << ". " << guestList[i]->name;
        if (guestList[i]->roomNumber != 0) {
            cout << " (Sedang menginap di kamar " << guestList[i]->roomNumber << ")";
        }
        cout << "\n";
    }
    
    int guestChoice;
    cin >> guestChoice;
    
    if (guestChoice < 1 || guestChoice > guestCount) {
        cout << "Pilihan tidak valid!\n";
        return;
    }
    
    room->occupied = true;
    guestList[guestChoice-1]->roomNumber = roomNum;
    cout << "Kamar " << roomNum << " berhasil dipesan oleh " 
         << guestList[guestChoice-1]->name << "!\n";
}

void checkOut() {
    cout << "Masukkan nomor kamar: ";
    int roomNum;
    cin >> roomNum;
    
    Room* room = findRoom(roomNum); // Pointer - Cari kamar
    if (!room || !room->occupied) {
        cout << "Kamar tidak terisi!\n";
        return;
    }
    
    room->occupied = false;
    availableRooms.push(roomNum); // Stack - Kembalikan kamar ke stack
    
    Guest* guest = linearSearchGuestByRoom(roomNum); // Searching - Cari tamu
    if (guest) {
        guest->roomNumber = 0;
        cout << "Check-out berhasil untuk kamar " << roomNum << "\n";
    }
}

/* ========== FUNGSI LAYANAN KAMAR ========== */
void requestRoomService() {
    cout << "Masukkan nomor kamar: ";
    int roomNum;
    cin >> roomNum;
    
    Room* room = findRoom(roomNum); // Pointer - Cari kamar
    if (!room || !room->occupied) {
        cout << "Kamar tidak terisi!\n";
        return;
    }
    
    cout << "Pilih layanan:\n";
    cout << "1. Sarapan\n2. Makan Siang\n3. Makan Malam\n";
    cout << "4. Minuman\n5. Kebersihan\n6. Ganti Handuk\nPilihan: ";
    
    int choice;
    cin >> choice;
    
    string services[] = {"Sarapan", "Makan Siang", "Makan Malam", 
                         "Minuman", "Kebersihan", "Ganti Handuk"};
    
    if (choice >= 1 && choice <= 6) {
        string request = "Kamar " + to_string(roomNum) + ": " + services[choice-1];
        
        // Queue - Enqueue permintaan layanan
        if (serviceQueue.count < MAX_SERVICE_REQUESTS) {
            serviceQueue.rear = (serviceQueue.rear + 1) % MAX_SERVICE_REQUESTS;
            serviceQueue.requests[serviceQueue.rear] = request;
            serviceQueue.count++;
            cout << "Permintaan layanan berhasil ditambahkan!\n";
        } else {
            cout << "Antrian layanan penuh!\n";
        }
    } else {
        cout << "Pilihan tidak valid!\n";
    }
}

// Queue - Dequeue dan proses permintaan layanan
void processServiceRequests() {
    if (serviceQueue.count == 0) {
        cout << "Tidak ada permintaan layanan\n";
        return;
    }
    
    cout << "Memproses permintaan layanan:\n";
    while (serviceQueue.count > 0) {
        string request = serviceQueue.requests[serviceQueue.front];
        serviceQueue.front = (serviceQueue.front + 1) % MAX_SERVICE_REQUESTS;
        serviceQueue.count--;
        cout << "-> " << request << " selesai\n";
    }
}

/* ========== FUNGSI PEMBAYARAN ========== */
void processPayment() {
    cout << "Masukkan nomor kamar: ";
    int roomNum;
    cin >> roomNum;
    
    Room* room = findRoom(roomNum); // Pointer - Cari kamar
    if (!room || !room->occupied) {
        cout << "Kamar tidak terisi!\n";
        return;
    }
    
    cout << "Lama menginap (malam): ";
    int nights;
    cin >> nights;
    
    if (nights <= 0) {
        cout << "Input tidak valid!\n";
        return;
    }
    
    int total = room->price * nights;
    cout << "Total pembayaran: Rp " << total << "\n";
    cout << "Pembayaran berhasil!\n";
    
    room->occupied = false;
    availableRooms.push(roomNum); // Stack - Kembalikan kamar ke stack
    
    Guest* guest = linearSearchGuestByRoom(roomNum); // Searching - Cari tamu
    if (guest) {
        guest->roomNumber = 0;
    }
}

/* ========== FUNGSI LAPORAN ========== */
void showSystemReport() {
    int occupied = 0, available = 0;
    Room* current = roomList;
    
    while (current) {
        if (current->occupied) occupied++;
        else available++;
        current = current->next;
    }
    
    cout << "\n=== LAPORAN SISTEM ===\n";
    cout << "Kamar Tersedia: " << available << "\n";
    cout << "Kamar Terisi: " << occupied << "\n";
    cout << "Total Tamu: " << guestCount << "\n";
    
    cout << "\nTamu yang menginap (diurut berdasarkan nama):\n";
    bubbleSortGuestsByName(); // Sorting - Urutkan sebelum menampilkan
    bool anyGuest = false;
    for (int i = 0; i < guestCount; i++) {
        if (guestList[i]->roomNumber != 0) {
            anyGuest = true;
            cout << "- " << guestList[i]->name << " (Kamar " 
                 << guestList[i]->roomNumber << ")\n";
        }
    }
    if (!anyGuest) cout << "Tidak ada tamu yang menginap\n";
    
    cout << "\nDaftar Semua Tamu (Inorder Traversal):\n";
    displayGuestsInOrder(guestTree); // Tree - Tampilkan dalam urutan inorder
    
    cout << "\nPermintaan layanan tertunda: " << serviceQueue.count << "\n";
}

/* ========== MENU UTAMA ========== */
void showMenu() {
    cout << "\nSISTEM MANAJEMEN HOTEL\n";
    cout << "1. Daftar Tamu Baru\n";
    cout << "2. Tampilkan Kamar Tersedia\n";
    cout << "3. Pesan Kamar\n";
    cout << "4. Check-Out\n";
    cout << "5. Minta Layanan Kamar\n";
    cout << "6. Proses Pembayaran\n";
    cout << "7. Tampilkan Semua Kamar\n";
    cout << "8. Proses Permintaan Layanan\n";
    cout << "9. Laporan Sistem\n";
    cout << "10. Cari Tamu (Binary Search)\n";
    cout << "11. Tampilkan Tamu (Preorder)\n";
    cout << "12. Tampilkan Tamu (Postorder)\n";
    cout << "13. Keluar\n";
    cout << "Pilihan: ";
}

void searchGuestMenu() {
    cout << "Masukkan nama tamu: ";
    string name;
    getline(cin, name);
    
    Guest* guest = binarySearchGuest(name); // Searching - Binary search
    if (guest) {
        cout << "Data tamu ditemukan:\n";
        cout << "Nama: " << guest->name << "\n";
        cout << "Telepon: " << guest->phone << "\n";
        if (guest->roomNumber != 0) {
            cout << "Kamar: " << guest->roomNumber << "\n";
        } else {
            cout << "Status: Tidak menginap\n";
        }
    } else {
        cout << "Tamu tidak ditemukan\n";
    }
}

int main() {
    initializeRooms();
    
    int choice;
    string input;
    do {
        showMenu();
        cin >> choice;
        getline(cin, input); // Membersihkan buffer
        
        switch(choice) {
            case 1: registerGuest(); break;
            case 2: showAvailableRooms(); break;
            case 3: bookRoom(); break;
            case 4: checkOut(); break;
            case 5: requestRoomService(); break;
            case 6: processPayment(); break;
            case 7: showAllRooms(); break;
            case 8: processServiceRequests(); break;
            case 9: showSystemReport(); break;
            case 10: searchGuestMenu(); break;
            case 11: 
                cout << "\nDaftar Tamu (Preorder):\n";
                displayGuestsPreOrder(guestTree);
                break;
            case 12: 
                cout << "\nDaftar Tamu (Postorder):\n";
                displayGuestsPostOrder(guestTree);
                break;
            case 13: cout << "Terima kasih!\n"; break;
            default: cout << "Pilihan tidak valid!\n";
        }
        
        if (choice != 13) {
            cout << "\nTekan Enter untuk melanjutkan...";
            getline(cin, input);
        }
    } while (choice != 13);
    
    return 0;
}