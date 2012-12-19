$2 ~ "OP_" {
    len = length($2);
    opcode = substr($2, 4, len-3);
    printf("opcodeTable.push_back(opcode(\"%s\", %s));\n", tolower(opcode), $2);
}

