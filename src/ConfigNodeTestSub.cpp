#include "NodeHandler.h"
#include "Config.pb.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <mutex>

std::mutex mutex_;

std::string moduleToString(config_msg::Module mod) {
    switch(mod) {
        case config_msg::MODULE_A: return "A";
        case config_msg::MODULE_B: return "B";
        case config_msg::MODULE_C: return "C";
        case config_msg::MODULE_D: return "D";
        default: return "?";
    }
}

std::string motorToString(config_msg::Motor mot) {
    switch(mot) {
        case config_msg::MOTOR_R: return "R";
        case config_msg::MOTOR_L: return "L";
        default: return "?";
    }
}

std::string modeToString(config_msg::ConfigMode mode) {
    return (mode == config_msg::READ) ? "READ" : "WRITE";
}

std::string typeToString(config_msg::ConfigType type) {
    return (type == config_msg::INT) ? "INT" : "FLOAT";
}

void cb(config_msg::ConfigStamped m) 
{
    mutex_.lock();

    std::cout << "------------------------------------------------\n";
    std::cout << "Seq: " << m.header().seq() << " | ";
    std::cout << "Time: " << m.header().stamp().sec() << "." 
              << std::setfill('0') << std::setw(6) << m.header().stamp().usec() << "\n";
    std::cout << "Target: [" << moduleToString(m.module()) << "][" << motorToString(m.motor()) << "] ";
    std::cout << "Mode: " << modeToString(m.mode()) << "\n";
    std::cout << "Addr: " << m.address() << " (" << typeToString(m.type()) << ")\n";

    if (m.type() == config_msg::INT) {
        std::cout << "Value: " << m.value_i() << " (Int)\n";
    } else {
        std::cout << "Value: " << std::fixed << std::setprecision(4) << m.value_f() << " (Float)\n";
    }

    std::cout << "State: " << (m.transmit() ? "BUSY (Transmit=1)" : "DONE (Transmit=0)") << " | ";
    
    if (m.error_code() == 0) {
        std::cout << "Error: OK (0)\n";
    } else {
        std::cout << "Error: CODE " << m.error_code() << "\n";
    }

    mutex_.unlock();
}

int main() {
    core::NodeHandler nh;
    core::Rate rate(1000); 

    core::Subscriber<config_msg::ConfigStamped> &sub = nh.subscribe<config_msg::ConfigStamped>("motor/config", 1000, cb, 10);
    
    std::cout << "Config Node Test Subscriber Started. Listening on 'motor/config'...\n";

    while (1)
    {
        core::spinOnce();
        rate.sleep();
    }
    return 0;
}