#include <iostream>
#include <Windows.h>
#include <cstdint>
#include <algorithm>

struct Telemetry {
    uint32_t sim_time_start;
    uint16_t speed;
    uint16_t rpm;
    uint8_t gear;
    uint32_t sim_time_end;
};

void printTelemetry(Telemetry telem) {
    std::cout << "====" << std::endl;
    std::cout << "sim_time: " << telem.sim_time_start << std::endl;
    std::cout << "\tspeed: " << telem.speed << std::endl;
    std::cout << "\trpm: " << telem.rpm << std::endl;
    std::cout << "\tgear: " << (int)telem.gear << std::endl;
}

void updateTelemetry(Telemetry &telem, uint32_t sim_time) {
    telem.sim_time_start = sim_time; // add current simulation_time
    int random_acc = rand() % 4 - 2; // <-2,2> random int
    int new_speed = 0;
    if (telem.speed + random_acc < 0) {
        new_speed = telem.speed - random_acc;
    } else {
        new_speed = telem.speed + random_acc;
    }
    telem.speed = (uint16_t)std::clamp(new_speed, 0, 400);
    telem.gear = 1 + (telem.speed / 20); // every 20km/h change the gear
    telem.rpm = (telem.speed % 20) * 200 + 1000; // map speed in gear to rpm
    telem.sim_time_end = sim_time; // add current simulation_time adn the tail for preventing torn reads
}

int main() {
    uint32_t sim_time = 0; // simulation time

    // car telemetry
    Telemetry telem = {
        .speed = 50, // initial speed
    };

    updateTelemetry(telem, sim_time);

    std::cout << "[Starting target app]" << std::endl;
    std::cout << "PID: " << GetCurrentProcessId() << std::endl;
    std::cout << "Telemetry address: " << &telem << std::endl;
    std::cout << std::endl;

    while (true) {
        // increment simulation time
        ++sim_time;

        // update telemetry
        updateTelemetry(telem, sim_time);

        // print telemetry data to stdout
        printTelemetry(telem);

        // wait a sec
        Sleep(500);

        // sanity check
        if (sim_time == INT_MAX) {
            std::cout << "End of time reached." << std::endl;
            break;
        }
    }

    return 0;
}