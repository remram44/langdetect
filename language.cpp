#include "language.h"

#include <QFile>
#include <QTextStream>

Language::Language(WordHolder *detector, const QString &name, QDir path)
  : m_Name(name)
{
    fprintf(stderr, "  Loading language %s...\n", name.toLocal8Bit().data());
    path.setFilter(QDir::Files);
    QStringList dicts = path.entryList();
    QStringList::ConstIterator it = dicts.begin();
    for(; it != dicts.end(); ++it)
        readDict(detector, path.filePath(*it));
    fprintf(stderr, "  Loaded %d words\n", words.size());
}

void Language::readDict(WordHolder *detector, QString dict)
{
    fprintf(stderr, "    Reading %s...\n", dict.toLocal8Bit().data());

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

    fprintf(stderr, "    Read %d words\n", nb_words);
}
