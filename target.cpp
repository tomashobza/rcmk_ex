#include <iostream>
#include <Windows.h>
#include <cstdint>
#include <algorithm>

struct Telemetry {
    uint16_t speed;
    uint16_t rpm;
    uint8_t gear;
};

void printTelemetry(Telemetry telem, int sim_time) {
    std::cout << "====" << std::endl;
    std::cout << "sim_time: " << sim_time << std::endl;
    std::cout << "\tspeed: " << telem.speed << std::endl;
    std::cout << "\trpm: " << telem.rpm << std::endl;
    std::cout << "\tgear: " << (int)telem.gear << std::endl;
}

void updateTelemetry(Telemetry &telem) {
    int random_acc = rand() % 4 - 2; // <-2,2> random int
    int new_speed = (int)telem.speed + random_acc;
    telem.speed = (uint16_t)std::clamp(new_speed, 0, 400);
    telem.gear = 1 + (telem.speed / 20); // every 20km/h change the gear
    telem.rpm = (telem.speed % 20) * 300; // map speed in gear to rpm
}

int main() {
    // car telemetry
    Telemetry telem = {
        .speed = 50,
    };

    updateTelemetry(telem);

    std::cout << "[Starting target app]" << std::endl;
    std::cout << "PID: " << GetCurrentProcessId() << std::endl;
    std::cout << "Telemetry address: " << &telem << std::endl;
    std::cout << std::endl;

    int sim_time = 0; // simulation time

    while (true) {
        // increment simulation time
        ++sim_time;

        // update telemetry
        updateTelemetry(telem);

        printTelemetry(telem, sim_time);

        Sleep(500);

        if (sim_time == INT_MAX) {
            std::cout << "End of time reached." << std::endl;
            break;
        }
    }

    return 0;
}