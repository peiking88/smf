// Copyright (c) 2016 Alexander Gallego. All rights reserved.
//
#include "smf/histogram.h"

#include <cstdlib>
#ifdef HAVE_HDR_HISTOGRAM
#include <hdr_histogram.h>
#include <hdr_histogram_log.h>
#endif

#include <iostream>

namespace smf {
seastar::lw_shared_ptr<histogram>
histogram::make_lw_shared(int64_t max_value) {
  auto x = seastar::make_lw_shared<histogram>(max_value);
#ifdef HAVE_HDR_HISTOGRAM
  assert(x->hist_->hist);
#endif
  return x;
}
std::unique_ptr<histogram>
histogram::make_unique(int64_t max_value) {
  std::unique_ptr<histogram> p(new histogram(max_value));
#ifdef HAVE_HDR_HISTOGRAM
  assert(p->hist_->hist);
#endif
  return p;
}

histogram::histogram(int64_t max_value)
  : hist_(std::make_unique<hist_t>(max_value)) {}
histogram::histogram(histogram &&o) noexcept : hist_(std::move(o.hist_)) {}

histogram &
histogram::operator+=(const histogram &o) {
#ifdef HAVE_HDR_HISTOGRAM
  ::hdr_add(hist_->hist, o.hist_->hist);
#endif
  return *this;
}
histogram &
histogram::operator+=(const hist_t *o) {
#ifdef HAVE_HDR_HISTOGRAM
  ::hdr_add(hist_->hist, o->hist);
#endif
  return *this;
}

histogram &
histogram::operator=(histogram &&o) noexcept {
  hist_ = std::move(o.hist_);
  return *this;
}

void
histogram::record(const uint64_t &v) {
  hist_->sample_count++;
  hist_->sample_sum += v;
#ifdef HAVE_HDR_HISTOGRAM
  ::hdr_record_value(hist_->hist, v);
#endif
}

void
histogram::record_multiple_times(const uint64_t &v, const uint32_t &times) {
  hist_->sample_count += times;
  hist_->sample_sum += v * times;
#ifdef HAVE_HDR_HISTOGRAM
  ::hdr_record_values(hist_->hist, v, times);
#endif
}

void
histogram::record_corrected(const uint64_t &v, const uint64_t &interval) {
  // TODO: how should summation work for coordinated omission values? the sum is
  // tracked outside hdr, currently.
  hist_->sample_count++;
  hist_->sample_sum += v;
#ifdef HAVE_HDR_HISTOGRAM
  ::hdr_record_corrected_value(hist_->hist, v, interval);
#endif
}

int64_t
histogram::value_at(double percentile) const {
#ifdef HAVE_HDR_HISTOGRAM
  return ::hdr_value_at_percentile(hist_->hist, percentile);
#else
  return 0;
#endif
}
double
histogram::stddev() const {
#ifdef HAVE_HDR_HISTOGRAM
  return ::hdr_stddev(hist_->hist);
#else
  return 0.0;
#endif
}
double
histogram::mean() const {
#ifdef HAVE_HDR_HISTOGRAM
  return ::hdr_mean(hist_->hist);
#else
  return 0.0;
#endif
}
size_t
histogram::memory_size() const {
#ifdef HAVE_HDR_HISTOGRAM
  return ::hdr_get_memory_size(hist_->hist);
#else
  return 0;
#endif
}

#ifdef HAVE_HDR_HISTOGRAM
hdr_histogram *
histogram::get() {
  assert(hist_);
  return hist_->hist;
}
#else
void *
histogram::get() {
  assert(hist_);
  return nullptr;
}
#endif

std::unique_ptr<histogram_measure>
histogram::auto_measure() {
  return std::make_unique<histogram_measure>(shared_from_this());
}

int
histogram::print(FILE *fp) const {
  assert(fp != nullptr);
#ifdef HAVE_HDR_HISTOGRAM
  return ::hdr_percentiles_print(hist_->hist,
                                 fp,        // File to write to
                                 5,         // Granularity of printed values
                                 1.0,       // Multiplier for results
                                 CLASSIC);  // Format CLASSIC/CSV supported.
#else
  return 0;
#endif
}

seastar::metrics::histogram
histogram::seastar_histogram_logform() const {
  seastar::metrics::histogram sshist;
  
#ifdef HAVE_HDR_HISTOGRAM
  // logarithmic histogram configuration. this will range from 10 microseconds
  // through around 6000 seconds with 26 buckets doubling.
  //
  // TODO:
  //   1) expose these settings through arguments
  //   2) upstream log_base fix for sub-2.0 values. in hdr the log_base is a
  //   double but is truncated (see the int64_t casts on log_base below which is
  //   the same as in the hdr C library). this means that if we want buckets
  //   with a log base of 1.5, the histogram becomes linear...
  constexpr size_t num_buckets = 26;
  constexpr int64_t first_value = 10;
  constexpr double log_base = 2.0;

  sshist.buckets.resize(num_buckets);

  sshist.sample_count = hist_->sample_count;
  sshist.sample_sum = static_cast<double>(hist_->sample_sum);

  // stack allocated; no cleanup needed
  struct hdr_iter iter;
  struct hdr_iter_log *log = &iter.specifics.log;
  hdr_iter_log_init(&iter, hist_->hist, first_value, log_base);

  // fill in buckets from hdr histogram logarithmic iteration. there may be more
  // or less hdr buckets reported than what will be returned to seastar.
  size_t bucket_idx = 0;
  for (; hdr_iter_next(&iter) && bucket_idx < sshist.buckets.size();
       bucket_idx++) {
    auto &bucket = sshist.buckets[bucket_idx];
    bucket.count = iter.cumulative_count;
    bucket.upper_bound = iter.value_iterated_to;
  }

  if (bucket_idx == 0) {
    // if the histogram is empty hdr_iter_init doesn't initialize the first
    // bucket value which is neede by the loop below.
    iter.value_iterated_to = first_value;
  } else if (bucket_idx < sshist.buckets.size()) {
    // if there are padding buckets that need to be created, advance the bucket
    // boundary which would normally be done by hdr_iter_next, except that
    // doesn't happen when iteration reaches the end of the recorded values.
    iter.value_iterated_to *= static_cast<int64_t>(log->log_base);
  }

  // prometheus expects a fixed number of buckets. hdr iteration will stop after
  // the max observed value. this loops pads buckets past iteration, if needed.
  for (; bucket_idx < sshist.buckets.size(); bucket_idx++) {
    auto &bucket = sshist.buckets[bucket_idx];
    bucket.count = iter.cumulative_count;
    bucket.upper_bound = iter.value_iterated_to;
    iter.value_iterated_to *= static_cast<int64_t>(log->log_base);
  }
#else
  // Simplified version when Hdrhistogram is not available
  sshist.sample_count = hist_->sample_count;
  sshist.sample_sum = static_cast<double>(hist_->sample_sum);
  // Create a simple bucket structure
  sshist.buckets.resize(1);
  sshist.buckets[0].count = hist_->sample_count;
  sshist.buckets[0].upper_bound = kDefaultHistogramMaxValue;
#endif

  return sshist;
}

histogram::~histogram() {}

}  // namespace smf
