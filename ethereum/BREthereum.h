//
//  BREthereum
//  breadwallet-core Ethereum
//
//  Created by Ed Gamble on 2/24/18.
//  Copyright (c) 2018 breadwallet LLC
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

#ifndef BR_Ethereum_H
#define BR_Ethereum_H

#define SUPPORT_JSON_RPC

#include <stdint.h>
#include "BRKey.h"
#include "BRSet.h"
#include "base/BREthereumBase.h"
#include "blockchain/BREthereumAmount.h"
#include "blockchain/BREthereumNetwork.h"

#define BRArrayOf(type)    type*
#define BRSetOf(type)      BRSet*

#ifdef __cplusplus
extern "C" {
#endif

/**
 * An Ethereum Wallet Manager (EWM)
 *
 */
typedef struct BREthereumEWMRecord *BREthereumEWM;

// Opaque Pointers
typedef int32_t BREthereumTransactionId;
typedef int32_t BREthereumAccountId;
typedef int32_t BREthereumWalletId;
typedef int32_t BREthereumBlockId;
typedef int32_t BREthereumListenerId;

//
// Temporarily Here
//
typedef struct {
    BREthereumHash hash;
    BRRlpData blob;
} BREthereumPersistData;

static inline size_t
persistDataHashValue (const void *t)
{
    return hashSetValue(&((BREthereumPersistData*) t)->hash);
}

static inline int
persistDataHashEqual (const void *t1, const void *t2) {
    return t1 == t2 || hashSetEqual (&((BREthereumPersistData*) t1)->hash,
                                     &((BREthereumPersistData*) t2)->hash);
}


//
// Errors - Right Up Front - 'The Emperor Has No Clothes' ??
//
typedef enum {
    SUCCESS,

    // Reference access
    ERROR_UNKNOWN_NODE,
    ERROR_UNKNOWN_TRANSACTION,
    ERROR_UNKNOWN_ACCOUNT,
    ERROR_UNKNOWN_WALLET,
    ERROR_UNKNOWN_BLOCK,
    ERROR_UNKNOWN_LISTENER,

    // Node
    ERROR_NODE_NOT_CONNECTED,

    // Transaction
    ERROR_TRANSACTION_HASH_MISMATCH,
    ERROR_TRANSACTION_SUBMISSION,

    // Acount
    // Wallet
    // Block
    // Listener

    // Numeric
    ERROR_NUMERIC_PARSE,

} BREthereumStatus;

//
// BREthereumClient
//
// Type definitions for client functions.  When configuring a EWM these functions must be
// provided.  A EWM has limited cababilities; these callbacks provide data back into the
// EWM (such as with the 'gas price' or with the BRD-indexed logs for a given address) or they
// request certain data be saved to reestablish EWM state on start or they announce events
// signifying changes in EWM state.
//
typedef void *BREthereumClientContext;

typedef void
(*BREthereumClientHandlerGetBalance) (BREthereumClientContext context,
                                      BREthereumEWM ewm,
                                      BREthereumWalletId wid,
                                      const char *address,
                                      int rid);

typedef void
(*BREthereumClientHandlerGetGasPrice) (BREthereumClientContext context,
                                       BREthereumEWM ewm,
                                       BREthereumWalletId wid,
                                       int rid);

typedef void
(*BREthereumClientHandlerEstimateGas) (BREthereumClientContext context,
                                       BREthereumEWM ewm,
                                       BREthereumWalletId wid,
                                       BREthereumTransactionId tid,
                                       const char *to,
                                       const char *amount,
                                       const char *data,
                                       int rid);

typedef void
(*BREthereumClientHandlerSubmitTransaction) (BREthereumClientContext context,
                                             BREthereumEWM ewm,
                                             BREthereumWalletId wid,
                                             BREthereumTransactionId tid,
                                             const char *transaction,
                                             int rid);

typedef void
(*BREthereumClientHandlerGetTransactions) (BREthereumClientContext context,
                                           BREthereumEWM ewm,
                                           const char *address,
                                           int rid);

typedef void
(*BREthereumClientHandlerGetLogs) (BREthereumClientContext context,
                                   BREthereumEWM ewm,
                                   const char *contract,
                                   const char *address,
                                   const char *event,
                                   int rid);

typedef void
(*BREthereumClientHandlerGetBlockNumber) (BREthereumClientContext context,
                                          BREthereumEWM ewm,
                                          int rid);

typedef void
(*BREthereumClientHandlerGetNonce) (BREthereumClientContext context,
                                    BREthereumEWM ewm,
                                    const char *address,
                                    int rid);

//
// Save Sync (and other) State
//
typedef enum {
    CLIENT_CHANGE_ADD,
    CLIENT_CHANGE_REM,
    CLIENT_CHANGE_UPD
} BREthereumClientChangeType;

typedef void
(*BREthereumClientHandlerSaveBlocks) (BREthereumClientContext context,
                                      BREthereumEWM ewm,
                                      BRArrayOf(BREthereumPersistData) persistData);
typedef void
(*BREthereumClientHandlerSavePeers) (BREthereumClientContext context,
                                     BREthereumEWM ewm,
                                     BRArrayOf(BREthereumPersistData) persistData);

typedef void
(*BREthereumClientHandlerChangeTransaction) (BREthereumClientContext context,
                                             BREthereumEWM ewm,
                                             BREthereumClientChangeType type,
                                             BREthereumPersistData persistData);

typedef void
(*BREthereumClientHandlerChangeLog) (BREthereumClientContext context,
                                     BREthereumEWM ewm,
                                     BREthereumClientChangeType type,
                                     BREthereumPersistData persistData);

//
// Wallet Event
//
typedef enum {
    WALLET_EVENT_CREATED = 0,
    WALLET_EVENT_BALANCE_UPDATED,
    WALLET_EVENT_DEFAULT_GAS_LIMIT_UPDATED,
    WALLET_EVENT_DEFAULT_GAS_PRICE_UPDATED,
    WALLET_EVENT_DELETED
} BREthereumWalletEvent;

#define WALLET_NUMBER_OF_EVENTS  (1 + WALLET_EVENT_DELETED)

typedef void (*BREthereumClientHandlerWalletEvent) (BREthereumClientContext context,
                                                    BREthereumEWM ewm,
                                                    BREthereumWalletId wid,
                                                    BREthereumWalletEvent event,
                                                    BREthereumStatus status,
                                                    const char *errorDescription);

//
// Block Event
//
typedef enum {
    BLOCK_EVENT_CREATED = 0,

    BLOCK_EVENT_CHAINED,
    BLOCK_EVENT_ORPHANED,

    BLOCK_EVENT_DELETED
} BREthereumBlockEvent;

#define BLOCK_NUMBER_OF_EVENTS (1 + BLOCK_EVENT_DELETED)

typedef void (*BREthereumClientHandlerBlockEvent) (BREthereumClientContext context,
                                                   BREthereumEWM ewm,
                                                   BREthereumBlockId bid,
                                                   BREthereumBlockEvent event,
                                                   BREthereumStatus status,
                                                   const char *errorDescription);

//
// Transaction Event
//
typedef enum {
    // Added/Removed from Wallet
    TRANSACTION_EVENT_CREATED = 0,

    // Transaction State
    TRANSACTION_EVENT_SIGNED,
    TRANSACTION_EVENT_SUBMITTED,
    TRANSACTION_EVENT_BLOCKED,  // aka confirmed
    TRANSACTION_EVENT_ERRORED,


    TRANSACTION_EVENT_GAS_ESTIMATE_UPDATED,
    TRANSACTION_EVENT_BLOCK_CONFIRMATIONS_UPDATED,

    TRANSACTION_EVENT_DELETED

} BREthereumTransactionEvent;

#define TRANSACTION_NUMBER_OF_EVENTS (1 + TRANSACTION_EVENT_DELETED)

typedef void (*BREthereumClientHandlerTransactionEvent) (BREthereumClientContext context,
                                                         BREthereumEWM ewm,
                                                         BREthereumWalletId wid,
                                                         BREthereumTransactionId tid,
                                                         BREthereumTransactionEvent event,
                                                         BREthereumStatus status,
                                                         const char *errorDescription);

//
// Peer Event
//
typedef enum {
    PEER_EVENT_CREATED = 0,
    PEER_EVENT_DELETED
    // added/removed/updated
} BREthereumPeerEvent;

#define PEER_NUMBER_OF_EVENTS   (1 + PEER_EVENT_DELETED)

typedef void (*BREthereumClientHandlerPeerEvent) (BREthereumClientContext context,
                                                  BREthereumEWM ewm,
                                                  // BREthereumWalletId wid,
                                                  // BREthereumTransactionId tid,
                                                  BREthereumPeerEvent event,
                                                  BREthereumStatus status,
                                                  const char *errorDescription);
//
// EWM Event
//
typedef enum {
    EWM_EVENT_CREATED = 0,
    EWM_EVENT_SYNC_STARTED,
    EWM_EVENT_SYNC_CONTINUES,
    EWM_EVENT_SYNC_STOPPED,
    EWM_EVENT_NETWORK_UNAVAILABLE,
    EWM_EVENT_DELETED
} BREthereumEWMEvent;

#define EWM_NUMBER_OF_EVENTS   (1 + EWM_EVENT_DELETED)

typedef void (*BREthereumClientHandlerEWMEvent) (BREthereumClientContext context,
                                                 BREthereumEWM ewm,
                                                 // BREthereumWalletId wid,
                                                 // BREthereumTransactionId tid,
                                                 BREthereumEWMEvent event,
                                                 BREthereumStatus status,
                                                 const char *errorDescription);

//
// EWM Configuration
//
// Used to configure a EWM appropriately for JSON_RPC or LES.  Starts with a
// BREthereumNetwork (one of ethereum{Mainnet,Testnet,Rinkeby} and then specifics for the
// type.
//
typedef struct {
    BREthereumClientContext context;

    // Backend Server Support - typically implemented with HTTP requests for JSON_RPC or DB
    // queries.  All of these functions *must* callback to the EWM with a corresponding
    // 'announce' function.
    BREthereumClientHandlerGetBalance funcGetBalance;
    BREthereumClientHandlerGetGasPrice funcGetGasPrice;
    BREthereumClientHandlerEstimateGas funcEstimateGas;
    BREthereumClientHandlerSubmitTransaction funcSubmitTransaction;
    BREthereumClientHandlerGetTransactions funcGetTransactions; // announce one-by-one
    BREthereumClientHandlerGetLogs funcGetLogs; // announce one-by-one
    BREthereumClientHandlerGetBlockNumber funcGetBlockNumber;
    BREthereumClientHandlerGetNonce funcGetNonce;

    // Save Sync (and other) State - required as Core does maintain and is not configured to
    // use persistent storage (aka an sqlite DB or simply disk)
    BREthereumClientHandlerSavePeers funcSavePeers;
    BREthereumClientHandlerSaveBlocks funcSaveBlocks;
    BREthereumClientHandlerChangeTransaction funcChangeTransaction;
    BREthereumClientHandlerChangeTransaction funcChangeLog;

    // Events - Announce changes to entities that normally impact the UI.
    BREthereumClientHandlerEWMEvent funcEWMEvent;
    BREthereumClientHandlerPeerEvent funcPeerEvent;
    BREthereumClientHandlerWalletEvent funcWalletEvent;
    BREthereumClientHandlerBlockEvent funcBlockEvent;
    BREthereumClientHandlerTransactionEvent funcTransactionEvent;

} BREthereumClient;

/**
 * Install 'wordList' as the default BIP39 Word List.  THIS IS SHARED MEMORY; DO NOT FREE wordList.
 *
 * @param wordList
 * @param wordListLength
 * @return
 */
extern int
installSharedWordList (const char *wordList[], int wordListLength);

//
// Ethereum Wallet Manager
//

/*!
 * @typedef BREthereumType
 *
 * @abstract
 * Two types of EWM - JSON_RPC or LES (Light Ethereum Subprotocol).  For a LES EWM
 * some of the Client callbacks will only be used as a fallback.
 */
typedef enum {
    NODE_TYPE_NONE,
    NODE_TYPE_JSON_RPC,
    NODE_TYPE_LES
} BREthereumType;

/*!
 * @typedef BREthereumSyncMode
 *
 * @abstract When starting the EWM we can prime the transaction synchronization with
 * transactions queried from the Bread endpoint or we can use a full blockchain
 * synchronization.  (After the first full sync, partial syncs are used).
 */
typedef enum {
    SYNC_MODE_FULL_BLOCKCHAIN,
    SYNC_MODE_PRIME_WITH_ENDPOINT
} BREthereumSyncMode;

/**
 * Create a EWM managing the account associated with the paperKey.  (The `paperKey` must
 * use words from the defaul wordList (Use installSharedWordList).  The `paperKey` is used for
 * BIP-32 generation of keys; the same paper key must be used when signing transactions for
 * this EWM's account.
 */
extern BREthereumEWM
ethereumCreate(BREthereumNetwork network,
               const char *paperKey,
               BREthereumType type,
               BREthereumSyncMode syncMode,
               BREthereumClient client,
               BRArrayOf(BREthereumPersistData) peers,
               BRArrayOf(BREthereumPersistData) blocks,
               BRArrayOf(BREthereumPersistData) transactions,
               BRArrayOf(BREthereumPersistData) logs);

/**
 * Create a EWM managing the account associated with the publicKey.  Public key is a
 * 0x04-prefixed, 65-byte array in BRKey - as returned by
 * ethereumGetAccountPrimaryAddressPublicKey().
 */
extern BREthereumEWM
ethereumCreateWithPublicKey(BREthereumNetwork network,
                            const BRKey publicKey,
                            BREthereumType type,
                            BREthereumSyncMode syncMode,
                            BREthereumClient client,
                            BRArrayOf(BREthereumPersistData) peers,
                            BRArrayOf(BREthereumPersistData) blocks,
                            BRArrayOf(BREthereumPersistData) transactions,
                            BRArrayOf(BREthereumPersistData) logs);

/**
 * Create an Ethereum Account using `paperKey` for BIP-32 generation of keys.  The same paper key
 * must be used when signing transactions for this account.
 */
extern BREthereumAccountId
ethereumGetAccount(BREthereumEWM ewm);

/**
 * Get the primary address for `account`.  This is the '0x'-prefixed, 40-char, hex encoded
 * string.  The returned char* is newly allocated, on each call - you MUST free() it.
 */
extern char *
ethereumGetAccountPrimaryAddress(BREthereumEWM ewm);

/**
 * Get the public key for `account`.  This is a 0x04-prefixed, 65-byte array.  You own this
 * memory and you MUST free() it.
 */
extern BRKey
ethereumGetAccountPrimaryAddressPublicKey(BREthereumEWM ewm);

/**
 * Get the private key for `account`.
 */
extern BRKey
ethereumGetAccountPrimaryAddressPrivateKey(BREthereumEWM ewm,
                                           const char *paperKey);


/**
 * Create Ether from a string representing a number.  The string can *only* contain digits and
 * a single decimal point.  No '-', no '+' no 'e' (exponents).
 *
 * @param ewm
 * @param number the amount as a decimal (base10) number.
 * @param unit the number's unit - typically ETH, GWEI or WEI.
 * @param status This MUST NOT BE NULL. If assigned anything but OK, the return Ether is 0.  In
 *        practice you must reference `status` otherwise you'll have unknown errors with 0 ETH.
 */
extern BREthereumAmount
ethereumCreateEtherAmountString(BREthereumEWM ewm,
                                const char *number,
                                BREthereumEtherUnit unit,
                                BRCoreParseStatus *status);

/**
 * Create Ether from a 'smallish' number and a unit
 *
 */
extern BREthereumAmount
ethereumCreateEtherAmountUnit(BREthereumEWM ewm,
                              uint64_t amountInUnit,
                              BREthereumEtherUnit unit);

extern BREthereumAmount
ethereumCreateTokenAmountString(BREthereumEWM ewm,
                                BREthereumToken token,
                                const char *number,
                                BREthereumTokenQuantityUnit unit,
                                BRCoreParseStatus *status);

/**
 * Convert `ether` to a char* in `unit`.  Caller owns the result and must call free()
 */
extern char *
ethereumCoerceEtherAmountToString(BREthereumEWM ewm,
                                  BREthereumEther ether,
                                  BREthereumEtherUnit unit);

extern char *
ethereumCoerceTokenAmountToString(BREthereumEWM ewm,
                                  BREthereumTokenQuantity token,
                                  BREthereumTokenQuantityUnit unit);

/**
 * Connect to the Ethereum Network;
 *
 * @param ewm
 * @return
 */
extern BREthereumBoolean
ethereumConnect(BREthereumEWM ewm);

extern BREthereumBoolean
ethereumDisconnect (BREthereumEWM ewm);

extern void
ethereumDestroy (BREthereumEWM ewm);

extern BREthereumNetwork
ethereumGetNetwork (BREthereumEWM ewm);

//
// Wallet
//
/**
 * Get the wallet for `account` holding ETHER.  This wallet is created, along with the account,
 * when a EWM itself is created.
 *
 * @param ewm
 * @return
 */
extern BREthereumWalletId
ethereumGetWallet(BREthereumEWM ewm);

/**
 * Get the wallet holding `token`.  If none exists, create one and return it.

 * @param ewm
 * @param token
 * @return
 */
extern BREthereumWalletId
ethereumGetWalletHoldingToken(BREthereumEWM ewm,
                              BREthereumToken token);

extern uint64_t
ethereumWalletGetDefaultGasLimit(BREthereumEWM ewm,
                                 BREthereumWalletId wid);

extern void
ethereumWalletSetDefaultGasLimit(BREthereumEWM ewm,
                                 BREthereumWalletId wid,
                                 uint64_t gasLimit);

extern uint64_t
ethereumWalletGetGasEstimate(BREthereumEWM ewm,
                             BREthereumWalletId wid,
                             BREthereumTransactionId transaction);

extern void
ethereumWalletSetDefaultGasPrice(BREthereumEWM ewm,
                                 BREthereumWalletId wid,
                                 BREthereumEtherUnit unit,
                                 uint64_t value);

// Returns the ETH/GAS price in WEI.  IF the value is too large to express in WEI as a uint64_t
// then ZERO is returned.  Caution warranted.
extern uint64_t
ethereumWalletGetDefaultGasPrice(BREthereumEWM ewm,
                                 BREthereumWalletId wid);

extern BREthereumAmount
ethereumWalletGetBalance(BREthereumEWM ewm,
                         BREthereumWalletId wid);

extern char *
ethereumWalletGetBalanceEther(BREthereumEWM ewm,
                              BREthereumWalletId wid,
                              BREthereumEtherUnit unit);
extern char *
ethereumWalletGetBalanceTokenQuantity(BREthereumEWM ewm,
                                      BREthereumWalletId wid,
                                      BREthereumTokenQuantityUnit unit);

extern BREthereumEther
ethereumWalletEstimateTransactionFee(BREthereumEWM ewm,
                                     BREthereumWalletId wid,
                                     BREthereumAmount amount,
                                     int *overflow);

/**
 * Create a transaction to transfer `amount` from `wallet` to `recvAddrss`.
 *
 * @param ewm
 * @param wallet the wallet
 * @param recvAddress A '0x' prefixed, strlen 42 Ethereum address.
 * @param amount to transfer
 * @return
 */
extern BREthereumTransactionId
ethereumWalletCreateTransaction(BREthereumEWM ewm,
                                BREthereumWalletId wid,
                                const char *recvAddress,
                                BREthereumAmount amount);

/**
 * Sign the transaction using the wallet's account (for the sender's address).  The paperKey
 * is used to 'lookup' the private key.
 *
 * @param ewm
 * @param wallet
 * @param transaction
 * @param paperKey
 */
extern void
ethereumWalletSignTransaction(BREthereumEWM ewm,
                              BREthereumWalletId wid,
                              BREthereumTransactionId tid,
                              const char *paperKey);

extern void
ethereumWalletSignTransactionWithPrivateKey(BREthereumEWM ewm,
                                            BREthereumWalletId wid,
                                            BREthereumTransactionId tid,
                                            BRKey privateKey);

extern void
ethereumWalletSubmitTransaction(BREthereumEWM ewm,
                                BREthereumWalletId wid,
                                BREthereumTransactionId tid);

/**
 * Returns a -1 terminated array of transaction identifiers.
 */
extern BREthereumTransactionId *
ethereumWalletGetTransactions(BREthereumEWM ewm,
                              BREthereumWalletId wid);

/**
 * Returns -1 on invalid wid
 */
extern int
ethereumWalletGetTransactionCount(BREthereumEWM ewm,
                                  BREthereumWalletId wid);

/**
 * Token can be NULL => holds Ether
 *
 * @param ewm
 * @param wid
 * @param token
 * @return
 */
extern BREthereumBoolean
ethereumWalletHoldsToken(BREthereumEWM ewm,
                         BREthereumWalletId wid,
                         BREthereumToken token);

extern BREthereumToken
ethereumWalletGetToken(BREthereumEWM ewm,
                       BREthereumWalletId wid);

//
// Block
//
extern uint64_t
ethereumGetBlockHeight (BREthereumEWM ewm);

extern uint64_t
ethereumBlockGetNumber (BREthereumEWM ewm,
                        BREthereumBlockId bid);

extern uint64_t
ethereumBlockGetTimestamp (BREthereumEWM ewm,
                           BREthereumBlockId bid);

extern char *
ethereumBlockGetHash (BREthereumEWM ewm,
                      BREthereumBlockId bid);

//
//
// Transaction
//
//
extern char * // receiver, target
ethereumTransactionGetRecvAddress(BREthereumEWM ewm,
                                  BREthereumTransactionId tid);

extern char * // sender, source
ethereumTransactionGetSendAddress(BREthereumEWM ewm,
                                  BREthereumTransactionId tid);

extern char *
ethereumTransactionGetHash(BREthereumEWM ewm,
                           BREthereumTransactionId tid);

extern char *
ethereumTransactionGetAmountEther(BREthereumEWM ewm,
                                  BREthereumTransactionId tid,
                                  BREthereumEtherUnit unit);

extern char *
ethereumTransactionGetAmountTokenQuantity(BREthereumEWM ewm,
                                          BREthereumTransactionId tid,
                                          BREthereumTokenQuantityUnit unit);

extern BREthereumAmount
ethereumTransactionGetAmount(BREthereumEWM ewm,
                             BREthereumTransactionId tid);

extern BREthereumAmount
ethereumTransactionGetGasPriceToo(BREthereumEWM ewm,
                                  BREthereumTransactionId tid);

extern char *
ethereumTransactionGetGasPrice(BREthereumEWM ewm,
                               BREthereumTransactionId tid,
                               BREthereumEtherUnit unit);

extern uint64_t
ethereumTransactionGetGasLimit(BREthereumEWM ewm,
                               BREthereumTransactionId tid);

extern uint64_t
ethereumTransactionGetGasUsed(BREthereumEWM ewm,
                              BREthereumTransactionId tid);

extern uint64_t
ethereumTransactionGetNonce(BREthereumEWM ewm,
                            BREthereumTransactionId transaction);

extern BREthereumHash
ethereumTransactionGetBlockHash(BREthereumEWM ewm,
                                BREthereumTransactionId tid);

extern uint64_t
ethereumTransactionGetBlockNumber(BREthereumEWM ewm,
                                  BREthereumTransactionId tid);

extern uint64_t
ethereumTransactionGetBlockConfirmations(BREthereumEWM ewm,
                                         BREthereumTransactionId tid);

extern BREthereumBoolean
ethereumTransactionIsConfirmed(BREthereumEWM ewm,
                               BREthereumTransactionId tid);

extern BREthereumBoolean
ethereumTransactionIsSubmitted(BREthereumEWM ewm,
                               BREthereumTransactionId tid);

extern BREthereumBoolean
ethereumTransactionHoldsToken(BREthereumEWM ewm,
                              BREthereumTransactionId tid,
                              BREthereumToken token);

extern BREthereumToken
ethereumTransactionGetToken(BREthereumEWM ewm,
                            BREthereumTransactionId tid);

extern BREthereumEther
ethereumTransactionGetFee(BREthereumEWM ewm,
                          BREthereumTransactionId tid,
                          int *overflow);

/**
 * Return the serialized raw data for `transaction`.  The value `*bytesPtr` points to a byte array;
 * the callee OWNs that byte array (and thus must call free).  The value `*bytesCountPtr` holds
 * the size of the byte array.
 *
 * @param ewm
 * @param transaction
 * @param bytesPtr
 * @param bytesCountPtr
 */

extern void
ethereumTransactionFillRawData(BREthereumEWM ewm,
                               BREthereumWalletId wid,
                               BREthereumTransactionId tid,
                               uint8_t **bytesPtr,
                               size_t *bytesCountPtr);

extern const char *
ethereumTransactionGetRawDataHexEncoded(BREthereumEWM ewm,
                                        BREthereumWalletId wid,
                                        BREthereumTransactionId tid,
                                        const char *prefix);


//
// Ethereum Client - Callbacks to announce BRD endpoint data.  These function are called by
// the IOS and Android (using JNI) apps in their thread/memory context.  Thus any pointer arguments,
// typically `char*`, will only reference valid memory in the calling context.  Therefore, the
// implementations of these functions must a) use the pointers to extract a new type, such as
// `BREthereumHash or uint64_t, or b) deep copy the calling context pointer.
//
extern BREthereumStatus
ethereumClientAnnounceBlockNumber (BREthereumEWM ewm,
                                   const char *blockNumber,
                                   int rid);

extern BREthereumStatus
ethereumClientAnnounceNonce (BREthereumEWM ewm,
                             const char *strAddress,
                             const char *strNonce,
                             int rid);

extern BREthereumStatus
ethereumClientAnnounceBalance (BREthereumEWM ewm,
                               BREthereumWalletId wid,
                               const char *balance,
                               int rid);

extern BREthereumStatus
ethereumClientAnnounceGasPrice(BREthereumEWM ewm,
                               BREthereumWalletId wid,
                               const char *gasEstimate,
                               int rid);

extern BREthereumStatus
ethereumClientAnnounceGasEstimate (BREthereumEWM ewm,
                                   BREthereumWalletId wid,
                                   BREthereumTransactionId tid,
                                   const char *gasEstimate,
                                   int rid);

extern BREthereumStatus
ethereumClientAnnounceSubmitTransaction(BREthereumEWM ewm,
                                        BREthereumWalletId wid,
                                        BREthereumTransactionId tid,
                                        const char *hash,
                                        int rid);

// A JSON_RPC call will occur to get all transactions associated with an account.  We'll
// process these transactions into the EWM (associated with a wallet).  Thereafter
// a 'EWM client' can get the announced transactions using non-JSON_RPC interfaces.
extern BREthereumStatus
ethereumClientAnnounceTransaction (BREthereumEWM ewm,
                                   int id,
                                   const char *hash,
                                   const char *from,
                                   const char *to,
                                   const char *contract,
                                   const char *amount, // value
                                   const char *gasLimit,
                                   const char *gasPrice,
                                   const char *data,
                                   const char *nonce,
                                   const char *gasUsed,
                                   const char *blockNumber,
                                   const char *blockHash,
                                   const char *blockConfirmations,
                                   const char *blockTransactionIndex,
                                   const char *blockTimestamp,
                                   // cumulative gas used,
                                   // confirmations
                                   // txreceipt_status
                                   const char *isError);

extern BREthereumStatus
ethereumClientAnnounceLog (BREthereumEWM ewm,
                           int id,
                           const char *strHash,
                           const char *strContract,
                           int topicCount,
                           const char **arrayTopics,
                           const char *strData,
                           const char *strGasPrice,
                           const char *strGasUsed,
                           const char *strLogIndex,
                           const char *strBlockNumber,
                           const char *strBlockTransactionIndex,
                           const char *strBlockTimestamp);

#ifdef __cplusplus
}
#endif

#endif // BR_Ethereum_H
