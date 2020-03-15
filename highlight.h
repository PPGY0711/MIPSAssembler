/**
** Author:		彭官妍 3160104633
** Highlight:	添加语法高亮元素
** Func:		为MIPS汇编语言添加语法高亮，区别指令关键字，寄存器，标号，立即数
*/

#ifndef _HIGHLIGHT_H
#define _HIGHLIGHT_H
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QString>

//text highlight
class Highlight :public QSyntaxHighlighter
{
    Q_OBJECT

public :
    Highlight(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text) Q_DECL_OVERRIDE;

private:
    struct RegExpHighlightRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    struct KeyWordHighlightRule
    {
        QStringList keywords;
        QTextCharFormat format;
    };

    //hightlight rules
    QVector<RegExpHighlightRule> RhighlightRules;
    QVector<KeyWordHighlightRule> KhighlightRules;

    //notations
    QRegExp commentStartExpression;
    QRegExp commentEndExpression;

    QTextCharFormat mnemonicFormat;
    QTextCharFormat registerFormat;
    QTextCharFormat labelFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat initializerFormat;
    QTextCharFormat addressFormat;
    QTextCharFormat coeFormat;
    QTextCharFormat errorFormat;
    void setmnemonicFormat();
    void setregisterFormat();
    void setlabelFormat();
    void setsingleLineCommentFormat();
    void setinitializerFormat();
    void setcoeFormat();
    void seterrorFormat();
    void setaddressFormat();
};

#endif // HIGHLIGHT_H
