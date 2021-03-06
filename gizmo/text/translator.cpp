#include "translator.h"
#include "utf8.h"

using namespace std;


Translator::Translator(const Dictionary &dict) : m_dict(dict), m_minSim(1.0f)
{
}

void Translator::connectWordsCallback(WordsCallback callback)
{
	m_wordsCb = callback;
}

void Translator::setMinWordsSim(float minSim)
{
	m_minSim = minSim;
}

void Translator::pushWord(const Word &word)
{
	const string lword = Utf8::toLower(word.text);
	auto it1 = m_dict.bestGuess(lword);
	auto it2 = it1;

	if (it1 == m_dict.end())
		return;

	for (; it1 != m_dict.end(); --it1)
	{
		float sim = compareWords(lword, it1->first);
		if (sim < m_minSim)
			break;

		for (auto &tr : it1->second)
			m_wordsCb(Word(tr, word.time, word.score*sim));
	}

	for (++it2; it2 != m_dict.end(); ++it2)
	{
		float sim = compareWords(lword, it2->first);
		if (sim < m_minSim)
			break;

		for (auto &tr : it2->second)
			m_wordsCb(Word(tr, word.time, word.score*sim));
	}
}
