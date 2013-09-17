#include <iostream>
#include <stdexcept>

#include <QDir>
#include <QMap>
#include <QSet>
#include <QTextStream>


class LanguageDetector;


class WordHolder {

protected:
    QSet<QString> words;

public:
    void addWord(const QString &word);

};

void WordHolder::addWord(const QString &word)
{
    words.insert(word);
}

class LoadingError : std::runtime_error {

public:
    LoadingError(const QString &msg);

};

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

LoadingError::LoadingError(const QString &msg)
  : std::runtime_error(msg.toLocal8Bit().data())
{
}

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
