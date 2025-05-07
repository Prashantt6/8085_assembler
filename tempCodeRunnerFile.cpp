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