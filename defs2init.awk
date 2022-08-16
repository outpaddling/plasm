BEGIN {
    split(ARGV[1], a, "-");
    class = a[1];
    printf("#include \"%s.h\"\n\n", class);
    printf("void statement_%s :: initTable(void)\n{\n", class);
}
$2 ~ "OP_" {
    len = length($2);
    split($2, a, "_");
    opcode = a[3];
    printf("    opcodeTable.push_back(opcode(\"%s\", %s));\n", tolower(opcode), $2);
}
END {
    printf("}\n");
}
