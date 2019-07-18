#!/usr/bin/env python3

import yaml
import json
import requests
from random import randint
from time import sleep


class Transfer:
    Address = ''
    Amount = 0

    def __str__(self):
        return "%.6f XIG to '%s'" % (prettyAmount(self.Amount), self.Address)


class Transaction:
    Transfers = []
    Anonymity = 0
    Fee = 0
    UnlockTime = 0

    def __str__(self):
        _str = "\n\tFee: %.6f\n\tAnonymity: %d\n\tTransfers:" % (
            prettyAmount(self.Fee), self.Anonymity)
        for transfer in self.Transfers:
            _str += ("\n\t\t%s" % transfer)
        return _str


class FusionTransaction:
    Threshold = 10000000000000000
    Anonymity = 0

    def __str__(self):
        return "\n\tDustThreshold: %.6f\n\tAnonymity: %d\n" % (prettyAmount(self.Threshold), self.Anonymity)


class Balance:
    Available = 0
    Locked = 0
    Total = 0

    def __init__(self, available, locked):
        self.Available = available
        self.Locked = locked

    def __str__(self):
        return "Availabe '%.6f', Locked '%.6f', Total '%.6f'" % (prettyAmount(self.Available), prettyAmount(self.Locked), prettyAmount(self.Total))


class PGService:
    Host = '',
    Port = 0,
    Password = ''

    def __init__(self, host, port, password):
        self.Host = host
        self.Port = port
        self.Password = password

    def getBalance(self):
        return self.makeCall("getBalance")

    def getAddress(self):
        return self.makeCall("getAddresses")

    def reset(self):
        return self.makeCall("reset")

    def sendTransaction(self, transaction: Transaction):
        transfers = []
        for transfer in transaction.Transfers:
            transfers.append({
                'address': transfer.Address,
                'amount': transfer.Amount
            })
        return self.makeCall("sendTransaction", {
            'transfers': transfers,
            'anonymity': transaction.Anonymity,
            'fee': transaction.Fee,
            'unlock_time': transaction.UnlockTime
        })

    def sendFusionTransaction(self, transaction: FusionTransaction):
        return self.makeCall("sendFusionTransaction", {
            'threshold': transaction.Threshold,
            'anonymity': transaction.Anonymity
        })

    def makeCall(self, method, params={}):
        payload = {
            'jsonrpc': '2.0',
            'method': method,
            'password': self.Password,
            'id': 'test',
            'params': params
        }
        endpoint = 'http://' + self.Host + ':' + str(self.Port) + '/json_rpc'
        # print(endpoint)
        # print(json.dumps(payload))
        response = requests.post(
            endpoint,
            data=json.dumps(payload),
            headers={
                'Content-Type': 'application/json'
            }).json()
        # print(response)
        if 'error' in response:
            raise ValueError(response['error'])
        return response


def prettyAmount(amount):
    return float(amount) / 1000000.0


class Wallet:
    Host = ''
    Port = 0
    Password = ''
    Api = None

    Address = ''
    Balance = 0

    def __init__(self, cfg):
        self.Host = cfg['Host']
        self.Port = cfg['Port']
        self.Password = cfg['Password']
        self.Api = PGService(self.Host, self.Port, self.Password)

    def __str__(self):
        return "%s\n\tBalance\t%s" % (self.Address, self.Balance)

    def updateBalance(self):
        jsonResult = self.Api.getBalance()
        self.Balance = Balance(
            jsonResult['result']['available_balance'],
            jsonResult['result']['locked_amount'])

    def sendTransaction(self, transaction: Transaction):
        print("\nSending transaction from %s...%s" %
              (self.Address, transaction))
        jsonResult = self.Api.sendTransaction(transaction)
        print("Transaction sent: %s" %
              jsonResult['result']['transaction_hash'])

    def sendFusionTransaction(self, transaction: FusionTransaction):
        print("\nSending fusion transaction from %s...%s" %
              (self.Address, transaction))
        jsonResult = self.Api.sendFusionTransaction(transaction)
        print("Fusion Transaction sent: %s" %
              jsonResult['result']['transaction_hash'])

    def initialize(self, reset: bool):
        self.Address = self.Api.getAddress()['result']['addresses'][0]
        if reset:
            self.Api.reset()
        self.updateBalance()
        print(self)


with open("MakeTransactions.yml", 'r') as ymlfile:
    config = yaml.load(ymlfile)

MinFee = config['MinimumFee']
MaxFee = config['MaximumFee']
MinAmount = config['MinimumAmount']
MaxAmount = config['MaximumAmount']
MinMixin = config['MinAnonymity']
MaxMixin = config['MaxAnonymity']
MinFusionThreshold = config['MinFusionThreshold']
MaxFusionThreshold = config['MaxFusionThreshold']
FusionPropability = config['FusionPropability']
ResetOnStart = config['ResetOnStart']
TPM = config['TransactionsPerMinute']

wallets = []
for wallet in config['Wallets']:
    wallets.append(Wallet(wallet))

for wallet in wallets:
    wallet.initialize(ResetOnStart)

if len(wallets) < 2:
    raise ValueError('The script expects at least 2 wallets.')


def makeRandomTransaction():
    transaction = Transaction()
    transaction.Transfers = []
    sortedWallets = sorted(
        wallets, key=lambda x: x.Balance.Available, reverse=True)
    senderIndex = 0
    recieverIndex = len(sortedWallets) - 1
    senderWallet = sortedWallets[senderIndex]
    recievers = []
    recievers.append(sortedWallets[recieverIndex].Address)

    transaction.Fee = randint(MinFee, MaxFee)
    transaction.Anonymity = randint(MinMixin, MaxMixin)

    totalAmount = transaction.Fee
    for reciever in recievers:
        transfer = Transfer()
        transfer.Address = reciever
        transfer.Amount = randint(MinAmount, MaxAmount)
        transaction.Transfers.append(transfer)
        totalAmount += transfer.Amount

    if totalAmount > senderWallet.Balance.Available:
        print("Not enough balance. Required: %.6f Available: %.6f" % (
            prettyAmount(totalAmount), prettyAmount(senderWallet.Balance.Available)))
    else:
        try:
            senderWallet.sendTransaction(transaction)
        except ValueError as error:
            print("Error sending transaction: '%s'" % error)


def makeRandomFusionTransaction():
    transaction = FusionTransaction()
    wallet = wallets[randint(0, len(wallets) - 1)]
    transaction.Threshold = randint(MinFusionThreshold, MaxFusionThreshold)
    transaction.Anonymity = randint(MinMixin, MaxMixin)
    try:
        wallet.sendFusionTransaction(transaction)
    except ValueError as error:
        print("Error sending fusion transaction: '%s'" % error)


while True:
    if randint(0, 99) < FusionPropability:
        makeRandomFusionTransaction()
    else:
        makeRandomTransaction()
    sleep(60.0 / float(TPM))
    for wallet in wallets:
        wallet.updateBalance()
        print("\n %s" % wallet)
