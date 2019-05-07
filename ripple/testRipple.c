//
//  BRRipple.h
//  Core
//
//  Created by Carl Cherry on 4/16/19.
//  Copyright © 2019 Breadwinner AG. All rights reserved.
//
//  See the LICENSE file at the project root for license information.
//  See the CONTRIBUTORS file at the project root for a list of contributors.
//
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "BRRipple.h"
#include "BRRippleBase58.h"
#include "BRCrypto.h"
#include "support/BRBIP32Sequence.h"
#include "support/BRBIP39WordsEn.h"
#include "BRKey.h"

static BRRippleAccount createTestRippleAccount(const char* paper_key,
                                           const char* expected_account_address)
{
    BRRippleAccount account = rippleAccountCreate(paper_key);
    
    // Get the string ripple address
    if (expected_account_address) {
        char * accountAddress = getRippleAddress(account);
        assert(0 == memcmp(expected_account_address, accountAddress, strlen(accountAddress)));
    }
    return account;
}

static void
testRippleTransaction (void /* ... */) {
    BRRippleTransaction transaction;
    
    uint8_t expected_output[] = {
        0x12, 0x00, 0x00, // transaction type - 16-bit (1,2)
        0x24, 0x00, 0x00, 0x00, 0x01, // Sequence - 32-bit (2,4)
        0x61, 0x40, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x42, 0x40, // Amount 1,000,000 - 64-bit
        0x68, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, // Fee 12 - 64-bit
        0x73, 0x21, 0x03, 0x2b, 0xe3, 0xce, 0xe5, 0x76, 0x03, // Public Key
        0x6f, 0x90, 0x76, 0x59, 0x6f, 0xc4, 0xf8, 0xd4, 0xa2,
        0xa0, 0x57, 0xf8, 0xf8, 0x65, 0x48, 0xc0, 0x98, 0x0e,
        0x02, 0x13, 0x16, 0x07, 0x2f, 0xed, 0xa2, 0xd3,
        0x81, 0x14, 0xB5, 0xF7, 0x62, 0x79, 0x8A, 0x53, 0xD5, // Source Address - type 8
        0x43, 0xA0, 0x14, 0xCA, 0xF8, 0xB2, 0x97, 0xCF, 0xF8, 0xF2, 0xF9, 0x37, 0xE8,
        0x83, 0x14, 0xB6, 0xF8, 0x63, 0x80, 0x8B, 0x54, 0xD6, // Target Address - type 8
        0x43, 0xA0, 0x14, 0xCA, 0xF8, 0xB2, 0x97, 0xCF, 0xF8, 0xF2, 0xF9, 0x37, 0xE8
    };

    // Here are the 20 byte addresses
    uint8_t sourceBytes[] = { 0xB5, 0xF7, 0x62, 0x79, 0x8A, 0x53, 0xD5,
        0x43, 0xA0, 0x14, 0xCA, 0xF8, 0xB2, 0x97, 0xCF, 0xF8, 0xF2, 0xF9, 0x37, 0xE8 };
    uint8_t destBytes[] = { 0xB6, 0xF8, 0x63, 0x80, 0x8B, 0x54, 0xD6,
        0x43, 0xA0, 0x14, 0xCA, 0xF8, 0xB2, 0x97, 0xCF, 0xF8, 0xF2, 0xF9, 0x37, 0xE8 };
    BRRippleAddress sourceAddress;
    BRRippleAddress targetAddress;
    memcpy(sourceAddress.bytes, sourceBytes, 20);
    memcpy(targetAddress.bytes, destBytes, 20);
    
    // Create an account so we can get a public key
    const char * paper_key = "patient doctor olympic frog force glimpse endless antenna online dragon bargain someone";
    BRRippleAccount account = rippleAccountCreate(paper_key);
    transaction = rippleTransactionCreate(sourceAddress, targetAddress, 1000000, 12);
    assert(transaction);

    rippleAccountDelete(account);
    deleteRippleTransaction(transaction);
}

static void
testRippleTransactionGetters (void /* ... */) {
    BRRippleTransaction transaction;
    
    uint8_t expected_output[] = {
        0x12, 0x00, 0x00, // transaction type - 16-bit (1,2)
        0x24, 0x00, 0x00, 0x00, 0x01, // Sequence - 32-bit (2,4)
        0x61, 0x40, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x42, 0x40, // Amount 1,000,000 - 64-bit
        0x68, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, // Fee 12 - 64-bit
        0x73, 0x21, 0x03, 0x2b, 0xe3, 0xce, 0xe5, 0x76, 0x03, // Public Key
        0x6f, 0x90, 0x76, 0x59, 0x6f, 0xc4, 0xf8, 0xd4, 0xa2,
        0xa0, 0x57, 0xf8, 0xf8, 0x65, 0x48, 0xc0, 0x98, 0x0e,
        0x02, 0x13, 0x16, 0x07, 0x2f, 0xed, 0xa2, 0xd3,
        0x81, 0x14, 0xB5, 0xF7, 0x62, 0x79, 0x8A, 0x53, 0xD5, // Source Address - type 8
        0x43, 0xA0, 0x14, 0xCA, 0xF8, 0xB2, 0x97, 0xCF, 0xF8, 0xF2, 0xF9, 0x37, 0xE8,
        0x83, 0x14, 0xB6, 0xF8, 0x63, 0x80, 0x8B, 0x54, 0xD6, // Target Address - type 8
        0x43, 0xA0, 0x14, 0xCA, 0xF8, 0xB2, 0x97, 0xCF, 0xF8, 0xF2, 0xF9, 0x37, 0xE8
    };
    
    // Here are the 20 byte addresses
    uint8_t sourceBytes[] = { 0xB5, 0xF7, 0x62, 0x79, 0x8A, 0x53, 0xD5,
        0x43, 0xA0, 0x14, 0xCA, 0xF8, 0xB2, 0x97, 0xCF, 0xF8, 0xF2, 0xF9, 0x37, 0xE8 };
    uint8_t destBytes[] = { 0xB6, 0xF8, 0x63, 0x80, 0x8B, 0x54, 0xD6,
        0x43, 0xA0, 0x14, 0xCA, 0xF8, 0xB2, 0x97, 0xCF, 0xF8, 0xF2, 0xF9, 0x37, 0xE8 };
    BRRippleAddress sourceAddress;
    BRRippleAddress targetAddress;
    memcpy(sourceAddress.bytes, sourceBytes, 20);
    memcpy(targetAddress.bytes, destBytes, 20);
    
    // Create an account so we can get a public key
    const char * paper_key = "patient doctor olympic frog force glimpse endless antenna online dragon bargain someone";
    BRRippleAccount account = rippleAccountCreate(paper_key);
    transaction = rippleTransactionCreate(sourceAddress, targetAddress, 1000000, 12);
    assert(transaction);

    uint64_t fee = rippleTransactionGetFee(transaction);
    assert(fee == 12);

    uint64_t amount = rippleTransactionGetAmount(transaction);
    assert(amount == 1000000);

    uint32_t sequence = rippleTransactionGetSequence(transaction);
    // Since we don't add the sequence until later - it should be 0
    assert(0 == sequence);

    BRRippleAddress source = rippleTransactionGetSource(transaction);
    assert(0 == memcmp(source.bytes, sourceAddress.bytes, 20));

    BRRippleAddress target = rippleTransactionGetTarget(transaction);
    assert(0 == memcmp(target.bytes, targetAddress.bytes, 20));
    
    rippleAccountDelete(account);
    deleteRippleTransaction(transaction);
}

static void
testCreateRippleAccountWithPaperKey (void /* ... */) {
    const char * paper_key = "patient doctor olympic frog force glimpse endless antenna online dragon bargain someone";
    // The above set of words should produce the following Ripple account address
    // string: r41vZ8exoVyUfVzs56yeN8xB5gDhSkho9a
    // raw bytes - EF FC 27 52 B5 C9 DA 22 88 C5 D0 1F 30 4E C8 29 51 E3 7C A2
    uint8_t expected_bytes[] = { 0xEF, 0xFC, 0x27, 0x52, 0xB5, 0xC9, 0xDA, 0x22, 0x88, 0xC5,
        0xD0, 0x1F, 0x30, 0x4E, 0xC8, 0x29, 0x51, 0xE3, 0x7C, 0xA2 };
    const char* expected_accountid_string = "r41vZ8exoVyUfVzs56yeN8xB5gDhSkho9a";

    // Create the account using the paper key
    BRRippleAccount account = rippleAccountCreate(paper_key);
    assert(account);

    // Get the 20 bytes that were created for the account
    uint8_t *accountBytes = getRippleAccountBytes(account);
    assert(0 == memcmp(accountBytes, expected_bytes, 20));

    // Get the ripple address string and compare
    const char *rippleAddress = getRippleAddress(account);
    assert(0 == memcmp(rippleAddress, expected_accountid_string, 25));

    rippleAccountDelete(account);
}

static void
testCreateRippleAccountWithSeed (void /* ... */) {
    const char * paper_key = "patient doctor olympic frog force glimpse endless antenna online dragon bargain someone";
    // The above set of words should produce the following Ripple account address
    // string: r41vZ8exoVyUfVzs56yeN8xB5gDhSkho9a
    const char* expected_accountid_string = "r41vZ8exoVyUfVzs56yeN8xB5gDhSkho9a";

    // Use the above paper key to create a seed value
    UInt512 seed = UINT512_ZERO;
    BRBIP39DeriveKey(seed.u8, paper_key, NULL); // no passphrase

    // If we pass the expected_accountid_string to this function it will validate for us
    BRRippleAccount account = rippleAccountCreateWithSeed(seed);
    assert(account);

    // Get the ripple address string and compare
    const char *rippleAddress = getRippleAddress(account);
    assert(0 == memcmp(rippleAddress, expected_accountid_string, 25));

    rippleAccountDelete(account);
}

static void
testCreateRippleAccountWithKey (void /* ... */) {
    const char * paper_key = "patient doctor olympic frog force glimpse endless antenna online dragon bargain someone";
    // The above set of words should produce the following Ripple account address
    // string: r41vZ8exoVyUfVzs56yeN8xB5gDhSkho9a
    const char* expected_accountid_string = "r41vZ8exoVyUfVzs56yeN8xB5gDhSkho9a";

    // Use the above paper key to create a seed value
    UInt512 seed = UINT512_ZERO;
    BRBIP39DeriveKey(seed.u8, paper_key, NULL); // no passphrase

    // Create the private key from the seed
    BRKey privateKey;
    // The BIP32 privateKey for m/44'/60'/0'/0/index
    BRBIP32PrivKeyPath(&privateKey, &seed, sizeof(UInt512), 5,
                       44 | BIP32_HARD,          // purpose  : BIP-44
                       144 | BIP32_HARD,        // coin_type: Ripple
                       0 | BIP32_HARD,          // account  : <n/a>
                       0,                        // change   : not change
                       0);                   // index    :

    privateKey.compressed = 0;

    // If we pass the expected_accountid_string to this function it will validate for us
    BRRippleAccount account = rippleAccountCreateWithKey(privateKey);
    assert(account);

    // Get the 20 bytes that were created for the account
    const char *rippleAddress = getRippleAddress(account);
    assert(0 == memcmp(rippleAddress, expected_accountid_string, 25));

    rippleAccountDelete(account);
}

static void getAccountInfo(const char* paper_key, const char* ripple_address) {
    BRRippleAccount account = createTestRippleAccount(paper_key, ripple_address);
    assert(account);
    
    // Get the 20 bytes that were created for the account
    uint8_t *bytes = getRippleAccountBytes(account);
    for(int i = 0; i < 20; i++) {
        if (i == 0) printf("ACCOUNT ID:\n");
        printf("%02X", bytes[i]);
        if (i == 19) printf("\n");
    }

    BRKey publicKey = rippleAccountGetPublicKey(account);
    for(int i = 0; i < 33; i++) {
        if (i == 0) printf("Public Key:\n");
        printf("%02X", publicKey.pubKey[i]);
        if (i == 32) printf("\n");
    }
    
    rippleAccountDelete(account);
}

static void
testSerializeWithSignature (void /* ... */) {
    BRRippleTransaction transaction;
    
    uint8_t expected_output[] = {
        0x12, 0x00, 0x00, // transaction type - 16-bit
        0x22, 0x80, 0x00, 0x00, 0x00, // Flags
        0x24, 0x00, 0x00, 0x00, 0x01, // Sequence - 32-bit
        0x61, 0x40, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x42, 0x40, // Amount 1,000,000 - 64-bit
        0x68, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, // Fee 12 - 64-bit
        0x73, 0x21, 0x03, 0x2b, 0xe3, 0xce, 0xe5, 0x76, 0x03, // Public Key
        0x6f, 0x90, 0x76, 0x59, 0x6f, 0xc4, 0xf8, 0xd4, 0xa2,
        0xa0, 0x57, 0xf8, 0xf8, 0x65, 0x48, 0xc0, 0x98, 0x0e,
        0x02, 0x13, 0x16, 0x07, 0x2f, 0xed, 0xa2, 0xd3,
        // NO SIGNATURE //
        0x81, 0x14, 0xEF, 0xFC, 0x27, 0x52, 0xB5, 0xC9, 0xDA, // Source address - type 8, field 1
        0x22, 0x88, 0xC5, 0xD0, 0x1F, 0x30, 0x4E, 0xC8, 0x29, 0x51, 0xE3, 0x7C, 0xA2,
        0x83, 0x14, 0xB6, 0xF8, 0x63, 0x80, 0x8B, 0x54, 0xD6, // Target Address - type 8, field 3
        0x43, 0xA0, 0x14, 0xCA, 0xF8, 0xB2, 0x97, 0xCF, 0xF8, 0xF2, 0xF9, 0x37, 0xE8
    };
    
    // Here are the 20 byte addresses
    //uint8_t destBytes[] = { 0xB6, 0xF8, 0x63, 0x80, 0x8B, 0x54, 0xD6,
    //    0x43, 0xA0, 0x14, 0xCA, 0xF8, 0xB2, 0x97, 0xCF, 0xF8, 0xF2, 0xF9, 0x37, 0xE8 };
    uint8_t destBytes[] = { 0xAF, 0x65, 0x53, 0xEE, 0x2C, 0xDC, 0xA1, 0x65, 0xAB, 0x03,
        0x75, 0xA3, 0xEF, 0xB0, 0xC7, 0x65, 0x0E, 0xA5, 0x53, 0x50 };
    
    // Create an account so we can get a public key
    //const char * paper_key = "patient doctor olympic frog force glimpse endless antenna online dragon bargain someone";
    const char * paper_key = "F603971FCF8366465537B6AD793B37BED5FF730D3764A9DC0F7F4AD911E7372C";
    BRRippleAccount account = createTestRippleAccount(paper_key, NULL);
    // Get the 20 bytes that were created for the account
    uint8_t *accountBytes = getRippleAccountBytes(account);

    BRRippleAddress sourceAddress;
    BRRippleAddress targetAddress;
    memcpy(sourceAddress.bytes, accountBytes, 20);
    memcpy(targetAddress.bytes, destBytes, 20);

    transaction = rippleTransactionCreate(sourceAddress, targetAddress, 1000000, 12);
    assert(transaction);

    uint32_t sequence_number = 25;

    // Before we sign the sequence should be 0
    uint32_t sequence = rippleTransactionGetSequence(transaction);
    // Since we don't add the sequence until later - it should be 0
    assert(0 == sequence);

    // Serialize and sign
    BRRippleSerializedTransaction s = rippleAccountSignTransaction(transaction, paper_key, sequence_number, 0);
    assert(s);
    int signed_size = getSerializedSize(s);
    uint8_t *signed_bytes = getSerializedBytes(s);
    // Print out the bytes as a string
    for (int i = 0; i < signed_size; i++) {
        if (i == 0) printf("SIGNED BYTES\n");
        printf("%02X", signed_bytes[i]);
        if (i == (signed_size -1)) printf("\n");
    }

    // After calling the sign function the sequence number should match what we passed in
    sequence = rippleTransactionGetSequence(transaction);
    // Since we don't add the sequence until later - it should be 0
    assert(sequence_number == sequence);

    // Compare the output with what we are expecting - ignore the signature
    //assert(0 == memcmp(signed_bytes, expected_output, 50));
    // Now compare the last 2 fields (source and destination)
    //assert(0 == memcmp(&signed_bytes[signed_size-44], &expected_output[sizeof(expected_output)-44], 44));
    
    deleteRippleTransaction(transaction);
    rippleAccountDelete(account);
}

extern BRRippleSignatureRecord rippleTransactionGetSignature(BRRippleTransaction transaction);

static void testTransactionDeserialize()
{
    uint8_t bytes[] = {
        0x12, 0x00, 0x00, // transaction type - 16-bit
        0x22, 0x80, 0x00, 0x00, 0x00, // Flags
        0x24, 0x00, 0x00, 0x00, 0x01, // Sequence - 32-bit
        0x61, 0x40, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x42, 0x40, // Amount 1,000,000 - 64-bit
        0x68, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, // Fee 12 - 64-bit
        0x73, 0x21, 0x03, 0x2b, 0xe3, 0xce, 0xe5, 0x76, 0x03, // Public Key
        0x6f, 0x90, 0x76, 0x59, 0x6f, 0xc4, 0xf8, 0xd4, 0xa2,
        0xa0, 0x57, 0xf8, 0xf8, 0x65, 0x48, 0xc0, 0x98, 0x0e,
        0x02, 0x13, 0x16, 0x07, 0x2f, 0xed, 0xa2, 0xd3,
        // signature goes here
        0x74, 0x46, 0x30, 0x44, 0x02, 0x20, 0x14, 0x37, 0x59, 0x43,
        0x7C, 0x04, 0xF7, 0xB6, 0x1F, 0x01, 0x25, 0x63, 0xAF, 0xE9,
        0x0D, 0x8D, 0xAF, 0xC4, 0x6E, 0x86, 0x03, 0x5E, 0x1D, 0x96,
        0x5A, 0x9C, 0xED, 0x28, 0x2C, 0x97, 0xD4, 0xCE, 0x02, 0x20,
        0x4C, 0xFD, 0x24, 0x1E, 0x86, 0xF1, 0x7E, 0x01, 0x12, 0x98,
        0xFC, 0x1A, 0x39, 0xB6, 0x33, 0x86, 0xC7, 0x43, 0x06, 0xA5,
        0xDE, 0x04, 0x7E, 0x21, 0x3B, 0x0F, 0x29, 0xEF, 0xA4, 0x57,
        0x1C, 0x2C,
        // Source address
        0x81, 0x14, 0xEF, 0xFC, 0x27, 0x52, 0xB5, 0xC9, 0xDA,
        0x22, 0x88, 0xC5, 0xD0, 0x1F, 0x30, 0x4E, 0xC8, 0x29, 0x51, 0xE3, 0x7C, 0xA2,
        // Destination address
        0x83, 0x14, 0xB6, 0xF8, 0x63, 0x80, 0x8B, 0x54, 0xD6,
        0x43, 0xA0, 0x14, 0xCA, 0xF8, 0xB2, 0x97, 0xCF, 0xF8, 0xF2, 0xF9, 0x37, 0xE8
    };
    BRRippleTransaction transaction = rippleTransactionCreateFromBytes(bytes, 29 + 26 + 44 + 72);
    assert(transaction);
    uint16_t transactionType = rippleTransactionGetType(transaction);
    assert(PAYMENT == transactionType);

    uint32_t flags = rippleTransactionGetFlags(transaction);
    assert(0x80000000 == flags);
    uint32_t sequence = rippleTransactionGetSequence(transaction);
    assert(1 == sequence);

    uint64_t fee = rippleTransactionGetFee(transaction);
    assert(12 == fee);
    uint64_t amount = rippleTransactionGetAmount(transaction);
    assert(1000000 == amount);

    BRRippleAddress sourceAddress = rippleTransactionGetSource(transaction);
    uint8_t expected_source_address[] = {
        0xEF, 0xFC, 0x27, 0x52, 0xB5, 0xC9, 0xDA, // Source address - type 8, field 1
        0x22, 0x88, 0xC5, 0xD0, 0x1F, 0x30, 0x4E, 0xC8, 0x29, 0x51, 0xE3, 0x7C, 0xA2
    };
    assert(0 == memcmp(sourceAddress.bytes, expected_source_address, 20));

    BRRippleAddress targetAddress = rippleTransactionGetTarget(transaction);
    uint8_t expected_target_address[] = {
        0xB6, 0xF8, 0x63, 0x80, 0x8B, 0x54, 0xD6, // Target Address - type 8, field 3
        0x43, 0xA0, 0x14, 0xCA, 0xF8, 0xB2, 0x97, 0xCF, 0xF8, 0xF2, 0xF9, 0x37, 0xE8
    };
    assert(0 == memcmp(targetAddress.bytes, expected_target_address, 20));
    
    BRRippleSignatureRecord signature = rippleTransactionGetSignature(transaction);
    uint8_t sig_bytes[] = {  0x30, 0x44, 0x02, 0x20, 0x14, 0x37, 0x59, 0x43,
        0x7C, 0x04, 0xF7, 0xB6, 0x1F, 0x01, 0x25, 0x63, 0xAF, 0xE9,
        0x0D, 0x8D, 0xAF, 0xC4, 0x6E, 0x86, 0x03, 0x5E, 0x1D, 0x96,
        0x5A, 0x9C, 0xED, 0x28, 0x2C, 0x97, 0xD4, 0xCE, 0x02, 0x20,
        0x4C, 0xFD, 0x24, 0x1E, 0x86, 0xF1, 0x7E, 0x01, 0x12, 0x98,
        0xFC, 0x1A, 0x39, 0xB6, 0x33, 0x86, 0xC7, 0x43, 0x06, 0xA5,
        0xDE, 0x04, 0x7E, 0x21, 0x3B, 0x0F, 0x29, 0xEF, 0xA4, 0x57,
        0x1C, 0x2C};
    assert(0 == memcmp(signature.signature, sig_bytes, signature.sig_length));
}

static void
testTransactionHash (void /* ... */) {
    BRRippleTransaction transaction;

    // Here are the 20 byte addresses
    uint8_t destBytes[] = { 0xAF, 0x65, 0x53, 0xEE, 0x2C, 0xDC, 0xA1, 0x65, 0xAB, 0x03,
        0x75, 0xA3, 0xEF, 0xB0, 0xC7, 0x65, 0x0E, 0xA5, 0x53, 0x50 };
    
    // Create an account so we can get a public key
    const char * paper_key = "F603971FCF8366465537B6AD793B37BED5FF730D3764A9DC0F7F4AD911E7372C";
    BRRippleAccount account = createTestRippleAccount(paper_key, NULL);
    // Get the 20 bytes that were created for the account
    uint8_t *accountBytes = getRippleAccountBytes(account);
    
    BRRippleAddress sourceAddress;
    BRRippleAddress targetAddress;
    memcpy(sourceAddress.bytes, accountBytes, 20);
    memcpy(targetAddress.bytes, destBytes, 20);
    
    transaction = rippleTransactionCreate(sourceAddress, targetAddress, 1000000, 12);
    assert(transaction);

    // Serialize and sign
    BRRippleSerializedTransaction s = rippleAccountSignTransaction(transaction, paper_key, 25, 0);
    assert(s);
    
    // Compare the transaction hash
    uint8_t expected_hash[] = {
        0xC0, 0x33, 0x01, 0x0E, 0x2A, 0x32, 0x69, 0xBF,
        0x21, 0x2A, 0xDB, 0x62, 0x81, 0x58, 0xD8, 0xB6,
        0x89, 0x7D, 0x15, 0x53, 0x54, 0x8F, 0xCB, 0xBD,
        0x9A, 0x2A, 0xC7, 0x43, 0xED, 0xB3, 0x09, 0x0F
    };
    BRRippleTransactionHash hash = rippleTransactionGetHash(transaction);
    assert(0 == memcmp(hash.bytes, expected_hash, 32));

    deleteRippleTransaction(transaction);
    rippleAccountDelete(account);
}


static void createAndDeleteWallet()
{
    const char * paper_key = "patient doctor olympic frog force glimpse endless antenna online dragon bargain someone";
    BRRippleAccount account = rippleAccountCreate(paper_key);
    BRRippleWallet wallet = rippleWalletCreate(account);
    assert(wallet);
    rippleWalletRelease(wallet);
}

static void testWalletBalance()
{
    const char * paper_key = "patient doctor olympic frog force glimpse endless antenna online dragon bargain someone";
    BRRippleAccount account = rippleAccountCreate(paper_key);
    BRRippleWallet wallet = rippleWalletCreate(account);
    assert(wallet);
    rippleWalletRelease(wallet);

    uint64_t balance = rippleWalletGetBalance(wallet);
    assert(balance == 0);

    uint64_t expected_balance = 25000000;
    rippleWalletSetBalance(wallet, expected_balance);
    balance = rippleWalletGetBalance(wallet);
    assert(balance == expected_balance);
}

static void runWalletTests()
{
    createAndDeleteWallet();
    testWalletBalance();
}

static void checkDecodeRippleAddress()
{
    uint8_t expected_bytes[] = { 0xEF, 0xFC, 0x27, 0x52, 0xB5, 0xC9, 0xDA, 0x22, 0x88, 0xC5,
        0xD0, 0x1F, 0x30, 0x4E, 0xC8, 0x29, 0x51, 0xE3, 0x7C, 0xA2 };
    const char* ripple_address = "r41vZ8exoVyUfVzs56yeN8xB5gDhSkho9a";
    
    BRRippleAddress address;
    int length = rippleAddressStringToAddress(ripple_address, &address);
    assert(length == 20);
    assert(0 == memcmp(expected_bytes, address.bytes, 20));
}

static void testRippleAddressCreate()
{
    uint8_t expected_bytes[] = { 0xEF, 0xFC, 0x27, 0x52, 0xB5, 0xC9, 0xDA, 0x22, 0x88, 0xC5,
        0xD0, 0x1F, 0x30, 0x4E, 0xC8, 0x29, 0x51, 0xE3, 0x7C, 0xA2 };
    const char* ripple_address = "r41vZ8exoVyUfVzs56yeN8xB5gDhSkho9a";
    
    BRRippleAddress address = rippleAddressCreate(ripple_address);
    assert(0 == memcmp(expected_bytes, address.bytes, 20));
}

static void testRippleAddressEqual()
{
    uint8_t address_input[] = { 0xEF, 0xFC, 0x27, 0x52, 0xB5, 0xC9, 0xDA, 0x22, 0x88, 0xC5,
        0xD0, 0x1F, 0x30, 0x4E, 0xC8, 0x29, 0x51, 0xE3, 0x7C, 0xA2 };

    BRRippleAddress a1;
    BRRippleAddress a2;
    memcpy(a1.bytes, address_input, 20);
    a2 = a1;
    int result = rippleAddressEqual(a1, a2);
    assert(1 == result);

    // Modify a2
    a2.bytes[0] = 0xFF;
    result = rippleAddressEqual(a1, a2);
    assert(0 == result);
}

void rippleAccountTests()
{
    testCreateRippleAccountWithPaperKey();
    testCreateRippleAccountWithSeed();
    testCreateRippleAccountWithKey();
    testRippleAddressCreate();
    testRippleAddressEqual();
}

extern void
runRippleTest (void /* ... */) {

    // Account tests
    rippleAccountTests();

    // Transaction tests
    testRippleTransaction();
    testRippleTransactionGetters();
    testSerializeWithSignature();
    testTransactionHash();
    testTransactionDeserialize();

    // base58 encoding tests
    checkDecodeRippleAddress();

    // Wallet tests
    runWalletTests();

    // If we pass the expected_accountid_string to this function it will validate for us
    const char* paper_key = "996F8505F48FCB74D291F7B459A5EA20FEA97ACEC803D7459B3742AF3DCF4B9D";
    const char* ripple_address = "rGzQdxGFHLtiP16x8k458Am5sy8mBcnhpD";
    getAccountInfo(paper_key, ripple_address);
}