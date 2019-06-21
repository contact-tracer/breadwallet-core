//
//  BRWalletManagerPrivate.h
//  BRCore
//
//  Created by Michael Carrara on 3/19/19.
//  Copyright (c) 2019 breadwallet LLC
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.

#ifndef BRWalletManagerPrivate_h
#define BRWalletManagerPrivate_h

#include "ethereum/event/BREvent.h"
#include "support/BRFileService.h"

#ifdef __cplusplus
extern "C" {
#endif

/// MARK: - BRWalletManager

struct BRWalletManagerStruct {

    /** The mode */
    BRSyncMode mode;

    /** The wallet */
    BRWallet *wallet;

    /** The peer manager */
    BRPeerManager  *peerManager;

    /** The client */
    BRWalletManagerClient client;

    /** The file service */
    BRFileService fileService;

    /**
     * The BlockHeight is the largest block number seen
     */
    uint32_t blockHeight;

    /**
     * An identiifer for a BRD Request
     */
    unsigned int requestId;

    /**
     * An EventHandler for Main.  All 'announcements' (via PeerManager (or BRD) hit here.
     */
    BREventHandler handler;

    /**
     * The Lock ensuring single thread access to BWM state.
     */
    pthread_mutex_t lock;

    /**
     * If we are syncing with BRD, instead of as P2P with PeerManager, then we'll keep a record to
     * ensure we've successfully completed the getTransactions() callbacks to the client.
     */
    struct {
        BRAddress lastExternalAddress;
        BRAddress lastInternalAddress;
        uint64_t begBlockNumber;
        uint64_t endBlockNumber;

        int rid;

        int completed:1;
    } brdSync;
};

/// MARK: - BlockNumber

extern int
bwmHandleAnnounceBlockNumber (BRWalletManager manager,
                              int rid,
                              uint64_t blockNumber);

extern void
bwmSignalAnnounceBlockNumber (BRWalletManager manager,
                              int rid,
                              uint64_t blockNumber);

/// MARK: - Transaction

extern int
bwmHandleAnnounceTransaction (BRWalletManager manager,
                              int id,
                              BRTransaction *transaction);

extern void
bwmSignalAnnounceTransaction (BRWalletManager manager,
                              int id,
                              BRTransaction *transaction);

extern void
bwmHandleAnnounceTransactionComplete (BRWalletManager manager,
                                      int rid,
                                      int success);

extern void
bwmSignalAnnounceTransactionComplete (BRWalletManager manager,
                                      int rid,
                                      int success);

/// MARK: - Submit

extern void
bwmHandleAnnounceSubmit (BRWalletManager manager,
                         int rid,
                         BRTransaction *transaction,
                         int error);

extern void
bwmSignalAnnounceSubmit (BRWalletManager manager,
                        int rid,
                        BRTransaction *transaction,
                        int error);

extern const BREventType *bwmEventTypes[];
extern const unsigned int bwmEventTypesCount;

#ifdef __cplusplus
}
#endif


#endif /* BRWalletManagerPrivate_h */