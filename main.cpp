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
        for (auto& op : operands) {
            std::string processedOp = op;
            if (!processedOp.empty() && processedOp.back() == 'H') {
                processedOp.pop_back();
            }
            if (labelTable.find(processedOp) != labelTable.end()) {
                processedOp = std::to_string(labelTable.at(processedOp));
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

            std::istringstream iss(trimmedLine);
            std::string mnemonic;
            iss >> mnemonic;
            if (!mnemonic.empty()) {
                address += 3; // assume worst-case (simplification)
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
            if (colonPos != std::string::npos) {
                line = line.substr(colonPos + 1);
            }

            std::string machineCode = parseLine(line, table, labelTable);
            std::cout << "Machine code for '" << line << "': " << machineCode << std::endl;
            outfile << machineCode << std::endl; 
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
