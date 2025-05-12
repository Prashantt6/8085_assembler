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
    table = {
        {"MOV A,B", {"78", 1}}, {"MOV A,C", {"79", 1}}, {"MOV A,D", {"7A", 1}},
        {"MOV A,E", {"7B", 1}}, {"MOV A,H", {"7C", 1}}, {"MOV A,L", {"7D", 1}},
        {"MOV B,A", {"47", 1}}, {"MOV C,A", {"4F", 1}}, {"MOV D,A", {"57", 1}},
        {"MOV E,A", {"5F", 1}}, {"MOV H,A", {"67", 1}}, {"MOV L,A", {"6F", 1}},
        {"MOV M,A", {"77", 1}}, {"MOV A,M", {"7E", 1}},

        {"MVI A,", {"3E", 2}}, {"MVI B,", {"06", 2}}, {"MVI C,", {"0E", 2}},
        {"MVI D,", {"16", 2}}, {"MVI E,", {"1E", 2}}, {"MVI H,", {"26", 2}},
        {"MVI L,", {"2E", 2}},

        {"ADD A", {"87", 1}}, {"ADD B", {"80", 1}}, {"ADD C", {"81", 1}},
        {"ADD D", {"82", 1}}, {"ADD E", {"83", 1}}, {"ADD H", {"84", 1}},
        {"ADD L", {"85", 1}}, {"ADD M", {"86", 1}},

        {"SUB A", {"97", 1}}, {"SUB B", {"90", 1}}, {"SUB C", {"91", 1}},
        {"SUB D", {"92", 1}}, {"SUB E", {"93", 1}}, {"SUB H", {"94", 1}},
        {"SUB L", {"95", 1}}, {"SUB M", {"96", 1}},

        {"INR A", {"3C", 1}}, {"INR B", {"04", 1}}, {"INR C", {"0C", 1}},
        {"INR D", {"14", 1}}, {"INR E", {"1C", 1}}, {"INR H", {"24", 1}},
        {"INR L", {"2C", 1}}, {"INR M", {"34", 1}},

        {"DCR A", {"3D", 1}}, {"DCR B", {"05", 1}}, {"DCR C", {"0D", 1}},
        {"DCR D", {"15", 1}}, {"DCR E", {"1D", 1}}, {"DCR H", {"25", 1}},
        {"DCR L", {"2D", 1}}, {"DCR M", {"35", 1}},

        {"JMP", {"C3", 3}}, {"JC", {"DA", 3}}, {"JZ", {"CA", 3}},
        {"JNZ", {"C2", 3}}, {"JM", {"FA", 3}}, {"JP", {"F2", 3}},
        {"JNC", {"D2", 3}},

        {"CALL", {"CD", 3}}, {"CC", {"DC", 3}}, {"CZ", {"D4", 3}},
        {"CNZ", {"C4", 3}}, {"CM", {"FC", 3}}, {"CP", {"F4", 3}},

        {"RET", {"C9", 1}}, {"RZ", {"C8", 1}}, {"RNZ", {"C0", 1}},
        {"RM", {"F8", 1}}, {"RP", {"F0", 1}},

        {"PCHL", {"E9", 1}}, {"RIM", {"20", 1}}, {"SIM", {"30", 1}},

        {"LDA", {"3A", 3}}, {"STA", {"32", 3}}, {"LHLD", {"2A", 3}}, {"SHLD", {"22", 3}},

        {"XCHG", {"EB", 1}},
        {"INX B", {"03", 1}}, {"INX D", {"13", 1}}, {"INX H", {"23", 1}},
        {"DCX B", {"0B", 1}}, {"DCX D", {"1B", 1}}, {"DCX H", {"2B", 1}},

        {"NOP", {"00", 1}}, {"HLT", {"76", 1}},
        {"DI", {"F3", 1}}, {"EI", {"FB", 1}}
    };
}

Instruction getOpcode(const std::string& mnemonic, const std::unordered_map<std::string, Instruction>& table) {
    auto it = table.find(mnemonic);
    if (it != table.end()) return it->second;
    throw std::runtime_error("Invalid instruction: " + mnemonic);
}

std::string parseLine(const std::string& line, const std::unordered_map<std::string, Instruction>& table, const std::unordered_map<std::string, int>& labelTable) {
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
    if (mnemonic == "MVI" && operands.size() >= 1) {
        lookupMnemonic = mnemonic + " " + operands[0] + ",";
    } else {
        lookupMnemonic = fullMnemonic;
        if (table.find(lookupMnemonic) == table.end()) lookupMnemonic = mnemonic;
    }

    Instruction instr = getOpcode(lookupMnemonic, table);
    std::string machinecode = instr.opcode;

    if (instr.size > 1 && !operands.empty()) {
        for (auto& op : operands) {
            std::string processedOp = op;
            if (!processedOp.empty() && processedOp.back() == 'H') processedOp.pop_back();
            if (labelTable.find(processedOp) != labelTable.end()) processedOp = std::to_string(labelTable.at(processedOp));
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
    }
    return machinecode;
}

void preprocessLabels(const std::string& filename, std::unordered_map<std::string, int>& labelTable) {
    std::ifstream file(filename);
    std::string line;
    int address = 0;

    if (file.is_open()) {
        while (std::getline(file, line)) {
            std::string trimmedLine = line;
            trimmedLine.erase(0, trimmedLine.find_first_not_of(" \t"));
            if (trimmedLine.empty() || trimmedLine[0] == ';') continue;

            size_t colonPos = trimmedLine.find(':');
            if (colonPos != std::string::npos) {
                std::string label = trimmedLine.substr(0, colonPos);
                label.erase(label.find_last_not_of(" \t") + 1);
                labelTable[label] = address;
                trimmedLine = trimmedLine.substr(colonPos + 1);
                trimmedLine.erase(0, trimmedLine.find_first_not_of(" \t"));
            }

            if (!trimmedLine.empty()) {
                std::istringstream iss(trimmedLine);
                std::string mnemonic;
                iss >> mnemonic;
                if (!mnemonic.empty()) {
                    address += 3; // Safe assumption
                }
            }
        }
        file.close();
    }
}

void readAssemblyFile(const std::string& filename, const std::unordered_map<std::string, Instruction>& table, const std::unordered_map<std::string, int>& labelTable) {
    std::ifstream file(filename);
    std::ofstream outfile("output.hex");
    std::string line;

    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == ';') continue;

            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) line = line.substr(colonPos + 1);

            std::string machineCode = parseLine(line, table, labelTable);
            if (!machineCode.empty()) {
                std::cout << "Machine code for '" << line << "': " << machineCode << std::endl;
                outfile << machineCode << std::endl;
            }
        }
        file.close();
        outfile.close();
    } else {
        std::cerr << "Error: Could not open file!" << std::endl;
    }
}

int main() {
    std::unordered_map<std::string, Instruction> opcodeTable;
    std::unordered_map<std::string, int> labelTable;
    initializeOpcodeTable(opcodeTable);

    std::string filename = "program.asm";

    preprocessLabels(filename, labelTable);
    readAssemblyFile(filename, opcodeTable, labelTable);

    return 0;
}