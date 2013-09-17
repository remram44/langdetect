#ifndef COMMON_H
#define COMMON_H

#include <QSet>
#include <QString>
#include <stdexcept>

class LoadingError : std::runtime_error {

public:
    LoadingError(const QString &msg);

};

class WordHolder {

protected:
    QSet<QString> words;

public:
    void addWord(const QString &word);

};

#endif // COMMON_H
