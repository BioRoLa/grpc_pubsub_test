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

    core::Publisher<config_msg::ConfigStamped> &pub = nh.advertise<config_msg::ConfigStamped>("config/bus");
    
    int seq = 0;

    config_msg::ConfigStamped draft_msg;

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
    std::cout << "--------------------------------------------------------\n";
    std::cout << "Batch Command:\n";
    std::cout << "  C <Mod> <Mot> <R/W> <I/F> <Addr> <Val>\n";
    std::cout << "  Example: C A R W F 25 50.5\n";
    std::cout << "--------------------------------------------------------\n";
    std::cout << "Single Commands:\n";
    std::cout << "  C M <A-D> | C m <R/L> | C d <R/W> | C t <I/F>\n";
    std::cout << "  C a <int> | C v <num>\n";
    std::cout << "--------------------------------------------------------\n";
    std::cout << "Actions:\n";
    std::cout << "  S  - Send Config\n";
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

        if (cmd == "C") 
        {
            std::string arg1;
            iss >> arg1;
            
            if (arg1.empty()) {
                std::cout << "Missing arguments.\n";
                continue;
            }

            char first_char = toupper(arg1[0]);
            bool is_batch_mode = (arg1.length() == 1 && (first_char >= 'A' && first_char <= 'D'));

            if (is_batch_mode) 
            {
                char mod_c = first_char;
                char mot_c, mode_c, type_c;
                int addr_i;
                float val_f;

                if (iss >> mot_c >> mode_c >> type_c >> addr_i >> val_f) 
                {
                    mot_c = toupper(mot_c);
                    mode_c = toupper(mode_c);
                    type_c = toupper(type_c);

                    // 1. Set Module
                    if (mod_c == 'A') draft_msg.set_module(config_msg::MODULE_A);
                    else if (mod_c == 'B') draft_msg.set_module(config_msg::MODULE_B);
                    else if (mod_c == 'C') draft_msg.set_module(config_msg::MODULE_C);
                    else if (mod_c == 'D') draft_msg.set_module(config_msg::MODULE_D);

                    // 2. Set Motor
                    if (mot_c == 'R') draft_msg.set_motor(config_msg::MOTOR_R);
                    else if (mot_c == 'L') draft_msg.set_motor(config_msg::MOTOR_L);

                    // 3. Set Mode
                    if (mode_c == 'R') draft_msg.set_mode(config_msg::READ);
                    else if (mode_c == 'W') draft_msg.set_mode(config_msg::WRITE);

                    // 4. Set Type
                    if (type_c == 'I') draft_msg.set_type(config_msg::INT);
                    else if (type_c == 'F') draft_msg.set_type(config_msg::FLOAT);

                    // 5. Set Address
                    draft_msg.set_address(addr_i);

                    // 6. Set Value
                    if (draft_msg.type() == config_msg::INT) {
                        draft_msg.set_value_i(static_cast<int32_t>(val_f));
                        draft_msg.set_value_f(0);
                    } else {
                        draft_msg.set_value_f(val_f);
                        draft_msg.set_value_i(0);
                    }
                    
                    std::cout << "Batch Config Set.\n";
                } 
                else 
                {
                    std::cout << "Invalid Batch Format. Usage: C <Mod> <Mot> <R/W> <I/F> <Addr> <Val>\n";
                }
            }
            else 
            {
                char subcmd = arg1[0]; 
                
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
                else if (subcmd == 'm') { // Motor
                    std::string val; iss >> val;
                    if(!val.empty()) {
                        char m = toupper(val[0]);
                        if (m=='R') draft_msg.set_motor(config_msg::MOTOR_R);
                        else if (m=='L') draft_msg.set_motor(config_msg::MOTOR_L);
                    }
                }
                else if (subcmd == 'd') { // Direction
                    std::string val; iss >> val;
                    if(!val.empty()) {
                        char d = toupper(val[0]);
                        if (d=='R') draft_msg.set_mode(config_msg::READ);
                        else if (d=='W') draft_msg.set_mode(config_msg::WRITE);
                    }
                }
                else if (subcmd == 't') { // Type
                    std::string val; iss >> val;
                    if(!val.empty()) {
                        char t = toupper(val[0]);
                        if (t=='I') draft_msg.set_type(config_msg::INT);
                        else if (t=='F') draft_msg.set_type(config_msg::FLOAT);
                    }
                }
                else if (subcmd == 'a') { // Address
                    int val; 
                    if(iss >> val) draft_msg.set_address(val);
                }
                else if (subcmd == 'v') { // Value
                    float val;
                    if(iss >> val) {
                        if (draft_msg.type() == config_msg::INT) draft_msg.set_value_i((int)val);
                        else draft_msg.set_value_f(val);
                    }
                }
                else {
                    std::cout << "Unknown command: " << arg1 << "\n";
                }
            }
            print_draft(draft_msg);
        }
        else if (cmd == "S") 
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
             std::cout << "Batch:  C <Mod> <Mot> <R/W> <I/F> <Addr> <Val>\n";
             std::cout << "Single: C <M/m/d/t/a/v> <Arg>\n";
             std::cout << "Action: S (Send)\n";
        }
        else 
        {
            std::cout << "Invalid command.\n";
        }
    }
    return 0;
}