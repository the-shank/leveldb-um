// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "db/builder.h"

#include "db/db_impl.h"
#include "db/dbformat.h"
#include "db/filename.h"
#include "db/table_cache.h"
#include "db/version_edit.h"
#include <iostream>

#include "leveldb/db.h"
#include "leveldb/env.h"
#include "leveldb/iterator.h"

#include "util/coding.h"

namespace leveldb {

Status BuildTable(const std::string& dbname, Env* env, const Options& options,
                  TableCache* table_cache, Iterator* iter, FileMetaData* meta) {
  Status s;
  meta->file_size = 0;
  iter->SeekToFirst();

  std::string fname = TableFileName(dbname, meta->number);
  if (iter->Valid()) {
    WritableFile* file;
    s = env->NewWritableFile(fname, &file);
    if (!s.ok()) {
      return s;
    }

    TableBuilder* builder = new TableBuilder(options, file);
    meta->smallest.DecodeFrom(iter->key());
    Slice key;
    DBImpl::um.mutex_.Lock();
    for (; iter->Valid(); iter->Next()) {
      // std::cout << ">> iter...\n";
      key = iter->key();
      // builder->Add(key, iter->value());

      // NOTE: shank: we should only add the records that are not obsolete as
      // per UM
      Slice value = iter->value();
      uint64_t timestamp = DecodeFixed64(value.data() + value.size() - 8);
      InternalKey internalKey;
      internalKey.DecodeFrom(key);
      auto key2 = internalKey.user_key().ToString();
      auto& memo = DBImpl::um.memo_;
      if (memo.find(key2) == memo.end()) {
        // TODO: shank: this error is being triggered in benchmark (#sid)
        std::cout << ">>>> ";
        DBImpl::um.print();
        std::cout << "key2: >>>" << key2 << "<<<" << std::endl;
        std::cout << "key2.length(): " << key2.length() << std::endl;
        throw std::runtime_error("key not found in memo");
      } else {
        auto& memo_entry = memo[key2];
        if (memo_entry.first > timestamp) {
          // obsolete
          continue;
        } else {
          // not obsolete
          builder->Add(key, value);
        }
      }
    }
    DBImpl::um.mutex_.Unlock();
    if (!key.empty()) {
      meta->largest.DecodeFrom(key);
    }

    // Finish and check for builder errors
    s = builder->Finish();
    if (s.ok()) {
      meta->file_size = builder->FileSize();
      assert(meta->file_size > 0);
    }
    delete builder;

    // Finish and check for file errors
    if (s.ok()) {
      s = file->Sync();
    }
    if (s.ok()) {
      s = file->Close();
    }
    delete file;
    file = nullptr;

    if (s.ok()) {
      // Verify that the table is usable
      Iterator* it = table_cache->NewIterator(ReadOptions(), meta->number,
                                              meta->file_size);
      s = it->status();
      delete it;
    }
  }

  // Check for input iterator errors
  if (!iter->status().ok()) {
    s = iter->status();
  }

  if (s.ok() && meta->file_size > 0) {
    // Keep it
  } else {
    env->RemoveFile(fname);
  }
  return s;
}

}  // namespace leveldb
