#include "languagedetector.h"

#include <QDebug>

LanguageDetector::LanguageDetector(const char *path)
{
    setup(QDir(path));
}

LanguageDetector::LanguageDetector(QDir lists)
{
    // This is only a separate function because delegating constructors are not
    // available before C++11
    setup(lists);
}

void LanguageDetector::setup(QDir lists)
{
    qDebug() << "Starting up, loading languages...";
    qDebug() << "loading from " << lists.absolutePath();
    lists.setFilter(QDir::Dirs);
    QStringList languages = lists.entryList();
    qDebug() << languages.size() << "languages to be loaded";
    QStringList::ConstIterator it = languages.begin();
    for(; it != languages.end(); ++it)
    {
        if(*it == "." || *it == "..")
            continue;
        addLanguage(new Language(this, *it, lists.filePath(*it)));
    }

    qDebug() << "Languages loaded, total" << words.size() << "words";
}

void LanguageDetector::addLanguage(Language *language)
{
    languages[language->name()] = language;
}

void LanguageDetector::detectLanguage(const QString &sentence) const
{
    detectLanguage(sentence.split(QRegExp("[ \t]"), QString::SkipEmptyParts));
}

bool sort_result_pairs(QPair<float, Language*> p1,
                       QPair<float, Language*> p2)
{
    return p1.first > p2.first;
}

void LanguageDetector::detectLanguage(const QStringList &sentence) const
{
    QStringList::ConstIterator w;
    qDebug() << "Recognizing sentence:" << sentence;
    QMap<Language*, float> language_scores;
    for(w = sentence.begin(); w != sentence.end(); ++w)
    {
        QList<Language*> matches;
        {
            QMap<QString, Language*>::ConstIterator lang;
            for(lang = languages.begin(); lang != languages.end(); ++lang)
            {
                if((*lang)->hasWord(*w))
                    matches.append(*lang);
            }
        }
        {
            QList<Language*>::ConstIterator lang;
            for(lang = matches.begin(); lang != matches.end(); ++lang)
            {
                float score = 1.0f/matches.size();
                qDebug() << "Word: " << *w
                         << ", lang: " << (*lang)->name()
                         << ", score: " << score;
                language_scores[*lang] = language_scores[*lang] + score;
            }
        }
    }

    QList<QPair<float, Language*> > results;

    qDebug() << "Scores:";
    QMap<Language*, float>::ConstIterator lang;
    for(lang = language_scores.begin(); lang != language_scores.end(); ++lang)
    {
        qDebug() << "  " << lang.key()->name() << ": " << lang.value();
        results.push_back(qMakePair(lang.value(), lang.key()));
    }

    qSort(results.begin(), results.end(), sort_result_pairs);

    qDebug() << "Ordered results:";
    QList<QPair<float, Language*> >::ConstIterator result;
    for(result = results.begin(); result != results.end(); ++result)
        fprintf(stdout, "%s\n", result->second->name().toLocal8Bit().data());
}
