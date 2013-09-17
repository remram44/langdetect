#include "language.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>

Language::Language(WordHolder *detector, const QString &name, QDir path)
  : m_Name(name)
{
    qDebug() << "  Loading language " << name << "...";
    path.setFilter(QDir::Files);
    QStringList dicts = path.entryList();
    QStringList::ConstIterator it = dicts.begin();
    for(; it != dicts.end(); ++it)
        readDict(detector, path.filePath(*it));
    qDebug() << "  Loaded" << words.size() << "words";
}

void Language::readDict(WordHolder *detector, QString dict)
{
    qDebug() << "    Reading" << dict << "...";

    QFile dictf(dict);
    if(!dictf.open(QIODevice::ReadOnly | QIODevice::Text))
        throw LoadingError(QString("Can't open file %1").arg(dict));

    QTextStream in(&dictf);
    size_t nb_words = 0;
    while(!in.atEnd())
    {
        QString line = in.readLine();
        if(line.length() == 0)
            continue;
        if(line[0] == '%')
            continue;
        detector->addWord(line);
        words.insert(line);
        nb_words++;
    }

    qDebug() << "    Read" << nb_words << "words";
}
