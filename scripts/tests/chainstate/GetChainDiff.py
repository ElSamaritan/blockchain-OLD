#!/usr/bin/env python3

import json
import requests
from jsondiff import diff


class BlockExplorerSerivce:
    Host = '',
    Port = 0

    def __init__(self, host, port):
        self.Host = host
        self.Port = port

    def getBlockInfo(self, height):
        return self.makeCall("explorer.block.detailed", {
            "type": "height",
            "value": height
        })

    def makeCall(self, method, params={}):
        payload = {
            'jsonrpc': '2.0',
            'method': method,
            'id': 'test',
            'params': params
        }
        endpoint = 'http://' + self.Host + ':' + str(self.Port) + '/rpc'
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


firstService = BlockExplorerSerivce('localhost', 22869)
secondService = BlockExplorerSerivce('localhost', 23869)

for i in range(1, 500):
    first = firstService.getBlockInfo(i)
    second = secondService.getBlockInfo(i)
    if first != second:
        print("Difference at height: %d" % (i))
        print(json.dumps(diff(first, second, syntax='symmetric'), indent=4))
        break
