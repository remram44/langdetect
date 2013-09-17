#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <QDir>
#include <QSet>
#include <QString>

#include "common.h"

class Language {

private:
    QString m_Name;
    QSet<QString> words;

public:
    Language(WordHolder*, const QString &name, QDir path);
    inline QString name() const
    { return m_Name; }
    inline bool hasWord(const QString &word) const
    { return words.contains(word); }

private:
    void readDict(WordHolder *detector, QString dict);

};

#endif // LANGUAGE_H
