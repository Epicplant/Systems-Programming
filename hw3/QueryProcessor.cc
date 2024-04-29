/*
 * Copyright ©2023 Chris Thachuk.  All rights reserved.  Permission is
 * hereby granted to students registered for University of Washington
 * CSE 333 for use solely during Fall Quarter 2023 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

#include "./QueryProcessor.h"

#include <iostream>
#include <algorithm>
#include <list>
#include <string>
#include <vector>
#include <map>

extern "C" {
  #include "./libhw1/CSE333.h"
}

using std::list;
using std::sort;
using std::string;
using std::vector;
using std::map;


namespace hw3 {

QueryProcessor::QueryProcessor(const list<string>& index_list, bool validate) {
  // Stash away a copy of the index list.
  index_list_ = index_list;
  array_len_ = index_list_.size();
  Verify333(array_len_ > 0);

  // Create the arrays of DocTableReader*'s. and IndexTableReader*'s.
  dtr_array_ = new DocTableReader* [array_len_];
  itr_array_ = new IndexTableReader* [array_len_];

  // Populate the arrays with heap-allocated DocTableReader and
  // IndexTableReader object instances.
  list<string>::const_iterator idx_iterator = index_list_.begin();
  for (int i = 0; i < array_len_; i++) {
    FileIndexReader fir(*idx_iterator, validate);
    dtr_array_[i] = fir.NewDocTableReader();
    itr_array_[i] = fir.NewIndexTableReader();
    idx_iterator++;
  }
}

QueryProcessor::~QueryProcessor() {
  // Delete the heap-allocated DocTableReader and IndexTableReader
  // object instances.
  Verify333(dtr_array_ != nullptr);
  Verify333(itr_array_ != nullptr);
  for (int i = 0; i < array_len_; i++) {
    delete dtr_array_[i];
    delete itr_array_[i];
  }

  // Delete the arrays of DocTableReader*'s and IndexTableReader*'s.
  delete[] dtr_array_;
  delete[] itr_array_;
  dtr_array_ = nullptr;
  itr_array_ = nullptr;
}

// Takes a single word and, using a provided
// IndexTableReader and DocTableReader, populates a map containing
// all files (at least used to make the index and doctable reader)
// that the word shows up in. These files are stored in the map with
// the key being said files docid and the values being
// query results (string file name + number of occurences). Returns
// the created map (empty if none of the words show up).
map<DocID_t, QueryProcessor::QueryResult>
ProcessFirstWord(const IndexTableReader* index,
                 const DocTableReader* doctable,
                 const DocIDTableReader* didreader);

// Takes a map of files (key containng docIds and values containing
// query result's that consist of string file paths and a particular
// words occurrences) called index_results and removes all
// files from the map that don't contain all of the words provided
// in the query. Index and doctable are the corresponding memindex and
// doctable for all the files we will be analyzing. Query contains the
// set of of words that we will be comparing to the files in the map;
// however, we start at the second word in query since we assume
// the first word was used to create index_results.
void ProcessWords(const IndexTableReader* index,
                  const DocTableReader* doctable,
                  map<DocID_t, QueryProcessor::QueryResult>* const
                  index_results,
                  const vector<string>& query);

vector<QueryProcessor::QueryResult>
QueryProcessor::ProcessQuery(const vector<string>& query) const {
  // STEP 1.
  // (the only step in this file)
  vector<QueryProcessor::QueryResult> final_result;

  // Iterate through every single file we are analyzing
  for (int i = 0; i < array_len_; i++) {
    // Receive both the index reader and the doctable reader for
    // the current file
    IndexTableReader* index = itr_array_[i];
    DocTableReader* doctable = dtr_array_[i];
    map<DocID_t, QueryResult> index_results;

    // Process the first word of the query in the file
    // and create a list with that

    // For this word, receive all files that contain it
    DocIDTableReader* didreader = index->LookupWord(query[0]);
    // The word exists nowhere in this index, so move onto next index
    if (didreader == nullptr) {
      continue;
    }

    // Takes the first word in the query and constructs a map out
    // of it (with files the word is found in having keys for
    // that files docid and its value being the file name and
    // the number of times the word occurs in the file)
    index_results = ProcessFirstWord(index, doctable, didreader);

    // Process the rest of the words for the query by removing
    // files from index_results that all words weren't found in
    // and adding to files number of word occurrences in cases
    // where all words are found in the file
    ProcessWords(index, doctable, &index_results, query);

    // Push all files that made the cut to the return vector.
    for (auto result : index_results) {
      final_result.push_back(result.second);
    }
  }

  // Sort the final results.
  sort(final_result.begin(), final_result.end());
  return final_result;
}

map<DocID_t, QueryProcessor::QueryResult>
ProcessFirstWord(const IndexTableReader* index,
                 const DocTableReader* doctable,
                 const DocIDTableReader* didreader) {
  map<DocID_t, QueryProcessor::QueryResult> index_results;

  // Receive all files that contain the wrod in a list
  list<DocIDElementHeader> docids = didreader->GetDocIDList();

  // Create a vector containing Query results that have all the
  // files that contain the first word as well as its respective
  // positions
  for (DocIDElementHeader didheader : docids) {
    QueryProcessor::QueryResult qr;
    string docname;
    doctable->LookupDocID(didheader.doc_id, &docname);
    qr.document_name = docname;
    qr.rank = didheader.num_positions;

    index_results[didheader.doc_id] = qr;
  }

  // Move onto any words past the first word and clean up this words didreader
  delete didreader;
  return index_results;
}

void
ProcessWords(const IndexTableReader* index,
             const DocTableReader* doctable,
             map<DocID_t, QueryProcessor::QueryResult>* const index_results,
             const vector<string>& query) {
  // Creates an iterator starting at the second word
  auto word_iter = query.begin();
  word_iter++;

  // For every remaining word, iterate through index_results
  // and if any of the files don't also contain that word remove it.
  while (word_iter < query.end()) {
    DocIDTableReader* didreader = index->LookupWord(*word_iter);

    // The word exists nowhere so return empty list;
    if (didreader == nullptr) {
      index_results->clear();
      break;
    }

    // Iterate through every key within the map
    auto index_iter = index_results->begin();
    while (index_iter != index_results->end()) {
      // Determine whether this word can also be found within
      // the docid as other words in query. If it can't, then
      // that means the file shouldn't be in the query and it is
      // removed; otherwise, add its contents to the map
      list<DocPositionOffset_t> positions;
      bool found = didreader->LookupDocID(index_iter->first, &positions);

      if (found) {
        (*index_results)[index_iter->first].rank += positions.size();
        index_iter++;
      } else {
        auto toErase = index_iter;
        index_iter++;
        index_results->erase(toErase);
      }
    }

    // Move onto the next QueryResult and remove the old didreader
    word_iter++;
    delete didreader;
  }
}

}  // namespace hw3
