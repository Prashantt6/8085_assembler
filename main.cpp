#include <iostream>
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <string>
#include <stdexcept>
#include <algorithm> // Include this header for std::remove
#include<vector>
struct Instruction {
    std::string opcode;
    int size;
};

void initializeOpcodeTable(std::unordered_map<std::string, Instruction>& table) {
    // Initialize the opcode table (same as before)
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
    table["INR B"]    = {"04", 1};
    table["INR C"]    = {"0C", 1};
    table["INR D"]    = {"14", 1};
    table["INR E"]    = {"1C", 1};
    table["INR H"]    = {"24", 1};
    table["INR L"]    = {"2C", 1};
    table["INR M"]    = {"34", 1};
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
    if (table.find(mnemonic) != table.end()) {
        return table.at(mnemonic);
    } else {
        throw std::runtime_error("Invalid instruction: " + mnemonic);
    }
}

std::string parseLine(const std::string& line, const std::unordered_map<std::string, Instruction>& table) {
    std::string trimmedLine = line;
    //remove  comments 
    trimmedLine = line.substr(0,line.find( ';'));
    //remove lead space 
    trimmedLine.erase(0, trimmedLine.find_first_not_of(" \t"));
    //remove tail space
    trimmedLine.erase(trimmedLine.find_last_not_of("/t")+1);
    
    if(trimmedLine.empty()){
        return "";
    }
    // extract mnemonic
    std::istringstream iss (trimmedLine);
    std:: string mnemonic ;
    iss>>mnemonic;
    // extract operand part
    std::string operandPart;
    std::getline(iss,operandPart);
    
    //remove the lead space 
    operandPart.erase(operandPart.find_first_not_of("/t"));

    //create vector for multiple operand like MOV A, B
    std::vector<std::string>operands;
    if(!operandPart.empty()){
        //for operands separeted by commas
        size_t commapos = operandPart.find(',');
        if(commapos!=std::string::npos){
            operands.push_back(operandPart.substr(0,commapos));
            operands.push_back(operandPart.substr(commapos+1));

        }
        else {
            operands.push_back(operandPart);
        }
        //trim space in operands
        for(auto& op : operands)
        {
            op.erase(0,op.find_first_not_of("/t"));
            op.erase(op.find_last_not_of("/t")+1);

        }

       }

       try{
        Instruction instr = getOpcode(mnemonic,table);
        std:: string machinecode = instr.opcode;

        if(instr.size>1 && !operands.empty()){
            for( const auto& op : operands){
                std:: string proccessedOp= op;
                if(proccessedOp.back()== 'H'){.
                    proccessedOp.pop_back();

                }
                machinecode = " " + proccessedOp;
            }
        }
     return machinecode;

    }
    catch (const std::exception& e){
        return "Error" + std::string(e.what());
    }
}
void readAssemblyFile(const std::string& filename, const std::unordered_map<std::string, Instruction>& table) {
    std::ifstream file(filename);
    std::string line;

    if (file.is_open()) {
        while (std::getline(file, line)) {
            // Ignore empty lines or comments
            if (line.empty() || line[0] == ';') {
                continue;
            }

            std::string machineCode = parseLine(line, table);
            std::cout << "Machine code for '" << line << "': " << machineCode << std::endl;
        }
        file.close();
    } else {
        std::cerr << "Error: Could not open file!" << std::endl;
    }
}

int main() {
    std::unordered_map<std::string, Instruction> opcodeTable;
    initializeOpcodeTable(opcodeTable);

    // Read assembly file and parse it (example "program.asm")
    readAssemblyFile("program.asm", opcodeTable);

    // Test it with a few instructions
    std::string input = "MOV A,B";
    try {
        Instruction instr = getOpcode(input, opcodeTable);
        std::cout << "Opcode for '" << input << "': "
                  << instr.opcode << " (Size: " << instr.size << " bytes)" << std::endl;

        input = "LDA";
        instr = getOpcode(input, opcodeTable);
        std::cout << "Opcode for '" << input << "': "
                  << instr.opcode << " (Size: " << instr.size << " bytes)" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
