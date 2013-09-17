#include <iostream>
#include <stdexcept>

#include <QString>
#include <QTextStream>
#include "languagedetector.h"

int main()
{
    LanguageDetector detector("wordlists");

    QTextStream in(stdin, QIODevice::ReadOnly);
    QString line = in.readLine();
    while(line.size() > 0)
    {
        detector.detectLanguage(line);
        line = in.readLine();
    }

    return 0;
}
