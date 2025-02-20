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

#ifndef _OCEABASE_COMMON_OB_COMMON_UTILITY_H_
#define _OCEABASE_COMMON_OB_COMMON_UTILITY_H_
#include <sys/time.h>
#include "lib/ob_define.h"
namespace oceanbase
{
namespace common
{
extern const char *print_server_role(const common::ObServerRole server_role);

//@brief recursive function call should use this function to check if recursion is too deep
//to avoid stack overflow, default reserved statck size is 1M
extern int64_t get_reserved_stack_size();
extern void set_reserved_stack_size(int64_t reserved_size);
extern int check_stack_overflow(
    bool &is_overflow,
    int64_t reserved_stack_size = get_reserved_stack_size(),
    int64_t *used_size = nullptr);
extern int get_stackattr(void *&stackaddr, size_t &stacksize);
extern void set_stackattr(void *stackaddr, size_t stacksize);

// return OB_SIZE_OVERFLOW if stack overflow
inline int check_stack_overflow(void)
{
  bool overflow = false;
  int ret = check_stack_overflow(overflow);
  return OB_LIKELY(OB_SUCCESS == ret) && OB_UNLIKELY(overflow) ? OB_SIZE_OVERFLOW : ret;
}

/**
 * @brief The ObFatalErrExtraInfoGuard class is used for printing extra info, when fatal error happens.
 *        The of pointer of the class is maintained on thread local.
 */
class ObFatalErrExtraInfoGuard
{
public:
  explicit ObFatalErrExtraInfoGuard();
  virtual ~ObFatalErrExtraInfoGuard();
  static const ObFatalErrExtraInfoGuard *get_thd_local_val_ptr();
  virtual int64_t to_string(char* buf, const int64_t buf_len) const;
private:
  static ObFatalErrExtraInfoGuard *&get_val();
  ObFatalErrExtraInfoGuard *last_;
};

inline int64_t get_cur_ts()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  const int64_t us =
            static_cast<int64_t>(tv.tv_sec) * static_cast<int64_t>(1000000) + static_cast<int64_t>(tv.tv_usec);
  return us;
}

class ObBasicTimeGuard
{
public:
  explicit ObBasicTimeGuard()
  {
    start_ts_ = get_cur_ts();
    last_ts_ = start_ts_;
    click_count_ = 0;
  }
  void click(const char *mod = NULL)
  {
    const int64_t cur_ts = get_cur_ts();
    if (OB_LIKELY(click_count_ < MAX_CLICK_COUNT)) {
      click_str_[click_count_] = mod;
      click_[click_count_++] = (int32_t)(cur_ts - last_ts_);
      last_ts_ = cur_ts;
    }
  }
  int64_t get_start_ts() const
  {
    return start_ts_;
  }
  int64_t get_diff() const
  {
    return get_cur_ts() - start_ts_;
  }
  int64_t to_string(char *buf, const int64_t buf_len) const;
protected:
  static const int64_t MAX_CLICK_COUNT = 16;
private:
  int64_t start_ts_;
  int64_t last_ts_;
  int64_t click_count_;
  int32_t click_[MAX_CLICK_COUNT];
  const char *click_str_[MAX_CLICK_COUNT];
};

} // end of namespace common
} // end of namespace oceanbase

#endif /* _OCEABASE_COMMON_OB_COMMON_UTILITY_H_ */
