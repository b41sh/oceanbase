/**
 * Copyright (c) 2021 OceanBase
 * OceanBase CE is licensed under Mulan PubL v2.
 * You can use this software according to the terms and conditions of the Mulan PubL v2.
 * You may obtain a copy of Mulan PubL v2 at:
 *          http://license.coscl.org.cn/MulanPubL-2.0
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PubL v2 for more details.
 */

#ifndef OCEANBASE_STORAGE_OB_VALUE_ROW_ITERATOR_
#define OCEANBASE_STORAGE_OB_VALUE_ROW_ITERATOR_

#include "lib/allocator/page_arena.h"
#include "lib/hash/ob_placement_hashmap.h"
#include "lib/container/ob_se_array.h"
#include "lib/allocator/page_arena.h"
#include "common/row/ob_row_iterator.h"
#include "common/rowkey/ob_rowkey.h"
#include "storage/ob_i_store.h"
#include "storage/ob_dml_param.h"
namespace oceanbase {
namespace storage {
class ObValueRowIterator : public common::ObNewRowIterator {
  static const int64_t DEFAULT_ROW_NUM = 2;
  typedef common::ObSEArray<common::ObNewRow, DEFAULT_ROW_NUM> RowArray;

public:
  ObValueRowIterator();
  virtual ~ObValueRowIterator();
  virtual int init(bool unique, int64_t data_table_rowkey_cnt);
  virtual int get_next_row(common::ObNewRow*& row);
  virtual int get_next_rows(common::ObNewRow*& rows, int64_t& row_count);
  virtual int add_row(common::ObNewRow& row);
  virtual void reset();

private:
  bool is_inited_;
  bool unique_;
  common::ObArenaAllocator allocator_;
  RowArray rows_;
  int64_t cur_idx_;
  int64_t data_table_rowkey_cnt_;

private:
  DISALLOW_COPY_AND_ASSIGN(ObValueRowIterator);
};

class ObSingleMerge;
class ObSingleRowGetter {
  typedef common::ObFixedArray<int32_t, common::ObIAllocator> Projector;

public:
  ObSingleRowGetter(common::ObIAllocator &allocator, ObPartitionStore &store);
  ~ObSingleRowGetter();

  int init_dml_access_ctx(const ObStoreCtx &store_ctx, const ObDMLBaseParam &dml_param);
  int init_dml_access_param(
      ObRelativeTable &data_table, const ObDMLBaseParam &dml_param, const common::ObIArray<uint64_t> &out_col_ids);
  ObTableAccessParam &get_access_param()
  {
    return access_param_;
  }
  ObTableAccessContext &get_access_ctx()
  {
    return access_ctx_;
  }
  void set_relative_table(ObRelativeTable *relative_table)
  {
    relative_table_ = relative_table;
  }
  int open(const ObStoreRowkey &rowkey, bool use_fuse_row_cache = false);
  int get_next_row(common::ObNewRow *&row);

private:
  int create_table_param();

private:
  ObPartitionStore &store_;
  ObSingleMerge *single_merge_;
  const ObStoreCtx *store_ctx_;
  Projector output_projector_;
  ObTableAccessParam access_param_;
  ObTableAccessContext access_ctx_;
  ObGetTableParam get_table_param_;
  ObRelativeTable *relative_table_;
  share::schema::ObTableParam *table_param_;
  union {
    ObExtStoreRowkey ext_rowkey_;
  };
  common::ObIAllocator &allocator_;
};
}  // end namespace storage
}  // end namespace oceanbase

#endif  // OCEANBASE_STORAGE_OB_VALUE_ROW_ITERATOR_
