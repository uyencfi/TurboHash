#pragma once
#include <inttypes.h>
#include <string>

namespace turbo {

const int kMaxProbeLen = 17;
const int kProbeStep = 1;

class ProbeWithinCell {
public:
    static const int MAX_PROBE_LEN = kMaxProbeLen;
    static const int PROBE_STEP = kProbeStep;

    ProbeWithinCell(uint64_t initial_hash, uint32_t associate_mask, uint32_t bucket_i) {
        h_               = initial_hash;
        associate_mask_  = associate_mask;
        associate_index_ = h_ & associate_mask_;
        bucket_i_        = bucket_i;
        probe_count_     = 0;
    }

    inline void reset() {
        associate_index_ = h_ & associate_mask_;
        probe_count_     = 0;
    }
    // indicate whether we have already probed all the assocaite cells
    inline operator bool() const {
        return probe_count_ < 1;
    }

    inline void next() {
        associate_index_ += PROBE_STEP;
        // assocaite_index mod AssociateCount, 
        // AssociateMask should be like 0b11
        associate_index_ &= associate_mask_;
        probe_count_++;
    }

    inline std::pair<uint32_t, uint32_t> offset() {
        return {bucket_i_, associate_index_};
    }

    static std::string name() {
        return "ProbeWithinCell";
    }
private:
    uint64_t  h_;
    uint32_t  associate_mask_;
    uint32_t  associate_index_;
    uint32_t  bucket_i_;
    uint32_t  probe_count_;
};


/**
 *  AssociateMask is used to mod associate_count
*/
class ProbeWithinBucket {
public:
    static const int MAX_PROBE_LEN = kMaxProbeLen;
    static const int PROBE_STEP = kProbeStep;
    ProbeWithinBucket(uint64_t initial_hash, uint32_t associate_mask, uint32_t bucket_i) {
        h_               = initial_hash;
        associate_mask_  = associate_mask;
        associate_index_ = h_ & associate_mask_;
        bucket_i_        = bucket_i;
        probe_count_     = 0;
    }

    inline void reset() {
        associate_index_ = h_ & associate_mask_;
        probe_count_     = 0;
    }
    
    // indicate whether we have already probed all the assocaite cells
    inline operator bool() const {
        return probe_count_ <= associate_mask_;
    }

    inline void next() {
        associate_index_ += PROBE_STEP;
        // assocaite_index mod AssociateCount, 
        // AssociateMask should be like 0b11
        associate_index_ &= associate_mask_;
        probe_count_++;
    }

    inline std::pair<uint32_t, uint32_t> offset() {
        return {bucket_i_, associate_index_};
    }

    static std::string name() {
        return "ProbeWithinBucket";
    }
private:
    uint64_t  h_;
    uint32_t  associate_mask_;
    uint32_t  associate_index_;
    uint32_t  bucket_i_;
    uint32_t  probe_count_;
};

// /**
//  *  Probe in one bucket and its next bucket
// */
// class ProbeContinousTwoBucket{
// public:
//     static const int MAX_PROBE_LEN = kMaxProbeLen;
//     ProbeContinousTwoBucket(uint64_t initial_hash, uint32_t associate_mask, uint32_t bucket_i, uint32_t bucket_count) {
//         h_               = initial_hash;
//         associate_mask_  = associate_mask;
//         associate_index_ = h_ & associate_mask_;
//         bucket_i_        = bucket_i;
//         bucket_count_    = bucket_count;
//         probe_count_     = 0;
//     }

//     void reset() {
//         associate_index_ = h_ & associate_mask_;
//         probe_count_     = 0;
//     }
//     // indicate whether we have already probed all the assocaite cells
//     operator bool() const {
//         return (probe_count_ >> 1) <= associate_mask_;
//     }

//     void next() {
//         associate_index_++;
//         // assocaite_index mod AssociateCount, 
//         // AssociateMask should be like 0b11
//         associate_index_ &= associate_mask_;
//         if (probe_count_ == associate_mask_) {
//             bucket_i_++;
//             if (bucket_i_ >= bucket_count_) {
//                 bucket_i_ = 0;
//             }
//         }
//         probe_count_++;
//     }

//     static std::string name() {
//         return "ProbeContinousTwoBucket";
//     }

//     std::pair<uint32_t, uint32_t> offset() {
//         return {bucket_i_, associate_index_};
//     }

// private:
//     uint64_t  h_;
//     uint32_t  associate_mask_;
//     uint32_t  associate_index_;
//     uint32_t  bucket_i_;
//     uint32_t  bucket_count_;
//     uint32_t  probe_count_;
// };


}