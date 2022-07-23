BEGIN {
    split(ARGV[1], a, "-");
    class = a[1];
    printf("#include \"%s.h\"\n\n", class);
    printf("void statement_%s :: initTable(void)\n{\n", class);
}
$2 ~ "OP_" {
    len = length($2);
    opcode = substr($2, 4, len-3);
    printf("    opcodeTable.push_back(opcode(\"%s\", %s));\n", tolower(opcode), $2);
}
END {
    printf("}\n");
}
