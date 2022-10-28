BEGIN {
    printf("a:  .int8    0:10\n");
    printf("b:  .int16   5\n");
    printf("c:  .int32   100\n");
    printf("d:  .int64   50\n\n");
    printf("x:  .float32 0.1:3\n");
    printf("y:  .float64 6.02e23\n\n");
    printf("p:  .string \"Hello, world!\\n\\0\"\n\n");
}

$2 ~ "OP_" {
    len = length($2);
    opcode = substr($2, 4, len-3);
    printf("        %s\tr0, r1, r2\n", tolower(opcode));
}

