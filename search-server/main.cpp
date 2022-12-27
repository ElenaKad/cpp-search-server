#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <cmath>
#include<numeric>
using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

// функция считывания слов
string ReadLine() {
	string s;
	getline(cin, s);
	return s;
}
// функция считывания количества слов
int ReadLineWithNumber() {
	int result;
	cin >> result;
	ReadLine();
	return result;
}
// функция разбиения на слова и записи в вектор слов words
vector<string> SplitIntoWords(const string& text) {
	vector<string> words;
	string word;
	for (const char c : text) {
		if (c == ' ') {
			words.push_back(word);
			word = "";
		}
		else {
			word += c;
		}
	}
	words.push_back(word);

	return words;
}
// объявление структуры документов с двумя переменными: id и relevance
struct Document {
	int id;
	double relevance;
};



// механизм поиска
class SearchServer {
public:
	// функция считывания стоп-слов и записи их в stop_words_
	void SetStopWords(const string& text) {
		for (const string& word : SplitIntoWords(text)) {
			stop_words_.insert(word);
		}
	}
	// функция добавления слов поискового запроса (без стоп-слов) в word_to_documents_
	void AddDocument(int document_id, const string& document) {
		++document_count_;
		const vector<string> words = SplitIntoWordsNoStop(document);
		for (const string& word : words) {

			// вычисляем TF - делим количество раз встречающегося слова на количество всех слов
			double TF = 1. / static_cast<double>(words.size());
			word_to_document_freqs_[word][document_id] += TF;

		}
	}
	// функция вывода 5 наиболее релевантных результатов из всех найденных
	vector<Document> FindTopDocuments(const string& R_query) const {
		const Query query = ParseQuery(R_query);
		auto matched_documents = FindAllDocuments(query);

		sort(matched_documents.begin(), matched_documents.end(), [](const Document& lhs, const Document& rhs) {
			return lhs.relevance > rhs.relevance;
			}
		);
		if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
			matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
		}
		return matched_documents;
	}

private:

	// объявление структуры запроса с плюс и минус словами
	struct Query {
		set<string> plus_word;
		set<string> minus_word;
	};
	struct QueryWord {
		string text;
		bool is_minus;
		bool is_stop;
	};
	//	map<string, set<int>> word_to_documents_; меняем словарь «слово → документы» на «документ → TF»
	map<string, map<int, double>> word_to_document_freqs_;
	int document_count_ = 0;
	set<string> stop_words_;

	bool IsStopWord(const string& word) const {
		return stop_words_.count(word) > 0;
	}
	// функция считывания слов поискового запроса и удаления из него стоп-слов (считывание плюс-слов)
	vector<string> SplitIntoWordsNoStop(const string& text) const {
		vector<string> words;
		for (const string& word : SplitIntoWords(text)) {
			if (!IsStopWord(word)) {
				words.push_back(word);
			}
		}
		return words;
	}
	// обработка минус-слов запроса
	QueryWord ParseQueryWord(string text) const {
		bool is_minus = false;
		// Слово не должно быть пустым
		if (text[0] == '-') {
			is_minus = true;
			text = text.substr(1);
		}
		return { text, is_minus, IsStopWord(text)
		};
	}
	Query ParseQuery(const string& text) const {
		Query query;
		for (const string& word : SplitIntoWords(text)) {
			const QueryWord query_word = ParseQueryWord(word);
			if (!query_word.is_stop) {
				if (query_word.is_minus) {
					query.minus_word.insert(query_word.text);
				}
				else {
					query.plus_word.insert(query_word.text);
				}
			}
		}
		return query;
	}

	// НОВАЯ функция вывода ВСЕХ найденных результатов по релевантности по формуле TF-IDF
	vector<Document> FindAllDocuments(const Query& query) const {
		map<int, double> document_to_relevance;
		// вычисляем IDF - делим количество документов где встречается слово на количество всех документов и берём нат.логарифм
		for (const string& word : query.plus_word) {
			if (word_to_document_freqs_.count(word) == 0) {
				continue;
			}
			double IDF = log(static_cast<double> (document_count_) / static_cast<double> (word_to_document_freqs_.at(word).size()));

			// IDF каждого слова запроса умножается на TF этого слова в этом документе и суммируем для результата
			for (auto& [document_id, TF] : word_to_document_freqs_.at(word)) {
				document_to_relevance[document_id] += IDF * TF;
			}
		}

		/*
		// старая функция вывода ВСЕХ найденных результатов по релевантности
				vector<Document> FindAllDocuments(const string& query) const {
						Query query_words = ParseQuery(query);
						map<int, int> document_to_relevance;
						for (const string& word : query_words.plus_words) {
							if (word_to_documents_.count(word) == 0) {
								continue;
							}
							for (const int document_id : word_to_documents_.at(word)) {
								++document_to_relevance[document_id];
							}
						}
		*/
		for (const string& word : query.minus_word) {
			if (word_to_document_freqs_.count(word) == 0) {
				continue;
			}
			for (auto& [document_id, valueTF] : word_to_document_freqs_.at(word)) {
				document_to_relevance.erase(document_id);
			}
		}
		vector<Document> matched_documents;
		for (const auto& [document_id, relevance] : document_to_relevance) {
			matched_documents.push_back({ document_id, relevance });

		}
		return matched_documents;
	}
};

// считываем всё с ввода
SearchServer CreateSearchServer() {
	SearchServer search_server;
	search_server.SetStopWords(ReadLine());

	const int document_count = ReadLineWithNumber();
	for (int document_id = 0; document_id < document_count; ++document_id) {
		search_server.AddDocument(document_id, ReadLine());
	}
	return search_server;
}


int main() {
	const SearchServer search_server = CreateSearchServer();
	const string query = ReadLine();

	// вывод результата поиска
	for (auto& [document_id, relevance] : search_server.FindTopDocuments(query)) {
		cout << "{ document_id = " << document_id << ", relevance = " << relevance << " }" << endl;
	}
}
