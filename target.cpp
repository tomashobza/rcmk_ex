#include <iostream>
#include <Windows.h>

struct Telemetry {
    uint16_t speed;
    uint16_t rpm;
    uint8_t gear;
};

void printTelemetry(Telemetry telem, int sim_time) {
    std::cout << "sim_time: " << sim_time << std::endl;
}

int main() {
    // car telemetry
    Telemetry telem = {
        .speed = 130.0f,
    };

    std::cout << "[Starting target app]" << std::endl;

    int sim_time = 0; // simulation time

    while (true) {
        // increment simulation time
        ++sim_time;

        // update telemetry
        int random_acc = rand() % 4 - 2; // <-2,2> random int
        telem.speed += random_acc; // change speed
        telem.gear = 1 + (telem.speed / 20); // every 20km/h change the gear
        telem.rpm = (telem.speed % 20) * 300; // map speed in gear to rpm

        printTelemetry(telem, sim_time);

        Sleep(500);

        if (sim_time == INT_MAX) {
            std::cout << "End of time reached." << std::endl;
            break;
        }
    }

    return 0;
}