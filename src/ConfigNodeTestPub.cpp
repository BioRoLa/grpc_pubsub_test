#include "NodeHandler.h"
#include "Config.pb.h"
#include <sstream>
#include <string>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <algorithm> 

void print_draft(const config_msg::ConfigStamped& msg) 
{
    std::cout << "Draft: [ ";
    
    // Module
    switch(msg.module()) {
        case config_msg::MODULE_A: std::cout << "Mod:A "; break;
        case config_msg::MODULE_B: std::cout << "Mod:B "; break;
        case config_msg::MODULE_C: std::cout << "Mod:C "; break;
        case config_msg::MODULE_D: std::cout << "Mod:D "; break;
        default: std::cout << "Mod:? "; break;
    }

    // Motor
    switch(msg.motor()) {
        case config_msg::MOTOR_R: std::cout << "Mot:R "; break;
        case config_msg::MOTOR_L: std::cout << "Mot:L "; break;
        default: std::cout << "Mot:? "; break;
    }

    // Mode
    if (msg.mode() == config_msg::READ) std::cout << "Mode:R ";
    else std::cout << "Mode:W ";

    // Type & Address
    if (msg.type() == config_msg::INT) std::cout << "Type:I ";
    else std::cout << "Type:F ";
    
    std::cout << "Addr:" << msg.address() << " ";

    // Value
    if (msg.type() == config_msg::INT) {
        std::cout << "Val:" << msg.value_i();
    } else {
        std::cout << "Val:" << std::fixed << std::setprecision(4) << msg.value_f();
    }
    std::cout << " ]" << std::endl;
}

int main() 
{
    core::NodeHandler nh;
    core::Rate rate(1000);

    core::Publisher<config_msg::ConfigStamped> &pub = nh.advertise<config_msg::ConfigStamped>("motor/config", 1000);
    
    int seq = 1;

    config_msg::ConfigStamped draft_msg;

    // Initial Defaults
    draft_msg.set_module(config_msg::MODULE_A);
    draft_msg.set_motor(config_msg::MOTOR_R);
    draft_msg.set_mode(config_msg::READ);
    draft_msg.set_type(config_msg::FLOAT);
    draft_msg.set_address(0);
    draft_msg.set_value_f(0.0f);
    draft_msg.set_value_i(0);
    draft_msg.set_transmit(false);
    draft_msg.set_error_code(0);

    std::cout << "Config Node Test Publisher Started.\n";
    std::cout << "--------------------------------------------------------------\n";
    std::cout << "Batch Mode (Start with B):\n";
    std::cout << "  B <Mod> <Mot> <R/W> <I/F> <Addr> <Val>\n";
    std::cout << "  Example: B A R W F 25 50.5\n";
    std::cout << "--------------------------------------------------------------\n";
    std::cout << "Single Mode (Start with S):\n";
    std::cout << "  S <SubCmd> <Arg>\n";
    std::cout << "  SubCmds: M(Mod), R(Mot), D(Mode), T(Type), A(Addr), V(Val)\n";
    std::cout << "  Example: S M A  |  S A 10  |  S V 50.5\n";
    std::cout << "--------------------------------------------------------------\n";
    std::cout << "Actions:\n";
    std::cout << "  T  - Transmit (Send Config)\n";
    std::cout << "  P  - Print Draft\n";
    std::cout << "  H  - Help\n";

    print_draft(draft_msg);

    while (1) 
    {
        std::string line;
        std::cout << "> ";
        std::getline(std::cin, line);
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;
        
        if (cmd.empty()) continue;
        std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

        // ---------------------------------------------------------
        // B: Batch Mode
        // ---------------------------------------------------------
        if (cmd == "B") 
        {
            char module, motor, mode, type;
            int addr;
            float value;

            if (iss >> module >> motor >> mode >> type >> addr >> value) 
            {
                module = toupper(module);
                motor = toupper(motor);
                mode = toupper(mode);
                type = toupper(type);

                // 1. Set Module
                if (module == 'A') draft_msg.set_module(config_msg::MODULE_A);
                else if (module == 'B') draft_msg.set_module(config_msg::MODULE_B);
                else if (module == 'C') draft_msg.set_module(config_msg::MODULE_C);
                else if (module == 'D') draft_msg.set_module(config_msg::MODULE_D);

                // 2. Set Motor
                if (motor == 'R') draft_msg.set_motor(config_msg::MOTOR_R);
                else if (motor == 'L') draft_msg.set_motor(config_msg::MOTOR_L);

                // 3. Set Mode
                if (mode == 'R') draft_msg.set_mode(config_msg::READ);
                else if (mode == 'W') draft_msg.set_mode(config_msg::WRITE);

                // 4. Set Type
                if (type == 'I') draft_msg.set_type(config_msg::INT);
                else if (type == 'F') draft_msg.set_type(config_msg::FLOAT);

                // 5. Set Address
                draft_msg.set_address(addr);

                // 6. Set Value
                if (draft_msg.type() == config_msg::INT) {
                    draft_msg.set_value_i(static_cast<int32_t>(value));
                    draft_msg.set_value_f(0);
                } else {
                    draft_msg.set_value_f(value);
                    draft_msg.set_value_i(0);
                }
                
                std::cout << "Batch Config Set.\n";
            } 
            else 
            {
                std::cout << "Invalid Batch Format. Usage: B <Mod> <Mot> <R/W> <I/F> <Addr> <Val>\n";
            }
            print_draft(draft_msg);
        }
        // ---------------------------------------------------------
        // S: Single Mode
        // ---------------------------------------------------------
        else if (cmd == "S") 
        {
            std::string arg1;
            iss >> arg1;
            
            if (!arg1.empty()) 
            {
                char subcmd = toupper(arg1[0]);

                if (subcmd == 'M') { // Module
                    std::string val; iss >> val; 
                    if(!val.empty()) {
                        char m = toupper(val[0]);
                        if (m=='A') draft_msg.set_module(config_msg::MODULE_A);
                        else if (m=='B') draft_msg.set_module(config_msg::MODULE_B);
                        else if (m=='C') draft_msg.set_module(config_msg::MODULE_C);
                        else if (m=='D') draft_msg.set_module(config_msg::MODULE_D);
                    }
                }
                else if (subcmd == 'R') { // Motor (R/L)
                    std::string val; iss >> val;
                    if(!val.empty()) {
                        char m = toupper(val[0]);
                        if (m=='R') draft_msg.set_motor(config_msg::MOTOR_R);
                        else if (m=='L') draft_msg.set_motor(config_msg::MOTOR_L);
                    }
                }
                else if (subcmd == 'D') { // Direction / Mode (R/W)
                    std::string val; iss >> val;
                    if(!val.empty()) {
                        char d = toupper(val[0]);
                        if (d=='R') draft_msg.set_mode(config_msg::READ);
                        else if (d=='W') draft_msg.set_mode(config_msg::WRITE);
                    }
                }
                else if (subcmd == 'T') { // Type (I/F)
                    std::string val; iss >> val;
                    if(!val.empty()) {
                        char t = toupper(val[0]);
                        if (t=='I') draft_msg.set_type(config_msg::INT);
                        else if (t=='F') draft_msg.set_type(config_msg::FLOAT);
                    }
                }
                else if (subcmd == 'A') { // Address
                    int val; 
                    if(iss >> val) draft_msg.set_address(val);
                }
                else if (subcmd == 'V') { // Value
                    float val;
                    if(iss >> val) {
                        if (draft_msg.type() == config_msg::INT) draft_msg.set_value_i((int)val);
                        else draft_msg.set_value_f(val);
                    }
                }
                else {
                    std::cout << "Unknown Single Command: " << arg1 << "\n";
                }
            }
            else
            {
                std::cout << "Missing Sub-Command. Usage: S <SubCmd> <Arg>\n";
            }
            print_draft(draft_msg);
        }
        // ---------------------------------------------------------
        // T: Transmit (Send)
        // ---------------------------------------------------------
        else if (cmd == "T") 
        {
            auto* header = draft_msg.mutable_header();
            header->set_seq(seq++);
            auto now = std::chrono::system_clock::now();
            auto epoch = now.time_since_epoch();
            header->mutable_stamp()->set_sec(static_cast<int32_t>(std::chrono::duration_cast<std::chrono::seconds>(epoch).count()));
            header->mutable_stamp()->set_usec(static_cast<int32_t>(std::chrono::duration_cast<std::chrono::microseconds>(epoch).count() % 1000000));

            draft_msg.set_transmit(true);

            pub.publish(draft_msg);
            std::cout << "Config SENT! (Seq: " << seq-1 << ")\n";
            
            draft_msg.set_transmit(false);
        }
        else if (cmd == "H") 
        {
             std::cout << "Batch:  B <Mod> <Mot> <R/W> <I/F> <Addr> <Val>\n";
             std::cout << "Single: S <M/R/D/T/A/V> <Arg>\n";
             std::cout << "Action: T (Transmit/Send)\n";
        }
        else if (cmd == "P") 
        {
            print_draft(draft_msg);
        }
        else 
        {
            std::cout << "Invalid command.\n";
        }
    }
    return 0;
}