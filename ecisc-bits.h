/*
 *  Opcodes
 */

// #define ECISC_OP_J        ECISC_OP_MOVL     /* j dest = movl dest, pc */
#define ECISC_OP_JL       0X01
#define ECISC_OP_RET      ECISC_OP_MOVL     /* ret = movl (sp)+, pc */
#define ECISC_OP_BEQ      0X02
#define ECISC_OP_BNE      0X03
#define ECISC_OP_BLT      0X04
#define ECISC_OP_BLE      0X05
#define ECISC_OP_BLTU     ECISC_OP_BNC
#define ECISC_OP_BLEU     0X06
#define ECISC_OP_BGT      0X07
#define ECISC_OP_BGE      0X08
#define ECISC_OP_BGTU     0X09
#define ECISC_OP_BGEU     ECISC_OP_BOC
#define ECISC_OP_BOC      0x0a
#define ECISC_OP_BNC      0X0B
#define ECISC_OP_BOV      0X0C
#define ECISC_OP_BNV      0X0D
#define ECISC_OP_J        0X0E
#define ECISC_OP_MOVB     0X10
#define ECISC_OP_MOVS     0X11
#define ECISC_OP_MOVL     0X12
#define ECISC_OP_MOVQ     0X13
#define ECISC_OP_MOVF     ECISC_OP_MOVL
#define ECISC_OP_MOVD     ECISC_OP_MOVQ
#define ECISC_OP_MOVLF    0x14
#define ECISC_OP_MOVFL    0x15
#define ECISC_OP_MOVFD    0x16
#define ECISC_OP_MOVDF    0X17
#define ECISC_OP_CMPB     0X20
#define ECISC_OP_CMPS     0X21
#define ECISC_OP_CMPL     0X22
#define ECISC_OP_CMPQ     0X23
#define ECISC_OP_CMPF     0X24
#define ECISC_OP_CMPD     0X25
#define ECISC_OP_NOTB     0X30
#define ECISC_OP_NOTS     0X31
#define ECISC_OP_NOTL     0X32
#define ECISC_OP_NOTQ     0X33
#define ECISC_OP_ANDB     0X34
#define ECISC_OP_ANDS     0X35
#define ECISC_OP_ANDL     0X36
#define ECISC_OP_ANDQ     0X37
#define ECISC_OP_ORB      0X38
#define ECISC_OP_ORS      0X39
#define ECISC_OP_ORL      0X3A
#define ECISC_OP_ORQ      0X3B
#define ECISC_OP_XORB     0X3C
#define ECISC_OP_XORS     0X3D
#define ECISC_OP_XORL     0X3E
#define ECISC_OP_XORQ     0X3F
#define ECISC_OP_SLLB     0X40
#define ECISC_OP_SLLS     0X41
#define ECISC_OP_SLLL     0X42
#define ECISC_OP_SLLQ     0X43
#define ECISC_OP_SRLB     0X44
#define ECISC_OP_SRLS     0X45
#define ECISC_OP_SRLL     0X46
#define ECISC_OP_SRLQ     0X47
#define ECISC_OP_SRAB     0X48
#define ECISC_OP_SRAS     0X49
#define ECISC_OP_SRAL     0X4A
#define ECISC_OP_SRAQ     0X4B
#define ECISC_OP_ROLB     0X4C
#define ECISC_OP_ROLS     0X4D
#define ECISC_OP_ROLL     0X4E
#define ECISC_OP_ROLQ     0X4F
#define ECISC_OP_CLRB     0X50
#define ECISC_OP_CLRS     0X51
#define ECISC_OP_CLRL     0X52
#define ECISC_OP_ADDB     0X60
#define ECISC_OP_ADDS     0X61
#define ECISC_OP_ADDL     0X62
#define ECISC_OP_ADDQ     0X63
#define ECISC_OP_ADDF     0X64
#define ECISC_OP_ADDD     0X65
#define ECISC_OP_ADDBC    0X66
#define ECISC_OP_ADDSC    0X67
#define ECISC_OP_ADDLC    0X68
#define ECISC_OP_INCB     0X69
#define ECISC_OP_INCS     0X6A
#define ECISC_OP_INCL     0X6B
#define ECISC_OP_DECB     0X6C
#define ECISC_OP_DECS     0X6D
#define ECISC_OP_DECL     0X6E
#define ECISC_OP_SUBB     0X70
#define ECISC_OP_SUBS     0X71
#define ECISC_OP_SUBL     0X72
#define ECISC_OP_SUBQ     0X73
#define ECISC_OP_SUBF     0X74
#define ECISC_OP_SUBD     0X75
#define ECISC_OP_MULB     0X80
#define ECISC_OP_MULS     0X81
#define ECISC_OP_MULL     0X82
#define ECISC_OP_MULQ     0X83
#define ECISC_OP_MULF     0X84
#define ECISC_OP_MULD     0X85
#define ECISC_OP_DIVB     0X90
#define ECISC_OP_DIVS     0X91
#define ECISC_OP_DIVL     0X92
#define ECISC_OP_DIVQ     0X93
#define ECISC_OP_DIVF     0X94
#define ECISC_OP_DIVD     0X95
#define ECISC_OP_REMB     0XA0
#define ECISC_OP_REMS     0XA1
#define ECISC_OP_REML     0XA2
#define ECISC_OP_REMQ     0XA3
#define ECISC_OP_INPUTB   0XB0
#define ECISC_OP_INPUTS   0XB1
#define ECISC_OP_INPUTL   0XB2
#define ECISC_OP_OUTPUTB  0XB4
#define ECISC_OP_OUTPUTS  0XB5
#define ECISC_OP_OUTPUTL  0XB6
#define ECISC_OP_SLEEPL   0xfe
#define ECISC_OP_HALT     0xff

/*
 *  Addressing modes.  Upper 4 bits of mode byte.  Lower 4 are register #.
 *  Similar to VAX.
 */

#define ECISC_MODE_REG_DIRECT         0x00
#define ECISC_MODE_REG_INDIRECT       0x10
#define ECISC_MODE_AUTO_INCREMENT     0x20
#define ECISC_MODE_AUTO_DECREMENT     0x30
#define ECISC_MODE_OFFSET             0x40

/*
 *  Modes that don't use registers.  Start with f and use the other nybble
 *  to distinguish.
 */

#define ECISC_MODE_MEM_DIRECT         0xf0
#define ECISC_MODE_MEM_INDIRECT       0xe0    // FIXME: Conflicts with comment

/*
 *  Pseudo modes
 */

#define ECISC_MODE_ADDRESS_OF         ECISC_MODE_AUTO_INCREMENT
#define ECISC_MODE_IMMEDIATE          ECISC_MODE_AUTO_INCREMENT
#define ECISC_MODE_IMMEDIATE_ADDRESS  ECISC_MODE_MEM_DIRECT

#define ECISC_REG_NUM(m)      ((m) & 0x0f)
#define ECISC_MODE_BITS(m)    ((m) & 0xf0)
