#include "highlight.h"

Highlight::Highlight(QTextDocument *parent)
    :QSyntaxHighlighter(parent)
{
    setmnemonicFormat();
    setregisterFormat();
    setlabelFormat();
    setsingleLineCommentFormat();
    setinitializerFormat();
    setcoeFormat();
    seterrorFormat();
    setaddressFormat();
}

void Highlight::highlightBlock(const QString &text)
{

    foreach (const RegExpHighlightRule &rule, RhighlightRules)
    {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }

    for(QVector<KeyWordHighlightRule>::iterator it = KhighlightRules.begin();
        it != KhighlightRules.end(); it++)
    {
        foreach (const QString &exp, it->keywords) {
            QRegExp pattern = QRegExp(exp);
            //pattern.setCaseSensitivity(Qt::CaseInsensitive);
            QRegExp expression(pattern);
            int index = expression.indexIn(text);
            while (index >= 0) {
                int length = expression.matchedLength();
                setFormat(index, length, it->format);
                index = expression.indexIn(text, index + length);
            }
        }
    }
}

void Highlight::setmnemonicFormat()
{
    KeyWordHighlightRule krule;
    //formats
    mnemonicFormat.setForeground(Qt::red);
    //mnemonicFormat.setFontWeight(QFont::Bold);
    krule.format = mnemonicFormat;
    krule.keywords  <<"add"<<"sub"<<"slt"<<"sltu"<<"and"<<"or"<<"xor"<<"nor"<<
                      "sll"<<"sllv"<<"srl"<<"srlv"<<"sra"<<"srav"<<"jr"<<"jalr"<<
                      "syscall"<<"mul"<<"mult"<<"multu"<<"div"<<"divu"<<"mfhi"<<
                      "mflo"<<"mthi"<<"mtlo"<<"mfc0"<<"mtc0"<<"eret"<<"lui"<<"addi"<<
                      "slti"<<"sltiu"<<"andi"<<"ori"<<"xori"<<"lw"<<"lwx"<<"lh"<<"lhx"<<
                      "lhu"<<"lhux"<<"sw"<<"swx"<<"sh"<<"shx"<<"beq"<<"bne"<<"bgezal"<<
                      "push"<<"pop"<<"move"<<"shi"<<"shix"<<"j"<<"jal"<<
                      "inc"<<"dec"<<"addu"<<"addiu"<<"subu"<<"not"<<"neg"<<
                      "abs"<<"swap"<<"b"<<"beqz"<<"bnez"<<"beqi"<<"bnei"<<
                      "blt"<<"ble"<<"bgt"<<"bge"<<"seq"<<"sne"<<"li"<<"la";

    KhighlightRules.append(krule);
}

void Highlight::setregisterFormat()
{
    KeyWordHighlightRule krule;
    registerFormat.setForeground(Qt::darkMagenta);
    krule.format = registerFormat;
    krule.keywords<<"\\$zero"<<"\\$v0"<<"\\$v1"<<"\\$a0"<<"\\$a1"<<"\\$a2"<<"\\$a3"<<"\\$t0"<<\
                    "\\$t1"<<"\\$t2"<<"\\$t3"<<"\\$t4"<<"\\$t5"<<"\\$t6"<<"\\$t7"<<"\\$s0"<<\
                    "\\$s1"<<"\\$s2"<<"\\$s3"<<"\\$s4"<<"\\$s5"<<"\\$s6"<<"\\$s7"<<"\\$t8"<<\
                    "\\$t9"<<"\\$gp"<<"\\$sp"<<"\\$fp"<<"\\$ra"<<"\\$0"<<"\\$0"<<"\\$1"<<"\\$2"
                    <<"\\$3"<<"\\$4"<<"\\$5"<<"\\$6"<<"\\$7"<<"\\$8"<<"\\$9"<<"\\$10"<<"\\$11"
                    <<"\\$12"<<"\\$13"<<"\\$14"<<"\\$15"<<"\\$16"<<"\\$17"<<"\\$18"<<"\\$19"
                    <<"\\$20"<<"\\$21"<<"\\$22"<<"\\$23"<<"\\$24"<<"\\$25"<<"\\$26"<<"\\$27"
                    <<"\\$28"<<"\\$29"<<"\\$30"<<"\\$31"
                    <<"\\STATUS"<<"\\CAUSE"<<"\\EPC";
    KhighlightRules.append(krule);
}

void Highlight::setlabelFormat()
{
    RegExpHighlightRule rrule;
    labelFormat.setForeground(Qt::blue);
    labelFormat.setFontWeight(QFont::Bold);
    rrule.format = labelFormat;
    rrule.pattern = QRegExp("^[A-Za-z_][A-Za-z0-9_]*:");
    rrule.pattern.setCaseSensitivity(Qt::CaseInsensitive);
    RhighlightRules.append(rrule);
}

void Highlight::setsingleLineCommentFormat()
{
    RegExpHighlightRule rrule;
    singleLineCommentFormat.setForeground(Qt::darkGray);
    rrule.format = singleLineCommentFormat;
    rrule.pattern = QRegExp("^\/\/[^\n]*|#[^\n]*");
    //rrule.pattern.setCaseSensitivity(Qt::CaseInsensitive);
    RhighlightRules.append(rrule);
}

void Highlight::setinitializerFormat()
{
    RegExpHighlightRule rrule;
    initializerFormat.setForeground(Qt::green);
    rrule.format=initializerFormat;
    rrule.pattern = QRegExp("^[.]{1}[A-Za-z]*");
    //rrule.pattern.setCaseSensitivity(Qt::CaseInsensitive);
    RhighlightRules.push_back(rrule);
}

void Highlight::setcoeFormat()
{
    KeyWordHighlightRule krule;
    coeFormat.setForeground(Qt::darkCyan);
    coeFormat.setFontWeight(QFont::Bold);
    krule.format = coeFormat;
    krule.keywords << "MEMORY_INITIALIZATION_RADIX=16;"<<"MEMORY_INITIALIZATION_VECTOR";
    KhighlightRules.append(krule);
}

void Highlight::seterrorFormat()
{
    RegExpHighlightRule rrule;
    errorFormat.setForeground(Qt::red);
    errorFormat.setFontWeight(QFont::Bold);
    errorFormat.setFontItalic(true);
    rrule.format=errorFormat;
    rrule.pattern = QRegExp("Error[^\n]*\!");
    rrule.pattern.setCaseSensitivity(Qt::CaseInsensitive);
    RhighlightRules.push_back(rrule);
}

void Highlight::setaddressFormat()
{
    RegExpHighlightRule rrule;
    addressFormat.setForeground(Qt::darkBlue);
    addressFormat.setFontWeight(QFont::Bold);
    rrule.format=addressFormat;
    rrule.pattern = QRegExp("baseAddre|DataAddre\:0x?X?[0-9]*[^\n]");
    RhighlightRules.push_back(rrule);
}
