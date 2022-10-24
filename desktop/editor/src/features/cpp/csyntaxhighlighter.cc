#include "features/cpp/csyntaxhighlighter.h"

CSyntaxHighlighter::CSyntaxHighlighter(QTextDocument *parent)
    :QSyntaxHighlighter(parent),
     m_csregexp(QRegularExpression("/\\*")),
     m_ceregexp(QRegularExpression("\\*/"))
{
    updatePreferences();

    connect(myApp, SIGNAL(updatePreferences()),
            this, SLOT(updatePreferences()));
}

void CSyntaxHighlighter::updatePreferences()
{
    QSettings settings("originull", "startext");
    settings.beginGroup("Preferences");

    settings.beginGroup("ColorThemes");
    settings.beginGroup("Default");

    c_comments = QColor(settings.value("CommentColor", QColor(Qt::green).name()).toString());
    c_functions = QColor(settings.value("FunctionColor", QColor(Qt::red).name()).toString());
    c_classes = QColor(settings.value("ClassColor", QColor(Qt::magenta).name()).toString());
    c_strings = QColor(settings.value("StringColor", QColor(Qt::black).name()).toString());
    c_keywords = QColor(settings.value("KeywordColor", QColor(Qt::red).name()).toString());

    // We load our keywords through a filesystem copy here.  If it doesn't exist
    // we fallback to a resource copy

#ifdef WINDOWS_DESKTOP
    QString path = QString("%1/Language Features").arg(QCoreApplication::applicationDirPath());
#endif

#ifdef Q_WS_MAC
    CFURLRef url = (CFURLRef)CFAutorelease((CFURLRef)CFBundleCopyBundleURL(CFBundleGetMainBundle()));
    QString path = QString("%1/Contents/Resources/Langugage Features").arg(QUrl::fromCFURL(url).path());
#endif

#ifdef __linux
    // TODO: this will suffice for packaged versions
    QString path = QString("/usr/share/hollywood");
#endif

    QString file = QString("%1/%2").arg(path).arg("cppkeywords.db");

    if(!QFile::exists(file))
        file = QString(":/Keywords/CPP");

    QFile f(file);
    if(f.open(QFile::ReadOnly))
    {
        QString data = f.readAll();
        m_keywords = data.split('\n');
        f.close();
    }


    createRuleComments();
    createRuleKeywords();
    createRuleStrings();
}

void CSyntaxHighlighter::highlightBlock(const QString &text)
{
    /* apply each rule in the chain */
    foreach(CHighlightRule rule, m_rules)
    {
        QRegularExpressionMatchIterator mi = rule.regexp.globalMatch(text);
        while(mi.hasNext())
        {
            QRegularExpressionMatch m = mi.next();
            setFormat(m.capturedStart(), m.capturedLength(), rule.format);
        }
    }

    setCurrentBlockState(0);

    /* format multi-line comments */
    QTextCharFormat format;
    format.setForeground(c_comments);

    int si = 0;
    if(previousBlockState() != 1)
        si = text.indexOf(m_csregexp);

    while(si >= 0)
    {
        QRegularExpressionMatch m = m_ceregexp.match(text, si);
        int ei = m.capturedStart();
        int cl = 0;
        if(ei == -1)
        {
            setCurrentBlockState(1);
            cl = text.length() - si;
        }
        else
            cl = ei - si + m.capturedLength();

        setFormat(si, cl, format);
        si = text.indexOf(m_csregexp, si + cl);
    }

    TextBlockData *data = new TextBlockData;

    int lpos = text.indexOf('(');
    while(lpos != -1)
    {
        ParenthesisInfo *info = new ParenthesisInfo;
        info->character = '(';
        info->pos = lpos;

        data->insert(info);
        lpos = text.indexOf('(', lpos + 1);
    }

    int rpos = text.indexOf(')');
    while(rpos != -1)
    {
        ParenthesisInfo *info = new ParenthesisInfo;
        info->character = ')';
        info->pos = rpos;

        data->insert(info);

        rpos = text.indexOf(')', rpos +1);
    }

    setCurrentBlockUserData(data);
}

void CSyntaxHighlighter::createRuleClass()
{
    // TODO: obtain a list of classes from the relevant include path/chain
}

void CSyntaxHighlighter::createRuleFunctions()
{
    // TODO: obtain a list of functions from this file and its partner
}

void CSyntaxHighlighter::createRuleKeywords()
{
    QTextCharFormat format;
    format.setForeground(c_keywords);

    foreach(QString w, m_keywords)
    {
        CHighlightRule rule;
        rule.regexp = QRegularExpression(QString("\\b%1\\b").arg(w));
        rule.format = format;
        m_rules.append(rule);
    }
}

void CSyntaxHighlighter::createRuleComments()
{
    /* this only handles single line comments,
     * multi-line comments are handled over in
     * the highlightBlock() function after all
     * other rules */

    QTextCharFormat format;
    format.setForeground(c_comments);

    CHighlightRule rule;
    rule.regexp = QRegularExpression("//[^\n]*");
    rule.format = format;
    m_rules.append(rule);
}

void CSyntaxHighlighter::createRuleStrings()
{
    QTextCharFormat format;

    CHighlightRule rule;
    format.setForeground(c_strings);
    rule.regexp = QRegularExpression("\".*\"");
    rule.format = format;
    m_rules.append(rule);
}
