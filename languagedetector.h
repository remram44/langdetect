#ifndef LANGUAGEDETECTOR_H
#define LANGUAGEDETECTOR_H

#include <QDir>
#include <QMap>
#include <QString>
#include <QStringList>

#include "common.h"
#include "language.h"

class LanguageDetector : public WordHolder {

private:
    QMap<QString, Language*> languages;

public:
    LanguageDetector(const char *path);
    LanguageDetector(QDir lists);

    void detectLanguage(const QString &sentence) const;
    void detectLanguage(const QStringList &sentence) const;

private:
    void setup(QDir lists);
    void addLanguage(Language *language);

};

#endif // LANGUAGEDETECTOR_H
