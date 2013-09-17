#include "languagedetector.h"

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
    fprintf(stderr, "Starting up, loading languages...\n");
    fprintf(stderr, "loading from %s\n", lists.absolutePath().toLocal8Bit().data());
    lists.setFilter(QDir::Dirs);
    QStringList languages = lists.entryList();
    fprintf(stderr, "%d languages to be loaded\n", languages.size());
    QStringList::ConstIterator it = languages.begin();
    for(; it != languages.end(); ++it)
    {
        if(*it == "." || *it == "..")
            continue;
        addLanguage(new Language(this, *it, lists.filePath(*it)));
    }

    fprintf(stderr,
            "Languages loaded, total %d words\n",
            words.size());
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
    fprintf(stderr, "Recognizing sentence:");
    for(w = sentence.begin(); w != sentence.end(); ++w)
        fprintf(stderr, " %s", w->toLocal8Bit().data());
    fprintf(stderr, "\n");
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
                fprintf(stderr, "Word: %s, lang: %s, score: %f\n",
                        w->toLocal8Bit().data(),
                        (*lang)->name().toLocal8Bit().data(),
                        score);
                language_scores[*lang] = language_scores[*lang] + score;
            }
        }
    }

    QList<QPair<float, Language*> > results;

    fprintf(stderr, "Scores:\n");
    QMap<Language*, float>::ConstIterator lang;
    for(lang = language_scores.begin(); lang != language_scores.end(); ++lang)
    {
        fprintf(stderr, "  %s: %f\n",
                lang.key()->name().toLocal8Bit().data(),
                lang.value());
        results.push_back(qMakePair(lang.value(), lang.key()));
    }

    qSort(results.begin(), results.end(), sort_result_pairs);

    fprintf(stderr, "Ordered results:\n");
    QList<QPair<float, Language*> >::ConstIterator result;
    for(result = results.begin(); result != results.end(); ++result)
        fprintf(stdout, "%s\n", result->second->name().toLocal8Bit().data());
}
