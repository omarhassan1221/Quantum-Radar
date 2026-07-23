#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

struct Violation {
    string description;
    int fee;
};

class Vehicle {
protected:
    string plateNumber;
    int speed;
    bool seatbeltFastened;
    string date;
public:
    Vehicle(string plate, int _speed, bool seatbelt, string _date)
        : plateNumber(plate), speed(_speed), seatbeltFastened(seatbelt), date(_date) {
    }

    virtual ~Vehicle() = default;

    virtual int getMaxSpeed() const = 0;
    virtual string getType() const = 0;


    string getPlateNumber() const {
        return plateNumber;
    }

    int getSpeed() const {
        return speed;
    }

    bool isSeatbeltFastened() const {
        return seatbeltFastened;
    }
};

class PrivateCar : public Vehicle {
public:
    using Vehicle::Vehicle;

    int getMaxSpeed() const override {
        return 80;
    }
    string getType() const override {
        return "Private car";
    }
};

class Truck : public Vehicle {
public:
    using Vehicle::Vehicle;

    int getMaxSpeed() const override {
        return 60;
    }
    string getType() const override {
        return "Truck";
    }
};

class Bus : public Vehicle {
public:
    using Vehicle::Vehicle;

    int getMaxSpeed() const override {
        return 50; // assumed
    }
    string getType() const override {
        return "Bus";
    }
};


class Rule {
public:
    virtual ~Rule() = default;
    virtual string getRuleName() const = 0;
    virtual bool checkViolation(const Vehicle* vehicle, Violation& outViolation) const = 0;
};

class SpeedRule : public Rule {
public:
    string getRuleName() const override {
        return "Speed Limit Rule";
    }

    bool checkViolation(const Vehicle* vehicle, Violation& outViolation) const override {
        if (vehicle->getSpeed() > vehicle->getMaxSpeed()) {
            outViolation.description = "speed of " + to_string(vehicle->getSpeed()) +
                " exceeded max allowed " + to_string(vehicle->getMaxSpeed());
            outViolation.fee = 300;
            return true;
        }
        return false;
    }
};

class SeatbeltRule : public Rule {
public:
    string getRuleName() const override {
        return "Seatbelt Rule";
    }

    bool checkViolation(const Vehicle* vehicle, Violation& outViolation) const override {
        if (!vehicle->isSeatbeltFastened()) {
            outViolation.description = "Seatbelt not fastned";
            outViolation.fee = 100;
            return true;
        }
        return false;
    }
};

class Radar {
private:
    vector<Rule*> rules;
    map<string, int> totalFinesPerPlate;
    map<string, int> violatedRulesCount;

public:

    void addRule(Rule* rule) {
        rules.push_back(rule);
    }

    void processObservation(const Vehicle* vehicle) {
        vector<Violation> currentViolations;
        int totalAmount = 0;

        for (Rule* rule : rules) {
            Violation v;
            if (rule->checkViolation(vehicle, v)) {
                currentViolations.push_back(v);
                totalAmount += v.fee;
                violatedRulesCount[rule->getRuleName()]++;
            }
        }

        if (!currentViolations.empty()) {
            totalFinesPerPlate[vehicle->getPlateNumber()] += totalAmount;

            cout << "Traffic fine for " << vehicle->getType() << " " << vehicle->getPlateNumber() << "\n";
            cout << "Total amount: " << totalAmount << " EGP\n";
            cout << "Violations:\n";
            for (const auto& v : currentViolations) {
                cout << "- " << v.description << " : " << v.fee << " EGP\n";
            }
            cout << "\n";
        }
    }

    void printAllFines() const {
        cout << "--- All Fines ---\n";
        for (const auto& pair : totalFinesPerPlate) {
            cout << "Plate: " << pair.first << " | Total Fines: " << pair.second << " EGP\n";
        }
        cout << "\n";
    }

    void printViolatedRulesCount() const {
        cout << "--- Violated Rules Count ---\n";
        for (const auto& pair : violatedRulesCount) {
            cout << pair.first << " : " << pair.second << " times\n";
        }
        cout << "\n";
    }

    ~Radar() {
        for (Rule* rule : rules) {
            delete rule;
        }
    }
};


int main() {
    Radar radar;

    radar.addRule(new SeatbeltRule());
    radar.addRule(new SpeedRule());

    PrivateCar car1("ABC1234", 94, false, "2026-7-23");
    Truck truck1("TRK999", 70, true, "2026-7-23");
    Bus bus1("BUS4321", 85, false, "2026-7-23");
    PrivateCar car2("XYZ567", 60, true, "2026-7-23");


    radar.processObservation(&car1);
    radar.processObservation(&truck1);
    radar.processObservation(&bus1);
    radar.processObservation(&car2);


    radar.printAllFines();
    radar.printViolatedRulesCount();

}
