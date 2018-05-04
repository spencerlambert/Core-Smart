// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef REWARDS_H
#define REWARDS_H

#include "amount.h"
#include "chain.h"
#include "coins.h"
#include "net.h"
#include "script/script_error.h"
#include "sync.h"
#include "versionbits.h"
#include "timedata.h"
#include "chainparams.h"
#include "txmempool.h"
#include "smartrewards/rewards.h"

#include <algorithm>
#include <exception>
#include <map>
#include <set>
#include <stdint.h>
#include <string>
#include <utility>
#include <vector>

#include <boost/unordered_map.hpp>
#include <boost/filesystem/path.hpp>

#include <smartrewards/rewardsdb.h>

using namespace std;

static const CAmount SMART_REWARDS_MIN_BALANCE = 1000 * COIN;
// Cache n blocks before the sync (leveldb batch write).
const int64_t nCacheBlocks = 50;

// Timestamps of the first round's start and end
const int64_t firstRoundStartTime = 1500966000;
const int64_t firstRoundEndTime = 1503644400;
const int64_t firstRoundStartBlock = 1;
const int64_t firstRoundEndBlock = 60001;

void ThreadSmartRewards();
CAmount CalculateRewardsForBlockRange(int64_t start, int64_t end);

struct CSmartRewardsUpdateResult
{
    int64_t disqualifiedEntries;
    int64_t disqualifiedSmart;
    CSmartRewardBlock block;
    CSmartRewardsUpdateResult() : disqualifiedEntries(0), disqualifiedSmart(0),block() {}
};

class CSmartRewards
{
    CSmartRewardsDB * pdb;

    std::vector<CSmartRewardBlock>blockEntries;
    std::vector<CSmartRewardEntry>updateEntries;
    std::vector<CSmartRewardEntry>removeEntries;

    mutable CCriticalSection csDb;

    void PrepareForUpdate(const CSmartRewardEntry &entry);
    void PrepareForRemove(const CSmartRewardEntry &entry);
    void ResetPrepared();
    bool AddBlock(const CSmartRewardBlock &block, bool sync);
public:

    CSmartRewards(CSmartRewardsDB *prewardsdb);

    bool GetLastBlock(CSmartRewardBlock &block);

    bool GetRound(const int number, CSmartRewardRound &round);
    bool GetCurrentRound(CSmartRewardRound &round);
    bool GetRewardRounds(std::vector<CSmartRewardRound> &vect);

    bool Verify();
    bool SyncPrepared();

    bool Update(CBlockIndex *pindexNew, const CChainParams& chainparams, CSmartRewardsUpdateResult &result, bool sync);
    bool UpdateCurrentRound(const CSmartRewardRound &round);
    bool UpdateRound(const CSmartRewardRound &round);

    bool GetRewardEntry(const CSmartRewardId &id, CSmartRewardEntry &entry);
    void GetRewardEntry(const CSmartRewardId &id, CSmartRewardEntry &entry, bool &added);
    bool GetRewardEntries(CSmartRewardEntryList &entries);
    void EvaluateRound(CSmartRewardRound &current, CSmartRewardRound &next, CSmartRewardEntryList &entries, CSmartRewardPayoutList &payouts);
    bool FinalizeRound(const CSmartRewardRound &next, const CSmartRewardEntryList &entries);
    bool FinalizeRound(const CSmartRewardRound &current, const CSmartRewardRound &next, const CSmartRewardEntryList &entries, const CSmartRewardPayoutList &payouts);

    bool GetRewardPayouts(const int16_t round, CSmartRewardPayoutList &payouts);
};

/** Global variable that points to the active rewards object (protected by cs_main) */
extern CSmartRewards *prewards;

#endif // REWARDS_H
