#include "common.h"

LoadingError::LoadingError(const QString &msg)
  : std::runtime_error(msg.toLocal8Bit().data())
{
}

void WordHolder::addWord(const QString &word)
{
    words.insert(word);
}
