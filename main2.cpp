#include <iostream>
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <iomanip>

struct Instruction {
    std::string opcode;
    int size;
};

void initializeOpcodeTable(std::unordered_map<std::string, Instruction>& table) {
    table["MOV A,B"]  = {"78", 1};
    table["MOV A,C"]  = {"79", 1};
    table["MOV A,D"]  = {"7A", 1};
    table["MOV A,E"]  = {"7B", 1};
    table["MOV A,H"]  = {"7C", 1};
    table["MOV A,L"]  = {"7D", 1};
    table["MOV B,A"]  = {"47", 1};
    table["MOV C,A"]  = {"4F", 1};
    table["MOV D,A"]  = {"57", 1};
    table["MOV E,A"]  = {"5F", 1};
    table["MOV H,A"]  = {"67", 1};
    table["MOV L,A"]  = {"6F", 1};
    table["MVI A,"]   = {"3E", 2};
    table["MVI B,"]   = {"06", 2};
    table["MVI C,"]   = {"0E", 2};
    table["MVI D,"]   = {"16", 2};
    table["MVI E,"]   = {"1E", 2};
    table["MVI H,"]   = {"26", 2};
    table["MVI L,"]   = {"2E", 2};
    table["MOV M,A"]  = {"77", 1};
    table["MOV A,M"]  = {"7E", 1};
    table["ADD B"]    = {"80", 1};
    table["ADD C"]    = {"81", 1};
    table["ADD D"]    = {"82", 1};
    table["ADD E"]    = {"83", 1};
    table["ADD H"]    = {"84", 1};
    table["ADD L"]    = {"85", 1};
    table["ADD M"]    = {"86", 1};
    table["ADD A"]    = {"87", 1};
    table["SUB B"]    = {"90", 1};
    table["SUB C"]    = {"91", 1};
    table["SUB D"]    = {"92", 1};
    table["SUB E"]    = {"93", 1};
    table["SUB H"]    = {"94", 1};
    table["SUB L"]    = {"95", 1};
    table["SUB M"]    = {"96", 1};
    table["SUB A"]    = {"97", 1};
    table["INR A"]    = {"3C", 1};
    table["INR B"]    = {"04", 1};
    table["INR C"]    = {"0C", 1};
    table["INR D"]    = {"14", 1};
    table["INR E"]    = {"1C", 1};
    table["INR H"]    = {"24", 1};
    table["INR L"]    = {"2C", 1};
    table["INR M"]    = {"34", 1};
    table["DCR A"]    = {"3D", 1};
    table["DCR B"]    = {"05", 1};
    table["DCR C"]    = {"0D", 1};
    table["DCR D"]    = {"15", 1};
    table["DCR E"]    = {"1D", 1};
    table["DCR H"]    = {"25", 1};
    table["DCR L"]    = {"2D", 1};
    table["DCR M"]    = {"35", 1};
    table["JMP"]      = {"C3", 3};
    table["JC"]       = {"DA", 3};
    table["JZ"]       = {"CA", 3};
    table["JNZ"]      = {"C2", 3};
    table["JM"]       = {"FA", 3};
    table["JP"]       = {"F2", 3};
    table["CALL"]     = {"CD", 3};
    table["CC"]       = {"DC", 3};
    table["CZ"]       = {"D4", 3};
    table["CNZ"]      = {"C4", 3};
    table["CM"]       = {"FC", 3};
    table["CP"]       = {"F4", 3};
    table["RET"]      = {"C9", 1};
    table["RZ"]       = {"C8", 1};
    table["RNZ"]      = {"C0", 1};
    table["RM"]       = {"F8", 1};
    table["RP"]       = {"F0", 1};
    table["PCHL"]     = {"E9", 1};
    table["RIM"]      = {"20", 1};
    table["SIM"]      = {"30", 1};
    table["LDA"]      = {"3A", 3};
    table["STA"]      = {"32", 3};
    table["LDAX"]     = {"00", 3};
    table["STAX"]     = {"02", 3};
    table["LHLD"]     = {"2A", 3};
    table["SHLD"]     = {"22", 3};
    table["XCHG"]     = {"EB", 1};
    table["INX B"]    = {"03", 1};
    table["INX D"]    = {"13", 1};
    table["INX H"]    = {"23", 1};
    table["DCX B"]    = {"0B", 1};
    table["DCX D"]    = {"1B", 1};
    table["DCX H"]    = {"2B", 1};
    table["NOP"]      = {"00", 1};
    table["HLT"]      = {"76", 1};
    table["DI"]       = {"F3", 1};
    table["EI"]       = {"FB", 1};
}

Instruction getOpcode(const std::string& mnemonic, const std::unordered_map<std::string, Instruction>& table) {
    auto it = table.find(mnemonic);
    if (it != table.end()) return it->second;
    throw std::runtime_error("Invalid instruction: " + mnemonic);
}

std::string parseLine(const std::string& line, const std::unordered_map<std::string, Instruction>& table) {
    std::string trimmedLine = line.substr(0, line.find(';'));
    trimmedLine.erase(0, trimmedLine.find_first_not_of(" \t"));
    trimmedLine.erase(trimmedLine.find_last_not_of(" \t") + 1);
    if (trimmedLine.empty()) return "";

    std::istringstream iss(trimmedLine);
    std::string mnemonic;
    iss >> mnemonic;

    std::string operandPart;
    std::getline(iss, operandPart);
    operandPart.erase(0, operandPart.find_first_not_of(" \t"));

    std::vector<std::string> operands;
    if (!operandPart.empty()) {
        size_t commapos = operandPart.find(',');
        if (commapos != std::string::npos) {
            operands.push_back(operandPart.substr(0, commapos));
            operands.push_back(operandPart.substr(commapos + 1));
        } else {
            operands.push_back(operandPart);
        }
        for (auto& op : operands) {
            op.erase(0, op.find_first_not_of(" \t"));
            op.erase(op.find_last_not_of(" \t") + 1);
        }
    }

    std::string fullMnemonic = mnemonic;
    if (!operands.empty()) {
        fullMnemonic += " " + operands[0];
        if (operands.size() > 1) {
            fullMnemonic += "," + operands[1];
        }
    }

    std::string lookupMnemonic;
    if ((mnemonic == "MVI") && operands.size() >= 1) {
        lookupMnemonic = mnemonic + " " + operands[0] + ",";
    } else if ((mnemonic == "LDA" || mnemonic == "STA" || mnemonic == "JMP" ||
                mnemonic == "LHLD" || mnemonic == "SHLD" || mnemonic == "CALL" ||
                mnemonic == "JC" || mnemonic == "JZ" || mnemonic == "JNZ" ||
                mnemonic == "JM" || mnemonic == "JP" || mnemonic == "CC" ||
                mnemonic == "CZ" || mnemonic == "CNZ" || mnemonic == "CM" ||
                mnemonic == "CP")) {
        lookupMnemonic = mnemonic;
    } else {
        lookupMnemonic = fullMnemonic;
    }

    Instruction instr = getOpcode(lookupMnemonic, table);
    std::string machinecode = instr.opcode;

    if (instr.size > 1 && !operands.empty()) {
        std::string processedOp = operands.back();
        if (!processedOp.empty() && processedOp.back() == 'H') {
            processedOp.pop_back();
        }
        std::stringstream ss;
        ss << std::hex << std::uppercase;
        if (instr.size == 2) {
            int value = std::stoi(processedOp, nullptr, 16);
            ss << std::setw(2) << std::setfill('0') << value;
            machinecode += " " + ss.str();
        } else if (instr.size == 3) {
            int address = std::stoi(processedOp, nullptr, 16);
            int lowbyte = address & 0xFF;
            int highbyte = (address >> 8) & 0xFF;
            ss << std::setw(2) << std::setfill('0') << lowbyte;
            machinecode += " " + ss.str();
            ss.str(""); ss.clear();
            ss << std::setw(2) << std::setfill('0') << highbyte;
            machinecode += " " + ss.str();
        }
    }
    return machinecode;
}

int main() {
    std::unordered_map<std::string, Instruction> opcodeTable;
    initializeOpcodeTable(opcodeTable);
    std::ofstream outfile("output.hex");

    std::string line;
    std::cout << "Enter assembly instructions (type 'exit' to quit):" << std::endl;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, line);
        if (line == "exit" || line == "quit") break;

        size_t semicolonPos = line.find(';');
        std::string checkLine = line.substr(0, semicolonPos);
        int instructionCount = std::count(checkLine.begin(), checkLine.end(), ';') + 1;

        // Check if multiple instructions are separated by ';'
        if (instructionCount > 1) {
            std::cout << "Invalid input: multiple instructions at once" << std::endl;
            continue;
        }

        try {
            std::string machineCode = parseLine(line, opcodeTable);
            if (!machineCode.empty()) {
                std::cout << "Machine code: " << machineCode << std::endl;
                outfile << machineCode << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }

    outfile.close();
    return 0;
}
