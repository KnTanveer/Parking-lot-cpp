
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <iomanip>
#include <limits>
#include <cctype>
#include <cstdlib>

using namespace std;

const int TOTAL_SLOTS = 7;
const double RATE_PER_HOUR = 30.0;

struct ParkingSlot {
    int id;
    string vehicleNumber;
    time_t entryTime;
    time_t expiryTime;
    double fee;
    bool occupied;
};

ParkingSlot slots[TOTAL_SLOTS];
double revenue = 0.0;

void clearScreen() {
    system("clear");
}

void pauseScreen() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void initializeSlots() {
    for (int i = 0; i < TOTAL_SLOTS; i++) {
        slots[i].id = i + 1;
        slots[i].vehicleNumber = "";
        slots[i].entryTime = 0;
        slots[i].expiryTime = 0;
        slots[i].fee = 0.0;
        slots[i].occupied = false;
    }
}

void saveData() {
    ofstream file("parking.txt");

    for (const auto& slot : slots) {
        file << slot.id << '\n';
        file << slot.vehicleNumber << '\n';
        file << slot.entryTime << '\n';
        file << slot.expiryTime << '\n';
        file << slot.fee << '\n';
        file << slot.occupied << '\n';
    }

    file.close();

    ofstream revenueFile("revenue.txt");
    revenueFile << fixed << setprecision(2) << revenue;
    revenueFile.close();
}

void loadData() {
    ifstream file("parking.txt");

    if (file.is_open()) {
        for (int i = 0; i < TOTAL_SLOTS; i++) {
            if (!(file >> slots[i].id))
                break;

            file.ignore(numeric_limits<streamsize>::max(), '\n');
            getline(file, slots[i].vehicleNumber);

            if (!(file >> slots[i].entryTime
                    >> slots[i].expiryTime
                    >> slots[i].fee
                    >> slots[i].occupied)) {
                initializeSlots();
                break;
            }

            file.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        file.close();
    }

    ifstream revenueFile("revenue.txt");

    if (revenueFile.is_open()) {
        revenueFile >> revenue;
        revenueFile.close();
    }
}

void checkExpiredSlots() {
    time_t currentTime = time(nullptr);
    bool changed = false;

    for (int i = 0; i < TOTAL_SLOTS; i++) {
        if (slots[i].occupied &&
            currentTime >= slots[i].expiryTime) {

            cout << "\nSlot " << slots[i].id
                << " expired for vehicle "
                << slots[i].vehicleNumber << ".\n";

            slots[i].vehicleNumber = "";
            slots[i].entryTime = 0;
            slots[i].expiryTime = 0;
            slots[i].fee = 0.0;
            slots[i].occupied = false;

            changed = true;
        }
    }

    if (changed)
        saveData();
}

void displaySlots() {
    clearScreen();
    checkExpiredSlots();

    cout << "\n========== PARKING SLOTS ==========\n";

    for (const auto& slot : slots) {
        cout << "Slot " << slot.id << " : ";

        if (slot.occupied) {
            time_t remaining = slot.expiryTime - time(nullptr);

            cout << "OCCUPIED"
                << " | Vehicle: " << slot.vehicleNumber
                << " | Fee: Rs. " << fixed
                << setprecision(2) << slot.fee
                << " | Remaining: "
                << remaining / 60 << " min\n";
        } else {
            cout << "EMPTY\n";
        }
    }

    cout << "===================================\n";
    pauseScreen();
}

void parkVehicle() {
    clearScreen();
    checkExpiredSlots();

    int slotNumber;
    int duration;

    cout << "\n========== PARK A VEHICLE ==========\n";
    cout << "Enter slot number (1-7): ";

    if (!(cin >> slotNumber)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input.\n";
        pauseScreen();
        return;
    }

    if (slotNumber < 1 || slotNumber > TOTAL_SLOTS) {
        cout << "Invalid slot number.\n";
        pauseScreen();
        return;
    }

    ParkingSlot& slot = slots[slotNumber - 1];

    if (slot.occupied) {
        cout << "This slot is already occupied.\n";
        pauseScreen();
        return;
    }

    string vehicle;

    cout << "Enter vehicle registration number: ";
    cin >> vehicle;

    for (char& ch : vehicle)
        ch = toupper(static_cast<unsigned char>(ch));

    for (const auto& existing : slots) {
        if (existing.occupied &&
            existing.vehicleNumber == vehicle) {
            cout << "This vehicle is already parked.\n";
            pauseScreen();
            return;
        }
    }

    cout << "Enter parking duration in hours (1-24): ";

    if (!(cin >> duration)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid duration.\n";
        pauseScreen();
        return;
    }

    if (duration < 1 || duration > 24) {
        cout << "Duration must be between 1 and 24 hours.\n";
        pauseScreen();
        return;
    }

    time_t currentTime = time(nullptr);

    slot.vehicleNumber = vehicle;
    slot.entryTime = currentTime;
    slot.expiryTime = currentTime + duration * 3600;
    slot.fee = duration * RATE_PER_HOUR;
    slot.occupied = true;

    revenue += slot.fee;

    saveData();

    cout << "\nVehicle parked successfully!\n";
    cout << "Slot: " << slot.id << '\n';
    cout << "Vehicle: " << slot.vehicleNumber << '\n';
    cout << "Duration: " << duration << " hours\n";
    cout << "Parking fee: Rs. " << fixed << setprecision(2) << slot.fee << '\n';
    cout << "Expiry time: " << put_time(localtime(&slot.expiryTime), "%d-%m-%Y %I:%M %p") << '\n';

    pauseScreen();
}

void viewRevenue() {
    clearScreen();

    cout << "\n========== REVENUE ==========\n";
    cout << "Total collected: Rs. " << fixed << setprecision(2) << revenue << '\n';
    cout << "=============================\n";

    pauseScreen();
}

void removeVehicle() {
    clearScreen();
    checkExpiredSlots();

    int slotNumber;

    cout << "\n========== REMOVE VEHICLE ==========\n";
    cout << "Enter slot number (1-7): ";

    if (!(cin >> slotNumber)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input.\n";
        pauseScreen();
        return;
    }

    if (slotNumber < 1 || slotNumber > TOTAL_SLOTS) {
        cout << "Invalid slot number.\n";
        pauseScreen();
        return;
    }

    ParkingSlot& slot = slots[slotNumber - 1];

    if (!slot.occupied) {
        cout << "This slot is already empty.\n";
        pauseScreen();
        return;
    }

    cout << "Vehicle " << slot.vehicleNumber << " removed from slot " << slot.id << ".\n";

    slot.vehicleNumber = "";
    slot.entryTime = 0;
    slot.expiryTime = 0;
    slot.fee = 0.0;
    slot.occupied = false;

    saveData();
    pauseScreen();
}

int main() {
    initializeSlots();
    loadData();
    checkExpiredSlots();

    int choice;

    do {
        clearScreen();

        cout << "\n=================================\n";
        cout << "     CAR PARKING MANAGEMENT\n";
        cout << "=================================\n";
        cout << "1. View parking slots\n";
        cout << "2. Park a vehicle\n";
        cout << "3. Remove a vehicle\n";
        cout << "4. View total revenue\n";
        cout << "5. Exit\n";
        cout << "=================================\n";
        cout << "Enter your choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\nPlease enter a valid number.\n";
            pauseScreen();
            continue;
        }

        switch (choice) {
            case 1:
                displaySlots();
                break;

            case 2:
                parkVehicle();
                break;

            case 3:
                removeVehicle();
                break;

            case 4:
                viewRevenue();
                break;

            case 5:
                saveData();
                cout << "\nData saved.\n";
                break;

            default:
                cout << "\nInvalid choice. Try again.\n";
                pauseScreen();
        }

    } while (choice != 5);

    return 0;
}
